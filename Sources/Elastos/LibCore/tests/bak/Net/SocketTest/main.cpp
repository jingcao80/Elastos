#include "test.h"
#include <stdio.h>
#include <stdlib.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::test_01,
    &CTest::test_bindLjava_net_SocketAddress,
    &CTest::test_bindLjava_net_SocketAddress_Proxy,
    &CTest::test_close,
    &CTest::test_connect_unknownhost,
    &CTest::test_connect_unresolved,
    &CTest::test_connectLjava_net_SocketAddress,
    &CTest::test_connectLjava_net_SocketAddress_AnyAddress,
    &CTest::test_connectLjava_net_SocketAddressI,

    &CTest::test_Constructor,
    &CTest::test_ConstructorLjava_lang_StringI,
    &CTest::test_ConstructorLjava_lang_StringILjava_net_InetAddressI,
    &CTest::test_ConstructorLjava_lang_StringIZ,
    &CTest::test_ConstructorLjava_net_InetAddressI,
    &CTest::test_ConstructorLjava_net_InetAddressILjava_net_InetAddressI,
    &CTest::test_ConstructorLjava_net_InetAddressIZ,
    &CTest::test_ConstructorLjava_net_Proxy_Exception,

    &CTest::test_getChannel,
    &CTest::test_getInetAddress,
    &CTest::test_getInputStream,
    &CTest::test_getKeepAlive,
    &CTest::test_getLocalAddress,
    &CTest::test_getLocalPort,
    &CTest::test_getLocalSocketAddress,
    &CTest::test_getOOBInline,

    &CTest::test_getOutputStream,
    &CTest::test_getPort,
    &CTest::test_getReceiveBufferSize,
    &CTest::test_getRemoteSocketAddress,
    &CTest::test_getReuseAddress,
    &CTest::test_getSendBufferSize,
    &CTest::test_getSoLinger,
    &CTest::test_getSoTimeout,

    &CTest::test_getTcpNoDelay,
    &CTest::test_getTrafficClass,
    &CTest::test_isBound,
    &CTest::test_isClosed,
    &CTest::test_isConnected,
    &CTest::test_isInputShutdown,
    &CTest::test_isOutputShutdown,
    &CTest::test_sendUrgentDataI,

    &CTest::test_setKeepAliveZ,
    &CTest::test_setOOBInlineZ,
    &CTest::test_setPerformancePreference_Int_Int_Int,
    &CTest::test_setReceiveBufferSizeI,
    &CTest::test_setReuseAddressZ,
    &CTest::test_setSendBufferSizeI,
    &CTest::test_setSocketImplFactoryLjava_net_SocketImplFactory,
    &CTest::test_setSoLingerZI,

    &CTest::test_setSoTimeoutI,
    &CTest::test_setTcpNoDelayZ,
    &CTest::test_setTrafficClassI,
    &CTest::test_shutdownInput,
    &CTest::test_shutdownOutput,
    &CTest::test_toString,
};

int main(int argc, char *argv[])
{
    if (argc == 1) {
        printf("*ERROR* Require test case number\n");
        return -1;
    }

    int nIndex = atoi(argv[1]) - 1;
    int nLength = sizeof(TestEntry) / sizeof(TestEntry[0]);

    if (nIndex < 0 || nIndex >= nLength) {
        printf("*ERROR* Invalid testing case number\n");
        return -1;
    }

//    TPINIT;

    CTest TestObject;

    int nQuitCode = (TestObject.*TestEntry[nIndex])(argc, argv);

//    TPEND;

    return nQuitCode;
}
