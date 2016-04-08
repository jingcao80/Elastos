#ifndef __DATAGRAMSOCKET_TEST_H_
#define __DATAGRAMSOCKET_TEST_H_

#include "cmdef.h"
#include "Elastos.CoreLibrary.h"
#include <elastos.h>
#include <elautoptr.h>
#include <elastos/Mutex.h>
#include <elastos/ThreadBase.h>

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Net;
using namespace Elastos::Utility;
using namespace Elastos::IO;
using namespace Elastos::IO::Channels;

#define TEST(a, x) a.test_##x

class CTest {
public:

    class Support_PortManager
    {
    public:
        static int getNextPort();

        static int getNextPortForUDP();

        static int* getNextPortsForUDP(int num);

        static int getNextPort_unsafe();

    private:
        static int somewhatRandomPort();

    private:
        static int lastAssignedPort;
        static Boolean failedOnce;
        static Mutex mLock;
    };

    class DatagramServer : public ThreadBase
    {
    public:
        DatagramServer(int aPort, IInetAddress* address);

        CARAPI run();

        CARAPI stopServer();
    public:
        AutoPtr<IDatagramSocket> ms;
        Boolean running;
        AutoPtr<ArrayOf<Byte> > rbuf;
        AutoPtr<IDatagramPacket> rdp;
    };

public:
    CTest();
    ~CTest();

public:
    int test_Constructor(int argc, char* argv[]); //1.
    int test_ConstructorI(int argc, char* argv[]); //2.
    int test_ConstructorILjava_net_InetAddress(int argc, char* argv[]); //3.
    int test_close(int argc, char* argv[]); //4.
    int test_connectLjava_net_InetAddressI(int argc, char* argv[]); //5.
    int test_disconnect(int argc, char* argv[]); //6.
    int test_getInetAddress(int argc, char* argv[]); //7.
    int test_getLocalPort(int argc, char* argv[]); //8.
    int test_getPort(int argc, char* argv[]); //9.
    int test_getReceiveBufferSize(int argc, char* argv[]); //10.
    int test_getSendBufferSize(int argc, char* argv[]); //11.
    int test_getSoTimeout(int argc, char* argv[]); //12.
    int test_receiveLjava_net_DatagramPacket(int argc, char* argv[]); //13.
    int test_sendLjava_net_DatagramPacket(int argc, char* argv[]); //14.
    int test_setSendBufferSizeI(int argc, char* argv[]); //15.
    int test_setReceiveBufferSizeI(int argc, char* argv[]); //16.
    int test_ConstructorLjava_net_SocketAddress(int argc, char* argv[]); //17.
    int test_bindLjava_net_SocketAddress(int argc, char* argv[]); //18.
    int test_bindLjava_net_SocketAddress_null(int argc, char* argv[]); //19.
    int test_bindLjava_net_SocketAddress_bad_address(int argc, char* argv[]); //20.
    int test_bindLjava_net_SocketAddress_address_in_use(int argc, char* argv[]); //21.
    int test_connectLjava_net_SocketAddress(int argc, char* argv[]); //22.
    int test_isBound(int argc, char* argv[]); //23.
    int test_isConnected(int argc, char* argv[]); //24.
    int test_getRemoteSocketAddress(int argc, char* argv[]); //25.
    int test_setReuseAddressZ(int argc, char* argv[]); //26.
    int test_getReuseAddress(int argc, char* argv[]); //27.
    int test_setBroadcastZ(int argc, char* argv[]); //28.
    int test_getBroadcast(int argc, char* argv[]); //29.
    int test_setTrafficClassI(int argc, char* argv[]); //30.
    int test_getTrafficClass(int argc, char* argv[]); //31.
    int test_isClosed(int argc, char* argv[]); //32.
    int test_getChannel(int argc, char* argv[]); //33.

protected:
    void setUp();
    void tearDown();
    void receive_oversize_java_net_DatagramPacket();

public:
    AutoPtr<IDatagramSocket> ds;
    AutoPtr<IDatagramPacket> dp;
    AutoPtr<IDatagramSocket> sds;
    String retval;
    String testString;
    Boolean interrupted;
};

#endif // __DATAGRAMSOCKET_TEST_H_