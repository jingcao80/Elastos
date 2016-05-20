#include "test.h"
#include <elastos/StringUtils.h>
#include <elastos/Thread.h>
#include <time.h>

static Int64 GetCurrentTimeMillis()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 curTime;
    system->GetCurrentTimeMillis(&curTime);
    return curTime;
}

//==========================================================
//       CTest::ClientThread
//==========================================================
CTest::ClientThread::ClientThread(CTest* host)
    : mHost(host)
{
}

CAR_INTERFACE_IMPL(CTest::ClientThread, IRunnable)

ECode CTest::ClientThread::Run()
{
    PEL("CTest::ClientThread::Run")
    // try {
    AutoPtr<ISocket> socket;
    CSocket::New((ISocket**)&socket);
    AutoPtr<IInetSocketAddress> addr;
    CInetSocketAddress::New(mHost->host, mHost->port, (IInetSocketAddress**)&addr);
    socket->Connect(addr);

    return socket->Close();
    // } catch (IOException e) {
    //     throw new RuntimeException(e);
    // }
}

//==========================================================
//       CTest::ServerThread
//==========================================================
const Int32 CTest::ServerThread::FIRST_TIME = 1;

const Int32 CTest::ServerThread::SECOND_TIME = 2;

CTest::ServerThread::ServerThread(CTest* host)
    : mHost(host)
    , ready(FALSE)
    , backlog(10)
    , serverSocketConstructor(0)
{
}

CAR_INTERFACE_IMPL(CTest::ServerThread, IRunnable)

ECode CTest::ServerThread::Run()
{
    PEL("CTest::ServerThread::Run")
    // try {
    AutoPtr<IServerSocket> socket;
    switch (serverSocketConstructor) {
        case FIRST_TIME:
        {
            AutoPtr<IInetSocketAddress> addr;
            CInetSocketAddress::New(mHost->host, mHost->port, (IInetSocketAddress**)&addr);
            AutoPtr<IInetAddress> netadd;
            addr->GetAddress((IInetAddress**)&netadd);
PFL
            CServerSocket::New(mHost->port, backlog, netadd, (IServerSocket**)&socket);
PFL
            socket->GetLocalPort(&mHost->port);
PFL
            break;
        }
        case SECOND_TIME:
        {   CServerSocket::New(mHost->port, backlog, (IServerSocket**)&socket);
            AutoPtr<IInetAddress> addr;
            socket->GetInetAddress((IInetAddress**)&addr);
            addr->GetHostName(&mHost->host);
            socket->GetLocalPort(&mHost->port);
            break;
        }
        default:
            CServerSocket::New((IServerSocket**)&socket);
            break;
    }

    {
        // Mutex::Autolock lock(mlock);
        ready = TRUE;
        // this->notifyAll();
    }

PFL
    socket->SetSoTimeout(5000);
PFL
    AutoPtr<ISocket> client;
PFL
    socket->Accept((ISocket**)&client);
PFL
    client->Close();
PFL
    socket->Close();
    // } catch (IOException e) {
    //     e.printStackTrace();
    // } catch (Throwable e) {
    //     e.printStackTrace();
    // }
}

ECode CTest::ServerThread::WaitCreated()
{
    Mutex::Autolock lock(mlock);
    while (!ready) {
        // this->wait();
    }
}

//==========================================================
//       CTest::OutputRunnable
//==========================================================
CAR_INTERFACE_IMPL(CTest::OutputRunnable, IRunnable)

CTest::OutputRunnable::OutputRunnable(IServerSocket* inserver)
{
    mServer = inserver;
}

ECode CTest::OutputRunnable::Run()
{
    // try {
    AutoPtr<ISocket> worker;
    mServer->Accept((ISocket**)&worker);
    mServer->Close();
    AutoPtr<IInputStream> in;
    worker->GetInputStream((IInputStream**)&in);
    Int32 value = 0;
    in->Read(&value);
    in->Close();
    return worker->Close();
    // } catch (IOException e) {
    //     fail();
    // }
}


//==========================================================
//       CTest
//==========================================================

CTest::CTest()
{
    interrupted = FALSE;
    host = String("localhost");
    port = 0;
}

CTest::~CTest()
{
    tearDown();
}


/**
 * Reads an unsigned 32 bit big endian number from the given offset in the buffer.
 */
Int64 Read32(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset)
{
    Byte b0 = (*buffer)[offset];
    Byte b1 =  (*buffer)[offset+1];
    Byte b2 =  (*buffer)[offset+2];
    Byte b3 =  (*buffer)[offset+3];

    // convert signed bytes to unsigned values
    Int32 i0 = ((b0 & 0x80) == 0x80 ? (b0 & 0x7F) + 0x80 : b0);
    Int32 i1 = ((b1 & 0x80) == 0x80 ? (b1 & 0x7F) + 0x80 : b1);
    Int32 i2 = ((b2 & 0x80) == 0x80 ? (b2 & 0x7F) + 0x80 : b2);
    Int32 i3 = ((b3 & 0x80) == 0x80 ? (b3 & 0x7F) + 0x80 : b3);

    return ((Int64)i0 << 24) + ((Int64)i1 << 16) + ((Int64)i2 << 8) + (Int64)i3;
}

/**
 * Reads the NTP time stamp at the given offset in the buffer and returns
 * it as a system time (milliseconds since January 1, 1970).
 */
Int64 ReadTimeStamp(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset)
{
    Int64 seconds = Read32(buffer, offset);
    Int64 fraction = Read32(buffer, offset + 4);
    Int64 OFFSET_1900_TO_1970 = (Int64)(((365L * 70L) + 17L) * 24L * 60L * 60L);
    return ((seconds - OFFSET_1900_TO_1970) * 1000) + ((fraction * 1000L) / 0x100000000L);
}


int CTest::test_01(int argc, char* argv[])
{
    AutoPtr<IDatagramSocket> socket;
    //try {
    ECode ec = CDatagramSocket::New((IDatagramSocket**)&socket);
    if (FAILED(ec) || socket == NULL) {
        return ec;
    }
    else {
        host = String("2.android.pool.ntp.org");
        Int32 timeout = 1139431012;
        socket->SetSoTimeout(timeout);
        AutoPtr<IInetAddressHelper> helper;
        CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
        AutoPtr<IInetAddress> address;
        helper->GetByName(host, (IInetAddress**)&address);
        PFL_EX("address: %p", address.Get())

        Int32 length = 48;
        AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(length);
        AutoPtr<IDatagramPacket> request;
        CDatagramPacket::New(buffer, length, address, 123, (IDatagramPacket**)&request);

        // set mode = 3 (client) and version = 3
        // mode is in low 3 bits of first byte
        // version is in bits 3-5 of first byte
        (*buffer)[0] = 3 | (3 << 3);

        // get current time and write it to the request packet
        Int64 requestTime = GetCurrentTimeMillis();
        Int64 requestTicks =  0;// SystemClock::GetElapsedRealtime();
        // WriteTimeStamp(buffer, 40, requestTime);

        ec = socket->Send(request);
        PFL_EX("ec : %p, socket:%p", ec , socket.Get())

        // read the response
        AutoPtr<IDatagramPacket> response;
        CDatagramPacket::New(buffer, length, (IDatagramPacket**)&response);
        ec = socket->Receive(response);
        PFL_EX("ec : %p, response:%p", ec , response.Get())

        // Int64 responseTicks = 0; //SystemClock::GetElapsedRealtime();
        // Int64 responseTime = requestTime + (responseTicks - requestTicks);

        // // extract the results
        Int64 originateTime = ReadTimeStamp(buffer, 24);
        Int64 receiveTime = ReadTimeStamp(buffer, 32);
        Int64 transmitTime = ReadTimeStamp(buffer, 40);
        PFL_EX("originateTime: %lld, receiveTime: %lld, transmitTime: %lld", originateTime, receiveTime, transmitTime)

        struct tm *tblock;
        tblock = localtime((time_t*)&originateTime);
        printf("originateTime time is: %s\n",asctime(tblock));

        tblock = localtime((time_t*)&receiveTime);
        printf("receiveTime time is: %s\n",asctime(tblock));

        tblock = localtime((time_t*)&transmitTime);
        printf("transmitTime time is: %s\n",asctime(tblock));

        // Int64 roundTripTime = responseTicks - requestTicks - (transmitTime - receiveTime);
        // // receiveTime = originateTime + transit + skew
        // // responseTime = transmitTime + transit - skew
        // // clockOffset = ((receiveTime - originateTime) + (transmitTime - responseTime))/2
        // //             = ((originateTime + transit + skew - originateTime) +
        // //                (transmitTime - (transmitTime + transit - skew)))/2
        // //             = ((transit + skew) + (transmitTime - transmitTime - transit + skew))/2
        // //             = (transit + skew - transit + skew)/2
        // //             = (2 * skew)/2 = skew
        // Int64 clockOffset = ((receiveTime - originateTime) + (transmitTime - responseTime))/2;
        // // if (false) Log.d(TAG, "round trip: " + roundTripTime + " ms");
        // // if (false) Log.d(TAG, "clock offset: " + clockOffset + " ms");

        // // save our results - use the times on this side of the network latency
        // // (response rather than request time)
        // mNtpTime = responseTime + clockOffset;
        // mNtpTimeReference = responseTicks;
        // mRoundTripTime = roundTripTime;
        // Slogger::E("Cher_CSntpClient", "the mNtpTime: %d mNtpTimeReference: %d mRoundTripTime: %d", mNtpTime, mNtpTimeReference, mRoundTripTime);

        //finally...
        socket->Close();
    }
    return 0;

}

