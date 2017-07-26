
/* 
 * China Mobile CMPP 2.0 Protocol Library
 * Copyright (C) 2017 typefo <typefo@qq.com>
 * Update: 2017-07-10
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <openssl/md5.h>
#include <iconv.h>
#include "cmpp.h"
#include "packet.h"
#include "socket.h"

unsigned int gen_sequence(void) {
    static unsigned int seq = 1;
    return (seq < 0x7fffffff) ? (seq++) : (seq = 1);
}

int cmpp_send(cmpp_sp_t *cmpp, void *pack, size_t len) {
    int ret;
    cmpp_head_t *chp;

    chp = (cmpp_head_t *)pack;

    if (ntohl(chp->totalLength) > len) {
    	return CMPP_ERR_PROPACKLENERR;
    }

    ret = cmpp_sock_send(&cmpp->sock, (unsigned char *)pack, ntohl(chp->totalLength));

    if (ret != ntohl(chp->totalLength)) {
        return CMPP_ERR_PROPACKLENERR;
    }
    
    return 0;
}

int cmpp_recv(cmpp_sp_t *cmpp, void *pack, size_t len) {
    int ret;
    cmpp_head_t *chp;
    int chpLen, pckLen;

    chpLen = sizeof(cmpp_head_t);

    if (len < chpLen) {
    	return CMPP_ERR_PROPACKLENERR;
    }

    ret = cmpp_sock_recv(&cmpp->sock, (unsigned char *)pack, chpLen);

    if (ret != chpLen) {
        if (ret == 0) {
            return CMPP_ERR_NODATA;
        }
        return CMPP_ERR_PROPACKLENERR;
    }

    chp = (cmpp_head_t *)pack;
    pckLen = ntohl(chp->totalLength);
    
    ret = cmpp_sock_recv(&cmpp->sock, (unsigned char *)pack + chpLen, pckLen - chpLen);
    if (ret != (pckLen - chpLen)) {
        return CMPP_ERR_PROPACKLENERR;
    }
    
    return 0;
}

int cmpp_add_header(cmpp_head_t *chp, unsigned int totalLength, unsigned int commandId, unsigned int sequenceId) {
    if (!chp) {
        return -1;
    }

    chp->totalLength = htonl(totalLength);
    chp->commandId = htonl(commandId);
    chp->sequenceId = htonl(sequenceId);

    return 0;
}

bool is_cmpp_command(void *pack, size_t len, unsigned int command) {
    if (pack && len >= sizeof(cmpp_head_t)) {
        cmpp_head_t *chp = (cmpp_head_t *)pack;
        if (ntohl(chp->commandId) == command) {
            return true;
        }
    }

    return false;
}

int cmpp_md5(unsigned char *md, unsigned char *src, unsigned int len) {
    if (md && src && len > 0) {
        MD5_CTX ctx;
        MD5_Init(&ctx);
        MD5_Update(&ctx, src, len);
        MD5_Final(md, &ctx);
        return 0;
    }

    return -1;
}

int cmpp_conv(const char *src, size_t slen, char *dst, size_t dlen, const char* fromcode, const char* tocode) {
    iconv_t cd;
    char *inbuf = (char *)src;
    size_t *inbytesleft = &slen;
    char *outbuf = dst;
    size_t *outbytesleft = &dlen;
    
    cd = iconv_open(tocode, fromcode);
    if (cd != (iconv_t)-1) {
        iconv(cd, &inbuf, inbytesleft, &outbuf, outbytesleft);
        iconv_close(cd);
        return 0;
    }

    return -1;
}

size_t cmpp_ucs2count(const char *src) {
    int i = 0;

    while (i < 140) {
        if (src[i] + src[i + 1] != 0) {
            i += 2;
        } else {
            break;
        }
    }

    return i;
}

char *cmpp_get_error(cmpp_error_t code) {
    char *error = "unknown error";
    if ((code >= 0) && (code < (sizeof(cmpp_error_strings) / sizeof(char *)))) {
        error = cmpp_error_strings[code];
    }
    
    return error;
}
