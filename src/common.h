
/* 
 * China Mobile CMPP 2.0 protocol library
 * By typefo <typefo@qq.com>
 * Update: 2017-07-10
 */

#ifndef _CMPP2_COMMON_H
#define _CMPP2_COMMON_H

#include <stdbool.h>
#include "cmpp.h"
#include "packet.h"

unsigned int gen_sequence(void);
int cmpp_send(CMPP_SP_T *cmpp, void *pack, size_t len);
int cmpp_recv(CMPP_SP_T *cmpp, void *pack, size_t len);
int cmpp_add_header(CMPP_HEAD_T *chp, unsigned int totalLength, unsigned int commandId, unsigned int sequenceId);
bool is_cmpp_command(void *pack, size_t len, unsigned int command);
int cmpp_md5(unsigned char *md, unsigned char *src, unsigned int len);
int cmpp_conv(const char *src, size_t slen, char *dst, size_t dlen, const char* fromcode, const char* tocode);
size_t cmpp_ucs2count(const char *src);
void cmpp_sleep(unsigned long long milliseconds);
char *cmpp_get_error(unsigned int code);

#endif