int CTest::test_bindLjava_net_SocketAddress(int argc, char* argv[])
{
    PEL("CTest::test_bindLjava_net_SocketAddress")
    // @SuppressWarnings("serial")
    // class UnsupportedSocketAddress extends SocketAddress {
    //     public UnsupportedSocketAddress() {
    //     }
    // };

    // Address we cannot bind to
    AutoPtr<ISocket> theSocket;
    CSocket::New((ISocket**)&theSocket);
    AutoPtr<IInetSocketAddress> bogusAddress;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr< ArrayOf<Byte> > nonLocalAddressBytes = ArrayOf<Byte>::Alloc(4);
    (*nonLocalAddressBytes)[0] = 1;
    (*nonLocalAddressBytes)[1] = 0;
    (*nonLocalAddressBytes)[2] = 0;
    (*nonLocalAddressBytes)[3] = 0;
    AutoPtr<IInetAddress> outnet;
    addhelp->GetByAddress(nonLocalAddressBytes, (IInetAddress**)&outnet);
    CInetSocketAddress::New(outnet, 42, (IInetSocketAddress**)&bogusAddress);
    // try {
    ECode ec = theSocket->Bind(bogusAddress);
    if (ec != NOERROR) {
        printf("No exception when binding to bad address expected~~~~\n");
    }
    // } catch (IOException ex) {
    //     // Expected
    // }
    theSocket->Close();

    // Now create a socket that is not bound and then bind it
    CSocket::New((ISocket**)&theSocket);
    AutoPtr<IInetAddress> localadd;
    addhelp->GetLocalHost((IInetAddress**)&localadd);
    CInetSocketAddress::New(localadd, 0, (IInetSocketAddress**)&bogusAddress);
    ec = theSocket->Bind(bogusAddress);
    PFL_EX("ec : %p", ec)
    int portNumber = 0;
    theSocket->GetLocalPort(&portNumber);

    // Validate that the localSocketAddress reflects the address we
    // bound to
    CInetSocketAddress::New(localadd, portNumber, (IInetSocketAddress**)&bogusAddress);
    AutoPtr<ISocketAddress> bindsock;
    theSocket->GetLocalSocketAddress((ISocketAddress**)&bindsock);
    PFL_EX("bogusAddress: %p, bindsock: %p", bogusAddress.Get(), bindsock.Get())
    assert(Object::Equals(bogusAddress, bindsock));

    // Make sure we can now connect and that connections appear to come
    // from the address we bound to.
    AutoPtr<IInetSocketAddress> theAddress;
    CInetSocketAddress::New(localadd, 0, (IInetSocketAddress**)&theAddress);

    AutoPtr<IServerSocket> server;
    CServerSocket::New((IServerSocket**)&server);
    ec = server->Bind(theAddress);
PFL_EX("ec : %p", ec)
    int sport = 0;
    server->GetLocalPort(&sport);
    AutoPtr<IInetSocketAddress> boundAddress;
    CInetSocketAddress::New(localadd, sport, (IInetSocketAddress**)&boundAddress);

PFL
    ec = theSocket->Connect(boundAddress);
PFL_EX("server: %p, theSocket:%p, ec : %p", server.Get(), theSocket.Get(), ec)
    AutoPtr<ISocket> worker;
    server->Accept((ISocket**)&worker);
PFL
    AutoPtr<ISocketAddress> remotesocket;
    worker->GetRemoteSocketAddress((ISocketAddress**)&remotesocket);
    assert(Object::Equals(bogusAddress, remotesocket));
PFL
    theSocket->Close();
    worker->Close();
    server->Close();

PFL
    // Validate if we pass in null that it picks an address for us and
    // all is ok
    CSocket::New((ISocket**)&theSocket);
    theSocket->Bind(NULL);
    AutoPtr<ISocketAddress> socketadd;
    theSocket->GetLocalSocketAddress((ISocketAddress**)&socketadd);
    assert(socketadd != NULL);
    theSocket->Close();

    // now check the error conditions

    // Address that we have already bound to
    CSocket::New((ISocket**)&theSocket);
    CInetSocketAddress::New(localadd, 0, (IInetSocketAddress**)&theAddress);
    theSocket->Bind(theAddress);

PFL
    AutoPtr<ISocket> theSocket2;
    CSocket::New((ISocket**)&theSocket2);
    // try {
    theSocket->GetLocalSocketAddress((ISocketAddress**)&socketadd);
    ec = theSocket2->Bind(socketadd);
    if (ec != NOERROR) {
        printf("No exception binding to address that is not available\n");
    }
    // } catch (IOException ex) {
    //     // Expected
    // }
    theSocket->Close();
    theSocket2->Close();

PFL
    // Unsupported SocketAddress subclass
    CSocket::New((ISocket**)&theSocket);
    // try {
    //     theSocket.bind(new UnsupportedSocketAddress());
    //     fail("No exception when binding using unsupported SocketAddress subclass");
    // } catch (IllegalArgumentException ex) {
    //     // Expected
    // }
    theSocket->Close();
    return 0;
}

int CTest::test_bindLjava_net_SocketAddress_Proxy(int argc, char* argv[])
{
    PEL("CTest::test_bindLjava_net_SocketAddress_Proxy")
    // The Proxy will not impact on the bind operation. It can be assigned
    // with any address.
    AutoPtr<IInetSocketAddress> socketadd;
    CInetSocketAddress::New(String("127.0.0.1"), 0, (IInetSocketAddress**)&socketadd);
    // AutoPtr<IProxy> proxy;
    // CProxy::New(ProxyType_SOCKS, socketadd, (IProxy**)&proxy);
    // AutoPtr<ISocket> socket;
    // CSocket::New(proxy, (ISocket**)&socket);

    // AutoPtr<IInetAddress> address;
    // AutoPtr<IInetAddressHelper> netaddhelp;
    // CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&netaddhelp);
    // netaddhelp->GetByName(String("localhost"), (IInetAddress**)&address);
    // CInetSocketAddress::New(address, 0, (IInetSocketAddress**)&socketadd);
    // socket->Bind(socketadd);

    // AutoPtr<IInetAddress> localadd;
    // socket->GetLocalAddress((IInetAddress**)&localadd);
    // assert(Object::Equals(address, localadd));
    // Int32 portnum = 0;
    // socket->GetLocalPort(&portnum);
    // assert(0 != portnum);

    // socket->Close();
    return 0;
}

int CTest::test_close(int argc, char* argv[])
{
    PEL("CTest::test_close")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> locnetadd;
    addhelp->GetLocalHost((IInetAddress**)&locnetadd);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    AutoPtr<ISocket> client;
    CSocket::New(locnetadd, portnum, (ISocket**)&client);

    client->SetSoLinger(false, 100);

    client->Close();
    // try {
    AutoPtr<IOutputStream> outstr;
    ECode ec = client->GetOutputStream((IOutputStream**)&outstr);
    if (ec != NOERROR) {
        printf("Failed to close socket\n");
    }
    // } catch (IOException expected) {
    // }

    server->Close();
    return 0;
}

int CTest::test_connect_unknownhost(int argc, char* argv[])
{
    PEL("CTest::test_connect_unknownhost")
    AutoPtr<ISocket> socket;
    CSocket::New((ISocket**)&socket);
    // try {
    AutoPtr<IInetSocketAddress> sockadd;
    CInetSocketAddress::New(String("unknownhost.invalid"), 12345, (IInetSocketAddress**)&sockadd);
    ECode ec = socket->Connect(sockadd);
    if (ec != NOERROR) {
        /* code */
        printf("fails expected~~~~~~~~~~\n");
    }
    // } catch (UnknownHostException expected) {
    // }
    return 0;
}

int CTest::test_connect_unresolved(int argc, char* argv[])
{
    PEL("CTest::test_connect_unresolved")
    AutoPtr<ISocket> socket;
    CSocket::New((ISocket**)&socket);
    AutoPtr<IInetSocketAddress> unresolved;
    AutoPtr<IInetSocketAddressHelper> nsaddhelper;
    CInetSocketAddressHelper::AcquireSingleton((IInetSocketAddressHelper**)&nsaddhelper);
    nsaddhelper->CreateUnresolved(String("www.apache.org"), 80, (IInetSocketAddress**)&unresolved);
    // try {
    ECode ec = socket->Connect(unresolved);
    if (ec != NOERROR) {
        printf("fail expected~~~~~~~~~~~~\n");
    }
    // } catch (UnknownHostException expected) {
    // }
    // try {
    ec = socket->ConnectEx(unresolved, 123);
    if (ec != NOERROR) {
        printf("fail expected~~~~~~~~~~~~\n");
    }
    // } catch (UnknownHostException expected) {
    // }
    return 0;
}

