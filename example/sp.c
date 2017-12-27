#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include "cmpp.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage: %s <phone> <message>\n", argv[0]);
        return 0;
    }
    
    int err;
    cmpp_sp_t cmpp;
    cmpp_pack_t pack;
    unsigned int sequenceId;
    
    signal(SIGPIPE, SIG_IGN);

    char *host = "112.90.92.219";
    int port = 7990;
    char *user = "81101";
    char *password = "81101";

    /* Message Receive Number */
    char *phone = argv[1];

    /* Message Content */
    char *message = argv[2];

    /* Delivery Report */
    bool delivery = false;

    /* Service Code */
    char *spcode = "10657307975611";

    /* Message Character Encoding */
    int msgFmt = 11;

    /* Enterprise Number */
    char *spid = user;


    /* Cmpp Socket Initialization */
    err = cmpp_init_sp(&cmpp, host, port);
    if (err) {
        printf("can't connect to %s server\n", host);
        return -1;
    }

    printf("connect to %s server successfull\n", host);

    /* Cmpp Login */
    sequenceId = cmpp_sequence();
    err = cmpp_connect(&cmpp.sock, sequenceId, user, password);
    if (err) {
        fprintf(stderr, "send cmpp_connect error\n");
        goto exit;
    }

    /* check login status */
    err = cmpp_recv(&cmpp.sock, &pack, sizeof(pack));
    if (err) {
        if (err == -1) {
            printf("close connection be from server\n");
            return -1;
        }

        printf("cmpp cmpp_recv() failed\n");
        return -1;
    }

    if (cmpp_check_method(&pack, sizeof(pack), CMPP_CONNECT_RESP)) {
        unsigned char status;
        cmpp_pack_get_integer(&pack, cmpp_connect_resp_status, &status, 1);
        switch (status) {
        case 0:
            printf("cmpp login successfull\n");
            break;
        case 1:
            printf("Incorrect protocol packets\n");
            return -1;
        case 2:
            printf("Illegal source address\n");
            return -1;
        case 3:
            printf("Authenticator failed\n");
            return -1;
        case 4:
            printf("The protocol version is too high\n");
            return -1;
        default:
            printf("Unknown error\n");
            return -1;
        }
    } else {
        printf("The server response packet cannot be resolved\n");
        return -1;
    }
    
    sleep(1);

    /* Cmpp Send Message */
    sequenceId = cmpp_sequence();
    err = cmpp_submit(&cmpp.sock, sequenceId, spid, spcode, phone, message, strlen(message), msgFmt, delivery);
    if (err) {
        fprintf(stderr, "cmpp cmpp_submit error\n");
        goto exit;
    }
    //Todo: should check the cmpp_submit_resp here to check whether the submit is success.
    printf("send message cmpp_submit successfull\n");
    sleep(1);
    
    /* Cmpp Logout */
    printf("send cmpp_terminate to cmpp server\n");
    sequenceId = cmpp_sequence();
    cmpp_terminate(&cmpp.sock, sequenceId);

    sleep(1);

exit:
    /* Close Cmpp Socket Connect */
    printf("closing server connection\n");
    cmpp_sp_close(&cmpp);
    return 0;
}
