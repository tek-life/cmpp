
/* 
 * China Mobile CMPP 2.0 protocol library
 * Copyright (C) 2017 typefo <typefo@qq.com>
 * Update: 2017-07-10
 */

#ifndef _CMPP2_H
#define _CMPP2_H

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#define CMPP_VERSION 0x20
#define CMPP_PACK_MAX 4096

/* Cmpp Protocol Port */
#define CMPP_LONG_PORT  7890
#define CMPP_SHORT_PORT 7900

/* Cmpp Socket Option */
#define CMPP_SOCK_CONTIMEOUT  1
#define CMPP_SOCK_SENDTIMEOUT 2
#define CMPP_SOCK_RECVTIMEOUT 3

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

/* CMPP HEADER */
#define cmpp_total_length                           0
#define cmpp_command_id                             4
#define cmpp_sequence_id                            8

/* CMPP_CONNECT */
#define cmpp_connect_source_addr                    12
#define cmpp_connect_authenticator_source           18
#define cmpp_connect_version                        34
#define cmpp_connect_timestamp                      35

/* CMPP_CONNECT_RESP */
#define cmpp_connect_resp_status                    12
#define cmpp_connect_resp_authenticatorismg         13
#define cmpp_connect_resp_version                   29

/* CMPP_SUBMIT */
#define cmpp_submit_msg_id                          12
#define cmpp_submit_pk_total                        20
#define cmpp_submit_pk_number                       21
#define cmpp_submit_registered_delivery             22
#define cmpp_submit_msg_level                       23
#define cmpp_submit_service_id                      24
#define cmpp_submit_fee_usertype                    34
#define cmpp_submit_fee_terminal_id                 35
#define cmpp_submit_tp_pid                          56
#define cmpp_submit_tp_udhi                         57
#define cmpp_submit_msg_fmt                         58
#define cmpp_submit_msg_src                         59
#define cmpp_submit_feetype                         65
#define cmpp_submit_feecode                         67
#define cmpp_submit_valid_time                      73
#define cmpp_submit_at_time                         90
#define cmpp_submit_src_id                          107
#define cmpp_submit_destusr_tl                      128
#define cmpp_submit_dest_terminal_id                129
#define cmpp_submit_msg_length                      150
#define cmpp_submit_msg_content                     151

/* CMPP_SUBMIT_RESP */
#define cmpp_submit_resp_msg_id                     12
#define cmpp_submit_resp_result                     20

/* CMPP_DELIVER */
#define cmpp_deliver_msg_id                         12
#define cmpp_deliver_dest_id                        20
#define cmpp_deliver_service_id                     41
#define cmpp_deliver_tp_pid                         51
#define cmpp_deliver_tp_udhi                        52
#define cmpp_deliver_msg_fmt                        53
#define cmpp_deliver_src_terminal_id                54
#define cmpp_deliver_registered_delivery            75
#define cmpp_deliver_msg_length                     76
#define cmpp_deliver_msg_content                    77

/* CMPP_DELIVER_MSG_CONTENT */
#define cmpp_deliver_msg_content_msg_id             77
#define cmpp_deliver_msg_content_stat               85
#define cmpp_deliver_msg_content_submit_time        92
#define cmpp_deliver_msg_content_done_time          102
#define cmpp_deliver_msg_content_dest_terminal_id   112
#define cmpp_deliver_msg_content_smsc_sequence      133

/* CMPP_DELIVER_RESP */
#define cmpp_deliver_resp_msg_id                    12
#define cmpp_deliver_resp_result                    20

/* CMPP_ACTIVE_TEST_RESP */
#define cmpp_active_test_resp_reserved              12

/* CMPP_MSG_CONTENT */
typedef struct {
    unsigned long long msgId;
    unsigned char stat[7];
    unsigned char submitTime[10];
    unsigned char doneTime[10];
    unsigned char destTerminalId[21];
    unsigned int smscSequence;
} cmpp_msg_content_t;

/* Cmpp Packet Message Header */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
} cmpp_head_t;

typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
    unsigned char data[CMPP_PACK_MAX];
} cmpp_pack_t;

typedef struct {
    int fd;
    long long conTimeout;
    long long sendTimeout;
    long long recvTimeout;
    pthread_mutex_t rlock;
    pthread_mutex_t wlock;
} cmpp_sock_t;

/* Cmpp Session Handle */
typedef struct {
    bool ok;
    cmpp_sock_t sock;
    pthread_mutex_t lock;
    unsigned char version;
} cmpp_sp_t;

/* Cmpp Session Handle */
typedef struct {
    cmpp_sock_t sock;
    unsigned char version;
    pthread_mutex_t lock;
} cmpp_ismg_t;

