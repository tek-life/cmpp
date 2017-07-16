
/* 
 * China Mobile CMPP 2.0 Protocol Library
 * By typefo <typefo@qq.com>
 * Update: 2017-05-18
 */

#ifndef _CMPP_SOCKET_H
#define _CMPP_SOCKET_H

#include <stdbool.h>

typedef struct {
    int fd;
    long long conTimeout;
    long long sendTimeout;
    long long recvTimeout;
    pthread_mutex_t rlock;
    pthread_mutex_t wlock;
} cmpp_sock_t;

#define CMPP_SOCK_SEND 1
#define CMPP_SOCK_RECV 2

/* Cmpp Socket Option */
#define CMPP_SOCK_CONTIMEOUT  1
#define CMPP_SOCK_SENDTIMEOUT 2
#define CMPP_SOCK_RECVTIMEOUT 3

int cmpp_sock_init(cmpp_sock_t *sock);
int cmpp_sock_setting(cmpp_sock_t *sock, int opt, long long val);
int cmpp_sock_create(void);
int cmpp_sock_bind(cmpp_sock_t *sock, const char *addr, unsigned short port, int backlog);
int cmpp_sock_connect(cmpp_sock_t *sock, const char *addr, unsigned short port);
int cmpp_sock_send(cmpp_sock_t *sock, unsigned char *buff, size_t len);
int cmpp_sock_recv(cmpp_sock_t *sock, unsigned char *buff, size_t len);
int cmpp_sock_nonblock(cmpp_sock_t *sock, bool enable);
int cmpp_sock_tcpnodelay(cmpp_sock_t *sock, bool enable);
int cmpp_sock_keepavlie(cmpp_sock_t *sock, int idle, int interval, int count);
int cmpp_sock_timeout(cmpp_sock_t *sock, int type, long long millisecond);
int cmpp_sock_readable(int fd, long long millisecond);
int cmpp_sock_writable(int fd, long long millisecond);
int cmpp_sock_close(cmpp_sock_t *sock);
#endif
