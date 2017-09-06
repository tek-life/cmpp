
/* 
 * China Mobile CMPP 2.0 Protocol Library
 * Copyright (C) 2017 typefo <typefo@qq.com>
 * Update: 2017-05-22
 */

#include <stdio.h>
#include <string.h>
 
void cmpp_pack_add_string(void *pack, char *val, size_t vallen, size_t *offset, size_t size) {
    if (!pack) {
        return;
    }

    size_t n;
    unsigned char *ptr = (unsigned char *)pack + *offset;

    n = (vallen <= size) ? vallen : size;
    memcpy(ptr, val, n);
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

void cmpp_pack_set_string(void *pack, size_t offset, char *val, size_t vallen, size_t size) {
    if (!pack) {
        return;
    }

    size_t n;
    unsigned char *ptr = (unsigned char *)pack + offset;

    n = (vallen <= size) ? vallen : size;
    memcpy(ptr, val, n);

    return;
}

void cmpp_pack_set_integer(void *pack, size_t offset, unsigned long long val, size_t size) {
    if (!pack) {
        return;
    }

    unsigned char *ptr = (unsigned char *)pack + offset;

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

    return;
}

void cmpp_pack_get_string(void *pack, size_t offset, char *val, size_t vallen, size_t len) {
    if (!pack || vallen < (len + 1)) {
        return;
    }

    unsigned char *ptr = (unsigned char *)pack + offset;

    memcpy(val, ptr, len);
    *(val + len + 1) = '\0';

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