int CTest::test_connectLjava_net_SocketAddress(int argc, char* argv[])
{
    PEL("CTest::test_connectLjava_net_SocketAddress")
    // @SuppressWarnings("serial")
    // class UnsupportedSocketAddress extends SocketAddress {
    //     public UnsupportedSocketAddress() {
    //     }
    // };

    AutoPtr<ISocket> theSocket;
    CSocket::New((ISocket**)&theSocket);
    // try {
    ECode ec = theSocket->Connect(NULL);
    if (ec != NOERROR) {
        printf("No exception for null arg~~~~~\n");
    }
    // } catch (IllegalArgumentException e) {
    //     // Expected
    // }

    // try {
    //     theSocket.connect(new UnsupportedSocketAddress());
    //     fail("No exception for invalid socket address");
    // } catch (IllegalArgumentException e) {
    //     // Expected
    // }

    // try {
    AutoPtr< ArrayOf<Byte> > byte4 = ArrayOf<Byte>::Alloc(4);
    (*byte4)[0] = 0;
    (*byte4)[1] = 0;
    (*byte4)[2] = 0;
    (*byte4)[3] = 0;
    AutoPtr<IInetAddressHelper> addhelp;
    AutoPtr<IInetAddress> outadd;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    addhelp->GetByAddress(byte4, (IInetAddress**)&outadd);
PFL
    AutoPtr<IInetSocketAddress> socketadd;
    CInetSocketAddress::New(outadd, 42, (IInetSocketAddress**)&socketadd);
    ec = theSocket->Connect(socketadd);
PFL
    if (ec != NOERROR) {
        printf("No exception with non-connectable address~~~~~~~~~~~");
    }
    // } catch (ConnectException e) {
    //     // Expected
    // }

    // // now validate that we get a connect exception if we try to connect to
    // // an address on which nobody is listening
    CSocket::New((ISocket**)&theSocket);
    // try {
    addhelp->GetLocalHost((IInetAddress**)&outadd);
    CInetSocketAddress::New(outadd, 0, (IInetSocketAddress**)&socketadd);
    ec = theSocket->Connect(socketadd);
    if (ec != NOERROR) {
        printf("No exception when connecting to address nobody listening on\n");
    }
    // } catch (ConnectException e) {
    //     // Expected
    // }

    // Now validate that we can actually connect when somebody is listening
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<IInetSocketAddress> boundAddress;
    addhelp->GetLocalHost((IInetAddress**)&outadd);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    CInetSocketAddress::New(outadd, portnum, (IInetSocketAddress**)&boundAddress);
    AutoPtr<ISocket> client;
    CSocket::New((ISocket**)&client);
    client->Connect(boundAddress);

    // validate that when a socket is connected that it answers
    // correctly to related queries
    Boolean isflag = FALSE;
    client->IsConnected(&isflag);
    assert(isflag);
    client->IsClosed(&isflag);
    assert(!isflag);
    client->IsBound(&isflag);
    assert(isflag);
    client->IsInputShutdown(&isflag);
    assert(!isflag);
    client->IsOutputShutdown(&isflag);
    assert(!isflag);
    client->GetLocalPort(&portnum);
    assert(portnum != 0);

    client->Close();
    server->Close();

    // Now validate that we get the right exception if we connect when we
    // are already connected
    CServerSocket::New(0, (IServerSocket**)&server);
    addhelp->GetLocalHost((IInetAddress**)&outadd);
    server->GetLocalPort(&portnum);
    CInetSocketAddress::New(outadd, portnum, (IInetSocketAddress**)&boundAddress);
    CSocket::New((ISocket**)&client);
    client->Connect(boundAddress);

    // try {
    ec = client->Connect(boundAddress);
    if (ec != NOERROR) {
        printf("No exception when we try to connect on a connected socket: \n");
    }
    // } catch (SocketException e) {
    //     // Expected
    // }
    client->Close();
    server->Close();
    return 0;
}

int CTest::test_connectLjava_net_SocketAddress_AnyAddress(int argc, char* argv[])
{
    PEL("CTest::test_connectLjava_net_SocketAddress_AnyAddress")
    connectTestImpl(ServerThread::FIRST_TIME);
    connectTestImpl(ServerThread::SECOND_TIME);
    return 0;
}

int CTest::test_connectLjava_net_SocketAddressI(int argc, char* argv[])
{
    PEL("CTest::test_connectLjava_net_SocketAddressI")
    // @SuppressWarnings("serial")
    // class UnsupportedSocketAddress extends SocketAddress {
    //     public UnsupportedSocketAddress() {
    //     }
    // };

    // // Start by validating the error checks
    AutoPtr<ISocket> theSocket;
    CSocket::New((ISocket**)&theSocket);
    // try {
    AutoPtr<IInetSocketAddress> sockadd;
    CInetSocketAddress::New(0, (IInetSocketAddress**)&sockadd);
    ECode ec = theSocket->ConnectEx(sockadd, -100);
    if (ec != NOERROR) {
        printf("No exception for negative timeout\n");
    }
    // } catch (IllegalArgumentException e) {
    //     // Expected
    // }

    // try {
    ec = theSocket->ConnectEx(NULL, 0);
    if (ec != NOERROR) {
        printf("No exception for null address\n");
    }
    // } catch (IllegalArgumentException e) {
    //     // Expected
    // }

    // try {
    //     theSocket.connect(new UnsupportedSocketAddress(), 1000);
    //     fail("No exception for invalid socket address type");
    // } catch (IllegalArgumentException e) {
    //     // Expected
    // }

    AutoPtr<ISocketAddress> nonConnectableAddress;
    AutoPtr< ArrayOf<Byte> > byte4 = ArrayOf<Byte>::Alloc(4);
    (*byte4)[0] = 0;
    (*byte4)[1] = 0;
    (*byte4)[2] = 0;
    (*byte4)[3] = 0;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> netadd;
    addhelp->GetByAddress(byte4, (IInetAddress**)&netadd);
    CInetSocketAddress::New(netadd, 0, (IInetSocketAddress**)&nonConnectableAddress);
    // try {
    ec = theSocket->ConnectEx(nonConnectableAddress, 1000);
    if (ec != NOERROR) {
        printf("No exception when non Connectable Address passed in: \n");
    }
    // } catch (SocketException e) {
    //     // Expected
    // }

    // Now validate that we get a connect exception if we try to connect to
    // an address on which nobody is listening
    CSocket::New((ISocket**)&theSocket);
    // try {
    addhelp->GetLocalHost((IInetAddress**)&netadd);
    CInetSocketAddress::New(netadd, 0, (IInetSocketAddress**)&sockadd);
    ec = theSocket->ConnectEx(sockadd, 0);
    if (ec != NOERROR) {
        printf("No exception when connecting to address nobody listening on\n");
    }
    // } catch (ConnectException e) {
    //     // Expected
    // }
    theSocket->Close();

    // Now validate that we can actually connect when somebody is listening
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    addhelp->GetLocalHost((IInetAddress**)&netadd);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    AutoPtr<IInetSocketAddress> boundAddress;
    CInetSocketAddress::New(netadd, portnum, (IInetSocketAddress**)&boundAddress);
    AutoPtr<ISocket> client;
    CSocket::New((ISocket**)&client);
    client->ConnectEx(boundAddress, 0);

    // Validate that when a socket is connected that it answers
    // correctly to related queries
    Boolean isflag = FALSE;
    client->IsConnected(&isflag);
    assert(isflag);
    client->IsClosed(&isflag);
    assert(!isflag);
    client->IsBound(&isflag);
    assert(isflag);
    client->IsInputShutdown(&isflag);
    assert(!isflag);
    client->IsOutputShutdown(&isflag);
    assert(!isflag);
    client->GetLocalPort(&portnum);
    assert(portnum != 0);

    client->Close();
    server->Close();

    // Now validate that we get a connect exception if we try to connect to
    // an address on which nobody is listening
    CSocket::New((ISocket**)&theSocket);
    AutoPtr<ISocketAddress> nonListeningAddress;
    addhelp->GetLocalHost((IInetAddress**)&netadd);
    CInetSocketAddress::New(netadd, 42, (IInetSocketAddress**)&sockadd);
    // try {
    ec = theSocket->ConnectEx(nonListeningAddress, 1000);
    if (ec != NOERROR) {
        printf("No exception when connecting to address nobody listening on");
    }
    // } catch (ConnectException e) {
    //     // Expected
    // } catch (SocketTimeoutException e) {
    //     // The other possibility is that the system timed us out.
    // }
    theSocket->Close();

    // Now validate that we get the right exception if we connect when we
    // are already connected
    CServerSocket::New(0, (IServerSocket**)&server);
    addhelp->GetLocalHost((IInetAddress**)&netadd);
    server->GetLocalPort(&portnum);
    CInetSocketAddress::New(netadd, portnum, (IInetSocketAddress**)&boundAddress);
    CSocket::New((ISocket**)&client);
    client->ConnectEx(boundAddress, 10000);

    // try {
    ec = client->ConnectEx(boundAddress, 10000);
    if (ec != NOERROR) {
        printf("No exception when we try to connect on a connected socket: \n");
    }
    // } catch (SocketException e) {
    //     // Expected
    // }
    client->Close();
    server->Close();
    return 0;
}

int CTest::test_Constructor(int argc, char* argv[])
{
    PEL("CTest::test_Constructor")
    // create the socket and then validate some basic state
    AutoPtr<ISocket> s;
    CSocket::New((ISocket**)&s);
    Boolean isflag = FALSE;
    s->IsConnected(&isflag);
    assert(!isflag);
    s->IsBound(&isflag);
    assert(!isflag);
    s->IsClosed(&isflag);
    assert(!isflag);
    s->IsInputShutdown(&isflag);
    assert(!isflag);
    s->IsOutputShutdown(&isflag);
    assert(!isflag);
    return 0;
}

int CTest::test_ConstructorLjava_lang_StringI(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorLjava_lang_StringI")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> netadd;
    addhelp->GetLocalHost((IInetAddress**)&netadd);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    CSocket::New(netadd, portnum, (ISocket**)&client);

    Int32 clientportnum = 0;
    client->GetPort(&clientportnum);
    assert(portnum == clientportnum);

    // Regression for HARMONY-946
    AutoPtr<IServerSocket> ss;
    CServerSocket::New(0, (IServerSocket**)&ss);
    ss->GetLocalPort(&portnum);
    AutoPtr<ISocket> s;
    CSocket::New(String("0.0.0.0"), portnum, (ISocket**)&s);
    ss->Close();
    s->Close();
    return 0;
}

