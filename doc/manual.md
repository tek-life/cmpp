# China Mobile point to point SMS protocol library 

- int **cmpp_init_sp**(cmpp_sp_t *cmpp, char *host, unsigned short port);

  功能：创建并初始化一个 SP 客户端，然后以此连接到 ISMG 网关端

  **cmpp** : SP 客户端句柄

  **host** : ISMG 网关 IP 地址

  **port** : ISMG 网关端口

- int **cmpp_init_ismg**(cmpp_ismg_t *cmpp, const char *addr, unsigned short port);

- int **cmpp_connect**(cmpp_sock_t *sock, unsigned int sequenceId, const char *user, const char *password);

- int **cmpp_connect_resp**(cmpp_sock_t *sock, unsigned int sequenceId, unsigned char status);

- int **cmpp_terminate**(cmpp_sock_t *sock, unsigned int sequenceId);

- int **cmpp_terminate_resp**(cmpp_sock_t *sock, unsigned int sequenceId);

- int **cmpp_submit**(cmpp_sock_t *sock, unsigned int sequenceId, char *spid, char *spcode, char *phone, char *content, int length, int msgFmt, bool delivery);

- int **cmpp_submit_resp**(cmpp_sock_t *sock, unsigned int sequenceId, unsigned long long msgId, unsigned char result);

- int **cmpp_deliver**(cmpp_sock_t *sock, unsigned int sequenceId, unsigned long long msgId, char *spcode, char *phone, char *content, int length, int msgFmt);

- int **cmpp_deliver_resp**(cmpp_sock_t *sock, unsigned long sequenceId, unsigned long long msgId, unsigned char result);

- int **cmpp_report**(cmpp_sock_t *sock, unsigned int sequenceId, unsigned long long msgId, char *stat, char *submitTime, char *doneTime, char *destTerminalId, unsigned int smscSequence);

- int **cmpp_active_test**(cmpp_sock_t *sock, unsigned int sequenceId);

- int **cmpp_active_test_resp**(cmpp_sock_t *sock, unsigned int sequenceId);

- bool **cmpp_check_connect**(cmpp_sock_t *sock);

- int **cmpp_sp_close**(cmpp_sp_t *cmpp);

- int **cmpp_ismg_close**(cmpp_ismg_t *cmpp);

- unsigned int **cmpp_sequence**(void);

- int **cmpp_send**(cmpp_sock_t *sock, void *pack, size_t len);

- int **cmpp_recv**(cmpp_sock_t *sock, void *pack, size_t len);

- int **cmpp_send_timeout**(cmpp_sock_t *sock, void *pack, size_t len, unsigned long long timeout);

- int **cmpp_recv_timeout**(cmpp_sock_t *sock, void *pack, size_t len, unsigned long long timeout);

- int **cmpp_free_pack**(cmpp_pack_t *pack);

- bool **cmpp_check_method**(void *pack, size_t len, unsigned int command);

- bool **cmpp_check_authentication**(cmpp_pack_t *pack, size_t size, const char *user, const char *password);

- int **cmpp_sock_setting**(cmpp_sock_t *sock, int opt, long long val);

- int **cmpp_md5**(unsigned char *md, unsigned char *src, unsigned int len);

- unsigned int **cmpp_sequence**(void);

- unsigned long long **cmpp_gen_msgid**(int mon, int day, int hour, int min, int sec, int gid, unsigned int seq);

- void **cmpp_pack_add_string**(void *pack, char *val, size_t vallen, size_t *offset, size_t size);

- void **cmpp_pack_add_integer**(void *pack, unsigned long long val, size_t *offset, size_t size);

- void **cmpp_pack_set_string**(void *pack, size_t offset, char *val, size_t vallen, size_t size);

- void **cmpp_pack_set_integer**(void *pack, size_t offset, unsigned long long val, size_t size);

- void **cmpp_pack_get_string**(void *pack, size_t offset, char *val, size_t vallen, size_t len);

- void **cmpp_pack_get_integer**(void *pack, size_t offset, void *val, size_t len);

- int **cmpp_sock_init**(cmpp_sock_t *sock, int fd);

- int **cmpp_sock_setting**(cmpp_sock_t *sock, int opt, long long val);

- int **cmpp_sock_create**(void);

- int **cmpp_sock_bind**(cmpp_sock_t *sock, const char *addr, unsigned short port, int backlog);

- int **cmpp_sock_connect**(cmpp_sock_t *sock, const char *addr, unsigned short port);

- int **cmpp_sock_send**(cmpp_sock_t *sock, unsigned char *buff, size_t len);

- int **cmpp_sock_recv**(cmpp_sock_t *sock, unsigned char *buff, size_t len);

- int **cmpp_sock_nonblock**(cmpp_sock_t *sock, bool enable);

- int **cmpp_sock_tcpnodelay**(cmpp_sock_t *sock, bool enable);

- int **cmpp_sock_keepavlie**(cmpp_sock_t *sock, int idle, int interval, int count);

- int **cmpp_sock_timeout**(cmpp_sock_t *sock, int type, long long millisecond);

- int **cmpp_sock_readable**(int fd, long long millisecond);

- int **cmpp_sock_writable**(int fd, long long millisecond);

- int **cmpp_sock_close**(cmpp_sock_t *sock);