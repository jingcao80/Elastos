
#include "ServiceManager.h"
#include "DataWrapper.h"
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cutils/log.h>

// come from marshal_rpc.h
typedef struct InterfacePack
{
    EMuid               m_clsid;            // clsid of object
    UInt32              m_uIndex;           // interface index in class
    String              m_sNetAddress;      // net address of stub
}   InterfacePack;

AutoPtr<ServiceManager> ServiceManager::sInstance;

AutoPtr<ServiceManager> ServiceManager::GetInstance()
{
    if (sInstance == NULL) {
        sInstance = new ServiceManager();
    }
    return sInstance;
}

ServiceManager::ServiceManager()
    : mIsConnected(FALSE)
    , mServerPort(-1)
{}

Boolean ServiceManager::ConnectServer()
{
    Mutex::AutoLock lock(mConnectedLock);

    if (mIsConnected) return TRUE;

    int socketfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (socketfd < 0) {
        ALOGD("ServiceManager::ConnectServer: create socket failed");
        return FALSE;
    }

    struct sockaddr_in dstName;
    dstName.sin_family = AF_INET;
    dstName.sin_port = htons(12321);
    const char* MULTICAST_ADDR = "224.0.0.1";
    if (inet_pton(dstName.sin_family, MULTICAST_ADDR, &dstName.sin_addr) == 0) {
        ALOGD("ServiceManager::ConnectServer: invalid multicast address");
        close(socketfd);
        return FALSE;
    }

    const char* cmd = "Call DCARServer";
    struct sockaddr_in svcName;
    socklen_t svcLen = sizeof(struct sockaddr_in);
    bzero(&svcName, svcLen);
    Byte data[1024];
    Int32 tryN, len;
    for (tryN = 0; tryN < 5; tryN++) {
        len = sendto(socketfd, cmd, strlen(cmd), 0, (struct sockaddr*)&dstName, sizeof(dstName));
        if (len < 0) {
            ALOGD("ServiceManager::ConnectServer: sendto socket failed: %s(errno: %d)", strerror(errno), errno);
            usleep(500000);
            continue;
        }

        len = recvfrom(socketfd, data, 1024, 0, (struct sockaddr *)&svcName, &svcLen);
        if (len > 0) {
            data[len] = '\0';
            break;
        }
    }

    if (tryN == 5) {
        ALOGD("ServiceManager::ConnectServer: sendto server failed.");
        close(socketfd);
        return FALSE;
    }

    DataWrapper dataWrapper(data, len);
    String svcAddress;
    dataWrapper.ReadString(&svcAddress);
    Int32 index = svcAddress.IndexOf(":");
    mServerIpAddress = svcAddress.Substring(0, index);
    mServerPort = atoi(svcAddress.Substring(index + 1).string());
    mIsConnected = TRUE;
    close(socketfd);
    return TRUE;
}

ECode ServiceManager::AddService(
    /* [in] */ const String& name,
    /* [in] */ IInterface* service)
{
    if (name.IsNullOrEmpty() || !service) {
        ALOGD("ServiceManager::AddService: invalid parameter. name:[%s], service:[%p]",
            name.string(), service);
        return E_FAIL;
    }

    void* buf = NULL;
    Int32 size;
    ECode ec = _CObject_MarshalInterface(service, MarshalType_RPC, &buf, &size);
    InterfacePack* ip = (InterfacePack*)buf;
    if (FAILED(ec)) {
        if (ip) ip->m_sNetAddress = NULL;
        if (buf != NULL) free(buf);
        ALOGD("ServiceManager::AddService: [%s] marshal service failed, ec=%08x", name.string(), ec);
        return ec;
    }

    if (!ConnectServer())  {
        ip->m_sNetAddress = NULL;
        free(buf);
        ALOGD("ServiceManager::AddService: [%s] connect to server failed.", name.string());
        return E_FAIL;
    }

    Byte data[1024];
    DataWrapper dataWrapper(data, 1024);
    dataWrapper.WriteString(String("AddService"));
    dataWrapper.WriteString(name);
    dataWrapper.WriteEMuid(ip->m_clsid);
    dataWrapper.WriteInt32(ip->m_uIndex);
    dataWrapper.WriteString(ip->m_sNetAddress);

    ip->m_sNetAddress = NULL;
    free(buf);

    Int32 socketfd = socket(PF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        ALOGD("ServiceManager::AddService: [%s] create socket failed.", name.string());
        return E_FAIL;
    }

    struct sockaddr_in svcName;
    svcName.sin_family = AF_INET;
    svcName.sin_addr.s_addr = inet_addr(mServerIpAddress);
    svcName.sin_port = htons(mServerPort);

    if (connect(socketfd, (struct sockaddr*)&svcName, sizeof(svcName)) < 0) {
        ALOGD("ServiceManager::AddService: [%s] connect socket failed. %s(%d)",
            name.string(), strerror(errno), errno);
        close(socketfd);
        return E_FAIL;
    }

    if (send(socketfd, dataWrapper.Data(), dataWrapper.DataSize(), 0) == -1) {
        ALOGD("ServiceManager::AddService: [%s] send socket failed.", name.string());
        close(socketfd);
        return E_FAIL;
    }

    Byte reply[1024];
    size = recv(socketfd, (void*)reply, sizeof(reply), 0);
    if (size <= 0) {
        ALOGD("ServiceManager::AddService: [%s] recv socket failed. %s(%d)",
            name.string(), strerror(errno), errno);
        close(socketfd);
        return E_FAIL;
    }

    reply[size] = '\0';
    DataWrapper replyWrapper(reply, size);
    Int32 ret;
    replyWrapper.ReadInt32(&ret);
    close(socketfd);
    return (ECode)ret;
}

