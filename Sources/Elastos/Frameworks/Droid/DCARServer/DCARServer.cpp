
#include "DCARServer.h"
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <cutils/log.h>

AutoPtr<DCARServer> DCARServer::sServer;

DCARServer::DCARServer()
    : mIsReady(FALSE)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
    pthread_mutex_init(&mWaitMutex, &attr);
    pthread_mutexattr_destroy(&attr);

    pthread_cond_init(&mWaitCond, NULL);
}

DCARServer::~DCARServer()
{
    HashMap<String, InterfacePack*>::Iterator it = mServices.Begin();
    while (it != mServices.End()) {
        if (it->mSecond != NULL) {
            ((InterfacePack *)it->mSecond)->m_sNetAddress = NULL;
            free(it->mSecond);
        }
        ++it;
    }
    pthread_mutex_destroy(&mWaitMutex);
    pthread_cond_destroy(&mWaitCond);
}

AutoPtr<DCARServer> DCARServer::Self()
{
    if (sServer == NULL) {
        sServer = new DCARServer();
    }
    return sServer;
}

void* DCARServer::ThreadEntry(void* arg)
{
    DCARServer* server = (DCARServer*)arg;

    Int32 socketfd = socket(PF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        ALOGD("create socket failed\n");
        return (void*)E_FAIL;
    }

    struct sockaddr_in name;
    name.sin_family = AF_INET;
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    name.sin_port = htons(0);
    if (bind(socketfd, (struct sockaddr*)&name, sizeof(name)) < 0) {
        ALOGD("bind socket failed\n");
        close(socketfd);
        return (void*)E_FAIL;
    }

    if (listen(socketfd, 10) == -1) {
        ALOGD("listen socket failed\n");
        close(socketfd);
        return (void*)E_FAIL;
    }

    // get net address of this stub
    struct ifreq ifr_ip;
    memset(&ifr_ip, 0, sizeof(ifr_ip));
    strncpy(ifr_ip.ifr_name, "eth0", sizeof(ifr_ip.ifr_name) - 1);

    if (ioctl(socketfd, SIOCGIFADDR, &ifr_ip) < 0) {
        ALOGD("get ip address failed\n");
        close(socketfd);
        return (void*)E_FAIL;
    }
    struct sockaddr_in* addr = (struct sockaddr_in*)&ifr_ip.ifr_addr;
    server->mNetAddress = inet_ntoa(addr->sin_addr);

    struct sockaddr_in realAddr;
    socklen_t socklen = sizeof(realAddr);
    if (getsockname(socketfd, (struct sockaddr*)&realAddr, &socklen) == -1) {
        ALOGD("getsocketname failed\n");
        close(socketfd);
        return (void*)E_FAIL;
    }
    server->mNetAddress.AppendFormat(":%d", ntohs(realAddr.sin_port));

    pthread_mutex_lock(&server->mWaitMutex);
    server->mIsReady = TRUE;
    pthread_cond_broadcast(&server->mWaitCond);
    pthread_mutex_unlock(&server->mWaitMutex);

    Byte data[1024];
    Byte reply[1024];
    while(TRUE) {
        struct sockaddr_in cname;
        socklen_t clen = sizeof(struct sockaddr_in);
        bzero(&cname, clen);

        Int32 connfd = accept(socketfd, (struct sockaddr*)&cname, &clen);
        if (connfd < 0) {
            ALOGD("accept socket failed: %s(errno: %d)", strerror(errno), errno);
            continue;
        }

        Int32 n = recv(connfd, data, 1024, 0);
        if (n > 0) {
            data[n] = '\0';
            DataWrapper dataWrapper(data, n);
            DataWrapper replyWrapper(reply, 1024);
            server->DoCommand(dataWrapper, replyWrapper);
            if (send(connfd, replyWrapper.Data(), replyWrapper.DataSize(), 0) == -1) {
                ALOGD("send socket failed: %s(errno: %d)", strerror(errno), errno);
            }
        }
        close(connfd);
    }
    close(socketfd);
    return (void*)0;
}

ECode DCARServer::Start()
{
    pthread_attr_t threadAddr;
    pthread_attr_init(&threadAddr);
    pthread_attr_setdetachstate(&threadAddr, PTHREAD_CREATE_DETACHED);

    pthread_t thread;
    int ret = pthread_create(&thread, &threadAddr, DCARServer::ThreadEntry, (void*)this);
    if (ret != 0) {
        return E_FAIL;
    }

    return NOERROR;
}

String DCARServer::GetNetAddress()
{
    return mNetAddress;
}

void DCARServer::WaitForReady()
{
    pthread_mutex_lock(&mWaitMutex);
    while (!mIsReady) {
        pthread_cond_wait(&mWaitCond, &mWaitMutex);
    }
    pthread_mutex_unlock(&mWaitMutex);
}

