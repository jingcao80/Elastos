#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <elautoptr.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::test_Constructor,
    &CTest::test_ConstructorI,
    &CTest::test_getInterface,
    &CTest::test_getNetworkInterface,
    &CTest::test_getTimeToLive,
    &CTest::test_getTTL,
    &CTest::test_joinGroupLjava_net_InetAddress_IPv4,
    &CTest::test_joinGroupLjava_net_InetAddress_IPv6,

    &CTest::test_joinGroup_null_null,
    &CTest::test_joinGroup_non_multicast_address_IPv4,
    &CTest::test_joinGroup_non_multicast_address_IPv6,
    &CTest::test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_IPv4,
    &CTest::test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_IPv6,
    &CTest::test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface,
    &CTest::test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_multiple_joins_IPv4,
    &CTest::test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_multiple_joins_IPv6,

    &CTest::test_leaveGroupLjava_net_InetAddress_IPv4,
    &CTest::test_leaveGroupLjava_net_InetAddress_IPv6,
    &CTest::test_leaveGroup_null_null,
    &CTest::test_leaveGroup_non_multicast_address_IPv4,
    &CTest::test_leaveGroup_non_multicast_address_IPv6,
    &CTest::test_leaveGroupLjava_net_SocketAddressLjava_net_NetworkInterface_IPv4,
    &CTest::test_leaveGroupLjava_net_SocketAddressLjava_net_NetworkInterface_IPv6,
    &CTest::test_sendLjava_net_DatagramPacketB_IPv4,

    &CTest::test_sendLjava_net_DatagramPacketB_IPv6,
    &CTest::test_setInterfaceLjava_net_InetAddress,
    &CTest::test_setInterface_unbound_address_IPv4,
    &CTest::test_setInterface_unbound_address_IPv6,
    &CTest::test_setNetworkInterfaceLjava_net_NetworkInterface_null,
    &CTest::test_setNetworkInterfaceLjava_net_NetworkInterface_round_trip,
    &CTest::test_setNetworkInterfaceLjava_net_NetworkInterface_IPv4,
    &CTest::test_setNetworkInterfaceLjava_net_NetworkInterface_IPv6,

    &CTest::test_setTimeToLiveI,
    &CTest::test_setTTLB,
    &CTest::test_ConstructorLjava_net_SocketAddress,
    &CTest::test_getLoopbackMode,
    &CTest::test_setLoopbackModeZ,
    &CTest::test_setLoopbackModeSendReceive_IPv4,
    &CTest::test_setLoopbackModeSendReceive_IPv6,
    &CTest::test_setReuseAddressZ,
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
