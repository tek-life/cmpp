
/* 
 * China Mobile CMPP 2.0 Protocol Library
 * By typefo <typefo@qq.com>
 * Update: 2017-05-18
 */

#ifndef _CMPP2_PACKET_H
#define _CMPP2_PACKET_H

#include "string.h"

/* CMPP_CONNECT Message Packet (SP -> ISMG) */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
    unsigned char sourceAddr[6];
    unsigned char authenticatorSource[16];
    unsigned char version;
    unsigned int timestamp;
} __attribute__((packed)) CMPP_CONNECT_T;


/* CMPP_CONNECT Message Packet (ISMG -> SP) */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
    unsigned char status;
    unsigned char authenticatorISMG[16];
    unsigned char version;
} __attribute__((packed)) CMPP_CONNECT_RESP_T;

/* CMPP_TERMINATE Message Packet (SP <=> ISMG) */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
} __attribute__((packed)) CMPP_TERMINATE_T;

/* CMPP_TERMINATE_RESP Message Packet (SP <=> ISMG) */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
} __attribute__((packed)) CMPP_TERMINATE_RESP_T;

/* CMPP_SUBMIT Message Packet (SP -> ISMG) */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
    unsigned char data[313];
} __attribute__((packed)) CMPP_SUBMIT_T;

/* CMPP_SUBMIT_RESP Message Packet (ISMG -> SP) */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
    unsigned long msgId;
    unsigned char result;
} __attribute__((packed)) CMPP_SUBMIT_RESP_T;

/* CMPP_DELIVER Message Packet (SP -> ISMG) */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
    unsigned long msgId;
    unsigned char destId[21];
    unsigned char serviceId[10];
    unsigned char tppId;
    unsigned char tpUdhi;
    unsigned char msgFmt;
    unsigned char srcTerminalId[32];
    unsigned char srcTerminalType;
    unsigned char registeredDelivery;
    unsigned char msgLength;
    unsigned char msgContent[160];
    unsigned char linkId[20];
} __attribute__((packed)) CMPP_DELIVER_T;


/* CMPP_DELIVER_RESP Message Packet (ISMG -> SP) */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
    unsigned long msgId;
    unsigned int result;
} __attribute__((packed)) CMPP_DELIVER_RESP_T;

/* CMPP_ACTIVE_TEST Message Packet (SP <=> ISMG) */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
} __attribute__((packed)) CMPP_ACTIVE_TEST_T;

/* CMPP_ACTIVE_TEST_RESP Message Packet (SP <=> ISMG) */
typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
    unsigned char reserved;
} __attribute__((packed)) CMPP_ACTIVE_TEST_RESP_T;

void cmpp_pack_add_string(void *pack, const char *data, size_t len, size_t *offset, size_t size);
void cmpp_pack_add_integer(void *pack, unsigned long int data, size_t *offset, size_t size);

#endif