extern int cmpp_init_sp(cmpp_sp_t *cmpp, char *host, unsigned short port);
extern int cmpp_init_ismg(cmpp_ismg_t *cmpp, const char *addr, unsigned short port);
extern int cmpp_connect(cmpp_sock_t *sock, unsigned int sequenceId, const char *user, const char *password);
extern int cmpp_connect_resp(cmpp_sock_t *sock, unsigned int sequenceId, unsigned char status);
extern int cmpp_terminate(cmpp_sock_t *sock, unsigned int sequenceId);
extern int cmpp_terminate_resp(cmpp_sock_t *sock, unsigned int sequenceId);
extern int cmpp_submit(cmpp_sock_t *sock, unsigned int sequenceId, char *spid, char *spcode, char *phone, char *content, int length, int msgFmt, bool delivery);
extern int cmpp_submit_resp(cmpp_sock_t *sock, unsigned int sequenceId, unsigned long long msgId, unsigned char result);
extern int cmpp_deliver(cmpp_sock_t *sock, unsigned int sequenceId, unsigned long long msgId, char *spcode, char *phone, char *content, int length, int msgFmt);
extern int cmpp_deliver_resp(cmpp_sock_t *sock, unsigned long sequenceId, unsigned long long msgId, unsigned char result);
extern int cmpp_report(cmpp_sock_t *sock, unsigned int sequenceId, unsigned long long msgId, char *destId, int stat, char *submitTime, char *doneTime,
                       char *destTerminalId, unsigned int smscSequence);
extern int cmpp_active_test(cmpp_sock_t *sock, unsigned int sequenceId);
extern int cmpp_active_test_resp(cmpp_sock_t *sock, unsigned int sequenceId);
extern bool cmpp_check_connect(cmpp_sock_t *sock);
extern int cmpp_sp_close(cmpp_sp_t *cmpp);
extern int cmpp_ismg_close(cmpp_ismg_t *cmpp);
extern unsigned int cmpp_sequence(void);
extern int cmpp_send(cmpp_sock_t *sock, void *pack, size_t len);
extern int cmpp_recv(cmpp_sock_t *sock, void *pack, size_t len);
extern int cmpp_send_timeout(cmpp_sock_t *sock, void *pack, size_t len, unsigned long long timeout);
extern int cmpp_recv_timeout(cmpp_sock_t *sock, void *pack, size_t len, unsigned long long timeout);
extern int cmpp_free_pack(cmpp_pack_t *pack);
extern bool cmpp_check_method(void *pack, size_t len, unsigned int command);
extern bool cmpp_check_authentication(cmpp_pack_t *pack, size_t size, const char *user, const char *password);
extern int cmpp_sock_setting(cmpp_sock_t *sock, int opt, long long val);
extern int cmpp_md5(unsigned char *md, unsigned char *src, unsigned int len);
extern unsigned int cmpp_sequence(void);
extern unsigned long long cmpp_gen_msgid(int mon, int day, int hour, int min, int sec, int gid, unsigned int seq);
extern void cmpp_pack_add_string(void *pack, char *val, size_t vallen, size_t *offset, size_t size);
extern void cmpp_pack_add_integer(void *pack, unsigned long long val, size_t *offset, size_t size);
extern void cmpp_pack_set_string(void *pack, size_t offset, char *val, size_t vallen, size_t size);
extern void cmpp_pack_set_integer(void *pack, size_t offset, unsigned long long val, size_t size);
extern void cmpp_pack_get_string(void *pack, size_t offset, char *val, size_t vallen, size_t len);
extern void cmpp_pack_get_integer(void *pack, size_t offset, void *val, size_t len);
extern int cmpp_sock_init(cmpp_sock_t *sock, int fd);
extern int cmpp_sock_setting(cmpp_sock_t *sock, int opt, long long val);
extern int cmpp_sock_create(void);
extern int cmpp_sock_bind(cmpp_sock_t *sock, const char *addr, unsigned short port, int backlog);
extern int cmpp_sock_connect(cmpp_sock_t *sock, const char *addr, unsigned short port);
extern int cmpp_sock_send(cmpp_sock_t *sock, unsigned char *buff, size_t len);
extern int cmpp_sock_recv(cmpp_sock_t *sock, unsigned char *buff, size_t len);
extern int cmpp_sock_nonblock(cmpp_sock_t *sock, bool enable);
extern int cmpp_sock_tcpnodelay(cmpp_sock_t *sock, bool enable);
extern int cmpp_sock_keepavlie(cmpp_sock_t *sock, int idle, int interval, int count);
extern int cmpp_sock_timeout(cmpp_sock_t *sock, int type, long long millisecond);
extern int cmpp_sock_readable(int fd, long long millisecond);
extern int cmpp_sock_writable(int fd, long long millisecond);
extern int cmpp_sock_close(cmpp_sock_t *sock);

#endif
