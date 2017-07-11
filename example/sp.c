#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "cmpp.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage: %s <phone> <message>\n", argv[0]);
        return 0;
    }
    
    int err;
    CMPP_SP_T cmpp;

    /* Cmpp Socket Initialization */
    err = cmpp_init_sp(&cmpp, "192.168.1.100", 7890);
    if (err && cmpp.err) {
        printf("[error] %s\n", cmpp.err);
        return 0;
    }

    printf("connect to server successfull\n");

    /* Cmpp Login */
    cmpp_connect(&cmpp, "901234", "123456");
    if (!cmpp.ok) {
        printf("[error] %s\n", cmpp.err);
        return 0;
    }

    printf("cmpp connect successfull\n");
    
    sleep(1);

    /* Message Receive Number */
    char *phone = argv[1];

    /* Message Content */
    char *message = argv[2];

    /* Delivery Report */
    bool delivery = false;

    /* Service Code */
    char *serviceId = "1065860008";

    /* Message Character Encoding */
    char *msgFmt = "UCS-2";

    /* Enterprise Number */
    char *msgSrc = "725668";

    /* Cmpp Send Message */
    err = cmpp_submit(&cmpp, phone, message, delivery, serviceId, msgFmt, msgSrc);
    if (err) {
        printf("[error] %s\n", cmpp.err);
        return 0;
    }
    printf("send message cmpp_submit successfull\n");
    sleep(1);
    
    /* Cmpp Logout */
    printf("send cmpp_terminate to cmpp server\n");
    cmpp_terminate(&cmpp);

    sleep(1);

    /* Close Cmpp Socket Connect */
    printf("closing server connection\n");
    cmpp_close(&cmpp);
    return 0;
}
