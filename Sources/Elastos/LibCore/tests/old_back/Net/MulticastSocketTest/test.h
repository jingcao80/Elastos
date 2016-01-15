#ifndef __MULTICASTSOCKETTEST_H__
#define __MULTICASTSOCKETTEST_H__

#include "cmdef.h"
#include "Elastos.CoreLibrary.h"
#include <elastos.h>
#include <elautoptr.h>
#include <elquintet.h>
#include <elastos/StringUtils.h>
#include <elastos/Mutex.h>
#include <elastos/ThreadBase.h>

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Utility;
using namespace Elastos::Net;
using namespace Elastos::IO;

#define TEST(a, x) a.test_##x

class CTest
{
public:
    class Support_PortManager
    {
    public:
        static int getNextPort();

        /**
         * Returns 1 free ports to be used.
         */
        static int getNextPortForUDP();

        /**
         * Returns the specified number of free ports to be used.
         */
        static int* getNextPortsForUDP(int num);

        static int getNextPort_unsafe();

        /*
          * Returns a different port number every 6 seconds or so. The port number
          * should be about += 100 at each 6 second interval
          */
    private:
        static int somewhatRandomPort();

    private:
        static int lastAssignedPort;
        static Boolean failedOnce;
        static Mutex mLock;
    };

    class MulticastServer : public ThreadBase
    {
    public:
        MulticastServer(IInetAddress* anAddress, int aPort);

        MulticastServer(ISocketAddress* anAddress, int aPort, INetworkInterface* netInterface);

        CARAPI Run();

        CARAPI stopServer();

    public:
        AutoPtr<IMulticastSocket> ms;

        Boolean running;

        /* volatile */ AutoPtr< ArrayOf<Byte> > rbuf;

        /* volatile */ AutoPtr<IDatagramPacket> rdp;

    private:
        AutoPtr<IInetAddress> groupAddr;
        AutoPtr<ISocketAddress> groupSockAddr;
        AutoPtr<INetworkInterface> groupNI;
    };
public:
    CTest();
    ~CTest();

public:
    int test_Constructor(int argc, char* argv[]);
    int test_ConstructorI(int argc, char* argv[]);
    int test_getInterface(int argc, char* argv[]);
    int test_getNetworkInterface(int argc, char* argv[]);
    int test_getTimeToLive(int argc, char* argv[]);
    int test_getTTL(int argc, char* argv[]);
    int test_joinGroupLjava_net_InetAddress_IPv4(int argc, char* argv[]);
    int test_joinGroupLjava_net_InetAddress_IPv6(int argc, char* argv[]);

    int test_joinGroup_null_null(int argc, char* argv[]);
    int test_joinGroup_non_multicast_address_IPv4(int argc, char* argv[]);
    int test_joinGroup_non_multicast_address_IPv6(int argc, char* argv[]);
    int test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_IPv4(int argc, char* argv[]);
    int test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_IPv6(int argc, char* argv[]);
    int test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface(int argc, char* argv[]);
    int test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_multiple_joins_IPv4(int argc, char* argv[]);
    int test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_multiple_joins_IPv6(int argc, char* argv[]);

    int test_leaveGroupLjava_net_InetAddress_IPv4(int argc, char* argv[]);
    int test_leaveGroupLjava_net_InetAddress_IPv6(int argc, char* argv[]);
    int test_leaveGroup_null_null(int argc, char* argv[]);
    int test_leaveGroup_non_multicast_address_IPv4(int argc, char* argv[]);
    int test_leaveGroup_non_multicast_address_IPv6(int argc, char* argv[]);
    int test_leaveGroupLjava_net_SocketAddressLjava_net_NetworkInterface_IPv4(int argc, char* argv[]);
    int test_leaveGroupLjava_net_SocketAddressLjava_net_NetworkInterface_IPv6(int argc, char* argv[]);
    int test_sendLjava_net_DatagramPacketB_IPv4(int argc, char* argv[]);

    int test_sendLjava_net_DatagramPacketB_IPv6(int argc, char* argv[]);
    int test_setInterfaceLjava_net_InetAddress(int argc, char* argv[]);
    int test_setInterface_unbound_address_IPv4(int argc, char* argv[]);
    int test_setInterface_unbound_address_IPv6(int argc, char* argv[]);
    int test_setNetworkInterfaceLjava_net_NetworkInterface_null(int argc, char* argv[]);
    int test_setNetworkInterfaceLjava_net_NetworkInterface_round_trip(int argc, char* argv[]);
    int test_setNetworkInterfaceLjava_net_NetworkInterface_IPv4(int argc, char* argv[]);
    int test_setNetworkInterfaceLjava_net_NetworkInterface_IPv6(int argc, char* argv[]);

    int test_setTimeToLiveI(int argc, char* argv[]);
    int test_setTTLB(int argc, char* argv[]);
    int test_ConstructorLjava_net_SocketAddress(int argc, char* argv[]);
    int test_getLoopbackMode(int argc, char* argv[]);
    int test_setLoopbackModeZ(int argc, char* argv[]);
    int test_setLoopbackModeSendReceive_IPv4(int argc, char* argv[]);
    int test_setLoopbackModeSendReceive_IPv6(int argc, char* argv[]);
    int test_setReuseAddressZ(int argc, char* argv[]);

protected:
    void setUp();

private:
    static AutoPtr<IInetAddress> lookup(String s);
    void test_joinGroupLjava_net_InetAddress(IInetAddress* group);
    void test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface(IInetAddress* group, IInetAddress* group2);
    void test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_multiple_joins(IInetAddress* group);
    void test_leaveGroupLjava_net_InetAddress(IInetAddress* group);
    void test_leaveGroupLjava_net_SocketAddressLjava_net_NetworkInterface(IInetAddress* group, IInetAddress* group2);
    void test_sendLjava_net_DatagramPacketB(IInetAddress* group);
    void test_setInterface_unbound_address(IInetAddress* address);
    void test_setNetworkInterfaceLjava_net_NetworkInterface(IInetAddress* group);
    void test_setLoopbackModeSendReceive(IInetAddress* group);

private:
    // These IP addresses aren't inherently "good" or "bad"; they're just used like that.
    // We use the "good" addresses for our actual group, and the "bad" addresses are for
    // a group that we won't actually set up.

    static AutoPtr<IInetAddress> GOOD_IPv4; // = lookup("224.0.0.3");
    static AutoPtr<IInetAddress> BAD_IPv4; // = lookup("224.0.0.4");

    static AutoPtr<IInetAddress> GOOD_IPv6; // = lookup("ff05::7:7");
    static AutoPtr<IInetAddress> BAD_IPv6; // = lookup("ff05::7:8");

    Boolean atLeastTwoInterfaces;

    AutoPtr<INetworkInterface> networkInterface1;

    AutoPtr<INetworkInterface> networkInterface2;

    AutoPtr<INetworkInterface> IPV6networkInterface1;
};

#endif // __MULTICASTSOCKETTEST_H__