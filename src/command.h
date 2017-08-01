
/* 
 * China Mobile CMPP 2.0 protocol library
 * Copyright (C) 2017 typefo <typefo@qq.com>
 * Update: 2017-07-10
 */

#ifndef _CMPP_COMMAND_H
#define _CMPP_COMMAND_H

#include <stdbool.h>
#include "socket.h"
#include "utils.h"
#include "packet.h"

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


int cmpp_connect(cmpp_sock_t *sock, const char *user, const char *password);
int cmpp_connect_resp(cmpp_sock_t *sock, unsigned int sequenceId, unsigned char status);
int cmpp_terminate(cmpp_sock_t *sock);
int cmpp_terminate_resp(cmpp_sock_t *sock, unsigned int sequenceId);
int cmpp_submit(cmpp_sock_t *sock, unsigned int sequenceId, const char *phone, const char *message, bool delivery,
                char *serviceId, char *msgFmt, char *msgSrc);
int cmpp_submit_resp(cmpp_sock_t *sock, int sequenceId, unsigned long long msgId, unsigned char result);
int cmpp_deliver(cmpp_sock_t *sock);
int cmpp_deliver_resp(cmpp_sock_t *sock, unsigned long sequenceId, unsigned long long msgId, unsigned char result);
int cmpp_active_test(cmpp_sock_t *sock);
int cmpp_active_test_resp(cmpp_sock_t *sock, unsigned int sequenceId);
bool cmpp_check_connect(cmpp_sock_t *sock);
int cmpp_sp_close(cmpp_sp_t *cmpp);
int cmpp_ismg_close(cmpp_ismg_t *cmpp);
unsigned int cmpp_sequence(void);
int cmpp_send(cmpp_sock_t *sock, void *pack, size_t len);
int cmpp_recv(cmpp_sock_t *sock, void *pack, size_t len);
int cmpp_free_pack(cmpp_pack_t *pack);
bool cmpp_check_method(void *pack, size_t len, unsigned int command);
bool cmpp_check_authentication(cmpp_pack_t *pack, size_t size, const char *user, const char *password);


#endif
