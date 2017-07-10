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
    CMPP_T cmpp;
    
    /* Cmpp Socket Initialization */
    err = cmpp_init(&cmpp, "192.168.1.100", 7890, CMPP_SP);
    if (err && cmpp.err) {
        printf("[error] %s\n", cmpp.err);
        return 0;
    }
    
    printf("connnect to server successfull\n");

    /* Cmpp Login */
    cmpp_connect(&cmpp, "901234", "12345678");
    if (!cmpp.ok) {
        printf("[error] %s\n", cmpp.err);
        return 0;
    }

    printf("cmpp login successfull\n");
    
    sleep(1);

    /* Phone Number */
    char *phone = argv[1];

    /* Message Content */
    char *message = argv[2];

    /* Status Report */
    bool delivery = false;

    /* Service Id */
    char *serviceId = "0000000000";

    /* Message Charset */
    char *msgFmt = "UCS-2";

    /* Enterprise ID */
    char *msgSrc = "000000";

    /* Enterprise ID */
    char *srcId = "000000";

    /* Cmpp Send Message */
    err = cmpp_submit(&cmpp, phone, message, delivery, serviceId, msgFmt, msgSrc, srcId);
    if (err && cmpp.err) {
        printf("[error] %s\n", cmpp.err);
        return 0;
    }
        
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