ECode DCARServer::AddService(
    /* [in] */ const String& name,
    /* [in] */ InterfacePack* ip)
{
    HashMap<String, InterfacePack*>::Iterator it = mServices.Find(name);
    if (it != mServices.End()) {
        if (it->mSecond != NULL) {
            it->mSecond->m_sNetAddress = NULL;
            free(it->mSecond);
        }
    }
    mServices[name] = ip;
    return NOERROR;
}

ECode DCARServer::GetService(
    /* [in] */ const String& name,
    /* [out] */ InterfacePack* pIp)
{
    HashMap<String, InterfacePack*>::Iterator it = mServices.Find(name);
    if (it == mServices.End()) return E_FAIL;
    InterfacePack* ip = it->mSecond;
    memset(pIp, 0, sizeof(InterfacePack));
    pIp->m_clsid = ip->m_clsid;
    pIp->m_uIndex = ip->m_uIndex;
    pIp->m_sNetAddress = ip->m_sNetAddress;
    return NOERROR;
}

ECode DCARServer::RemoveService(
    /* [in] */ const String& name)
{
    HashMap<String, InterfacePack*>::Iterator it = mServices.Find(name);
    if (it != mServices.End()) {
        InterfacePack* ip = it->mSecond;
        mServices.Erase(it);
        ip->m_sNetAddress = NULL;
        free(ip);
    }
    return NOERROR;
}

ECode DCARServer::DoCommand(
    /* [in] */ DataWrapper& data,
    /* [out] */ DataWrapper& reply)
{
    String cmd;
    data.ReadString(&cmd);
    if (cmd.Equals("AddService")) {
        String svcName;
        data.ReadString(&svcName);
        InterfacePack* ip = (InterfacePack*)calloc(1, sizeof(InterfacePack));
        EMuid id;
        data.ReadEMuid(&id);
        Int32 index;
        data.ReadInt32(&index);
        String svcAddr;
        data.ReadString(&svcAddr);
        ip->m_clsid = id;
        ip->m_uIndex = index;
        if (!IsValidNetAddress(svcAddr)) {
            free(ip);
            reply.WriteInt32(E_FAIL);
            return E_FAIL;
        }
        ip->m_sNetAddress = svcAddr;
        ECode ec = AddService(svcName, ip);
        reply.WriteInt32(ec);
        return NOERROR;
    }
    else if (cmd.Equals("GetService")) {
        String svcName;
        data.ReadString(&svcName);
        InterfacePack ip;
        ECode ec = GetService(svcName, &ip);
        reply.WriteInt32(ec);
        if (SUCCEEDED(ec)) {
            reply.WriteEMuid(ip.m_clsid);
            reply.WriteInt32(ip.m_uIndex);
            reply.WriteString(ip.m_sNetAddress);
        }
        return NOERROR;
    }
    else if (cmd.Equals("RemoveService")) {
        String svcName;
        data.ReadString(&svcName);
        ECode ec = RemoveService(svcName);
        reply.WriteInt32(ec);
        return NOERROR;
    }

    return E_FAIL;
}

Boolean DCARServer::IsValidNetAddress(
    /* [in] */ const String& netAddress)
{
    Int32 i = netAddress.IndexOf(":");
    if (i == -1) return FALSE;
    String ip = netAddress.Substring(0, i);
    if (!IsValidIPAddress(ip)) return FALSE;
    String port = netAddress.Substring(i + 1);
    if (!IsValidPort(port)) return FALSE;
    return TRUE;
}

Boolean DCARServer::IsValidIPAddress(
    /* [in] */ const String& _ip)
{
    String ip = _ip;
    Int32 i = ip.IndexOf(".");
    if (i == -1) return FALSE;
    String part = ip.Substring(0, i);
    Int32 n = atoi(part);
    if (n < 0 || n > 255) return FALSE;

    ip = ip.Substring(i + 1);
    i = ip.IndexOf(".");
    if (i == -1) return FALSE;
    part = ip.Substring(0, i);
    n = atoi(part);
    if (n < 0 || n > 255) return FALSE;

    ip = ip.Substring(i + 1);
    i = ip.IndexOf(".");
    if (i == -1) return FALSE;
    part = ip.Substring(0, i);
    n = atoi(part);
    if (n < 0 || n > 255) return FALSE;

    part = ip.Substring(i + 1);
    n = atoi(part);
    if (n < 0 || n > 255) return FALSE;

    return TRUE;
}

Boolean DCARServer::IsValidPort(
    /* [in] */ const String& port)
{
    Int32 n = atoi(port);
    if (n < 1024 || n > 65535) return FALSE;
    return TRUE;
}
