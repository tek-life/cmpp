
/* 
 * China Mobile CMPP 2.0 protocol library
 * By typefo <typefo@qq.com>
 * Update: 2017-07-10
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include "cmpp.h"
#include "packet.h"
#include "common.h"
#include "socket.h"

int cmpp_init_sp(CMPP_SP_T *cmpp, char *host, unsigned short port) {
    if (!cmpp) {
        return -1;
    }

    int err;
    cmpp->ok = false;
    cmpp->err = 0;
    cmpp->version = CMPP_VERSION;
    pthread_mutex_init(&cmpp->lock, NULL);

    /* Initialize the socket settings */
    cmpp_sock_init(&cmpp->sock);

    /* Create a new socket */
    cmpp->sock.fd = cmpp_sock_create();
    if (cmpp->sock.fd < 1) {
        cmpp->err = CMPP_ERR_INITCCS;
        return -1;
    }

    /* Connect to server */
    err = cmpp_sock_connect(&cmpp->sock, host, port);
    if (err) {
        cmpp->err = CMPP_ERR_INITCCTS;
        return -1;
    }

    /* TCP NONBLOCK */
    cmpp_sock_nonblock(&cmpp->sock, true);

    /* TCP NODELAY */
    cmpp_sock_tcpnodelay(&cmpp->sock, true);

    /* TCP KEEPAVLIE */
    cmpp_sock_keepavlie(&cmpp->sock, 30, 5, 3);

    /* Sequence Number Generator */
    cmpp->sequence = gen_sequence;

    return 0;
}

int cmpp_close(CMPP_SP_T *cmpp) {
    if (cmpp) {
        cmpp->ok = false;
        cmpp->err = 0;
        cmpp_sock_close(&cmpp->sock);
        return 0;
    }

    return -1;
}