int CTest::test_ConstructorLjava_lang_StringILjava_net_InetAddressI(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorLjava_lang_StringILjava_net_InetAddressI")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    Int32 serverPort = 0;
    server->GetLocalPort(&serverPort);
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    String hostname;
    lochost->GetHostName(&hostname);
    AutoPtr<ISocket> client;
    CSocket::New(hostname, serverPort, lochost, 0, (ISocket**)&client);
    Int32 portnum = 0;
    client->GetPort(&portnum);
    assert(portnum == serverPort);
    client->Close();

    AutoPtr<ISocket> theSocket;
    // try {
    CSocket::New(String("127.0.0.1"), serverPort, lochost, 0, (ISocket**)&theSocket);
    // } catch (IOException e) {
    //     // check here if InetAddress.getLocalHost() is returning the
    //     // loopback address, if so that is likely the cause of the failure
    //     assertFalse(
    //             "Misconfiguration - local host is the loopback address",
    //             InetAddress.getLocalHost().isLoopbackAddress());
    //     throw e;
    // }
    Boolean isflag = FALSE;
    theSocket->IsConnected(&isflag);
    assert(isflag);

    // try {
    AutoPtr<ISocket> testSock;
    AutoPtr<IInetAddress> locadd;
    theSocket->GetLocalAddress((IInetAddress**)&locadd);
    theSocket->GetLocalPort(&portnum);
    ECode ec = CSocket::New(String("127.0.0.1"), serverPort, locadd, portnum, (ISocket**)&testSock);
    if (ec != NOERROR) {
        /* code */
        printf("fails expected~~~~~~\n");
    }
    //     fail("Was able to create two sockets on same port");
    // } catch (IOException e) {
    //     // Expected
    // }

    theSocket->Close();
    server->Close();
    return 0;
}

int CTest::test_ConstructorLjava_lang_StringIZ(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorLjava_lang_StringIZ")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    int serverPort = 0;
    server->GetLocalPort(&serverPort);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> netadd;
    addhelp->GetLocalHost((IInetAddress**)&netadd);
    String strhost;
    netadd->GetHostAddress(&strhost);
    CSocket::New(strhost, serverPort, TRUE, (ISocket**)&client);

    Int32 clientport = 0;
    client->GetPort(&clientport);
    assert(serverPort == clientport);
    client->Close();

    String hostname;
    netadd->GetHostName(&hostname);
    CSocket::New(hostname, serverPort, FALSE, (ISocket**)&client);
    client->Close();
    server->Close();
    return 0;
}

int CTest::test_ConstructorLjava_net_InetAddressI(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorLjava_net_InetAddressI")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> netadd;
    addhelp->GetLocalHost((IInetAddress**)&netadd);
    Int32 serverport = 0;
    server->GetLocalPort(&serverport);
    CSocket::New(netadd, serverport, (ISocket**)&client);

    Int32 clientport = 0;
    client->GetPort(&clientport);
    assert(serverport == clientport);

    client->Close();
    server->Close();
    return 0;
}

int CTest::test_ConstructorLjava_net_InetAddressILjava_net_InetAddressI(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorLjava_net_InetAddressILjava_net_InetAddressI")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> netadd;
    addhelp->GetLocalHost((IInetAddress**)&netadd);
    Int32 serverport = 0;
    server->GetLocalPort(&serverport);
    CSocket::New(netadd, serverport, netadd, 0, (ISocket**)&client);
    Int32 clientport = 0;
    client->GetLocalPort(&clientport);
    assert(0 != clientport);
    return 0;
}

int CTest::test_ConstructorLjava_net_InetAddressIZ(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorLjava_net_InetAddressIZ")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    Int32 serverPort = 0;
    server->GetLocalPort(&serverPort);

    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> netadd;
    addhelp->GetLocalHost((IInetAddress**)&netadd);
    CSocket::New(netadd, serverPort, TRUE, (ISocket**)&client);
    Int32 clientport = 0;
    client->GetPort(&clientport);
    assert(serverPort == clientport);

    CSocket::New(netadd, serverPort, FALSE, (ISocket**)&client);
    client->Close();
    return 0;
}

int CTest::test_ConstructorLjava_net_Proxy_Exception(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorLjava_net_Proxy_Exception")
    AutoPtr<ISocketAddress> addr1;
    AutoPtr<IInetSocketAddressHelper> netsockhelp;
    CInetSocketAddressHelper::AcquireSingleton((IInetSocketAddressHelper**)&netsockhelp);
    netsockhelp->CreateUnresolved(String("127.0.0.1"), 80, (IInetSocketAddress**)&addr1);
    AutoPtr<ISocketAddress> addr2;
    CInetSocketAddress::New(String("localhost"), 80, (IInetSocketAddress**)&addr2);

    // AutoPtr<IProxy> proxy1;
    // CProxy::New(ProxyType_HTTP, addr1, (IProxy**)&proxy1);
    // // IllegalArgumentException test
    // // try {
    // AutoPtr<ISocket> testsock;
    // ECode ec = CSocket::New(proxy1, (ISocket**)&testsock);
    // if (ec != NOERROR) {
    //     printf("should throw IllegalArgumentException~~~~~~\n");
    // }
    // // } catch (IllegalArgumentException e) {
    // //     // expected
    // // }

    // AutoPtr<IProxy> proxy2;
    // CProxy::New(ProxyType_SOCKS, addr1, (IProxy**)&proxy2);
    // // should not throw any exception
    // ec = CSocket::New(proxy2, (ISocket**)&testsock);
    // AutoPtr<IProxyHelper> proxyhelp;
    // CProxyHelper::AcquireSingleton((IProxyHelper**)&proxyhelp);
    // proxyhelp->GetNO_PROXY((IProxy**)&proxy1);
    // ec = CSocket::New(proxy1, (ISocket**)&testsock);
    return 0;
}

int CTest::test_getChannel(int argc, char* argv[])
{
    PEL("CTest::test_getChannel")
    AutoPtr<ISocket> testsock;
    CSocket::New((ISocket**)&testsock);
    AutoPtr<ISocketChannel> socketchan;
    testsock->GetChannel((ISocketChannel**)&socketchan);
    assert(socketchan == NULL);
    return 0;
}

int CTest::test_getInetAddress(int argc, char* argv[])
{
    PEL("CTest::test_getInetAddress")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    CSocket::New(lochost, portnum, (ISocket**)&client);

    AutoPtr<IInetAddress> clientadd;
    client->GetInetAddress((IInetAddress**)&clientadd);
    assert(Object::Equals(clientadd, lochost));

    client->Close();
    server->Close();
    return 0;
}

int CTest::test_getInputStream(int argc, char* argv[])
{
    PEL("CTest::test_getInputStream")
    // Simple fetch test
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    CSocket::New(lochost, portnum, (ISocket**)&client);
    AutoPtr<IInputStream> is;
    client->GetInputStream((IInputStream**)&is);
    assert(is != NULL);
    is->Close();
    client->Close();
    server->Close();
    return 0;
}

int CTest::test_getKeepAlive(int argc, char* argv[])
{
    PEL("CTest::test_getKeepAlive")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    CSocket::New(lochost, portnum, NULL, 0, (ISocket**)&client);

    client->SetKeepAlive(TRUE);
    Boolean isflag = FALSE;
    client->GetKeepAlive(&isflag);
    assert(isflag);

    client->SetKeepAlive(FALSE);
    client->GetKeepAlive(&isflag);
    assert(!isflag);
    return 0;
}

int CTest::test_getLocalAddress(int argc, char* argv[])
{
    PEL("CTest::test_getLocalAddress")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    CSocket::New(lochost, portnum, (ISocket**)&client);

    AutoPtr<IInetAddress> clientadd;
    client->GetLocalAddress((IInetAddress**)&clientadd);
    assert(Object::Equals(clientadd, lochost));

    CSocket::New((ISocket**)&client);
    AutoPtr<IInetAddress> nameadd;
    addhelp->GetByName(String("0.0.0.0"), (IInetAddress**)&nameadd);
    AutoPtr<IInetSocketAddress> sockadd;
    CInetSocketAddress::New(nameadd, 0, (IInetSocketAddress**)&sockadd);
    client->Bind(sockadd);

    client->GetLocalAddress((IInetAddress**)&clientadd);
    Boolean isflag = FALSE;
    clientadd->IsAnyLocalAddress(&isflag);
    assert(isflag);

    client->Close();
    server->Close();
    return 0;
}

int CTest::test_getLocalPort(int argc, char* argv[])
{
    PEL("CTest::test_getLocalPort")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    CSocket::New(lochost, portnum, (ISocket**)&client);

    client->GetLocalPort(&portnum);
    assert(0 != portnum);

    client->Close();
    server->Close();
    return 0;
}

int CTest::test_getLocalSocketAddress(int argc, char* argv[])
{
    PEL("CTest::test_getLocalSocketAddress")
    // set up server connect and then validate that we get the right
    // response for the local address
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    AutoPtr<ISocket> client;
    CSocket::New(lochost, portnum, (ISocket**)&client);
    int clientport = 0;
    client->GetLocalPort(&clientport);
    AutoPtr<IInetSocketAddress> netsock;
    CInetSocketAddress::New(lochost, clientport, (IInetSocketAddress**)&netsock);
    AutoPtr<ISocketAddress> clientadd;
    client->GetLocalSocketAddress((ISocketAddress**)&clientadd);
    assert(Object::Equals(netsock, clientadd));
    client->Close();
    server->Close();

    // now create a socket that is not bound and validate we get the
    // right answer
    CSocket::New((ISocket**)&client);
    client->GetLocalSocketAddress((ISocketAddress**)&clientadd);
    assert(clientadd == NULL);

    // now bind the socket and make sure we get the right answer
    CInetSocketAddress::New(lochost, 0, (IInetSocketAddress**)&netsock);
    client->Bind(netsock);
    client->GetLocalPort(&clientport);
    CInetSocketAddress::New(lochost, clientport, (IInetSocketAddress**)&netsock);
    client->GetLocalSocketAddress((ISocketAddress**)&clientadd);
    assert(Object::Equals(netsock, clientadd));
    client->Close();

    // now validate the behaviour when the any address is returned
    CSocket::New((ISocket**)&client);
    AutoPtr<IInetAddress> nameadd;
    addhelp->GetByName(String("0.0.0.0"), (IInetAddress**)&nameadd);
    CInetSocketAddress::New(nameadd, 0, (IInetSocketAddress**)&netsock);
    client->Bind(netsock);
    client->GetLocalSocketAddress((ISocketAddress**)&clientadd);
    AutoPtr<IInetSocketAddress> outnetsoc = IInetSocketAddress::Probe(clientadd);
    AutoPtr<IInetAddress> outadd;
    outnetsoc->GetAddress((IInetAddress**)&outadd);
    Boolean isflag = FALSE;
    outadd->IsAnyLocalAddress(&isflag);
    assert(isflag);
    client->Close();

    // now validate the same for getLocalAddress
    CSocket::New((ISocket**)&client);
    addhelp->GetByName(String("0.0.0.0"), (IInetAddress**)&nameadd);
    CInetSocketAddress::New(nameadd, 0, (IInetSocketAddress**)&netsock);
    client->Bind(netsock);
    client->GetLocalAddress((IInetAddress**)&nameadd);
    nameadd->IsAnyLocalAddress(&isflag);
    assert(isflag);
    client->Close();
    return 0;
}

