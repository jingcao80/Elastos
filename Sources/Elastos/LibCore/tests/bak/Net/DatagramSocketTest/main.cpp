#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <elautoptr.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::test_Constructor, //1. ok
    &CTest::test_ConstructorI, //2. ok
    &CTest::test_ConstructorILjava_net_InetAddress, //3. ok
    &CTest::test_close, //4. ok
    &CTest::test_connectLjava_net_InetAddressI, //5. part ok, Thread::start() have some problem
    &CTest::test_disconnect, //6. not ok, InetAddress::UNSPECIFIED used by CPlainDatagramSocketImpl::disconnect()
    &CTest::test_getInetAddress, //7. part ok
    &CTest::test_getLocalPort, //8. ok
    &CTest::test_getPort, //9. ok
    &CTest::test_getReceiveBufferSize, //10. ok
    &CTest::test_getSendBufferSize, //11. ok
    &CTest::test_getSoTimeout, //12. ok
    &CTest::test_receiveLjava_net_DatagramPacket, //13. not implement,need server/client
    &CTest::test_sendLjava_net_DatagramPacket, //14. not implement,need server/client
    &CTest::test_setSendBufferSizeI, //15. ok
    &CTest::test_setReceiveBufferSizeI, //16. ok
    &CTest::test_ConstructorLjava_net_SocketAddress, //17. ok
    &CTest::test_bindLjava_net_SocketAddress, //18. part ok, Thread::start() have some problem
    &CTest::test_bindLjava_net_SocketAddress_null, //19. ok
    &CTest::test_bindLjava_net_SocketAddress_bad_address, //20. ok
    &CTest::test_bindLjava_net_SocketAddress_address_in_use, //21. ok
    &CTest::test_connectLjava_net_SocketAddress, //22. part ok, Thread::start() have some problem
    &CTest::test_isBound, //23. ok
    &CTest::test_isConnected, //24. not ok, InetAddress::UNSPECIFIED used by CPlainDatagramSocketImpl::disconnect()
    &CTest::test_getRemoteSocketAddress, //25. ok
    &CTest::test_setReuseAddressZ, //26. ok
    &CTest::test_getReuseAddress, //27. ok
    &CTest::test_setBroadcastZ, //28. ok
    &CTest::test_getBroadcast, //29. ok
    &CTest::test_setTrafficClassI, //30. ok
    &CTest::test_getTrafficClass, //31. ok
    &CTest::test_isClosed, //32. ok
    &CTest::test_getChannel, //33. ok
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

    CTest TestObject;

    int nQuitCode = (TestObject.*TestEntry[nIndex])(argc, argv);

    return nQuitCode;
}