int cmpp_connect(CMPP_SP_T *cmpp, const char *user, const char *password) {
    if (!cmpp) {
        return -1;
    }
    
    CMPP_CONNECT_T ccp;
    memset(&ccp, 0, sizeof(ccp));
    
    /* Header */
    cmpp_add_header((CMPP_HEAD_T *)&ccp, sizeof(ccp), CMPP_CONNECT, cmpp->sequence());

    /* Source_Addr */
    memcpy(ccp.sourceAddr, user, sizeof(ccp.sourceAddr));
    
    /* Version */
    ccp.version = cmpp->version;

    /* Timestamp */
    time_t now;
    struct tm *t;
    time(&now);
    t = localtime(&now);
    char timestamp[11] = {0};
    sprintf(timestamp, "%02d%02d%02d%02d%02d", t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    ccp.timestamp = htonl(atoi(timestamp));

    /* AuthenticatorSource */
    int len;
    unsigned char buff[128];

    len = strlen(user) + 9 + strlen(password) + strlen(timestamp);
    if (len > sizeof(buff)) {
        cmpp->err = CMPP_ERR_CONUPTL;
        return -1;
    }
    
    memset(buff, 0, sizeof(buff));
    memcpy(buff, user, strlen(user));
    memcpy(buff + strlen(user) + 9, password, strlen(password));
    memcpy(buff + strlen(user) + 9 + strlen(password), timestamp, strlen(timestamp));
    cmpp_md5(ccp.authenticatorSource, buff, len);

    /* Send Cmpp_Connect packet */
    len = sizeof(ccp);

    if (cmpp_send(cmpp, &ccp, sizeof(ccp)) != 0) {
        cmpp->err = CMPP_ERR_CONSCPE;
        return -1;
    }
    
    /* Confirm response status */
    int status;
    CMPP_PACK_T pack;
    CMPP_CONNECT_RESP_T *ccrp;

    cmpp_recv(cmpp, &pack, sizeof(pack));

    if (!is_cmpp_command(&pack, sizeof(pack), CMPP_CONNECT_RESP)) {
        cmpp->err = CMPP_ERR_CONSRPE;
        return -1;
    }

    ccrp = (CMPP_CONNECT_RESP_T *)&pack;
    /* status = ntohl(ccrp->status); */
    status = ccrp->status;

    if (status == 0) {
        cmpp->ok = true;
    }
    
    return status;
}

int cmpp_active_test(CMPP_SP_T *cmpp) {
    if (!cmpp) {
        return -1;
    }

    CMPP_ACTIVE_TEST_T catp;
    memset(&catp, 0, sizeof(catp));
    cmpp_add_header((CMPP_HEAD_T *)&catp, sizeof(catp), CMPP_ACTIVE_TEST, cmpp->sequence());

    if (cmpp_send(cmpp, &catp, sizeof(catp)) != 0) {
        cmpp->err = CMPP_ERR_ACTSCPE;
        return -1;
    }

    CMPP_PACK_T pack;

    cmpp_recv(cmpp, &pack, sizeof(pack));

    if (!is_cmpp_command(&pack, sizeof(pack), CMPP_ACTIVE_TEST_RESP)) {
        cmpp->err = CMPP_ERR_ACTSRPE;
        return -1;
    }

    return 0;
}

int cmpp_terminate(CMPP_SP_T *cmpp) {
    if (!cmpp) {
        return -1;
    }

    CMPP_TERMINATE_T ctp;
    memset(&ctp, 0, sizeof(ctp));
    cmpp_add_header((CMPP_HEAD_T *)&ctp, sizeof(ctp), CMPP_TERMINATE, cmpp->sequence());

    if (cmpp_send(cmpp, &ctp, sizeof(ctp)) != 0) {
        cmpp->err = CMPP_ERR_TERSTPE;
        return -1;
    }

    CMPP_PACK_T pack;

    cmpp_recv(cmpp, &pack, sizeof(pack));    

    if (!is_cmpp_command(&pack, sizeof(pack), CMPP_TERMINATE_RESP)) {
        cmpp->err = CMPP_ERR_TERSRPE;
        return -1;
    }
    
    cmpp->ok = false;
    return 0;
}

int cmpp_submit(CMPP_SP_T *cmpp, const char *phone, const char *message, bool delivery,
                char *serviceId, char *msgFmt, char *msgSrc) {

    if (!cmpp) {
        return -1;
    }

    char buff[140];
    CMPP_SUBMIT_T csp;
    size_t offset, msgLen;
    
    memset(&csp, 0, sizeof(csp));
    cmpp_add_header((CMPP_HEAD_T *)&csp, sizeof(csp), CMPP_SUBMIT, cmpp->sequence());

    offset = sizeof(CMPP_HEAD_T);
    
    /* Msg_Id */
    cmpp_pack_add_integer(&csp, 0, &offset, 8);
    
    /* Pk_Total */
    cmpp_pack_add_integer(&csp, 1, &offset, 1);
    
    /* Pk_Number */
    cmpp_pack_add_integer(&csp, 1, &offset, 1);
    
    /* Registered_Delivery */
    cmpp_pack_add_integer(&csp, (delivery ? 1 : 0), &offset, 1);

    /* Msg_Level */
    cmpp_pack_add_integer(&csp, 1, &offset, 1);
    
    /* Service_Id */
    cmpp_pack_add_string(&csp, serviceId, strlen(serviceId), &offset, 10);
    
    /* Fee_User_Type */
    cmpp_pack_add_integer(&csp, 0, &offset, 1);
    
    /* FeeTerminal_Id */
    cmpp_pack_add_string(&csp, phone, strlen(phone), &offset, 21);
    
    /* Tpp_Id */
    cmpp_pack_add_integer(&csp, 0, &offset, 1);
    
    /* Tp_Udhi */
    cmpp_pack_add_integer(&csp, 0, &offset, 1);
    
    /* Msg_Fmt */
    unsigned char mft = 0;

    if (strcasecmp(msgFmt, "ASCII") == 0) {
        mft = 0;
    } else if (strcasecmp(msgFmt, "UCS-2") == 0) {
        mft = 8;
    } else if (strcasecmp(msgFmt, "GBK") == 0) {
        mft  = 15;
    }

    cmpp_pack_add_integer(&csp, mft, &offset, 1);
    
    /* Msg_Src */
    cmpp_pack_add_string(&csp, msgSrc, strlen(msgSrc), &offset, 6);
    
    /* Fee_Type */
    cmpp_pack_add_string(&csp, "02", 2, &offset, 2);
    
    /* Fee_Code */
    cmpp_pack_add_string(&csp, "000000", 6, &offset, 6);

    /* ValId_Time */
    cmpp_pack_add_string(&csp, "0", 1, &offset, 17);
    
    /* At_Time */
    cmpp_pack_add_string(&csp, "0", 1, &offset, 17);
    
    /* Src_Id */
    cmpp_pack_add_string(&csp, serviceId, strlen(serviceId), &offset, 21);
    
    /* DestUsr_Tl */
    cmpp_pack_add_integer(&csp, 1, &offset, 1);
    
    /* Dest_Terminal_Id */
    cmpp_pack_add_string(&csp, phone, strlen(phone), &offset, 21);
    

    /* Msg_Length  */
    memset(buff, 0, sizeof(buff));
    cmpp_conv(message, strlen(message), (char *)buff, sizeof(buff), "UTF-8", msgFmt);
    msgLen = cmpp_ucs2count(buff);
    cmpp_pack_add_integer(&csp, msgLen, &offset, 1);
    
    /* Msg_Content */
    cmpp_pack_add_string(&csp, buff, msgLen, &offset, msgLen);

    /* Reserve  */
    cmpp_pack_add_string(&csp, "0", 1, &offset, 8);

    /* Total_Length */
    csp.totalLength = htonl(offset);

    if (cmpp_send(cmpp, &csp, sizeof(csp)) != 0) {
        cmpp->err = CMPP_ERR_SUBSSPE;
        return -1;
    }

    CMPP_PACK_T pack;
    CMPP_SUBMIT_RESP_T *csrp;

    cmpp_recv(cmpp, &pack, sizeof(pack));    

    if (!is_cmpp_command(&pack, sizeof(pack), CMPP_SUBMIT_RESP)) {
        cmpp->err = CMPP_ERR_SUBSRPE;
        return -1;
    }

    csrp = (CMPP_SUBMIT_RESP_T *)&pack;
    //int result = ntohs(csrp->result);
    
    return csrp->result;
}

int cmpp_deliver_resp(CMPP_SP_T *cmpp, unsigned long sequenceId, unsigned long long msgId, unsigned char result) {
    CMPP_DELIVER_RESP_T cdrp;

    memset(&cdrp, 0, sizeof(cdrp));
    cmpp_add_header((CMPP_HEAD_T *)&cdrp, sizeof(cdrp), CMPP_DELIVER_RESP, sequenceId);

    cdrp.msgId = msgId;
    cdrp.result = result;

    if (cmpp_send(cmpp, &cdrp, sizeof(cdrp)) != 0) {
        cmpp->err = CMPP_ERR_DELSPFE;
        return -1;
    }

    return 0;
}
