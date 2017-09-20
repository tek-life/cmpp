
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
#include "command.h"
#include "utils.h"
#include "packet.h"
#include "socket.h"

int cmpp_connect(cmpp_sock_t *sock, unsigned int sequenceId, const char *user, const char *password) {
    int err;
    cmpp_connect_t ccp;
    memset(&ccp, 0, sizeof(ccp));
    
    /* Header */
    err = cmpp_add_header((cmpp_head_t *)&ccp, sizeof(ccp), CMPP_CONNECT, sequenceId);
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

int cmpp_active_test(cmpp_sock_t *sock, unsigned int sequenceId) {
    int err;
    cmpp_active_test_t catp;
    memset(&catp, 0, sizeof(catp));
    err = cmpp_add_header((cmpp_head_t *)&catp, sizeof(catp), CMPP_ACTIVE_TEST, sequenceId);
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

    err = cmpp_send(sock, &catrp, sizeof(catrp));
    if (err) {
        return (err == -1) ? err : 2;
    }

    return 0;
}

int cmpp_terminate(cmpp_sock_t *sock, unsigned int sequenceId) {
    int err;
    cmpp_terminate_t ctp;

    memset(&ctp, 0, sizeof(ctp));
    err = cmpp_add_header((cmpp_head_t *)&ctp, sizeof(ctp), CMPP_TERMINATE, sequenceId);
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

int cmpp_submit(cmpp_sock_t *sock, unsigned int sequenceId, char *spid, char *spcode, char *phone, char *content, int length, int msgFmt, bool delivery) {
    int err;
    cmpp_head_t *head;
    cmpp_pack_t pack;
    size_t offset;
    
    memset(&pack, 0, sizeof(pack));
    head = (cmpp_head_t *)&pack;
    err = cmpp_add_header(head, sizeof(cmpp_head_t), CMPP_SUBMIT, sequenceId);
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
    cmpp_pack_add_string(&pack, spcode, strlen(spcode), &offset, 10);
    
    /* Fee_User_Type */
    cmpp_pack_add_integer(&pack, 0, &offset, 1);
    
    /* FeeTerminal_Id */
    cmpp_pack_add_string(&pack, phone, strlen(phone), &offset, 21);
    
    /* Tpp_Id */
    cmpp_pack_add_integer(&pack, 0, &offset, 1);
    
    /* Tp_Udhi */
    cmpp_pack_add_integer(&pack, 0, &offset, 1);
    
    /* Msg_Fmt */
    cmpp_pack_add_integer(&pack, msgFmt, &offset, 1);
    
    /* Msg_Src */
    cmpp_pack_add_string(&pack, spid, strlen(spid), &offset, 6);
    
    /* Fee_Type */
    cmpp_pack_add_string(&pack, "02", 2, &offset, 2);
    
    /* Fee_Code */
    cmpp_pack_add_string(&pack, "000000", 6, &offset, 6);

    /* ValId_Time */
    cmpp_pack_add_string(&pack, "0", 1, &offset, 17);
    
    /* At_Time */
    cmpp_pack_add_string(&pack, "0", 1, &offset, 17);
    
    /* Src_Id */
    cmpp_pack_add_string(&pack, spcode, strlen(spcode), &offset, 21);
    
    /* DestUsr_Tl */
    cmpp_pack_add_integer(&pack, 1, &offset, 1);
    
    /* Dest_Terminal_Id */
    cmpp_pack_add_string(&pack, phone, strlen(phone), &offset, 21);

    /* Msg_Length  */
    cmpp_pack_add_integer(&pack, length < 160 ? length : 159, &offset, 1);
    
    /* Msg_Content */
    cmpp_pack_add_string(&pack, content, length, &offset, length);

    /* Reserve  */
    cmpp_pack_add_string(&pack, "0", 1, &offset, 8);

    /* Total_Length */
    head->totalLength = htonl(offset);

    err = cmpp_send(sock, &pack, offset);
    if (err) {
        return (err == -1) ? err : 4;
    }

    return 0;
}

int cmpp_submit_resp(cmpp_sock_t *sock, unsigned int sequenceId, unsigned long long msgId, unsigned char result) {
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

int cmpp_deliver(cmpp_sock_t *sock, unsigned int sequenceId, unsigned long long msgId, char *spcode, char *phone, char *content, int length, int msgFmt) {
    int err;
    size_t offset;
    cmpp_pack_t pack;
    cmpp_head_t *head;

    
    memset(&pack, 0, sizeof(pack));
    head = (cmpp_head_t *)&pack;
    err = cmpp_add_header(head, sizeof(cmpp_head_t), CMPP_DELIVER, sequenceId);
    if (err) {
        return 1;
    }

    offset = sizeof(cmpp_head_t);
    
    /* Msg_Id */
    cmpp_pack_add_integer(&pack, msgId, &offset, 8);
    
    /* Dest_Id */
    cmpp_pack_add_string(&pack, spcode, strlen(spcode), &offset, 21);
    
    /* Service_Id */
    cmpp_pack_add_string(&pack, "0000000000", 10, &offset, 10);
    
    /* TP_pid */
    cmpp_pack_add_integer(&pack, 0, &offset, 1);

    /* TP_udhi */
    cmpp_pack_add_integer(&pack, 0, &offset, 1);
    
    /* Msg_Fmt */
    cmpp_pack_add_integer(&pack, msgFmt, &offset, 1);
    
    /* Src_terminal_Id */
    cmpp_pack_add_string(&pack, phone, strlen(phone), &offset, 21);
    
    /* Registered_Delivery */
    cmpp_pack_add_integer(&pack, 0, &offset, 1);
    
    /* Msg_Length  */
    cmpp_pack_add_integer(&pack, length < 160 ? length : 159, &offset, 1);

    /* Msg_Content */
    cmpp_pack_add_string(&pack, content, length, &offset, length);

    /* Reserve  */
    cmpp_pack_add_string(&pack, "0", 1, &offset, 8);

    /* Total_Length */
    head->totalLength = htonl(offset);

    err = cmpp_send(sock, &pack, offset);
    if (err) {
        return (err == -1) ? err : 3;
    }
    
    return 0;
}

int cmpp_deliver_resp(cmpp_sock_t *sock, unsigned int sequenceId, unsigned long long msgId, unsigned char result) {
    int err;
    size_t offset;
    cmpp_pack_t pack;
    cmpp_head_t *head;
    
    memset(&pack, 0, sizeof(pack));
    head = (cmpp_head_t *)&pack;
    err = cmpp_add_header((cmpp_head_t *)&pack, sizeof(pack), CMPP_DELIVER_RESP, sequenceId);
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


int cmpp_report(cmpp_sock_t *sock, unsigned int sequenceId, unsigned long long msgId, char *destId, int stat, char *submitTime, char *doneTime,
                char *destTerminalId, unsigned int smscSequence) {

    int err;
    cmpp_pack_t pack;
    cmpp_head_t *head;
    size_t offset;
    
    memset(&pack, 0, sizeof(pack));
    head = (cmpp_head_t *)&pack;
    err = cmpp_add_header(head, sizeof(cmpp_head_t), CMPP_DELIVER, sequenceId);
    if (err) {
        return 1;
    }

    offset = sizeof(cmpp_head_t);
    
    /* Msg_Id */
    cmpp_pack_add_integer(&pack, msgId, &offset, 8);
    
    /* Dest_Id */
    cmpp_pack_add_string(&pack, destId, strlen(destId), &offset, 21);
    
    /* Service_Id */
    cmpp_pack_add_string(&pack, "0000000000", 10, &offset, 10);
    
    /* TP_pid */
    cmpp_pack_add_integer(&pack, 0, &offset, 1);

    /* TP_udhi */
    cmpp_pack_add_integer(&pack, 0, &offset, 1);

    /* Msg_Fmt */
    cmpp_pack_add_integer(&pack, 0, &offset, 1);
    
    /* Src_terminal_Id */
    cmpp_pack_add_string(&pack, destTerminalId, strlen(destTerminalId), &offset, 21);
    
    /* Registered_Delivery */
    cmpp_pack_add_integer(&pack, 1, &offset, 1);
    
    /* Msg_Length */
    cmpp_pack_add_integer(&pack, 60, &offset, 1);

    /* Msg_Content -> Msg_Id */
    cmpp_pack_add_integer(&pack, msgId, &offset, 8);
    
    /* Msg_Content -> Stat */
    char *status = "UNKNOWN";

    switch (stat) {
        case 1:
            status = "DELIVRD";
            break;
        case 2:
            status = "EXPIRED";
            break;
        case 3:
            status = "DELETED";
            break;
        case 4:
            status = "UNDELIV";
            break;
        case 5:
            status = "ACCEPTD";
            break;
        case 6:
            status = "UNKNOWN";
            break;
        case 7:
            status = "REJECTD";
            break;
    }

    cmpp_pack_add_string(&pack, status, 7, &offset, 7);
    
    /* Msg_Content -> Submit_time */
    cmpp_pack_add_string(&pack, submitTime, 10, &offset, 10);
    
    /* Msg_Content -> Done_time */
    cmpp_pack_add_string(&pack, doneTime, 10, &offset, 10);
    
    /* Msg_Content -> Dest_terminal_Id */
    cmpp_pack_add_string(&pack, destTerminalId, 21, &offset, 21);
    
    /* Msg_Content -> SMSC_sequence */
    cmpp_pack_add_integer(&pack, smscSequence, &offset, 4);

    /* Reserve  */
    cmpp_pack_add_string(&pack, "0", 1, &offset, 8);

    /* Total_Length */
    head->totalLength = htonl(offset);

    err = cmpp_send(sock, &pack, offset);
    if (err) {
        return (err == -1) ? err : 2;
    }
    
    return 0;
}