int CTest::test_getOOBInline(int argc, char* argv[])
{
    PEL("CTest::test_getOOBInline")
    AutoPtr<ISocket> theSocket;
    CSocket::New((ISocket**)&theSocket);

    theSocket->SetOOBInline(TRUE);
    Boolean isflag = FALSE;
    theSocket->GetOOBInline(&isflag);
    assert(isflag);

    theSocket->SetOOBInline(FALSE);
    theSocket->GetOOBInline(&isflag);
    assert(!isflag);

    theSocket->SetOOBInline(FALSE);
    theSocket->GetOOBInline(&isflag);
    assert(!isflag);
    return 0;
}

int CTest::test_getOutputStream(int argc, char* argv[])
{
    PEL("CTest::test_getOutputStream")
    // Simple fetch test
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    AutoPtr<ISocket> client;
    CSocket::New(lochost, portnum, (ISocket**)&client);
    AutoPtr<IOutputStream> os;
    client->GetOutputStream((IOutputStream**)&os);
    assert(os != NULL);
    os->Close();
    client->Close();
    server->Close();

    // Simple read/write test over the IO streams
    AutoPtr<IServerSocket> sinkServer;
    CServerSocket::New(0, (IServerSocket**)&sinkServer);
    AutoPtr<IRunnable> runnable = new OutputRunnable(sinkServer);

    AutoPtr<IThread> thread;
    CThread::New(runnable, String("Socket.getOutputStream"), (IThread**)&thread);
    thread->Start();

    AutoPtr<ISocket> pingClient;
    sinkServer->GetLocalPort(&portnum);
    CSocket::New(lochost, portnum, (ISocket**)&pingClient);

    // Busy wait until the client is connected.
    int c = 0;
    Boolean isflag = FALSE;
    while (!(pingClient->IsConnected(&isflag), isflag)) {
        // try {
        Thread::Sleep(2000);
        // } catch (InterruptedException e) {
        // }
        if (++c > 4) {
            printf("thread is not alive ~~~~~~~~\n");
        }
    }

    // Write some data to the server
    AutoPtr<IOutputStream> out;
    pingClient->GetOutputStream((IOutputStream**)&out);
    AutoPtr< ArrayOf<Byte> > byte256 = ArrayOf<Byte>::Alloc(256);
    out->WriteBytes(*byte256);

    // Wait for the server to finish
    Thread::Yield();
    c = 0;
    thread->IsAlive(&isflag);
    while (isflag) {
        // try {
        Thread::Sleep(200);
        // } catch (InterruptedException e) {
        // }
        if (++c > 4) {
            printf("read call did not exit~~~~~\n");
        }
    }

    ECode ec;
    // Subsequent writes should throw an exception
    // try {
    // The output buffer may remain valid until the close completes
    for (int i = 0; i < 400; i++) {
        ec = out->WriteBytes(*byte256);
    }
    if (ec != NOERROR) {
        printf("write to closed socket did not cause exception~~~~\n");
    }
    // } catch (IOException e) {
    //     // Expected
    // }

    out->Close();
    pingClient->Close();
    sinkServer->Close();

    // Regression test for HARMONY-873
    AutoPtr<IServerSocket> ss2;
    CServerSocket::New(0, (IServerSocket**)&ss2);
    AutoPtr<ISocket> s;
    ss2->GetLocalPort(&portnum);
    CSocket::New(String("127.0.0.1"), portnum, (ISocket**)&s);
    AutoPtr<ISocket> acceptsock;
    ss2->Accept((ISocket**)&acceptsock);
    s->ShutdownOutput();
    // try {
    ec = s->GetOutputStream((IOutputStream**)&out);
    if (ec != NOERROR) {
        printf("should throw SocketException\n");
    }
    // } catch (SocketException e) {
    //     // expected
    // }
    return 0;
}

int CTest::test_getPort(int argc, char* argv[])
{
    PEL("CTest::test_getPort")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    int serverPort = 0;
    server->GetLocalPort(&serverPort);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    CSocket::New(lochost, serverPort, (ISocket**)&client);
    Int32 clientport = 0;
    client->GetPort(&clientport);
    assert(serverPort == clientport);

    client->Close();
    server->Close();
    return 0;
}

int CTest::test_getReceiveBufferSize(int argc, char* argv[])
{
    PEL("CTest::test_getReceiveBufferSize")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    CSocket::New(lochost, portnum, (ISocket**)&client);
    client->SetReceiveBufferSize(130);

    Int32 bufsize = 0;
    client->GetReceiveBufferSize(&bufsize);
    assert(bufsize >= 130);

    client->Close();
    server->Close();
    return 0;
}

int CTest::test_getRemoteSocketAddress(int argc, char* argv[])
{
    PEL("CTest::test_getRemoteSocketAddress")
    // set up server connect and then validate that we get the right
    // response for the remote address
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    int serverPort = 0;
    server->GetLocalPort(&serverPort);
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    AutoPtr<ISocket> client;
    CSocket::New(lochost, serverPort, (ISocket**)&client);

    AutoPtr<IInetSocketAddress> netsockadd;
    CInetSocketAddress::New(lochost, serverPort, (IInetSocketAddress**)&netsockadd);
    AutoPtr<ISocketAddress> clientsock;
    client->GetRemoteSocketAddress((ISocketAddress**)&clientsock);
    assert(Object::Equals(netsockadd, clientsock));
    client->Close();

    // now create one that is not connected and validate that we get the
    // right answer
    AutoPtr<ISocket> theSocket;
    CSocket::New((ISocket**)&theSocket);
    CInetSocketAddress::New(lochost, 0, (IInetSocketAddress**)&netsockadd);
    theSocket->Bind(netsockadd);
    theSocket->GetRemoteSocketAddress((ISocketAddress**)&clientsock);
    assert(clientsock == NULL);

    // now connect and validate we get the right answer
    CInetSocketAddress::New(lochost, serverPort, (IInetSocketAddress**)&netsockadd);
    theSocket->Connect(netsockadd);
    CInetSocketAddress::New(lochost, serverPort, (IInetSocketAddress**)&netsockadd);
    theSocket->GetRemoteSocketAddress((ISocketAddress**)&clientsock);
    assert(Object::Equals(netsockadd, clientsock));
    theSocket->Close();

    server->Close();
    return 0;
}

int CTest::test_getReuseAddress(int argc, char* argv[])
{
    PEL("CTest::test_getReuseAddress")
    AutoPtr<ISocket> theSocket;
    CSocket::New((ISocket**)&theSocket);
    theSocket->SetReuseAddress(true);
    Boolean isflag = FALSE;
    theSocket->GetReuseAddress(&isflag);
    assert(isflag);
    theSocket->SetReuseAddress(false);
    theSocket->GetReuseAddress(&isflag);
    assert(!isflag);
    return 0;
}

int CTest::test_getSendBufferSize(int argc, char* argv[])
{
    PEL("CTest::test_getSendBufferSize")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    CSocket::New(lochost, portnum, (ISocket**)&client);
    client->SetSendBufferSize(134);
    Int32 bufsize = 0;
    client->GetSendBufferSize(&bufsize);
    assert(bufsize >= 134);
    client->Close();
    server->Close();
    return 0;
}

int CTest::test_getSoLinger(int argc, char* argv[])
{
    PEL("CTest::test_getSoLinger")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    CSocket::New(lochost, portnum, (ISocket**)&client);
    client->SetSoLinger(true, 200);
    client->GetSoLinger(&portnum);
    assert(200 == portnum);
    client->SetSoLinger(false, 0);
    client->Close();
    server->Close();
    return 0;
}

int CTest::test_getSoTimeout(int argc, char* argv[])
{
    PEL("CTest::test_getSoTimeout")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    CSocket::New(lochost, portnum, (ISocket**)&client);
    client->SetSoTimeout(100);
    client->GetSoTimeout(&portnum);
    assert(100 == portnum);
    client->Close();
    server->Close();
    return 0;
}

int CTest::test_getTcpNoDelay(int argc, char* argv[])
{
    PEL("CTest::test_getTcpNoDelay")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    CSocket::New(lochost, portnum, (ISocket**)&client);

    Boolean isflag = FALSE;
    client->GetTcpNoDelay(&isflag);
PFL
    isflag = !isflag;
    client->SetTcpNoDelay(isflag);
    Boolean isflag2 = FALSE;
    client->GetTcpNoDelay(&isflag2);
    assert(isflag2 == isflag);

    client->Close();
    server->Close();
    return 0;
}

int CTest::test_getTrafficClass(int argc, char* argv[])
{
    PEL("CTest::test_getTrafficClass")
    /*
     * We cannot actually check that the values are set as if a platform
     * does not support the option then it may come back unset even
     * though we set it so just get the value to make sure we can get it
     */
    int trafficClass = 0;
    AutoPtr<ISocket>  theSocket;
    CSocket::New((ISocket**)&theSocket);
    theSocket->GetTrafficClass(&trafficClass);
    assert(0 <= trafficClass);
    assert(trafficClass <= 255);
    return 0;
}

