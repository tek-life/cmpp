
/* 
 * China Mobile CMPP 2.0 Protocol Library
 * By typefo <typefo@qq.com>
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
#include "common.h"

int cmpp_sock_init(CMPP_SOCK_T *sock) {
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

int cmpp_sock_setting(CMPP_SOCK_T *sock, int conTimeout, int sendTimeout, int recvTimeout) {
    sock->conTimeout = conTimeout;
    sock->sendTimeout = sendTimeout;
    sock->recvTimeout = recvTimeout;

    return 0;
}

int cmpp_sock_bind(CMPP_SOCK_T *sock, const char *addr, unsigned short port, int backlog) {
    struct sockaddr_in servaddr;

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (inet_pton(AF_INET, addr, &servaddr.sin_addr) < 1) {
        return -1;
    }

    /* socket bind */
    if (bind(sock->fd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) == -1) {
        return -1;
    }

    /* socket listen */
    if (listen(sock->fd, backlog) == -1) {
        return -1;
    }

    return 0;
}

int cmpp_sock_connect(CMPP_SOCK_T *sock, const char *addr, unsigned short port) {
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (inet_pton(AF_INET, addr, &servaddr.sin_addr) < 1) {
        return -1;
    }

    /* Set connection timeout */
    cmpp_sock_timeout(sock, CMPP_SOCK_SEND, sock->conTimeout);

    if (connect(sock->fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        return -1;
    }

    return 0;
}

int cmpp_sock_send(CMPP_SOCK_T *sock, unsigned char *buff, size_t len) {
    int ret = 0;
    int offset = 0;

    pthread_mutex_lock(&sock->wlock);

    /* Start sending data */
    while (offset < len) {
        ret = write(sock->fd, buff + offset, len - offset);
        if (ret > 0) {
            offset += ret;
            continue;
        }

        break;
    }

    pthread_mutex_unlock(&sock->wlock);

    return offset;
}

int cmpp_sock_recv(CMPP_SOCK_T *sock, unsigned char *buff, size_t len) {
    int ret;
    int offset = 0;

    pthread_mutex_lock(&sock->rlock);

    /* Begin to receive data */
    while (offset < len) {
        if (cmpp_check_timeout(sock->fd, sock->recvTimeout) > 0) {
          ret = read(sock->fd, buff + offset, len - offset);
          if (ret > 0) {
            offset += ret;
            continue;
          }
        }

        break;
    }

    pthread_mutex_unlock(&sock->rlock);

    return offset;
}

int cmpp_sock_close(CMPP_SOCK_T *sock) {
    return close(sock->fd);
}

int cmpp_sock_nonblock(CMPP_SOCK_T *sock, bool enable) {
    int ret;
    int flag;

    flag = fcntl(sock->fd, F_GETFL, 0);

    if (flag == -1) {
        return -1;
    }

    if (enable) {
        flag |= O_NONBLOCK;
    } else {
        flag &= ~O_NONBLOCK;
    }

    ret = fcntl(sock->fd, F_SETFL, flag);

    if (ret == -1) {
        return -1;
    }

    return 0;
}

int cmpp_sock_tcpnodelay(CMPP_SOCK_T *sock, bool enable) {
    int ret;
    int flag;

    flag = enable ? 1 : 0;
    ret = setsockopt(sock->fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
    if (ret == -1) {
        return -1;
    }

    return 0;
}

int cmpp_sock_keepavlie(CMPP_SOCK_T *sock, int idle, int interval, int count) {
    int keepalive = 1;
    setsockopt(sock->fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof(keepalive));
    setsockopt(sock->fd, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&idle, sizeof(idle));
    setsockopt(sock->fd, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&interval, sizeof(interval));
    setsockopt(sock->fd, IPPROTO_TCP, TCP_KEEPCNT, (void *)&count, sizeof(count));

    return 0;
}

int cmpp_sock_timeout(CMPP_SOCK_T *sock, int type, long long millisecond) {
    struct timeval timeout;

    timeout.tv_sec = millisecond / 1000;
    timeout.tv_usec = (millisecond % 1000) * 1000;
    type = (type == CMPP_SOCK_SEND) ? SO_SNDTIMEO : SO_RCVTIMEO;

    if (setsockopt(sock->fd, SOL_SOCKET, type, (void *)&timeout, sizeof(timeout)) == -1) {
        return -1;
    }

    return 0;
}

static int cmpp_check_timeout(int fd, long long millisecond) {
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