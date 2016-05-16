#ifndef __SOCK_H
# define __SOCK_H



extern ssize_t sock_send_all(int sfd, void const *buf, size_t len);

extern ssize_t sock_recv_all(int sfd, void *buf, size_t len);

extern int sock_make_pack(int type,
        void const *buf, int len,
        void const **rbuf, int *rlen);

extern int sock_send_msg(int sfd,
        int type,
        void const *buf,
        int len);

extern int sock_recv_msg(int sfd,
        int *type,
        void **buf,
        int *len);

extern int sock_connect(char const *ip, int port);

extern int sock_close(int sid);

#endif
