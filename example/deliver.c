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
            cmpp_sleep(1000);
            continue;
        }

        if (is_cmpp_command(&pack, sizeof(pack), CMPP_DELIVER)) {
            unsigned long long msg_id;
            cmpp_pack_get_integer(&pack, 12, &msg_id, 8);
            printf("msg_id: %lld\n", msg_id);

            unsigned char dest_id[22];
            memset(dest_id, 0, sizeof(dest_id));
            cmpp_pack_get_string(&pack, 20, dest_id, sizeof(dest_id), 21);
            printf("dest_id: %s\n", dest_id);
            
            unsigned char service_id[11];
            memset(service_id, 0, sizeof(service_id));
            cmpp_pack_get_string(&pack, 41, service_id, sizeof(service_id), 10);
            printf("service_id: %s\n", service_id);

            unsigned char tp_pid;
            cmpp_pack_get_integer(&pack, 51, &tp_pid, 1);
            printf("tp_pid: %d\n", tp_pid);

            unsigned char tp_udhi;
            cmpp_pack_get_integer(&pack, 52, &tp_udhi, 1);
            printf("tp_udhi: %d\n", tp_udhi);

            unsigned char msg_fmt;
            cmpp_pack_get_integer(&pack, 53, &msg_fmt, 1);
            printf("msg_fmt: %d\n", msg_fmt);

            unsigned char src_terminal_id[22];
            memset(src_terminal_id, 0, sizeof(src_terminal_id));
            cmpp_pack_get_string(&pack, 54, src_terminal_id, sizeof(src_terminal_id), 21);
            printf("src_terminal_id: %s\n", src_terminal_id);

            unsigned char registered_delivery;
            cmpp_pack_get_integer(&pack, 75, &registered_delivery, 1);
            printf("registered_delivery: %d\n", registered_delivery);

            unsigned char msg_length;
            cmpp_pack_get_integer(&pack, 76, &msg_length, 1);
            printf("msg_length: %d\n", msg_length);

            char buff[161];
            unsigned char msg_content[161];
            memset(buff, 0, sizeof(buff));
            memset(msg_content, 0, sizeof(msg_content));
            cmpp_pack_get_string(&pack, 77, (unsigned char *)buff, sizeof(buff), (size_t)msg_length);
            cmpp_conv(buff, strlen(buff), (char *)msg_content, sizeof(msg_content), "UCS-2", "UTF-8");
            printf("msg_content: %s\n", msg_content);

            unsigned char reserved[9];
            memset(reserved, 0, sizeof(reserved));
            cmpp_pack_get_string(&pack, 77 + msg_length, reserved, sizeof(reserved), 8);
            printf("reserved: %s\n", reserved);
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
