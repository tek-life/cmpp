
/* 
 * China Mobile CMPP 2.0 protocol library
 * Copyright (C) 2017 typefo <typefo@qq.com>
 * Update: 2017-07-10
 */

#ifndef _CMPP_UTILS_H
#define _CMPP_UTILS_H

#include <stdbool.h>
#include "packet.h"
#include "socket.h"

#define CMPP_PACK_MAX 4096

typedef struct {
    unsigned int totalLength;
    unsigned int commandId;
    unsigned int sequenceId;
    unsigned char data[CMPP_PACK_MAX];
} cmpp_pack_t;

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

int cmpp_init_sp(cmpp_sp_t *cmpp, char *host, unsigned short port);
int cmpp_init_ismg(cmpp_ismg_t *cmpp, const char *addr, unsigned short port);
int cmpp_send(cmpp_sock_t *sock, void *pack, size_t len);
int cmpp_recv(cmpp_sock_t *sock, void *pack, size_t len);
int cmpp_add_header(cmpp_head_t *chp, unsigned int totalLength, unsigned int commandId, unsigned int sequenceId);
bool cmpp_check_method(void *pack, size_t len, unsigned int command);
bool cmpp_check_authentication(cmpp_pack_t *pack, size_t size, const char *user, const char *password);
int cmpp_md5(unsigned char *md, unsigned char *src, unsigned int len);
int cmpp_conv(const char *src, size_t slen, char *dst, size_t dlen, const char* fromcode, const char* tocode);
size_t cmpp_ucs2count(const char *src);
int cmpp_free_pack(cmpp_pack_t *pack);

#endif
