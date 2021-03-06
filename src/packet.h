
/* 
 * China Mobile CMPP 2.0 Protocol Library
 * Copyright (C) 2017 typefo <typefo@qq.com>
 * Update: 2017-05-18
 */

#ifndef _CMPP_PACKET_H
#define _CMPP_PACKET_H

#include <string.h>

#pragma pack(1)

/* Cmpp Packet Message Header */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
} cmpp_head_t;

/* CMPP_CONNECT Message Packet (SP -> ISMG) */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
    unsigned char sourceAddr[6];
    unsigned char authenticatorSource[16];
    unsigned char version;
    unsigned int timestamp;
} cmpp_connect_t;

/* CMPP_CONNECT Message Packet (ISMG -> SP) */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
    unsigned char status;
    unsigned char authenticatorISMG[16];
    unsigned char version;
} cmpp_connect_resp_t;

/* CMPP_TERMINATE Message Packet (SP <=> ISMG) */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
} cmpp_terminate_t;

/* CMPP_TERMINATE_RESP Message Packet (SP <=> ISMG) */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
} cmpp_terminate_resp_t;

/* CMPP_DELIVER_RESP Message Packet (ISMG -> SP) */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
    unsigned long long msgId;
    unsigned char result;
} cmpp_deliver_resp_t;

/* CMPP_ACTIVE_TEST Message Packet (SP <=> ISMG) */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
} cmpp_active_test_t;

/* CMPP_ACTIVE_TEST_RESP Message Packet (SP <=> ISMG) */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
    unsigned char reserved;
} cmpp_active_test_resp_t;

/* CMPP_MSG_CONTENT */
typedef struct {
    unsigned long long msgId;
    unsigned char stat[7];
    unsigned char submitTime[10];
    unsigned char doneTime[10];
    unsigned char destTerminalId[21];
    unsigned int smscSequence;
} cmpp_msg_content_t;
    
#pragma pack()

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

void cmpp_pack_add_string(void *pack, char *val, size_t vallen, size_t *offset, size_t size);
void cmpp_pack_add_integer(void *pack, unsigned long long val, size_t *offset, size_t size);
void cmpp_pack_set_string(void *pack, size_t offset, char *val, size_t vallen, size_t size);
void cmpp_pack_set_integer(void *pack, size_t offset, unsigned long long val, size_t size);
void cmpp_pack_get_string(void *pack, size_t offset, char *val, size_t vallen, size_t len);
void cmpp_pack_get_integer(void *pack, size_t offset, void *val, size_t len);

#endif
