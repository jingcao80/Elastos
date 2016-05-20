#ifndef __INETSOCKETADDRESSTEST_H__
#define __INETSOCKETADDRESSTEST_H__

#include "cmdef.h"
#include "Elastos.CoreLibrary.h"
#include <elastos.h>
#include <elautoptr.h>
#include <elquintet.h>
#include <elastos/StringUtils.h>
#include <elastos/Mutex.h>

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Utility;
using namespace Elastos::Net;
using namespace Elastos::IO;

#define TEST(a, x) a.test_##x

class CTest
{
private:
    class ClientThread
        : public IRunnable
        , public ElRefBase
    {
    public:
        ClientThread(CTest* host);

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        CTest* mHost;
    };

    class ServerThread
        : public IRunnable
        , public ElRefBase
    {
    public:
        ServerThread(CTest* host);

        CAR_INTERFACE_DECL()

        CARAPI Run();

        CARAPI WaitCreated();

    public:
        Boolean ready;

        static const Int32 FIRST_TIME;

        static const Int32 SECOND_TIME;

        Int32 backlog;

        Int32 serverSocketConstructor;

        Mutex mlock;

        CTest* mHost;
    };

    class OutputRunnable
        : public IRunnable
        , public ElRefBase
    {
    public:
        OutputRunnable(IServerSocket* inserver);

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        AutoPtr<IServerSocket> mServer;
    };

public:
    CTest();
    ~CTest();

public:
    int test_01(int argc, char* argv[]);
    int test_bindLjava_net_SocketAddress(int argc, char* argv[]);
    int test_bindLjava_net_SocketAddress_Proxy(int argc, char* argv[]);
    int test_close(int argc, char* argv[]);
    int test_connect_unknownhost(int argc, char* argv[]);
    int test_connect_unresolved(int argc, char* argv[]);
    int test_connectLjava_net_SocketAddress(int argc, char* argv[]);
    int test_connectLjava_net_SocketAddress_AnyAddress(int argc, char* argv[]);
    int test_connectLjava_net_SocketAddressI(int argc, char* argv[]);

    int test_Constructor(int argc, char* argv[]);
    int test_ConstructorLjava_lang_StringI(int argc, char* argv[]);
    int test_ConstructorLjava_lang_StringILjava_net_InetAddressI(int argc, char* argv[]);
    int test_ConstructorLjava_lang_StringIZ(int argc, char* argv[]);
    int test_ConstructorLjava_net_InetAddressI(int argc, char* argv[]);
    int test_ConstructorLjava_net_InetAddressILjava_net_InetAddressI(int argc, char* argv[]);
    int test_ConstructorLjava_net_InetAddressIZ(int argc, char* argv[]);
    int test_ConstructorLjava_net_Proxy_Exception(int argc, char* argv[]);

    int test_getChannel(int argc, char* argv[]);
    int test_getInetAddress(int argc, char* argv[]);
    int test_getInputStream(int argc, char* argv[]);
    int test_getKeepAlive(int argc, char* argv[]);
    int test_getLocalAddress(int argc, char* argv[]);
    int test_getLocalPort(int argc, char* argv[]);
    int test_getLocalSocketAddress(int argc, char* argv[]);
    int test_getOOBInline(int argc, char* argv[]);

    int test_getOutputStream(int argc, char* argv[]);
    int test_getPort(int argc, char* argv[]);
    int test_getReceiveBufferSize(int argc, char* argv[]);
    int test_getRemoteSocketAddress(int argc, char* argv[]);
    int test_getReuseAddress(int argc, char* argv[]);
    int test_getSendBufferSize(int argc, char* argv[]);
    int test_getSoLinger(int argc, char* argv[]);
    int test_getSoTimeout(int argc, char* argv[]);

    int test_getTcpNoDelay(int argc, char* argv[]);
    int test_getTrafficClass(int argc, char* argv[]);
    int test_isBound(int argc, char* argv[]);
    int test_isClosed(int argc, char* argv[]);
    int test_isConnected(int argc, char* argv[]);
    int test_isInputShutdown(int argc, char* argv[]);
    int test_isOutputShutdown(int argc, char* argv[]);
    int test_sendUrgentDataI(int argc, char* argv[]);

    int test_setKeepAliveZ(int argc, char* argv[]);
    int test_setOOBInlineZ(int argc, char* argv[]);
    int test_setPerformancePreference_Int_Int_Int(int argc, char* argv[]);
    int test_setReceiveBufferSizeI(int argc, char* argv[]);
    int test_setReuseAddressZ(int argc, char* argv[]);
    int test_setSendBufferSizeI(int argc, char* argv[]);
    int test_setSocketImplFactoryLjava_net_SocketImplFactory(int argc, char* argv[]);
    int test_setSoLingerZI(int argc, char* argv[]);

    int test_setSoTimeoutI(int argc, char* argv[]);
    int test_setTcpNoDelayZ(int argc, char* argv[]);
    int test_setTrafficClassI(int argc, char* argv[]);
    int test_shutdownInput(int argc, char* argv[]);
    int test_shutdownOutput(int argc, char* argv[]);
    int test_toString(int argc, char* argv[]);

private:
    void connectTestImpl(int ssConsType);
    void tearDown();
    Boolean isUnix();

private:
    Boolean interrupted;

    String host;
    int port;

    AutoPtr<IThread> t;

};

#endif // __INETSOCKETADDRESSTEST_H__