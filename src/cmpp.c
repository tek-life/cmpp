
/* 
 * China Mobile CMPP 2.0 protocol library
 * Copyright (C) 2017 typefo <typefo@qq.com>
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
#include "socket.h"
#include "packet.h"
#include "common.h"
#include "cmpp.h"

int cmpp_init_sp(cmpp_sp_t *cmpp, char *host, unsigned short port) {
    if (!cmpp) {
        return -1;
    }

    int fd, err;
    cmpp->ok = false;
    cmpp->version = CMPP_VERSION;
    pthread_mutex_init(&cmpp->lock, NULL);

    /* Create a new socket */
    fd = cmpp_sock_create();
    if (fd < 1) {
        return 1;
    }

    /* Initialize the socket settings */
    cmpp_sock_init(&cmpp->sock, fd);

    /* Connect to server */
    err = cmpp_sock_connect(&cmpp->sock, host, port);
    if (err) {
        return 2;
    }

    /* TCP NONBLOCK */
    cmpp_sock_nonblock(&cmpp->sock, true);

    /* TCP NODELAY */
    cmpp_sock_tcpnodelay(&cmpp->sock, true);

    /* TCP KEEPAVLIE */
    cmpp_sock_keepavlie(&cmpp->sock, 30, 5, 3);

    return 0;
}

int cmpp_init_ismg(cmpp_ismg_t *cmpp, const char *addr, unsigned short port) {
    if (!cmpp) {
        return -1;
    }

    int fd, err;
    cmpp->version = CMPP_VERSION;
    pthread_mutex_init(&cmpp->lock, NULL);

    /* Create a new socket */
    fd = cmpp_sock_create();
    if (fd < 1) {
        return 1;
    }
    
    /* Initialize the socket settings */
    cmpp_sock_init(&cmpp->sock, fd);

    /* bind to address */
    err = cmpp_sock_bind(&cmpp->sock, addr, port, 1024);
    if (err) {
        return 2;
    }

    /* TCP NONBLOCK */
    cmpp_sock_nonblock(&cmpp->sock, true);

    /* TCP NODELAY */
    cmpp_sock_tcpnodelay(&cmpp->sock, true);

    /* TCP KEEPAVLIE */
    cmpp_sock_keepavlie(&cmpp->sock, 30, 5, 3);

    return 0;
}

int cmpp_sp_close(cmpp_sp_t *cmpp) {
    if (cmpp) {
        cmpp->ok = false;
        cmpp_sock_close(&cmpp->sock);
        return 0;
    }

    return -1;
}

int cmpp_ismg_close(cmpp_ismg_t *cmpp) {
    if (cmpp) {
        cmpp_sock_close(&cmpp->sock);
        return 0;
    }

    return -1;
}

int cmpp_connect(cmpp_sock_t *sock, const char *user, const char *password) {
    int err;
    cmpp_connect_t ccp;
    memset(&ccp, 0, sizeof(ccp));
    
    /* Header */
    err = cmpp_add_header((cmpp_head_t *)&ccp, sizeof(ccp), CMPP_CONNECT, cmpp_sequence());
    if (err) {
        return 1;
    }

    /* Source_Addr */
    memcpy(ccp.sourceAddr, user, sizeof(ccp.sourceAddr));
    
    /* Version */
    ccp.version = CMPP_VERSION;

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
        return 2;
    }
    
    memset(buff, 0, sizeof(buff));
    memcpy(buff, user, strlen(user));
    memcpy(buff + strlen(user) + 9, password, strlen(password));
    memcpy(buff + strlen(user) + 9 + strlen(password), timestamp, strlen(timestamp));
    err = cmpp_md5(ccp.authenticatorSource, buff, len);
    if (err) {
        return 3;
    }

    /* Send Cmpp_Connect packet */
    err = cmpp_send(sock, &ccp, sizeof(ccp));
    if (err) {
        return (err == -1) ? err : 4;
    }

    return 0;
}

int cmpp_connect_resp(cmpp_sock_t *sock, unsigned int sequenceId, unsigned char status) {
    int err;
    cmpp_connect_resp_t ccrp;
    memset(&ccrp, 0, sizeof(ccrp));
    err = cmpp_add_header((cmpp_head_t *)&ccrp, sizeof(ccrp), CMPP_CONNECT_RESP, sequenceId);
    if (err) {
        return 1;
    }

    ccrp.status = status;
    ccrp.version = CMPP_VERSION;

    err = cmpp_send(sock, &ccrp, sizeof(ccrp));
    if (err) {
        return (err == -1) ? err : 2;
    }

    return 0;
}

