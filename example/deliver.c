#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "cmpp.h"

#define errlog(msg) printf("[error] %s:%d %s\n", __FILE__, __LINE__, (msg))

int main(void) {
    int err;
    CMPP_T cmpp;

    err = cmpp_init(&cmpp, "10.10.24.1", 7890);
    if (err) {
        cmpp.err ? errlog(cmpp.err) : printf("ERROR: cmpp init error\n");
        return 0;
    }
    printf("cmpp_init is successfull\n");

    err = cmpp_connect(&cmpp, "901234", "1234");
    if (err) {
        cmpp.err ? errlog(cmpp.err) : printf("ERROR: cmpp connect error\n");
        return 0;
    }

    ssize_t ret = 0;
    ssize_t offset = 0;
    unsigned char buff[4096];
    memset(buff, 0, sizeof(buff));
    
    
    while(1) {
        ret = read(cmpp.fd, buff + offset, 4096 - offset);
        if (ret < 1) {
            printf("read cmpp socket complete\n");
            break;
        }
        offset += ret;
    }

    CMPP_CONNECT_RESP_T *csrp;

    csrp = (CMPP_CONNECT_RESP_T *)buff;

    printf("==========================================\n");
    printf("Total_Length: %d\n", ntohl(csrp->totalLength));
    printf("Command_Id: %d\n", ntohl(csrp->commandId));
    printf("Sequence_Id: %d\n", ntohl(csrp->sequenceId));
    printf("Status: %d\n", ntohl(csrp->status));
    printf("AuthenticatorISMG: null\n");
    printf("Version: %d\n", ntohl(csrp->version));
    printf("===========================================\n");
    
    sleep(3);
    
    printf("send cmpp_terminate to cmpp server\n");
    cmpp_terminate(&cmpp);

    sleep(1);  
    printf("closing server connection\n");
    cmpp_close(&cmpp);
    return 0;
}
