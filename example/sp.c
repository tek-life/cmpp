#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <cmpp.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage: %s <phone> <message>\n", argv[0]);
        return 0;
    }
    
    int err;
    cmpp_sp_t cmpp;

    signal(SIGPIPE, SIG_IGN);
    
    /* Cmpp Socket Initialization */
    err = cmpp_init_sp(&cmpp, "192.168.1.100", 7890);
    if (err) {
        printf("[error] %s\n", cmpp_get_error(err));
        return 0;
    }

    printf("connect to server successfull\n");

    /* Cmpp Login */
    err = cmpp_connect(&cmpp.sock, "901234", "123456");
    if (err) {
        fprintf(stderr, "%s\n", cmpp_get_error(err));
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
    err = cmpp_submit(&cmpp.sock, phone, message, delivery, serviceId, msgFmt, msgSrc);
    if (err) {
        fprintf(stderr, "%s\n", cmpp_get_error(err));
        goto exit;
    }
    
    printf("send message cmpp_submit successfull\n");
    sleep(1);
    
    /* Cmpp Logout */
    printf("send cmpp_terminate to cmpp server\n");
    cmpp_terminate(&cmpp.sock);

    sleep(1);

exit:
    /* Close Cmpp Socket Connect */
    printf("closing server connection\n");
    cmpp_sp_close(&cmpp);
    return 0;
}
