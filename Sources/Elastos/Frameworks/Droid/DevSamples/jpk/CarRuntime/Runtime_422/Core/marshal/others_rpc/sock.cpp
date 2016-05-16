
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>



ssize_t sock_send_all(int sfd, void const *buf, size_t len)
{
    int nrb;
    void const *p;

    nrb = len;
    p = buf;
    while (nrb > 0) {
        int nb;

        nb = send(sfd, p, nrb, 0);
        if (nb == -1)
            return -1;
        p += nb;
        nrb -= nb;
    }
    return len < 0 ? -1 : len;
}

ssize_t sock_recv_all(int sfd, void *buf, size_t len)
{
    int nrb;
    void *p;

    nrb = len;
    p = buf;
    while (nrb > 0) {
        int nb;

        nb = recv(sfd, p, nrb, 0);
	if (nb == 0)
	    return 0;
        if (nb == -1)
            return -1;
        p += nb;
        nrb -= nb;
    }
    return len < 0 ? -1 : len;
}

int sock_make_pack(int type,
        void const *buf, int len,
        void **rbuf, int *rlen)
{
    int32_t _type = type;
    int32_t lenn;
    void *pb;

    if (buf == NULL)
        len = 0;

    lenn = sizeof _type + len;

    *rlen = sizeof lenn + lenn;

    lenn = htonl(lenn);

    *rbuf = malloc(*rlen);
    if (*rbuf == NULL)
        return -1;

    pb = *rbuf;
    pb = mempcpy(pb, &lenn, sizeof lenn);
    pb = mempcpy(pb, &_type, sizeof _type);
    if (buf != NULL)
        memcpy(pb, buf, len);

    return 0;
}

int sock_send_msg(int sfd, int type, void const *buf, int len)
{
    int ret = 0;
    void *_buf;
    int _len;

    if (sock_make_pack(type, buf, len, &_buf, &_len))
        return -1;

    if (sock_send_all(sfd, _buf, _len) != _len)
        ret = -1;

    free(_buf);

    return ret;
}

int sock_recv_msg(int sfd, int *type, void **buf, int *len)
{
    int _type = -1;
    void *__buf = 0;
    int32_t __len = 0;
    ssize_t nb;
    int32_t _len;

    nb = sock_recv_all(sfd, &_len, sizeof _len);
    if (nb == 0)
	goto out;
    if (nb != sizeof _len)
	return -1;

    _len = ntohl(_len);

    char _buf[_len];

    nb = sock_recv_all(sfd, _buf, _len);
    if (nb == 0)
	goto out;
    if (nb != _len)
        return -1;

    __len = _len;

    void const *p;

    p = _buf;

    _type = *(int32_t *)p;
    p += 4;
    __len -= 4;

    __buf = malloc(__len);
    if (__buf == NULL)
        return -1;

    memcpy(__buf, p, __len);

out:
    *type = _type;
    *buf = __buf;
    *len = __len;

    return 0;
}

int sock_connect(char const *ip, int port)
{
    int sfd;
    struct sockaddr_in saddr;

    sfd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (sfd == -1)
        return -1;
    memset(&saddr, 0, sizeof saddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(ip);
    if (connect(sfd, (struct sockaddr const *)&saddr, sizeof saddr)) {
        close(sfd);
        return -1;
    }
    return sfd;
}

int sock_close(int sid)
{
    if (sid < 0)
        return 0;
    return close(sid);
}