int CTest::test_isBound(int argc, char* argv[])
{
    PEL("CTest::test_isBound")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    CSocket::New(lochost, portnum, (ISocket**)&client);
    AutoPtr<ISocket> worker;
    server->Accept((ISocket**)&worker);
    Boolean isflag = FALSE;
    client->IsBound(&isflag);
    assert(isflag);
    worker->Close();
    client->Close();
    server->Close();

    CSocket::New((ISocket**)&client);
    client->IsBound(&isflag);
    assert(!isflag);

    CServerSocket::New((IServerSocket**)&server);
    AutoPtr<IInetSocketAddress> netsockadd;
    CInetSocketAddress::New(lochost, 0, (IInetSocketAddress**)&netsockadd);
    server->Bind(netsockadd);

    AutoPtr<IInetSocketAddress> boundAddress;
    AutoPtr<IInetAddress> netadd;
    server->GetInetAddress((IInetAddress**)&netadd);
    server->GetLocalPort(&portnum);
    CInetSocketAddress::New(netadd, portnum, (IInetSocketAddress**)&boundAddress);
    client->Connect(boundAddress);
    server->Accept((ISocket**)&worker);
    client->IsBound(&isflag);
    assert(isflag);
    worker->Close();
    client->Close();
    server->Close();

    // now test when we bind explicitly
    AutoPtr<IInetSocketAddress> theLocalAddress;
    CInetSocketAddress::New(lochost, 0, (IInetSocketAddress**)&theLocalAddress);
    CSocket::New((ISocket**)&client);
    client->IsBound(&isflag);
    assert(!isflag);
    client->Bind(theLocalAddress);
    client->IsBound(&isflag);
    assert(isflag);
    client->Close();
    client->IsBound(&isflag);
    assert(isflag);
    return 0;
}

int CTest::test_isClosed(int argc, char* argv[])
{
    PEL("CTest::test_isClosed")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    CSocket::New(lochost, portnum, (ISocket**)&client);
    AutoPtr<ISocket> worker;
    server->Accept((ISocket**)&worker);

    // validate isClosed returns expected values
    Boolean isflag = FALSE;
    client->IsClosed(&isflag);
    assert(!isflag);
    client->Close();
    client->IsClosed(&isflag);
    assert(isflag);

    // validate that isClosed works ok for sockets returned from
    // ServerSocket.accept()
    worker->IsClosed(&isflag);
    assert(!isflag);
    worker->Close();
    worker->IsClosed(&isflag);
    assert(isflag);

    // and finally for the server socket
    server->IsClosed(&isflag);
    assert(!isflag);
    server->Close();
    server->IsClosed(&isflag);
    assert(isflag);
    return 0;
}

int CTest::test_isConnected(int argc, char* argv[])
{
    PEL("CTest::test_isConnected")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    CSocket::New(lochost, portnum, (ISocket**)&client);
    AutoPtr<ISocket> worker;
    server->Accept((ISocket**)&worker);

    Boolean isflag = FALSE;
    client->IsConnected(&isflag);
    assert(isflag);
    client->Close();
    worker->Close();
    server->Close();

    // now do it with the new constructors and revalidate
    AutoPtr<IInetSocketAddress> theAddress;
    CInetSocketAddress::New(lochost, 0, (IInetSocketAddress**)&theAddress);
    CSocket::New((ISocket**)&client);
    client->IsConnected(&isflag);
    assert(!isflag);

    CServerSocket::New((IServerSocket**)&server);
    server->Bind(theAddress);
    AutoPtr<IInetSocketAddress> boundAddress;
    AutoPtr<IInetAddress> netadd;
    server->GetInetAddress((IInetAddress**)&netadd);
    server->GetLocalPort(&portnum);
    CInetSocketAddress::New(netadd, portnum, (IInetSocketAddress**)&boundAddress);
    client->Connect(boundAddress);
    server->Accept((ISocket**)&worker);
    client->IsConnected(&isflag);
    assert(isflag);
    client->Close();
    worker->Close();
    server->Close();
    return 0;
}

int CTest::test_isInputShutdown(int argc, char* argv[])
{
    PEL("CTest::test_isInputShutdown")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    AutoPtr<ISocket> client;
    CSocket::New(lochost, portnum, (ISocket**)&client);

    AutoPtr<ISocket> worker;
    server->Accept((ISocket**)&worker);
    AutoPtr<IInputStream> theInput;
    client->GetInputStream((IInputStream**)&theInput);
    AutoPtr<IOutputStream> theOutput;
    worker->GetOutputStream((IOutputStream**)&theOutput);

    Boolean isflag = FALSE;
    // make sure we get the right answer with newly connected socket
    client->IsInputShutdown(&isflag);
    assert(!isflag);

    // shutdown the output
    client->ShutdownInput();

    // make sure we get the right answer once it is shut down
    client->IsInputShutdown(&isflag);
    assert(isflag);

    client->Close();
    worker->Close();
    server->Close();

    // make sure we get the right answer for closed sockets
    worker->IsInputShutdown(&isflag);
    assert(!isflag);

    theInput->Close();
    theOutput->Close();
    return 0;
}

int CTest::test_isOutputShutdown(int argc, char* argv[])
{
    PEL("CTest::test_isOutputShutdown")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> lochost;
    addhelp->GetLocalHost((IInetAddress**)&lochost);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    CSocket::New(lochost, portnum, (ISocket**)&client);

    AutoPtr<ISocket> worker;
    server->Accept((ISocket**)&worker);
    AutoPtr<IInputStream> theInput;
    client->GetInputStream((IInputStream**)&theInput);
    AutoPtr<IOutputStream> theOutput;
    worker->GetOutputStream((IOutputStream**)&theOutput);

    // make sure we get the right answer with newly connected socket
    Boolean isflag = FALSE;
    worker->IsOutputShutdown(&isflag);
    assert(!isflag);

    // shutdown the output
    worker->ShutdownOutput();

    // make sure we get the right answer once it is shut down
    worker->IsOutputShutdown(&isflag);
    assert(isflag);

    client->Close();
    worker->Close();
    server->Close();

    // make sure we get the right answer for closed sockets
    client->IsOutputShutdown(&isflag);
    assert(!isflag);

    theInput->Close();
    theOutput->Close();
    return 0;
}

