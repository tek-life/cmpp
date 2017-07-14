#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <cmpp2.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage: %s <phone> <message>\n", argv[0]);
        return 0;
    }
    
    int err;
    CMPP_SP_T cmpp;

    signal(SIGPIPE, SIG_IGN);
    
    /* Cmpp Socket Initialization */
    err = cmpp_init_sp(&cmpp, "192.168.1.100", 7890);
    if (err) {
        printf("[error] %s\n", cmpp_get_error(cmpp.err));
        return 0;
    }

    printf("connect to server successfull\n");

    /* Cmpp Login */
    err = cmpp_connect(&cmpp, "901234", "123456");
    if (!cmpp.ok) {
        switch(err) {
        case -1:
            printf("[error] %s\n", cmpp_get_error(cmpp.err));
            break;
        case 1:
            printf("[error] protocol packet error\n");
            break;
        case 2:
            printf("[error] illegal source address\n");
            break;
        case 3:
            printf("[error] authentication failed\n");
            break;
        case 4:
            printf("[error] protocol version is too high\n");
            break;
        default:
            printf("[error] unknown error\n");
            break;
        }
        
        goto exit;
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
    char *msgSrc = "901234";

    /* Cmpp Send Message */
    err = cmpp_submit(&cmpp, phone, message, delivery, serviceId, msgFmt, msgSrc);
    if (err) {
        switch (err) {
        case -1:
            printf("[error] %s\n", cmpp_get_error(cmpp.err));
            break;
        case 1:
            printf("[error] protocol packet error\n");
            break;
        case 2:
            printf("[error] protocol command error\n");
            break;
        case 3:
            printf("[error] message numberrepeat\n");
            break;
        case 4:
            printf("[error] message length error\n");
            break;
        case 5:
            printf("[error] tariff code error\n");
            break;
        case 6:
            printf("[error] greater than maximum information length\n");
            break;
        case 7:
            printf("[error] service code error\n");
            break;
        case 8:
            printf("[error] flow control error\n");
            break;
        default:
            printf("[error] unknown error\n");
            break;
        }

        goto exit;
    }
    
    printf("send message cmpp_submit successfull\n");
    sleep(1);
    
    /* Cmpp Logout */
    printf("send cmpp_terminate to cmpp server\n");
    cmpp_terminate(&cmpp);

    sleep(1);

exit:
    /* Close Cmpp Socket Connect */
    printf("closing server connection\n");
    cmpp_close(&cmpp);
    return 0;
}
