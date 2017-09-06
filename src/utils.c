
/* 
 * China Mobile CMPP 2.0 Protocol Library
 * Copyright (C) 2017 typefo <typefo@qq.com>
 * Update: 2017-07-10
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <openssl/md5.h>
#include <iconv.h>
#include "packet.h"
#include "socket.h"
#include "command.h"
#include "utils.h"

int cmpp_init_sp(cmpp_sp_t *cmpp, char *host, unsigned short port) {
    if (!cmpp) {
        return -1;
    }

    int fd, err;
    cmpp->ok = false;
    cmpp->version = CMPP_VERSION;
    pthread_mutex_init(&cmpp->lock, NULL);

    /* Create a new socket */
    fd = cmpp_sock_create();
    if (fd < 1) {
        return 1;
    }

    /* Initialize the socket settings */
    cmpp_sock_init(&cmpp->sock, fd);

    /* Connect to server */
    err = cmpp_sock_connect(&cmpp->sock, host, port);
    if (err) {
        return 2;
    }

    /* TCP NONBLOCK */
    cmpp_sock_nonblock(&cmpp->sock, true);

    /* TCP NODELAY */
    cmpp_sock_tcpnodelay(&cmpp->sock, true);

    return 0;
}

int cmpp_init_ismg(cmpp_ismg_t *cmpp, const char *addr, unsigned short port) {
    if (!cmpp) {
        return -1;
    }

    int fd, err;
    cmpp->version = CMPP_VERSION;
    pthread_mutex_init(&cmpp->lock, NULL);

    /* Create a new socket */
    fd = cmpp_sock_create();
    if (fd < 1) {
        return 1;
    }
    
    /* Initialize the socket settings */
    cmpp_sock_init(&cmpp->sock, fd);

    /* bind to address */
    err = cmpp_sock_bind(&cmpp->sock, addr, port, 1024);
    if (err) {
        return 2;
    }

    /* TCP NONBLOCK */
    cmpp_sock_nonblock(&cmpp->sock, true);

    /* TCP NODELAY */
    cmpp_sock_tcpnodelay(&cmpp->sock, true);

    /* TCP KEEPAVLIE */
    cmpp_sock_keepavlie(&cmpp->sock, 30, 5, 3);

    return 0;
}

int cmpp_sp_close(cmpp_sp_t *cmpp) {
    if (cmpp) {
        cmpp->ok = false;
        cmpp_sock_close(&cmpp->sock);
        return 0;
    }

    return -1;
}

int cmpp_ismg_close(cmpp_ismg_t *cmpp) {
    if (cmpp) {
        cmpp_sock_close(&cmpp->sock);
        return 0;
    }

    return -1;
}

int cmpp_send(cmpp_sock_t *sock, void *pack, size_t len) {
    int ret;
    cmpp_head_t *chp;

    chp = (cmpp_head_t *)pack;

    if (ntohl(chp->totalLength) > len) {
    	return 1;
    }

    ret = cmpp_sock_send(sock, (unsigned char *)pack, ntohl(chp->totalLength));

    if (ret != ntohl(chp->totalLength)) {
        switch (ret) {
        case -1:
            return -1;
        default:
            return 2;
        }
    }
    
    return 0;
}

int cmpp_send_timeout(cmpp_sock_t *sock, void *pack, size_t len, unsigned long long timeout) {
    cmpp_sock_setting(sock, CMPP_SOCK_SENDTIMEOUT, timeout);
    return cmpp_send(sock, pack, len);
}

int cmpp_recv(cmpp_sock_t *sock, void *pack, size_t len) {
    int ret;
    cmpp_head_t *chp;
    int chpLen, pckLen;

    chpLen = sizeof(cmpp_head_t);
    
    if (len < chpLen) {
    	return 1;
    }

    ret = cmpp_sock_recv(sock, (unsigned char *)pack, chpLen);

    if (ret != chpLen) {
        switch (ret) {
        case -1:
            return -1;
        case 0:
            return 2;
        default:
            return 3;
        }
    }

    chp = (cmpp_head_t *)pack;
    pckLen = ntohl(chp->totalLength);
    
    if (pckLen > len) {
        return 4;
    }

    ret = cmpp_sock_recv(sock, (unsigned char *)pack + chpLen, pckLen - chpLen);
    if (ret != (pckLen - chpLen)) {
        switch (ret) {
        case -1:
            return -1;
        default:
            return 5;
        }
    }

    return 0;
}

