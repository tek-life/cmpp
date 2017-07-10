
/* 
 * China Mobile CMPP 2.0 protocol library
 * By typefo <typefo@qq.com>
 * Update: 2017-07-10
 */

#ifndef _CMPP_COMMON_H
#define _CMPP_COMMON_H

#include <stdbool.h>
#include "cmpp.h"
#include "packet.h"

unsigned int gen_sequence(void);
bool cmpp_send(CMPP_T *cmpp, void *pack, size_t len);
bool cmpp_recv(CMPP_T *cmpp, void *pack, size_t len);
int cmpp_head(CMPP_HEAD_T *chp, unsigned int tl, unsigned int ci, unsigned int si);
bool is_cmpp_command(void *pack, size_t len, unsigned int command);
int cmpp_md5(unsigned char *md, unsigned char *src, unsigned int len);
int cmpp_conv(const char *src, size_t slen, char *dst, size_t dlen, const char* fromcode, const char* tocode);
int cmpp_event_loop(CMPP_T *cmpp);
size_t cmpp_ucs2count(const char *src);
/* void cmpp_sleep(int milliseconds); */

#endif
