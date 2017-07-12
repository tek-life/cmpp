
/* 
 * China Mobile CMPP 2.0 Protocol Library
 * By typefo <typefo@qq.com>
 * Update: 2017-05-18
 */

#ifndef _CMPP2_SOCKET_H
#define _CMPP2_SOCKET_H

#include <stdbool.h>
#include "cmpp.h"

#define CMPP_SOCK_SEND 1
#define CMPP_SOCK_RECV 2

int cmpp_sock_init(CMPP_SOCK_T *sock);
int cmpp_sock_setting(CMPP_SOCK_T *sock, int opt, long long val);
int cmpp_sock_create(void);
int cmpp_sock_bind(CMPP_SOCK_T *sock, const char *addr, unsigned short port, int backlog);
int cmpp_sock_connect(CMPP_SOCK_T *sock, const char *addr, unsigned short port);
int cmpp_sock_send(CMPP_SOCK_T *sock, unsigned char *buff, size_t len);
int cmpp_sock_recv(CMPP_SOCK_T *sock, unsigned char *buff, size_t len);
int cmpp_sock_nonblock(CMPP_SOCK_T *sock, bool enable);
int cmpp_sock_tcpnodelay(CMPP_SOCK_T *sock, bool enable);
int cmpp_sock_keepavlie(CMPP_SOCK_T *sock, int idle, int interval, int count);
int cmpp_sock_timeout(CMPP_SOCK_T *sock, int type, long long millisecond);
int cmpp_check_readable(int fd, long long millisecond);
int cmpp_sock_close(CMPP_SOCK_T *sock);
#endif