int CTest::test_sendUrgentDataI(int argc, char* argv[])
{
    PEL("CTest::test_sendUrgentDataI")
    /*
     * Some platforms may not support urgent data in this case we will not
     * run these tests. For now run on all platforms until we find those
     * that do not support urgent data
     */
    String platform = System::GetProperty(String("os.name"));
    Boolean isflag = FALSE;
    if (platform.Equals("Dummy")) {
        return -1;
    }

    /*
     * Test 1: Validate that when OOBInline is false that any urgent data is
     * silently ignored
     */
    AutoPtr<IInetAddress> localHost;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    addhelp->GetLocalHost((IInetAddress**)&localHost);
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, 5, localHost, (IServerSocket**)&server);
    AutoPtr<ISocketAddress> serverAddress;
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    CInetSocketAddress::New(localHost, portnum, (IInetSocketAddress**)&serverAddress);

    AutoPtr<ISocket> client;
    CSocket::New((ISocket**)&client);
    client->SetOOBInline(false);

    client->Connect(serverAddress);
    AutoPtr<ISocket> worker;
    server->Accept((ISocket**)&worker);
    worker->SetTcpNoDelay(true);
    AutoPtr<IOutputStream> theOutput;
    worker->GetOutputStream((IOutputStream**)&theOutput);

    // Send the regular data
    AutoPtr< ArrayOf<Char16> > byte16 = String("Test").GetChar16s();
    AutoPtr< ArrayOf<Byte> > sendBytes = ArrayOf<Byte>::Alloc(byte16->GetLength());
    for (int i = 0; i < byte16->GetLength(); i++) {
        (*sendBytes)[i] = (*byte16)[i] & 0xff;
    }
    theOutput->WriteBytes(*sendBytes);
    // theOutput->Flush();

    // Send the urgent data byte which should not be received
    AutoPtr< ArrayOf<Char16> > UrgentData = String("UrgentData").GetChar16s();
    worker->SendUrgentData((*UrgentData)[0]);
    theOutput->WriteBytes(*sendBytes);
    worker->ShutdownOutput();
    worker->Close();

    // Try to read the bytes back
    int totalBytesRead = 0;
    AutoPtr< ArrayOf<Byte> > myBytes = ArrayOf<Byte>::Alloc(100);
    AutoPtr<IInputStream> theInput;
    client->GetInputStream((IInputStream**)&theInput);
    while (true) {
        int bytesRead = 0;
        ECode ec = theInput->ReadBytesEx(myBytes, totalBytesRead, myBytes->GetLength() - totalBytesRead, &bytesRead);
        PFL_EX("ec: %p, byteRead: %d", ec, bytesRead)
        if (bytesRead == -1) {
            break;
        }
        totalBytesRead = totalBytesRead + bytesRead;
    }

    client->Close();
    server->Close();

    AutoPtr< ArrayOf<Byte> > expectBytes = ArrayOf<Byte>::Alloc(2 * sendBytes->GetLength());
    expectBytes->Copy(0, sendBytes, 0, sendBytes->GetLength());
    expectBytes->Copy(sendBytes->GetLength(), sendBytes, sendBytes->GetLength());

    AutoPtr< ArrayOf<Byte> > resultBytes = ArrayOf<Byte>::Alloc(totalBytesRead);
    resultBytes->Copy(0, myBytes, 0, totalBytesRead);

    PFL_EX("myBytes: %p , len: %d, totalBytesRead:%d", myBytes.Get(), myBytes->GetLength(), totalBytesRead)

    PFL_EX("expectBytes: %p, len1: %d, resultBytes: %p, len2: %d",
                expectBytes.Get(), expectBytes->GetLength(), resultBytes.Get(), resultBytes->GetLength())
    assert(expectBytes->Equals(resultBytes));

    /*
     * Test 2: Now validate that urgent data is received as expected. Expect
     * that it should be between the two writes.
     */
    CServerSocket::New(0, 5, localHost, (IServerSocket**)&server);
    server->GetLocalPort(&portnum);
    CInetSocketAddress::New(localHost, portnum, (IInetSocketAddress**)&serverAddress);

    CSocket::New((ISocket**)&client);
    client->SetOOBInline(true);

    client->Connect(serverAddress);
    server->Accept((ISocket**)&worker);
    worker->SetTcpNoDelay(true);
    worker->GetOutputStream((IOutputStream**)&theOutput);

    // Send the regular data
    byte16 = String("Test - Urgent Data").GetChar16s();
    sendBytes = ArrayOf<Byte>::Alloc(byte16->GetLength());
    for (int i = 0; i < byte16->GetLength(); i++) {
        (*sendBytes)[i] = (*byte16)[i] & 0xff;
    }
    theOutput->WriteBytes(*sendBytes);

    // Send the urgent data (one byte) which should be received
    client->SetOOBInline(true);
    Byte urgentByte = (*UrgentData)[0];
    worker->SendUrgentData(urgentByte);

    // Send more data, the urgent byte must stay in position
    theOutput->WriteBytes(*sendBytes);
    worker->ShutdownOutput();
    worker->Close();

    // Try to read the bytes back
    totalBytesRead = 0;
    myBytes = ArrayOf<Byte>::Alloc(100);
    client->GetInputStream((IInputStream**)&theInput);
    while (true) {
        int bytesRead = 0;
        theInput->ReadBytesEx(myBytes, totalBytesRead, myBytes->GetLength() - totalBytesRead, &bytesRead);
        if (bytesRead == -1) {
            break;
        }
        totalBytesRead = totalBytesRead + bytesRead;
    }

    client->Close();
    server->Close();

    expectBytes = ArrayOf<Byte>::Alloc(2 * sendBytes->GetLength() + 1);
    expectBytes->Copy(0, sendBytes, 0, sendBytes->GetLength());
    (*expectBytes)[sendBytes->GetLength()] = urgentByte;
    expectBytes->Copy(sendBytes->GetLength() + 1, sendBytes, 0, sendBytes->GetLength());

    resultBytes = ArrayOf<Byte>::Alloc(totalBytesRead);
    resultBytes->Copy(0, myBytes, 0, totalBytesRead);

    assert(expectBytes->Equals(resultBytes));

    /*
     * Test 3: Now validate that urgent data is received as expected. Expect
     * that it should be between the two writes.
     */
    CServerSocket::New(0, 5, localHost, (IServerSocket**)&server);
    server->GetLocalPort(&portnum);
    CInetSocketAddress::New(localHost, portnum, (IInetSocketAddress**)&serverAddress);

    CSocket::New((ISocket**)&client);
    client->SetOOBInline(true);

    client->Connect(serverAddress);
    server->Accept((ISocket**)&worker);
    worker->SetTcpNoDelay(true);
    worker->GetOutputStream((IOutputStream**)&theOutput);

    // Send the regular data
    byte16 = String("Test - Urgent Data").GetChar16s();
    sendBytes = ArrayOf<Byte>::Alloc(byte16->GetLength());
    for (int i = 0; i < byte16->GetLength(); i++) {
        (*sendBytes)[i] = (*byte16)[i] & 0xff;
    }
    theOutput->WriteBytes(*sendBytes);

    // Send the urgent data (one byte) which should be received
    client->SetOOBInline(true);
    Byte urgentByte1 = (*UrgentData)[0];
    Byte urgentByte2 = (*UrgentData)[1];
    worker->SendUrgentData(urgentByte1);
    worker->SendUrgentData(urgentByte2);

    // Send more data, the urgent byte must stay in position
    theOutput->WriteBytes(*sendBytes);
    worker->ShutdownOutput();
    worker->Close();

    // Try to read the bytes back
    totalBytesRead = 0;
    myBytes = ArrayOf<Byte>::Alloc(100);
    client->GetInputStream((IInputStream**)&theInput);
    while (true) {
        int bytesRead = 0;
        theInput->ReadBytesEx(myBytes, totalBytesRead, myBytes->GetLength() - totalBytesRead, &bytesRead);
        if (bytesRead == -1) {
            break;
        }
        totalBytesRead = totalBytesRead + bytesRead;
    }

    client->Close();
    server->Close();

    expectBytes = ArrayOf<Byte>::Alloc(2 * sendBytes->GetLength() + 2);
    expectBytes->Copy(0, sendBytes, 0, sendBytes->GetLength());
    (*expectBytes)[sendBytes->GetLength()] = urgentByte1;
    (*expectBytes)[sendBytes->GetLength() + 1] = urgentByte2;
    expectBytes->Copy(sendBytes->GetLength() + 2, sendBytes, 0, sendBytes->GetLength());
    resultBytes = ArrayOf<Byte>::Alloc(totalBytesRead);
    resultBytes->Copy(0, myBytes, 0, totalBytesRead);

    assert(expectBytes->Equals(resultBytes));

    /*
     * Test 4: Now test the case where there is only urgent data.
     */
    CServerSocket::New(0, 5, localHost, (IServerSocket**)&server);
    server->GetLocalPort(&portnum);
    CInetSocketAddress::New(localHost, portnum, (IInetSocketAddress**)&serverAddress);

    CSocket::New((ISocket**)&client);
    client->SetOOBInline(true);

    client->Connect(serverAddress);
    server->Accept((ISocket**)&worker);
    worker->SetTcpNoDelay(true);

    // Send the urgent data (one byte) which should be received
    client->SetOOBInline(true);
    urgentByte = (*UrgentData)[0];
    worker->SendUrgentData(urgentByte);
    worker->Close();

    // Try to read the bytes back
    client->GetInputStream((IInputStream**)&theInput);
    int byteRead = 0;
    theInput->Read(&byteRead);

    client->Close();
    server->Close();

    assert((int) (urgentByte & 0xff) == byteRead);
    return 0;
}

int CTest::test_setKeepAliveZ(int argc, char* argv[])
{
    PEL("CTest::test_setKeepAliveZ")
    // class TestSocket extends Socket {
    //     public TestSocket(SocketImpl impl) throws SocketException {
    //         super(impl);
    //     }
    // };

    // There is not really a good test for this as it is there to detect
    // crashed machines. Just make sure we can set it
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> localHost;
    addhelp->GetLocalHost((IInetAddress**)&localHost);
    Int32 portnum = 0;
    server->GetLocalPort(&portnum);
    CSocket::New(localHost, portnum, (ISocket**)&client);

    client->SetKeepAlive(true);
    client->SetKeepAlive(false);
    client->Close();
    server->Close();

    // Regression test for HARMONY-1136
    // new TestSocket((SocketImpl) null).setKeepAlive(true);
    return 0;
}

int CTest::test_setOOBInlineZ(int argc, char* argv[])
{
    PEL("CTest::test_setOOBInlineZ")
    AutoPtr<ISocket> theSocket;
    CSocket::New((ISocket**)&theSocket);
    theSocket->SetOOBInline(true);
    Boolean isflag = FALSE;
    theSocket->GetOOBInline(&isflag);
    assert(isflag);
    return 0;
}

int CTest::test_setPerformancePreference_Int_Int_Int(int argc, char* argv[])
{
    PEL("CTest::test_setPerformancePreference_Int_Int_Int")
    AutoPtr<ISocket> theSocket;
    CSocket::New((ISocket**)&theSocket);
    theSocket->SetPerformancePreferences(1, 1, 1);
    return 0;
}

int CTest::test_setReceiveBufferSizeI(int argc, char* argv[])
{
    PEL("CTest::test_setReceiveBufferSizeI")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> localHost;
    Int32 portnum = 0;
    addhelp->GetLocalHost((IInetAddress**)&localHost);
    server->GetLocalPort(&portnum);
    CSocket::New(localHost, portnum, (ISocket**)&client);

    client->SetReceiveBufferSize(130);
    Int32 bufsize = 0;
    client->GetReceiveBufferSize(&bufsize);
    assert(bufsize >= 130);

    client->Close();
    server->Close();
    return 0;
}

int CTest::test_setReuseAddressZ(int argc, char* argv[])
{
    PEL("CTest::test_setReuseAddressZ")
    AutoPtr<ISocket> theSocket;
    CSocket::New((ISocket**)&theSocket);
    theSocket->SetReuseAddress(false);
    // Bind to any available port on the given address
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> localHost;
    Int32 portnum = 0;
    addhelp->GetLocalHost((IInetAddress**)&localHost);

    AutoPtr<IInetSocketAddress> netsockadd;
    CInetSocketAddress::New(localHost, 0, (IInetSocketAddress**)&netsockadd);
    theSocket->Bind(netsockadd);
    AutoPtr<IInetSocketAddress> localAddress1;
    AutoPtr<IInetAddress> theadd;
    theSocket->GetLocalAddress((IInetAddress**)&theadd);
    theSocket->GetLocalPort(&portnum);
    CInetSocketAddress::New(theadd, portnum, (IInetSocketAddress**)&localAddress1);

    AutoPtr<ISocket> theSocket2;
    CSocket::New((ISocket**)&theSocket2);
    theSocket2->SetReuseAddress(false);

    /*
     * Try to invoke a bind while the port is busy (TIME_WAIT). Note
     * that we may not succeed, which will cause the test to pass
     * without testing the reuseaddr behavior.
     */
    theSocket->Close();
    theSocket2->Bind(localAddress1);

    theSocket2->Close();
    return 0;
}

int CTest::test_setSendBufferSizeI(int argc, char* argv[])
{
    PEL("CTest::test_setSendBufferSizeI")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> localHost;
    Int32 portnum = 0;
    addhelp->GetLocalHost((IInetAddress**)&localHost);
    server->GetLocalPort(&portnum);
    CSocket::New(localHost, portnum, (ISocket**)&client);
    client->SetSendBufferSize(134);
    Int32 bufsize = 0;
    client->GetSendBufferSize(&bufsize);
    assert(bufsize >= 134);
    client->Close();
    server->Close();
    return 0;
}

