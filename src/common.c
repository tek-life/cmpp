
/* 
 * China Mobile CMPP 2.0 Protocol Library
 * By typefo <typefo@qq.com>
 * Update: 2017-07-10
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
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

bool cmpp_send(CMPP_SP_T *cmpp, void *pack, size_t len) {
    int ret = 0;
    CMPP_HEAD_T *chp;

    chp = (CMPP_HEAD_T *)pack;

    if (ntohl(chp->totalLength) > len) {
    	return false;
    }

    ret = cmpp_sock_send(&cmpp->sock, (unsigned char *)pack, ntohl(chp->totalLength));

    if (ret != ntohl(chp->totalLength)) {
        return false;
    }

    return true;
}

bool cmpp_recv(CMPP_SP_T *cmpp, void *pack, size_t len) {
    int ret;
    int chpLen = 0;
    int pckLen = 0;
    CMPP_HEAD_T *chp;

    chpLen = sizeof(CMPP_HEAD_T);

    if (len < chpLen) {
    	return false;
    }

    ret = cmpp_sock_recv(&cmpp->sock, (unsigned char *)pack, chpLen);

    if (ret != chpLen) {
        return false;
    }

    chp = (CMPP_HEAD_T *)pack;
    pckLen = ntohl(chp->totalLength);
    
    ret = cmpp_sock_recv(&cmpp->sock, (unsigned char *)pack + chpLen, pckLen - chpLen);
    if (ret != (pckLen - chpLen)) {
        return false;
    }
    
    return true;
}

int cmpp_add_header(CMPP_HEAD_T *chp, unsigned int totalLength, unsigned int commandId, unsigned int sequenceId) {
    if (chp) {
        chp->totalLength = htonl(totalLength);
        chp->commandId = htonl(commandId);
        chp->sequenceId = htonl(sequenceId);
        return 0;
    }

    return -1;
}

bool is_cmpp_command(void *pack, size_t len, unsigned int command) {
    if (pack && len >= sizeof(CMPP_HEAD_T)) {
        CMPP_HEAD_T *chp = (CMPP_HEAD_T *)pack;
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

int cmpp_event_loop(CMPP_SP_T *cmpp) {
    int client;
    socklen_t socklen;
    struct sockaddr_in sockaddr;

    socklen = sizeof(sockaddr);

    while (true) {
        client = accept(cmpp->sock.fd, (struct sockaddr *)&sockaddr, &socklen);
        if (client < 0) {
          if (errno == EAGAIN || errno == EWOULDBLOCK) {
            sleep(3);
            continue;
          } else {
            printf("cmpp accept socket error!\n");
            break;
          }
        }

        printf("new a client accept successfull\n");
        sleep(1);
        close(client);
    }

    return 0;
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

/*
void cmpp_sleep(int milliseconds) {
    struct timespec ts;

    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = milliseconds % 1000 * 1000000;
    nanosleep(&ts, NULL);
    return;
}

*/
