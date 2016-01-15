
#include "DCARServer.h"
#include "DataWrapper.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cutils/log.h>

static const char* MULTICAST_ADDR = "224.0.0.1";

void doCommand(DataWrapper& data, DataWrapper& reply)
{
    String cmd;
    data.ReadString(&cmd);
    if (cmd.Equals("Call DCARServer")) {
        reply.WriteString(DCARServer::Self()->GetNetAddress());
        return;
    }

    ALOGD("cmd %s is illegal.", cmd.string());
}

int main(int argv, char** argc)
{
    if (FAILED(DCARServer::Self()->Start())) {
        return -1;
    }

    DCARServer::Self()->WaitForReady();

    int socketfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (socketfd < 0) {
        ALOGD("create socket failed");
        return -1;
    }

    int looped = 1;
    if (setsockopt(socketfd, IPPROTO_IP, IP_MULTICAST_LOOP, (void *)&looped, sizeof(looped)) == -1) {
        ALOGD("enable looping failed");
        return -1;
    }

    ip_mreq mreq;
    if (inet_pton(AF_INET, MULTICAST_ADDR, &mreq.imr_multiaddr) == 0) {
        ALOGD("invalid multicast address");
        return -1;
    }
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(socketfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&mreq, sizeof(mreq)) == -1) {
        ALOGD("add membershipt failed");
        return -1;
    }

    struct sockaddr_in name;
    name.sin_family = AF_INET;
    name.sin_addr = mreq.imr_interface;
    name.sin_port = htons(12321);
    if (bind(socketfd, (struct sockaddr*)&name, sizeof(name)) < 0) {
        ALOGD("bind socket failed\n");
        return -1;
    }

    Byte data[1024];
    Byte reply[1024];
    while (true) {
        struct sockaddr_in cname;
        socklen_t clen = sizeof(struct sockaddr_in);
        bzero(&cname, clen);
        int n = recvfrom(socketfd, (void*)data, sizeof(data), 0, (struct sockaddr *)&cname, &clen);
        if (n > 0) {
            data[n] = '\0';
            DataWrapper dataWrapper(data, n);
            DataWrapper replyWrapper(reply, 1024);
            doCommand(dataWrapper, replyWrapper);
            if ((sendto(socketfd, replyWrapper.Data(), replyWrapper.DataSize(), 0,
                (struct sockaddr *)&cname, sizeof(cname))) == -1) {
                ALOGD("send socket failed\n");
            }
        }
    }

    close(socketfd);
    return 0;
}