int CTest::test_setSocketImplFactoryLjava_net_SocketImplFactory(int argc, char* argv[])
{
    PEL("CTest::test_setSocketImplFactoryLjava_net_SocketImplFactory")
    // Cannot test as setting will cause the factory to be changed for
    // all subsequent sockets
    return 0;
}

int CTest::test_setSoLingerZI(int argc, char* argv[])
{
    PEL("CTest::test_setSoLingerZI")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> localHost;
    Int32 portnum = 0;
    addhelp->GetLocalHost((IInetAddress**)&localHost);
    server->GetLocalPort(&portnum);
    CSocket::New(localHost, portnum, (ISocket**)&client);
    client->SetSoLinger(true, 500);
    Int32 soint = 0;
    client->GetSoLinger(&soint);
    assert(500 == soint);
    client->SetSoLinger(false, 0);
    client->Close();
    server->Close();
    return 0;
}

int CTest::test_setSoTimeoutI(int argc, char* argv[])
{
    PEL("CTest::test_setSoTimeoutI")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> localHost;
    Int32 portnum = 0;
    addhelp->GetLocalHost((IInetAddress**)&localHost);
    server->GetLocalPort(&portnum);
    CSocket::New(localHost, portnum, (ISocket**)&client);
    client->SetSoTimeout(100);
    Int32 timeint = 0;
    client->GetSoTimeout(&timeint);
    assert(100 == timeint);
    client->Close();
    server->Close();
    return 0;
}

int CTest::test_setTcpNoDelayZ(int argc, char* argv[])
{
    PEL("CTest::test_setTcpNoDelayZ")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> localHost;
    Int32 portnum = 0;
    addhelp->GetLocalHost((IInetAddress**)&localHost);
    server->GetLocalPort(&portnum);
    CSocket::New(localHost, portnum, (ISocket**)&client);

    Boolean bool1 = FALSE;
    client->GetTcpNoDelay(&bool1);
    bool1 = !bool1;
    client->SetTcpNoDelay(bool1);
    Boolean isflag = FALSE;
    client->GetTcpNoDelay(&isflag);
    assert(isflag == bool1);

    client->Close();
    server->Close();
    return 0;
}

int CTest::test_setTrafficClassI(int argc, char* argv[])
{
    PEL("CTest::test_setTrafficClassI")
    int IPTOS_LOWCOST = 0x2;
    int IPTOS_RELIABILTY = 0x4;
    int IPTOS_THROUGHPUT = 0x8;
    int IPTOS_LOWDELAY = 0x10;

    AutoPtr<ISocket> theSocket;
    CSocket::New((ISocket**)&theSocket);

    // validate that value set must be between 0 and 255
    // try {
    ECode ec = theSocket->SetTrafficClass(256);
    if (ec != NOERROR) {
        printf("No exception was thrown when traffic class set to 256\n");
    }
    // } catch (IllegalArgumentException expected) {
    // }

    // try {
    ec = theSocket->SetTrafficClass(-1);
    if (ec != NOERROR) {
        printf("No exception was thrown when traffic class set to -1\n");
    }
    // } catch (IllegalArgumentException expected) {
    // }

    // now validate that we can set it to some good values
    theSocket->SetTrafficClass(IPTOS_LOWCOST);
    theSocket->SetTrafficClass(IPTOS_RELIABILTY);
    theSocket->SetTrafficClass(IPTOS_THROUGHPUT);
    theSocket->SetTrafficClass(IPTOS_LOWDELAY);
    return 0;
}

int CTest::test_shutdownInput(int argc, char* argv[])
{
    PEL("CTest::test_shutdownInput")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    int port = 0;
    server->GetLocalPort(&port);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> localHost;
    Int32 portnum = 0;
    addhelp->GetLocalHost((IInetAddress**)&localHost);
    CSocket::New(localHost, port, (ISocket**)&client);

    AutoPtr<ISocket> worker;
    server->Accept((ISocket**)&worker);
    worker->SetTcpNoDelay(true);

    AutoPtr<IInputStream> theInput;
    client->GetInputStream((IInputStream**)&theInput);
    AutoPtr<IOutputStream> theOutput;
    worker->GetOutputStream((IOutputStream**)&theOutput);

    // shutdown the input
    client->ShutdownInput();

    // send the regular data
    AutoPtr< ArrayOf<Char16> > byte16 = String("Test").GetChar16s();
    AutoPtr< ArrayOf<Byte> > sendBytes = ArrayOf<Byte>::Alloc(byte16->GetLength());
    for (int i = 0; i < byte16->GetLength(); i++) {
        (*sendBytes)[i] = (*byte16)[i] & 0xff;
    }
    theOutput->WriteBytes(*sendBytes);
    // theOutput.flush();

    // RI fails here. It is a RI bug not to return 0 to indicate EOF
    Int32 avalue = 0;
    theInput->Available(&avalue);
    assert(0 == avalue);

    client->Close();
    server->Close();

    // Regression test for HARMONY-2944
    // Port 0 is not allowed to be used in connect() on some platforms,
    // Since server has been closed here, so the port is free now
    AutoPtr<ISocket> s;
    CSocket::New(String("0.0.0.0"), port, false, (ISocket**)&s);
    s->ShutdownInput();
    // try {
    ECode ec = s->ShutdownInput();
    if (ec != NOERROR) {
        printf("should throw SocketException\n");
    }
    // } catch (SocketException se) {
    //     // Expected
    // }
    s->Close();
    return 0;
}

int CTest::test_shutdownOutput(int argc, char* argv[])
{
    PEL("CTest::test_shutdownOutput")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    int port = 0;
    server->GetLocalPort(&port);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> localHost;
    Int32 portnum = 0;
    addhelp->GetLocalHost((IInetAddress**)&localHost);
    CSocket::New(localHost, port, (ISocket**)&client);

    AutoPtr<ISocket> worker;
    server->Accept((ISocket**)&worker);
    AutoPtr<IOutputStream> theOutput;
    worker->GetOutputStream((IOutputStream**)&theOutput);

    // shutdown the output
    worker->ShutdownOutput();

    // send the regular data
    AutoPtr< ArrayOf<Char16> > byte16 = String("Test").GetChar16s();
    AutoPtr< ArrayOf<Byte> > sendBytes = ArrayOf<Byte>::Alloc(byte16->GetLength());
    for (int i = 0; i < byte16->GetLength(); i++) {
        (*sendBytes)[i] = (*byte16)[i] & 0xff;
    }
    // try {
    ECode ec = theOutput->WriteBytes(*sendBytes);
    // theOutput->Flush();
    if (ec != NOERROR) {
        printf("No exception when writing on socket with output shutdown\n");
    }
    // } catch (IOException e) {
    //     // Expected
    // }

    client->Close();
    server->Close();

    // Regression test for HARMONY-2944
    // Port 0 is not allowed to be used in connect() on some platforms,
    // Since server has been closed here, so the port is free now
    AutoPtr<ISocket> s;
    CSocket::New(String("0.0.0.0"), port, false, (ISocket**)&s);
    s->ShutdownOutput();
    // try {
    ec = s->ShutdownOutput();
    if (ec != NOERROR) {
        printf("should throw SocketException\n");
    }
    // } catch (SocketException se) {
    //     // Expected
    // }
    s->Close();
    return 0;
}

int CTest::test_toString(int argc, char* argv[])
{
    PEL("CTest::test_toString")
    AutoPtr<IServerSocket> server;
    CServerSocket::New(0, (IServerSocket**)&server);
    AutoPtr<ISocket> client;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    AutoPtr<IInetAddress> localHost;
    Int32 portnum = 0;
    addhelp->GetLocalHost((IInetAddress**)&localHost);
    server->GetLocalPort(&portnum);
    CSocket::New(localHost, portnum, (ISocket**)&client);
    // RI has "addr" and "localport" instead of "address" and "localPort".
    String localHostStr;
    String portnumstr;
    String localPortStr;
    localHost->ToString(&localHostStr);
    client->GetPort(&portnum);
    portnumstr = StringUtils::Int32ToString(portnum);
    client->GetLocalPort(&portnum);
    localPortStr = StringUtils::Int32ToString(portnum);
    String expected = String("Socket[address=")
                    + localHostStr
                    + String(",port=")
                    + portnumstr
                    + String(",localPort=")
                    + localPortStr
                    + String("]");
    String clientstr;
    // client->ToString(&clientstr);
    // assert(expected == clientstr);
    client->Close();
    server->Close();
    return 0;
}

void CTest::connectTestImpl(int ssConsType)
{
    PEL("CTest::connectTestImpl")
    AutoPtr<ServerThread> server = new ServerThread(this);
    server->serverSocketConstructor = ssConsType;
    AutoPtr<IThread> serverThread;
    CThread::New((IRunnable*) server->Probe(EIID_IRunnable), (IThread**)&serverThread);
PFL
    serverThread->Start();
PFL
    server->WaitCreated();

PFL
    AutoPtr<ClientThread> client = new ClientThread(this);
PFL
    AutoPtr<IThread> clientThread;
    CThread::New((IRunnable*)client->Probe(EIID_IRunnable), (IThread**)&clientThread);
PFL
    clientThread->Start();
    // try {
PFL
    serverThread->Join();
PFL
    clientThread->Join();

    // } catch (InterruptedException e) {
    //     e.printStackTrace();
    // }
}

void CTest::tearDown()
{
    // try {
    if (t != NULL) {
        t->Interrupt();
    }
    // } catch (Exception e) {
    // }
    t = NULL;
    interrupted = false;
}

Boolean CTest::isUnix()
{
    String osName = System::GetProperty(String("os.name"));

    // only comparing ASCII, so assume english locale
    osName = (osName.IsNull() ? String(NULL) : osName.ToLowerCase());

    if (!osName.IsNull() && osName.StartWith("windows")) { //$NON-NLS-1$
        return false;
    }
    return true;
}