ECode ServiceManager::GetService(
    /* [in] */ const String& name,
    /* [out] */ IInterface** service)
{
    if (!service) {
        ALOGD("ServiceManager::GetService: invalid parameter. service is null.");
        return E_FAIL;
    }

    *service = NULL;

    if (name.IsNullOrEmpty()) {
        ALOGD("ServiceManager::GetService: Service name is null or empty!");
        return E_FAIL;
    }

    if (!ConnectServer())  {
        ALOGD("ServiceManager::GetService: [%s] connect to server failed.", name.string());
        return E_FAIL;
    }

    Byte data[1024];
    DataWrapper dataWrapper(data, 1024);
    dataWrapper.WriteString(String("GetService"));
    dataWrapper.WriteString(name);

    Int32 socketfd = socket(PF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        ALOGD("ServiceManager::GetService: [%s] create socket failed.", name.string());
        return E_FAIL;
    }

    struct sockaddr_in svcName;
    svcName.sin_family = AF_INET;
    svcName.sin_addr.s_addr = inet_addr(mServerIpAddress);
    svcName.sin_port = htons(mServerPort);

    if (connect(socketfd, (struct sockaddr*)&svcName, sizeof(svcName)) < 0) {
        ALOGD("ServiceManager::GetService: [%s] connect socket failed.", name.string());
        close(socketfd);
        return E_FAIL;
    }

    if (send(socketfd, dataWrapper.Data(), dataWrapper.DataSize(), 0) == -1) {
        ALOGD("ServiceManager::GetService: [%s] send socket failed.", name.string());
        close(socketfd);
        return E_FAIL;
    }

    Byte reply[1024];
    Int32 size = recv(socketfd, reply, 1024, 0);
    if (size <= 0) {
        ALOGD("ServiceManager::GetService: [%s] recv socket failed.", name.string());
        close(socketfd);
        return E_FAIL;
    }

    reply[size] = '\0';
    DataWrapper replyWrapper(reply, size);
    Int32 ret;
    replyWrapper.ReadInt32(&ret);
    ECode ec = (ECode)ret;
    if (FAILED(ec)) {
        ALOGD("ServiceManager::GetService: [%s] read reply failed.", name.string());
        close(socketfd);
        return ec;
    }

    InterfacePack* ip = (InterfacePack*)calloc(1, sizeof(InterfacePack));
    EMuid id;
    replyWrapper.ReadEMuid(&id);
    ip->m_clsid = id;
    Int32 index;
    replyWrapper.ReadInt32(&index);
    ip->m_uIndex = index;
    String netAddress;
    replyWrapper.ReadString(&netAddress);
    ip->m_sNetAddress = netAddress;

    ec = _CObject_UnmarshalInterface(
        (void*)ip, MarshalType_RPC, UnmarshalFlag_Noncoexisting, service, &size);
    free(ip);
    if (FAILED(ec)) {
        ALOGD("ServiceManager::GetService: [%s] unmarshal service failed.", name.string());
    }
    close(socketfd);
    return ec;
}