int cmpp_recv_timeout(cmpp_sock_t *sock, void *pack, size_t len, unsigned long long timeout) {
    cmpp_sock_setting(sock, CMPP_SOCK_RECVTIMEOUT, timeout);
    return cmpp_recv(sock, pack, len);
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

bool cmpp_check_method(void *pack, size_t len, unsigned int command) {
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

size_t cmpp_ucs2_strlen(const char *str) {
    int i = 0;

    while (i < 140) {
        if ((str[i] + str[i + 1]) != 0) {
            i += 2;
        } else {
            break;
        }
    }

    return i;
}

size_t cmpp_gbk_strlen(const char *str) {  
    const char *ptr = str;
    while (*ptr) {
        if ((*ptr < 0) && (*(ptr + 1) < 0 || *(ptr + 1) > 63)) {
            str++;
            ptr += 2;
        } else {
            ptr++;  
        }  
    }

    return (size_t)(ptr - str);
}

int cmpp_msg_content(cmpp_msg_content_t *pack, size_t len, unsigned long long msgId, unsigned char *stat,
                     unsigned char *submitTime, unsigned char *doneTime, unsigned char *destTerminalId,
                     unsigned int smscSequence) {

    pack->msgId = msgId;
    memcpy(pack->stat, stat, 7);
    memcpy(pack->submitTime, submitTime, 10);
    memcpy(pack->doneTime, submitTime, 10);
    memcpy(pack->destTerminalId, destTerminalId, 21);
    pack->smscSequence = smscSequence;

    return 0;
}

bool cmpp_check_authentication(cmpp_pack_t *pack, size_t size, const char *user, const char *password) {
    if (!pack || size < sizeof(cmpp_connect_t)) {
        return false;
    }

    cmpp_connect_t *ccp = (cmpp_connect_t *)pack;

    int len;
    char timestamp[11];
    unsigned char buff[128];
    unsigned char authenticatorSource[16];

    len = strlen(user) + 9 + strlen(password) + 10;
    if (len > sizeof(buff)) {
        return false;
    }
    
    memset(buff, 0, sizeof(buff));
    memcpy(buff, user, strlen(user));
    memcpy(buff + strlen(user) + 9, password, strlen(password));

    if (ntohl(ccp->timestamp) > 9999999999) {
        return false;
    }
    
    sprintf(timestamp, "%010u", ntohl(ccp->timestamp));
    memcpy(buff + strlen(user) + 9 + strlen(password), timestamp, 10);
    cmpp_md5(authenticatorSource, buff, len);

    if (memcmp(authenticatorSource, ccp->authenticatorSource, 16) != 0) {
        return false;
    }

    return true;
}

int cmpp_free_pack(cmpp_pack_t *pack) {
    if (pack == NULL) {
        return -1;
    }
    
    free(pack);
    pack = NULL;

    return 0;
}

bool cmpp_check_connect(cmpp_sock_t *sock) {
    if (!sock) {
        return false;
    }

    cmpp_pack_t pack;
    unsigned int sequenceId;
    unsigned int responseId;

    pthread_mutex_lock(&sock->wlock);
    pthread_mutex_lock(&sock->rlock);

    sequenceId = cmpp_sequence();
    if (cmpp_active_test(sock, sequenceId) != 0) {
        return false;
    }

    if (cmpp_recv(sock, &pack, sizeof(cmpp_pack_t)) != 0) {
        return false;
    }

    pthread_mutex_unlock(&sock->wlock);
    pthread_mutex_unlock(&sock->rlock);
    
    if (!cmpp_check_method(&pack, sizeof(pack), CMPP_ACTIVE_TEST_RESP)) {
        return false;
    }

    cmpp_pack_get_integer(&pack, cmpp_sequence_id, &responseId, 4);
    if (sequenceId != responseId) {
        return false;
    }

    return true;
}

unsigned int cmpp_sequence(void) {
    static unsigned int seq = 1;
    return (seq < 0x7fffffff) ? (seq++) : (seq = 1);
}

unsigned long long cmpp_gen_msgid(int mon, int day, int hour, int min, int sec, int gid, unsigned int seq) {
    unsigned long long x = 0ULL;

    x |= ((unsigned long long)mon) << 60;
    x |= ((unsigned long long)day) << 55;
    x |= ((unsigned long long)hour) << 50;
    x |= ((unsigned long long)min) << 44;
    x |= ((unsigned long long)sec) << 38;
    x |= ((unsigned long long)gid) << 16;
    x |= (unsigned long long)seq;

    return x;
}
