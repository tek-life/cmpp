
/* 
 * China Mobile CMPP 2.0 protocol library
 * Copyright (C) 2017 typefo <typefo@qq.com>
 * Update: 2017-07-10
 */

#ifndef _CMPP_COMMAND_H
#define _CMPP_COMMAND_H

#include <stdbool.h>
#include "socket.h"

#define CMPP_VERSION 0x20

/* Cmpp Protocol Command ID */
#define CMPP_CONNECT                   0x00000001
#define CMPP_CONNECT_RESP              0x80000001
#define CMPP_TERMINATE                 0x00000002
#define CMPP_TERMINATE_RESP            0x80000002
#define CMPP_SUBMIT                    0x00000004
#define CMPP_SUBMIT_RESP               0x80000004
#define CMPP_DELIVER                   0x00000005
#define CMPP_DELIVER_RESP              0x80000005
#define CMPP_QUERY                     0x00000006
#define CMPP_QUERY_RESP                0x80000006
#define CMPP_CANCEL                    0x00000007
#define CMPP_CANCEL_RESP               0x80000007
#define CMPP_ACTIVE_TEST               0x00000008
#define CMPP_ACTIVE_TEST_RESP          0x80000008
#define CMPP_FWD                       0x00000009
#define CMPP_FWD_RESP                  0x80000009
#define CMPP_MT_ROUTE                  0x00000010
#define CMPP_MT_ROUTE_RESP             0x80000010
#define CMPP_MO_ROUTE                  0x00000011
#define CMPP_MO_ROUTE_RESP             0x80000011
#define CMPP_GET_ROUTE                 0x00000012
#define CMPP_GET_ROUTE_RESP            0x80000012
#define CMPP_MT_ROUTE_UPDATE           0x00000013
#define CMPP_MT_ROUTE_UPDATE_RESP      0x80000013
#define CMPP_MO_ROUTE_UPDATE           0x00000014
#define CMPP_MO_ROUTE_UPDATE_RESP      0x80000014
#define CMPP_PUSH_MT_ROUTE_UPDATE      0x00000015
#define CMPP_PUSH_MT_ROUTE_UPDATE_RESP 0x80000015
#define CMPP_PUSH_MO_ROUTE_UPDATE      0x00000016
#define CMPP_PUSH_MO_ROUTE_UPDATE_RESP 0x80000016

int cmpp_connect(cmpp_sock_t *sock, unsigned int sequenceId, const char *user, const char *password);
int cmpp_connect_resp(cmpp_sock_t *sock, unsigned int sequenceId, unsigned char status);
int cmpp_terminate(cmpp_sock_t *sock, unsigned int sequenceId);
int cmpp_terminate_resp(cmpp_sock_t *sock, unsigned int sequenceId);
int cmpp_submit(cmpp_sock_t *sock, unsigned int sequenceId, char *spid, char *spcode, char *phone, char *content, int length, int msgFmt, bool delivery);
int cmpp_submit_resp(cmpp_sock_t *sock, unsigned int sequenceId, unsigned long long msgId, unsigned char result);
int cmpp_deliver(cmpp_sock_t *sock, unsigned int sequenceId, unsigned long long msgId, char *spcode, char *phone, char *content, int length, int msgFmt);
int cmpp_deliver_resp(cmpp_sock_t *sock, unsigned int sequenceId, unsigned long long msgId, unsigned char result);
int cmpp_active_test(cmpp_sock_t *sock, unsigned int sequenceId);
int cmpp_active_test_resp(cmpp_sock_t *sock, unsigned int sequenceId);
int cmpp_report(cmpp_sock_t *sock, unsigned int sequenceId, unsigned long long msgId, char *destId, int stat, char *submitTime, char *doneTime,
                char *destTerminalId, unsigned int smscSequence);
#endif
