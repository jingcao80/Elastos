
#include <sys/socket.h>
#include <sys/un.h>
#include <cutils/log.h>

#define LOG_TAG "EthCli"

int main(int argc, char** argv)
{
    int socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        ALOGE("socket() failed!");
        return 1;
    }

    struct sockaddr_un address;

    /* start with a clean address structure */
    memset(&address, 0, sizeof(struct sockaddr_un));

    address.sun_family = AF_UNIX;
    snprintf(address.sun_path, UNIX_PATH_MAX, "/dev/socket/netd");

    if (connect(socket_fd,
            (struct sockaddr *) &address,
            sizeof(struct sockaddr_un)) != 0) {
        ALOGE("connect() failed!");
        return 1;
    }

    char buffer[256];

    int nbytes = snprintf(buffer, 256, "hello from a client");
    write(socket_fd, buffer, nbytes);

    nbytes = read(socket_fd, buffer, 256);
    buffer[nbytes] = 0;

    ALOGD("MESSAGE FROM SERVER: %s", buffer);

    close(socket_fd);

    return 0;
}