int cmpp_active_test(cmpp_sock_t *sock) {
    int err;
    cmpp_active_test_t catp;
    memset(&catp, 0, sizeof(catp));
    err = cmpp_add_header((cmpp_head_t *)&catp, sizeof(catp), CMPP_ACTIVE_TEST, cmpp_sequence());
    if (err) {
        return 1;
    }

    err = cmpp_send(sock, &catp, sizeof(catp));
    if (err == -1) {
        return (err == -1) ? err : 2;
    }

    return 0;
}

int cmpp_active_test_resp(cmpp_sock_t *sock, unsigned int sequenceId) {
    int err;
    cmpp_active_test_resp_t catrp;
    
    memset(&catrp, 0, sizeof(cmpp_active_test_resp_t));
    err = cmpp_add_header((cmpp_head_t *)&catrp, sizeof(catrp), CMPP_ACTIVE_TEST_RESP, sequenceId);
    if (err) {
        return 1;
    }

    return 0;
}

int cmpp_terminate(cmpp_sock_t *sock) {
    int err;
    cmpp_terminate_t ctp;

    memset(&ctp, 0, sizeof(ctp));
    err = cmpp_add_header((cmpp_head_t *)&ctp, sizeof(ctp), CMPP_TERMINATE, cmpp_sequence());
    if (err) {
        return 1;
    }

    err = cmpp_send(sock, &ctp, sizeof(ctp));
    if (err) {
        return (err == -1) ? err : 2;
    }

    return 0;
}

int cmpp_terminate_resp(cmpp_sock_t *sock, unsigned int sequenceId) {
    int err;
    cmpp_head_t pack;

    memset(&pack, 0, sizeof(pack));
    err = cmpp_add_header(&pack, sizeof(cmpp_head_t), CMPP_TERMINATE_RESP, sequenceId);
    if (err) {
        return 1;
    }

    err = cmpp_send(sock, &pack, sizeof(pack));
    if (err) {
        return (err == -1) ? err : 2;
    }

    return 0;
}

unsigned int cmpp_submit(cmpp_sock_t *sock, const char *phone, const char *message, bool delivery,
                         char *serviceId, char *msgFmt, char *msgSrc) {

    int err;
    char buff[140];
    cmpp_head_t *head;
    cmpp_pack_t pack;
    size_t offset, msgLen;
    
    memset(&pack, 0, sizeof(pack));
    head = (cmpp_head_t *)&pack;
    err = cmpp_add_header(head, sizeof(cmpp_head_t), CMPP_SUBMIT, cmpp_sequence());
    if (err) {
        return 1;
    }

    offset = sizeof(cmpp_head_t);
    
    /* Msg_Id */
    cmpp_pack_add_integer(&pack, 0, &offset, 8);
    
    /* Pk_Total */
    cmpp_pack_add_integer(&pack, 1, &offset, 1);
    
    /* Pk_Number */
    cmpp_pack_add_integer(&pack, 1, &offset, 1);
    
    /* Registered_Delivery */
    cmpp_pack_add_integer(&pack, (delivery ? 1 : 0), &offset, 1);

    /* Msg_Level */
    cmpp_pack_add_integer(&pack, 1, &offset, 1);
    
    /* Service_Id */
    cmpp_pack_add_string(&pack, serviceId, strlen(serviceId), &offset, 10);
    
    /* Fee_User_Type */
    cmpp_pack_add_integer(&pack, 0, &offset, 1);
    
    /* FeeTerminal_Id */
    cmpp_pack_add_string(&pack, phone, strlen(phone), &offset, 21);
    
    /* Tpp_Id */
    cmpp_pack_add_integer(&pack, 0, &offset, 1);
    
    /* Tp_Udhi */
    cmpp_pack_add_integer(&pack, 0, &offset, 1);
    
    /* Msg_Fmt */
    unsigned char mft = 0;

    if (strcasecmp(msgFmt, "ASCII") == 0) {
        mft = 0;
    } else if (strcasecmp(msgFmt, "UCS-2") == 0) {
        mft = 8;
    } else if (strcasecmp(msgFmt, "GBK") == 0) {
        mft  = 15;
    }

    cmpp_pack_add_integer(&pack, mft, &offset, 1);
    
    /* Msg_Src */
    cmpp_pack_add_string(&pack, msgSrc, strlen(msgSrc), &offset, 6);
    
    /* Fee_Type */
    cmpp_pack_add_string(&pack, "02", 2, &offset, 2);
    
    /* Fee_Code */
    cmpp_pack_add_string(&pack, "000000", 6, &offset, 6);

    /* ValId_Time */
    cmpp_pack_add_string(&pack, "0", 1, &offset, 17);
    
    /* At_Time */
    cmpp_pack_add_string(&pack, "0", 1, &offset, 17);
    
    /* Src_Id */
    cmpp_pack_add_string(&pack, serviceId, strlen(serviceId), &offset, 21);
    
    /* DestUsr_Tl */
    cmpp_pack_add_integer(&pack, 1, &offset, 1);
    
    /* Dest_Terminal_Id */
    cmpp_pack_add_string(&pack, phone, strlen(phone), &offset, 21);
    

    /* Msg_Length  */
    memset(buff, 0, sizeof(buff));
    err = cmpp_conv(message, strlen(message), (char *)buff, sizeof(buff), "UTF-8", msgFmt);
    if (err) {
        return 2;
    }

    msgLen = cmpp_ucs2count(buff);
    cmpp_pack_add_integer(&pack, msgLen, &offset, 1);
    
    /* Msg_Content */
    cmpp_pack_add_string(&pack, buff, msgLen, &offset, msgLen);

    /* Reserve  */
    cmpp_pack_add_string(&pack, "0", 1, &offset, 8);

    /* Total_Length */
    head->totalLength = htonl(offset);

    err = cmpp_send(sock, &pack, offset);
    if (err) {
        return (err == -1) ? err : 3;
    }

    return pack.sequenceId;
}

