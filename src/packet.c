
/* 
 * China Mobile CMPP 2.0 Protocol Library
 * Copyright (C) 2017 typefo <typefo@qq.com>
 * Update: 2017-05-22
 */

#include <stdio.h>
#include <string.h>
 
void cmpp_pack_add_string(void *pack, unsigned char *data, size_t len, size_t *offset, size_t size) {
    if (!pack) {
        return;
    }

    unsigned char *ptr = (unsigned char *)pack + *offset;

    size_t n = (len <= size) ? len : size;
    memcpy(ptr, data, n);
    *offset += size;

    return;
}

void cmpp_pack_add_integer(void *pack, unsigned long long val, size_t *offset, size_t size) {
    if (!pack) {
        return;
    }

    unsigned char *ptr = (unsigned char *)pack + *offset;

    switch (size) {
    case 1:
        *((unsigned char *)ptr) = (unsigned char)val;
        break;
    case 2:
        *((unsigned short *)ptr) = (unsigned short)val;
        break;
    case 4:
        *((unsigned int *)ptr) = (unsigned int)val;
        break;
    case 8:
        *((unsigned long long *)ptr) = (unsigned long int)val;
        break;
    default:
        break;
    }

    *offset += size;
    return;
}

void cmpp_pack_get_string(void *pack, size_t offset, unsigned char *buff, size_t size, size_t len) {
    if (!pack || size < (len + 1)) {
        return;
    }

    unsigned char *ptr = (unsigned char *)pack + offset;

    memcpy(buff, ptr, len);
    *(buff + len + 1) = '\0';

    return;
}

void cmpp_pack_get_integer(void *pack, size_t offset, void *val, size_t len) {
    if (!pack) {
        return;
    }

    unsigned char *ptr = (unsigned char *)pack + offset;

    switch (len) {
    case 1:
        *((unsigned char *)val) = *(unsigned char *)ptr;
        break;
    case 2:
        *((unsigned short *)val) = *(unsigned short *)ptr;
        break;
    case 4:
        *((unsigned int *)val) = *(unsigned int *)ptr;
        break;
    case 8:
        *((unsigned long long *)val) = *(unsigned long long *)ptr;
        break;
    default:
        break;
    }

    return;
}
