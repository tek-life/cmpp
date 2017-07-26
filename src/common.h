
/* 
 * China Mobile CMPP 2.0 protocol library
 * Copyright (C) 2017 typefo <typefo@qq.com>
 * Update: 2017-07-10
 */

#ifndef _CMPP_COMMON_H
#define _CMPP_COMMON_H

#include <stdbool.h>
#include "cmpp.h"

unsigned int gen_sequence(void);
int cmpp_send(cmpp_sp_t *cmpp, void *pack, size_t len);
int cmpp_recv(cmpp_sp_t *cmpp, void *pack, size_t len);
int cmpp_add_header(cmpp_head_t *chp, unsigned int totalLength, unsigned int commandId, unsigned int sequenceId);
bool is_cmpp_command(void *pack, size_t len, unsigned int command);
int cmpp_md5(unsigned char *md, unsigned char *src, unsigned int len);
int cmpp_conv(const char *src, size_t slen, char *dst, size_t dlen, const char* fromcode, const char* tocode);
size_t cmpp_ucs2count(const char *src);
const char *cmpp_get_error(cmpp_error_t code);

#endif