int cmpp_submit_resp(cmpp_sock_t *sock, int sequenceId, unsigned long long msgId, unsigned char result) {
    int err;
    size_t offset;
    cmpp_pack_t pack;
    cmpp_head_t *head;

    memset(&pack, 0, sizeof(pack));
    head = (cmpp_head_t *)&pack;
    err = cmpp_add_header(head, sizeof(cmpp_head_t), CMPP_SUBMIT_RESP, sequenceId);
    if (err) {
        return 1;
    }

    offset = sizeof(cmpp_head_t);

    /* Msg_Id */
    cmpp_pack_add_integer(&pack, msgId, &offset, 8);

    /* Result */
    cmpp_pack_add_integer(&pack, result, &offset, 1);

    /* Total_Length */
    head->totalLength = htonl(offset);

    err = cmpp_send(sock, &pack, offset);
    if (err) {
        return (err == -1) ? err : 2;
    }

    return 0;
}

int cmpp_deliver_resp(cmpp_sock_t *sock, unsigned long sequenceId, unsigned long long msgId, unsigned char result) {
    int err;
    cmpp_deliver_resp_t cdrp;

    memset(&cdrp, 0, sizeof(cdrp));
    err = cmpp_add_header((cmpp_head_t *)&cdrp, sizeof(cdrp), CMPP_DELIVER_RESP, sequenceId);
    if (err) {
        return 1;
    }

    cdrp.msgId = msgId;
    cdrp.result = result;

    err = cmpp_send(sock, &cdrp, sizeof(cdrp));
    if (err) {
        return (err == -1) ? err : 2;
    }

    return 0;
}

bool cmpp_check_authentication(cmpp_pack_t *pack, size_t size, const char *user, const char *password) {
    if (!pack || size < sizeof(cmpp_connect_t)) {
        return false;
    }

    cmpp_connect_t *ccp = (cmpp_connect_t *)pack;

    int len;
    char timestamp[11];
    unsigned char buff[128];
    unsigned char authenticatorSource[16];

    len = strlen(user) + 9 + strlen(password) + 10;
    if (len > sizeof(buff)) {
        return false;
    }
    
    memset(buff, 0, sizeof(buff));
    memcpy(buff, user, strlen(user));
    memcpy(buff + strlen(user) + 9, password, strlen(password));

    if (ntohl(ccp->timestamp) > 9999999999) {
        return false;
    }
    
    sprintf(timestamp, "%010u", ntohl(ccp->timestamp));
    memcpy(buff + strlen(user) + 9 + strlen(password), timestamp, 10);
    cmpp_md5(authenticatorSource, buff, len);

    if (memcmp(authenticatorSource, ccp->authenticatorSource, 16) != 0) {
        return false;
    }

    return true;
}

int cmpp_free_pack(cmpp_pack_t *pack) {
    if (pack == NULL) {
        return -1;
    }
    
    free(pack);
    pack = NULL;

    return 0;
}

bool cmpp_check_connect(cmpp_sock_t *sock) {
    if (!sock) {
        return false;
    }

    cmpp_pack_t pack;
    pthread_mutex_lock(&sock->wlock);
    pthread_mutex_lock(&sock->rlock);
    
    if (cmpp_active_test(sock) != 0) {
        return false;
    }

    if (cmpp_recv(sock, &pack, sizeof(cmpp_pack_t)) != 0) {
        return false;
    }

    pthread_mutex_unlock(&sock->wlock);
    pthread_mutex_unlock(&sock->rlock);
    
    if (!cmpp_check_method(&pack, sizeof(cmpp_pack_t), CMPP_ACTIVE_TEST_RESP)) {
        return false;
    }

    return true;
}

unsigned int cmpp_sequence(void) {
    static unsigned int seq = 1;
    return (seq < 0x7fffffff) ? (seq++) : (seq = 1);
}
