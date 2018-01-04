
/* 
 * China Mobile CMPP 2.0 Protocol Library
 * Copyright (C) 2017 typefo <typefo@qq.com>
 * Update: 2017-05-06
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include "socket.h"

int cmpp_sock_init(cmpp_sock_t *sock, int fd) {
    sock->fd = fd;
    sock->conTimeout = 3000;
    sock->sendTimeout =5000;
    sock->recvTimeout =5000;
    pthread_mutex_init(&sock->rlock, NULL);
    pthread_mutex_init(&sock->wlock, NULL);
    return 0;
}

int cmpp_sock_create(void) {
    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return -1;
    }

    return fd;
}

int cmpp_sock_setting(cmpp_sock_t *sock, int opt, long long val) {
    switch (opt) {
    case CMPP_SOCK_CONTIMEOUT:
        sock->conTimeout = val;
        break;
    case CMPP_SOCK_SENDTIMEOUT:
        sock->sendTimeout = val;
        break;
    case CMPP_SOCK_RECVTIMEOUT:
        sock->recvTimeout = val;
        break;
    }

    return 0;
}

int cmpp_sock_bind(cmpp_sock_t *sock, const char *addr, unsigned short port, int backlog) {
    struct sockaddr_in servaddr;

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (inet_pton(AF_INET, addr, &servaddr.sin_addr) < 1) {
        return 1;
    }

    /* socket bind */
    if (bind(sock->fd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) == -1) {
        return 2;
    }

    /* socket listen */
    if (listen(sock->fd, backlog) == -1) {
        return 3;
    }

    return 0;
}

int cmpp_sock_connect(cmpp_sock_t *sock, const char *addr, unsigned short port) {
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (inet_pton(AF_INET, addr, &servaddr.sin_addr) < 1) {
        return 1;
    }

    /* Set connection timeout */
    cmpp_sock_timeout(sock, CMPP_SOCK_SEND, sock->conTimeout);

    if (connect(sock->fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        return 2;
    }

    return 0;
}

int cmpp_sock_send(cmpp_sock_t *sock, unsigned char *buff, size_t len) {
    int ret;
    int offset = 0;

    pthread_mutex_lock(&sock->wlock);

    /* Start sending data */
    while (offset < len) {
        if (cmpp_sock_writable(sock->fd, sock->sendTimeout) > 0) {
            ret = write(sock->fd, buff + offset, len - offset);
            if (ret > 0) {
                offset += ret;
                continue;
            } else {
                if (ret == 0) {
                    offset = -1;
                    break;
                }

                if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {
                    continue;
                }
                break;
            }
        } else {
            offset = -1;
            break;
        }
    }

    pthread_mutex_unlock(&sock->wlock);

    return offset;
}

int cmpp_sock_recv(cmpp_sock_t *sock, unsigned char *buff, size_t len) {
    int ret;
    int offset = 0;

    pthread_mutex_lock(&sock->rlock);

    /* Begin to receive data */
    while (offset < len) {
        if (cmpp_sock_readable(sock->fd, sock->recvTimeout) > 0) {
            ret = read(sock->fd, buff + offset, len - offset);
            if (ret > 0) {
                offset += ret;
                continue;
            } else {
                if (ret == 0) {
                    return -1;
                }

                if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {
                    continue;
                }
                
                break;
            }
        } else {
            offset = -1;
            break;
        }
    }

    pthread_mutex_unlock(&sock->rlock);

    return offset;
}

int cmpp_sock_close(cmpp_sock_t *sock) {
    close(sock->fd);
    pthread_mutex_destroy(&sock->rlock);
    pthread_mutex_destroy(&sock->wlock);
    return 0;
}

int cmpp_sock_nonblock(cmpp_sock_t *sock, bool enable) {
    int ret, flag;

    flag = fcntl(sock->fd, F_GETFL, 0);

    if (flag == -1) {
        return 1;
    }

    if (enable) {
        flag |= O_NONBLOCK;
    } else {
        flag &= ~O_NONBLOCK;
    }

    ret = fcntl(sock->fd, F_SETFL, flag);

    if (ret == -1) {
        return 2;
    }

    return 0;
}

int cmpp_sock_tcpnodelay(cmpp_sock_t *sock, bool enable) {
    int ret, flag;

    flag = enable ? 1 : 0;
    ret = setsockopt(sock->fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
    if (ret == -1) {
        return -1;
    }

    return 0;
}

int cmpp_sock_keepavlie(cmpp_sock_t *sock, int idle, int interval, int count) {
    int keepalive = 1;
    setsockopt(sock->fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof(keepalive));
    setsockopt(sock->fd, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&idle, sizeof(idle));
    setsockopt(sock->fd, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&interval, sizeof(interval));
    setsockopt(sock->fd, IPPROTO_TCP, TCP_KEEPCNT, (void *)&count, sizeof(count));

    return 0;
}

int cmpp_sock_timeout(cmpp_sock_t *sock, int type, long long millisecond) {
    struct timeval timeout;

    timeout.tv_sec = millisecond / 1000;
    timeout.tv_usec = (millisecond % 1000) * 1000;
    type = (type == CMPP_SOCK_SEND) ? SO_SNDTIMEO : SO_RCVTIMEO;

    if (setsockopt(sock->fd, SOL_SOCKET, type, (void *)&timeout, sizeof(timeout)) == -1) {
        return -1;
    }

    return 0;
}

int cmpp_sock_readable(int fd, long long millisecond) {
    int ret;
    fd_set rset;
    struct timeval timeout;

    timeout.tv_sec = millisecond / 1000;
    timeout.tv_usec = (millisecond % 1000) * 1000;

    FD_ZERO(&rset);
    FD_SET(fd, &rset);
    ret = select(fd + 1, &rset, NULL, NULL, &timeout);

    return ret;
}

int cmpp_sock_writable(int fd, long long millisecond) {
    int ret;
    fd_set wset;
    struct timeval timeout;

    timeout.tv_sec = millisecond / 1000;
    timeout.tv_usec = (millisecond % 1000) * 1000;

    FD_ZERO(&wset);
    FD_SET(fd, &wset);
    ret = select(fd + 1, NULL, &wset, NULL, &timeout);

    return ret;
}
