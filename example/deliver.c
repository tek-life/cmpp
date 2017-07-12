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
    
    while (true) {
        CMPP_PACK_T pack;
        if (cmpp_recv(&cmpp, &pack, sizeof(pack)) != 0) {
            printf("receive deliver message error\n");
            continue;
        }

        if (is_cmpp_command(&pack, sizeof(pack), CMPP_DELIVER)) {
            CMPP_DELIVER_T *cdp = (CMPP_DELIVER_T)&pack;

            cmpp_pack_get_string(&pack, cmpp_deliver_src_terminal_id, buff, sizeof(buff), 21);

            cmpp_pack_get_integer(&pack, cmpp_deliver_msg_length, lenght, 21);
            
            printf("msgId: %lld\n", cdp->msgId);
            printf("destId: %s\n", cdp->destId);
            printf("serviceId: %s\n", cdp->serviceId);
            printf("msgId: %lld\n", cdp->msgId);
            printf("msgId: %lld\n", cdp->msgId);
            printf("msgId: %lld\n", cdp->msgId);
            printf("msgId: %lld\n", cdp->msgId);
            printf("msgId: %lld\n", cdp->msgId);
        }
        
    }
    
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
