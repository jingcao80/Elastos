#include "test.h"

int CTest::Support_PortManager::lastAssignedPort = somewhatRandomPort();
Boolean CTest::Support_PortManager::failedOnce = false;
Mutex CTest::Support_PortManager::mLock;

int CTest::Support_PortManager::getNextPort()
{
   if (!failedOnce) {
        AutoPtr<IServerSocket> ss;
        ECode ec = CServerSocket::New(0, (IServerSocket**)&ss);
        if (ec != NOERROR) {
            goto final;
        }
        Int32 port;
        ec = ss->GetLocalPort(&port);
        if (ec != NOERROR) {
            goto final;
        }

        ec = ss->Close();
        if (ec != NOERROR) {
            goto final;
        }
        return port;
    }
final:
    failedOnce = true;
    return getNextPort_unsafe();
}

int CTest::Support_PortManager::getNextPortForUDP()
{
     return getNextPortsForUDP(1)[0];
}

int* CTest::Support_PortManager::getNextPortsForUDP(int num)
{
    if (num <= 0) {
        // throw new IllegalArgumentException("Invalid ports number: " + num);
        return NULL;
    }
    AutoPtr< ArrayOf<IDatagramSocket*> > dss =
        ArrayOf<IDatagramSocket*>::Alloc(num);
    int* ports = new int[num];

    // try {
    for (int i = 0; i < num; ++i) {
        AutoPtr<IDatagramSocket> res;
        CDatagramSocket::New(0, (IDatagramSocket**)&res);
        dss->Set(i, res);
        (*dss)[i]->GetLocalPort(&ports[i]);
    }
    // } catch (Exception ex) {
    //     throw new Error("Unable to get " + num + " ports for UDP: " + ex);
    // } finally {
    for (int i = 0; i < num; ++i) {
        if ((*dss)[i] != NULL) {
            (*dss)[i]->Close();
        }
    }
    // }
    return ports;
}

int CTest::Support_PortManager::getNextPort_unsafe()
{
    if (++lastAssignedPort > 65534) {
        lastAssignedPort = 6000;
    }
    return lastAssignedPort;
}

int CTest::Support_PortManager::somewhatRandomPort()
{
    AutoPtr<ITimeZoneHelper> timeZoneHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&timeZoneHelper);
    AutoPtr<ITimeZone> timeZone;
    timeZoneHelper->GetTimeZone(String("UTC"), (ITimeZone**)&timeZone);
    AutoPtr<ICalendarHelper> calendarHelper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&calendarHelper);
    AutoPtr<ICalendar> c;
    calendarHelper->GetInstanceEx2(timeZone, (ICalendar**)&c);
    Int32 minutes;
    c->Get(ICalendar::MINUTE, &minutes);
    Int32 seconds;
    c->Get(ICalendar::SECOND, &seconds);

    return 6000 + (1000 * minutes) + ((seconds / 6) * 100);
}

CTest::DatagramServer::DatagramServer(int aPort, IInetAddress* address)
    : running(TRUE)
    , rdp(NULL)
{
    rbuf = ArrayOf<Byte>::Alloc(512);
    (*rbuf)[0] = -1;
    CDatagramPacket::New(rbuf, rbuf->GetLength(), (IDatagramPacket**)&rdp);
    CDatagramSocket::New(aPort, address, (IDatagramSocket**)&ms);
    ms->SetSoTimeout(2000);
}

ECode CTest::DatagramServer::run()
{
    // try {
        while (running) {
            // try {
                ms->Receive(rdp);
                ms->Send(rdp);
            // } catch (java.io.InterruptedIOException e)
            // {
            //     Thread.yield();
            // };
        };

    // } catch (java.io.IOException e) {
        // System.out.println("DatagramServer server failed: " + e);
    // } finally {
            ms->Close();
    // }
}

ECode CTest::DatagramServer::stopServer()
{
    running = FALSE;
}

CTest::CTest()
    : testString(String("Test String"))
    , retval(String("Bogus retval"))
{}

CTest::~CTest()
{
    if (ds != NULL) {
        ds->Close();
    }

    if (sds != NULL) {
        sds->Close();
    }
}

//Num: Test1
int CTest::test_Constructor(int argc, char* argv[])
{
    ECode ec = CDatagramSocket::New((IDatagramSocket**)&ds);
    assert(ec == NOERROR);
}

//Num: Test2
int CTest::test_ConstructorI(int argc, char* argv[])
{
    Int32 portNumber = Support_PortManager::getNextPortForUDP();
    CDatagramSocket::New(portNumber, (IDatagramSocket**)&ds);
    Int32 testPort;
    ds->GetLocalPort(&testPort);
    assert(testPort == portNumber);
}

//Num: Test3
int CTest::test_ConstructorILjava_net_InetAddress(int argc, char* argv[])
{
    // Test for method java.net.DatagramSocket(int, java.net.InetAddress)
    Int32 portNumber = Support_PortManager::getNextPortForUDP();
    PFL_EX("test3, portNumber=%d", portNumber);
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> inetAddress;
    inetAddressHelper->GetLocalHost((IInetAddress**)&inetAddress);
    CDatagramSocket::New(portNumber, inetAddress, (IDatagramSocket**)&ds);
    Int32 port;
    ds->GetLocalPort(&port);
    assert(port == portNumber);

    AutoPtr<IInetAddress> inet;
    ds->GetLocalAddress((IInetAddress**)&inet);
    Boolean flag = FALSE;
    inetAddress->Equals(inet, &flag);
    assert(flag == TRUE);

    AutoPtr<IDatagramSocket> ids;
    ECode ec = CDatagramSocket::New(1, inetAddress, (IDatagramSocket**)&ids);
    assert(ec == NOERROR);
}

//Num: Test4
int CTest::test_close(int argc, char* argv[])
{
    Int32 portNumber = Support_PortManager::getNextPortForUDP();
    PFL_EX("test4, portNumber=%d", portNumber);
    CDatagramSocket::New(portNumber, (IDatagramSocket**)&ds);
    String testString("Test String");
    AutoPtr<ArrayOf<Char32> > testStringArray = testString.GetChars(0);
    Int32 byteLength = testString.GetByteLength();
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(byteLength);
    for (Int32 i = 0; i < byteLength; ++i) {
        (*data)[i] = (*testStringArray)[i];
    }

    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> inetAddress;
    inetAddressHelper->GetLocalHost((IInetAddress**)&inetAddress);

    CDatagramPacket::New(data, 11, inetAddress, 0, (IDatagramPacket**)&dp);
    ds->Close();

    ECode ec = ds->Send(dp);
}

//Num: Test5
int CTest::test_connectLjava_net_InetAddressI(int argc, char* argv[])
{
    CDatagramSocket::New((IDatagramSocket**)&ds);
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> inetAddress;
    inetAddressHelper->GetLocalHost((IInetAddress**)&inetAddress);
    ds->ConnectEx(inetAddress, 0);
    AutoPtr<IInetAddress> inet;
    ds->GetInetAddress((IInetAddress**)&inet);
    Boolean flag = FALSE;
    inet->Equals(inetAddress, &flag);
    assert(flag == TRUE);
    Int32 port;
    ds->GetPort(&port);
    assert(port == 0);
    // ds->Disconnect();

    CDatagramSocket::New((IDatagramSocket**)&ds);
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    inetAddressHelper->GetByName(String("FE80:0000:0000:0000:020D:60FF:FE0F:A776%4"), (IInetAddress**)&inetAddress);
    Int32 portNumber;
    portNumber = Support_PortManager::getNextPortForUDP();
    ds->ConnectEx(inetAddress, portNumber);
    AutoPtr<IInetAddress> address;
    ds->GetInetAddress((IInetAddress**)&address);
    address->Equals(inetAddress, &flag);
    assert(flag == TRUE);
    ds->GetPort(&port);
    assert(port == portNumber);
    // ds->Disconnect();

    // Create a connected datagram socket to test
    // PlainDatagramSocketImpl.peek()
    AutoPtr<IInetAddress> localhost;
    inetAddressHelper->GetLocalHost((IInetAddress**)&localhost);
    CDatagramSocket::New((IDatagramSocket**)&ds);
    ds->GetLocalPort(&port);
    ds->ConnectEx(localhost, port);
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(10);
    AutoPtr<IDatagramPacket> send;
    CDatagramPacket::New(bytes, 10, localhost, port, (IDatagramPacket**)&send);
    ds->Send(send);

    AutoPtr<IDatagramPacket> receive;
    AutoPtr<ArrayOf<Byte> > bytes1 = ArrayOf<Byte>::Alloc(20);
    CDatagramPacket::New(bytes1, 20, (IDatagramPacket**)&receive);
    ds->SetSoTimeout(2000);
    ds->Receive(receive);
    ds->Close();
    Int32 rcLength;
    receive->GetLength(&rcLength);
    assert(rcLength == 10);

    receive->GetAddress((IInetAddress**)&address);
    Boolean flag1 = FALSE;
    address->Equals(localhost, &flag1);
    assert(flag1 == TRUE);

    // validate that we get the PortUnreachable exception if we try to
    // send a dgram to a server that is not running and then do a recv
    // try {

    CDatagramSocket::New((IDatagramSocket**)&ds);
    inetAddressHelper->GetLocalHost((IInetAddress**)&inetAddress);
    portNumber = Support_PortManager::getNextPortForUDP();
    ds->ConnectEx(inetAddress, portNumber);
    AutoPtr<ArrayOf<Byte> > bytes2 = ArrayOf<Byte>::Alloc(10);
    CDatagramPacket::New(bytes2, 10, (IDatagramPacket**)&send);
    ds->Send(send);
    AutoPtr<ArrayOf<Byte> > bytes3 = ArrayOf<Byte>::Alloc(20);
    CDatagramPacket::New(bytes3, 20, (IDatagramPacket**)&receive);
    ds->SetSoTimeout(10000);
    ds->Receive(receive);
    ds->Close();
    // } catch (PortUnreachableException e) {
        //Expected
    // }

    // validate that we can send/receive with datagram sockets connected at
    // the native level
    DatagramServer* server = NULL;
    int* ports = Support_PortManager::getNextPortsForUDP(3);
    int serverPortNumber = ports[0];
    CDatagramSocket::New(ports[1], (IDatagramSocket**)&ds);
    AutoPtr<IDatagramSocket> ds2;
    CDatagramSocket::New(ports[2], (IDatagramSocket**)&ds2);
    server = new DatagramServer(serverPortNumber, localhost);
    ECode ec = server->Start();
    Thread::Sleep(1000);

    ds->GetLocalPort(&port);
    ds->ConnectEx(localhost, serverPortNumber);
    AutoPtr<ArrayOf<Byte> > sendBytes = ArrayOf<Byte>::Alloc(5);
    CDatagramPacket::New(sendBytes, sendBytes->GetLength(), (IDatagramPacket**)&send);
    ds->Send(send);
    AutoPtr<ArrayOf<Byte> > bytes4 = ArrayOf<Byte>::Alloc(20);
    CDatagramPacket::New(bytes4, 20, (IDatagramPacket**)&receive);
    ds->SetSoTimeout(2000);
    ds->Receive(receive);
    ds->Close();
    receive->GetLength(&rcLength);
    assert(rcLength == sendBytes->GetLength());
    receive->GetAddress((IInetAddress**)&address);
    Boolean flag2 = FALSE;
    address->Equals(localhost, &flag2);
    assert(flag2 == TRUE);
    if (server != NULL) {
        server->stopServer();
    }
}

//Num: Test6
int CTest::test_disconnect(int argc, char* argv[])
{
    CDatagramSocket::New((IDatagramSocket**)&ds);
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> inetAddress;
    inetAddressHelper->GetLocalHost((IInetAddress**)&inetAddress);
    Int32 portNumber = Support_PortManager::getNextPortForUDP();
    ds->ConnectEx(inetAddress, portNumber);
    ds->Disconnect();
    AutoPtr<IInetAddress> inet;
    ds->GetInetAddress((IInetAddress**)&inet);
    assert(NULL == inet);

    Int32 port;
    ds->GetPort(&port);
    assert(-1 == port);

    CDatagramSocket::New((IDatagramSocket**)&ds);
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    inetAddressHelper->GetByName(String("2001:4860:8004::67"), (IInetAddress**)&inetAddress);
    portNumber = Support_PortManager::getNextPortForUDP();
    ds->ConnectEx(inetAddress, portNumber);
    ds->Disconnect();

    AutoPtr<IInetAddress> address;
    ds->GetInetAddress((IInetAddress**)&address);
    assert(NULL == address);

    ds->GetPort(&port);
    assert(-1 == port);
}

//Num: Test7
int CTest::test_getInetAddress(int argc, char* argv[])
{
    AutoPtr<IVector> ias;
    CVector::New((IVector**)&ias);

    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> inetAddress;
    inetAddressHelper->GetLocalHost((IInetAddress**)&inetAddress);
    Boolean flag = FALSE;
    inetAddressHelper->GetByName(String("2001:4860:8004::67"), (IInetAddress**)&inetAddress);
    ias->Add(inetAddress, &flag);
    assert(TRUE == flag);
    inetAddressHelper->GetByName(String("localhost"), (IInetAddress**)&inetAddress);
    ias->Add(inetAddress, &flag);
    assert(TRUE == flag);
    ias->Add(inetAddress, &flag);
    assert(TRUE == flag);


    inetAddressHelper->GetByName(String("127.0.0.1"), (IInetAddress**)&inetAddress);
    Int32 size;
    ias->Capacity(&size);
    for (Int32 i = 0; i < size; ++i)
    {
        Int32 portNumber = Support_PortManager::getNextPortForUDP();
        AutoPtr<IDatagramSocket> ds;
        CDatagramSocket::New((IDatagramSocket**)&ds);
        AutoPtr<IInterface> interf;
        ias->ElementAt(i, (IInterface**)&interf);
        AutoPtr<IInetAddress> ia = (IInetAddress*)IInetAddress::Probe(interf);
        ds->ConnectEx(ia,portNumber);
        ds->GetInetAddress((IInetAddress**)&inetAddress);
        inetAddress->Equals(ia, &flag);
        assert(flag == TRUE);
    }
    // ds->Disconnect();
    ds->Close();
}

//Num: Test8
int CTest::test_getLocalPort(int argc, char* argv[])
{
    Int32 portNumber = Support_PortManager::getNextPortForUDP();
    CDatagramSocket::New(portNumber, (IDatagramSocket**)&ds);
    Int32 port;
    ds->GetLocalPort(&port);
    assert(port == portNumber);
}

//Num: Test9
int CTest::test_getPort(int argc, char* argv[])
{
    Int32 portNumber = Support_PortManager::getNextPortForUDP();
    AutoPtr<IDatagramSocket> theSocket;
    CDatagramSocket::New(portNumber, (IDatagramSocket**)&theSocket);
    Int32 port;
    theSocket->GetPort(&port);
    assert(-1 == port);

    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> inetAddr;
    inetAddressHelper->GetLocalHost((IInetAddress**)&inetAddr);
    theSocket->ConnectEx(inetAddr, portNumber);
    theSocket->GetPort(&port);
    assert(port == portNumber);
}

//Num: Test10
int CTest::test_getReceiveBufferSize(int argc, char* argv[])
{
    Int32 portNumber = Support_PortManager::getNextPortForUDP();
    CDatagramSocket::New(portNumber, (IDatagramSocket**)&ds);
    ds->SetReceiveBufferSize(130);
    Int32 size;
    ds->GetReceiveBufferSize(&size);
    assert(size >= 130);
    ds->Close();
}

//Num: Test11
int CTest::test_getSendBufferSize(int argc, char* argv[])
{
    Int32 portNumber = Support_PortManager::getNextPortForUDP();
    CDatagramSocket::New(portNumber, (IDatagramSocket**)&ds);
    ds->SetSendBufferSize(134);
    Int32 size;
    ds->GetSendBufferSize(&size);
    assert(size >= 134);
    ds->Close();
}

//Num: Test12
int CTest::test_getSoTimeout(int argc, char* argv[])
{
    AutoPtr<IDatagramSocket> ds;
    CDatagramSocket::New((IDatagramSocket**)&ds);
    ds->SetSoTimeout(100);
    Int32 ms;
    ds->GetSoTimeout(&ms);
    assert(ms == 100);
}

//Num: Test13
int CTest::test_receiveLjava_net_DatagramPacket(int argc, char* argv[])
{
    return 0;
}

//Num: Test14
int CTest::test_sendLjava_net_DatagramPacket(int argc, char* argv[])
{
    return 0;
}

//Num: Test15
int CTest::test_setSendBufferSizeI(int argc, char* argv[])
{
    Int32 portNumber = Support_PortManager::getNextPortForUDP();
    CDatagramSocket::New(portNumber, (IDatagramSocket**)&ds);
    ds->SetSendBufferSize(134);
    Int32 size;
    ds->GetSendBufferSize(&size);
    assert(size >= 134);
    ds->Close();
}

//Num: Test16
int CTest::test_setReceiveBufferSizeI(int argc, char* argv[])
{
    Int32 portNumber = Support_PortManager::getNextPortForUDP();
    CDatagramSocket::New(portNumber, (IDatagramSocket**)&ds);
    ds->SetReceiveBufferSize(130);
    Int32 size;
    ds->GetReceiveBufferSize(&size);
    assert(size >= 130);
}

//Num: Test17
int CTest::test_ConstructorLjava_net_SocketAddress(int argc, char* argv[])
{
    Int32 portNumber = Support_PortManager::getNextPortForUDP();
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> localhost;
    inetAddressHelper->GetLocalHost((IInetAddress**)&localhost);
    AutoPtr<IInetSocketAddress> inetSocketAddress;
    CInetSocketAddress::New(localhost, portNumber, (IInetSocketAddress**)&inetSocketAddress);
    AutoPtr<ISocketAddress> socketAddress = ISocketAddress::Probe(inetSocketAddress);
    CDatagramSocket::New(socketAddress, (IDatagramSocket**)&ds);
    Boolean flag = FALSE;
    ds->GetBroadcast(&flag);
    assert(flag == TRUE);

    Int32 port;
    ds->GetLocalPort(&port);
    assert(port == portNumber);

    AutoPtr<IInetAddress> localAddress;
    ds->GetLocalAddress((IInetAddress**)&localAddress);
    localhost->Equals(localAddress, &flag);
    assert(flag == TRUE);

    //regression for Harmony-894
    AutoPtr<ISocketAddress> isa0;
    CDatagramSocket::New(isa0, (IDatagramSocket**)&ds);
    ds->GetBroadcast(&flag);
    assert(flag == TRUE);
}

//Num: Test18
int CTest::test_bindLjava_net_SocketAddress(int argc, char* argv[])
{
    int* ports = Support_PortManager::getNextPortsForUDP(3);
    Int32 serverPortNumber = ports[1];

    // now create a socket that is not bound and then bind it
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> localhost;
    inetAddressHelper->GetLocalHost((IInetAddress**)&localhost);
    AutoPtr<IInetSocketAddress> localAddress1;
    CInetSocketAddress::New(localhost, ports[0], (IInetSocketAddress**)&localAddress1);
    AutoPtr<IDatagramSocket> theSocket;
    CDatagramSocket::New(localAddress1, (IDatagramSocket**)&theSocket);
    AutoPtr<ISocketAddress> socketAddress;
    theSocket->GetLocalSocketAddress((ISocketAddress**)&socketAddress);

    // validate that the localSocketAddress reflects the address we bound to
    AutoPtr<IObject> obj1 = IObject::Probe(localAddress1);
    AutoPtr<IObject> obj2 = IObject::Probe(socketAddress);
    Boolean flag = FALSE;
    obj1->Equals(obj2, &flag);
    assert(flag == TRUE);

    // now make sure that datagrams sent from this socket appear to come
    // from the address we bound to
    AutoPtr<IInetSocketAddress> localAddress2;
    CInetSocketAddress::New(localhost, ports[2], (IInetSocketAddress**)&localAddress2);
    AutoPtr<IDatagramSocket> ds;
    AutoPtr<ISocketAddress> sa;
    CDatagramSocket::New(sa, (IDatagramSocket**)&ds);
    AutoPtr<ISocketAddress> _localAddress2 = (ISocketAddress*)ISocketAddress::Probe(localAddress2);
    ds->Bind(_localAddress2);

//--------------------need server-------------------------------------------//
/*    DatagramServer server = new DatagramServer(serverPortNumber, localHost);
    server.start();
    Thread.sleep(1000);

    ds.connect(new InetSocketAddress(localHost, serverPortNumber));

    byte[] sendBytes = { 'T', 'e', 's', 't', 0 };
    DatagramPacket send = new DatagramPacket(sendBytes, sendBytes.length);
    ds.send(send);
    Thread.sleep(1000);
    ds.close();
    // Check that the address in the packet matches the bound address.
    assertEquals(localAddress2, server.rdp.getSocketAddress());

    if (server != null) {
        server.stopServer();
    }*/

  }

//Num: Test19
int CTest::test_bindLjava_net_SocketAddress_null(int argc, char* argv[])
{
    // validate if we pass in null that it picks an address for us.
    AutoPtr<IDatagramSocket> theSocket;
    AutoPtr<ISocketAddress> sa;
    CDatagramSocket::New(sa, (IDatagramSocket**)&theSocket);
    theSocket->Bind(NULL);
    theSocket->GetLocalSocketAddress((ISocketAddress**)&sa);
    ECode ec = theSocket->Close();
    assert(ec == NOERROR);
}

//Num: Test20
int CTest::test_bindLjava_net_SocketAddress_bad_address(int argc, char* argv[])
{
    AutoPtr<IDatagramSocket> theSocket;
    AutoPtr<ISocketAddress> sa;
    CDatagramSocket::New(sa, (IDatagramSocket**)&theSocket);
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<ArrayOf<Byte> > nonLocalAddressBytes;
    nonLocalAddressBytes->Set(0, 1);
    nonLocalAddressBytes->Set(1, 0);
    nonLocalAddressBytes->Set(2, 0);
    nonLocalAddressBytes->Set(3, 0);
    AutoPtr<IInetAddress> badAddress;
    inetAddressHelper->GetByAddress(nonLocalAddressBytes, (IInetAddress**)&badAddress);

    AutoPtr<IInetSocketAddress> isa;
    Int32 port = Support_PortManager::getNextPortForUDP();
    CInetSocketAddress::New(badAddress, port, (IInetSocketAddress**)&isa);
    AutoPtr<ISocketAddress> _isa = (ISocketAddress*)ISocketAddress::Probe(isa);
    theSocket->Bind(_isa);
    theSocket->Close();
}

//Num: Test21
int CTest::test_bindLjava_net_SocketAddress_address_in_use(int argc, char* argv[])
{
    // Address that we have already bound to
    AutoPtr<ArrayOf<Int32> > ports = ArrayOf<Int32>::Alloc(2);
    (*ports)[0] = 46297;
    (*ports)[1] = 48105;
    // Support_PortManager::GetNextPortsForUDP(2, (ArrayOf<Int32>**)&ports);
    AutoPtr<IDatagramSocket> theSocket1;
    AutoPtr<ISocketAddress> sa;
    CDatagramSocket::New(sa, (IDatagramSocket**)&theSocket1);
    AutoPtr<IDatagramSocket> theSocket2;
    CDatagramSocket::New((*ports)[0], (IDatagramSocket**)&theSocket2);

    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> inetAddr;
    inetAddressHelper->GetLocalHost((IInetAddress**)&inetAddr);

    AutoPtr<IInetSocketAddress> theAddress;
    CInetSocketAddress::New(inetAddr, (*ports)[1], (IInetSocketAddress**)&theAddress);
    theSocket1->Bind(theAddress);

    theSocket2->Bind(theAddress);
    theSocket1->Close();
    theSocket2->Close();
}

//Num: Test22
int CTest::test_connectLjava_net_SocketAddress(int argc, char* argv[])
{
    // validate that we get the PortUnreachable exception if we try to
    // send a dgram to a server that is not running and then do a recv
    CDatagramSocket::New((IDatagramSocket**)&ds);
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> inetAddress;
    inetAddressHelper->GetLocalHost((IInetAddress**)&inetAddress);
    Int32 portNumber = Support_PortManager::getNextPortForUDP();
    AutoPtr<IInetSocketAddress> inetSocketAddress;
    CInetSocketAddress::New(inetAddress, portNumber, (IInetSocketAddress**)&inetSocketAddress);
    AutoPtr<ISocketAddress> socketAddress = ISocketAddress::Probe(inetSocketAddress);
    ds->Connect(socketAddress);
    AutoPtr<ArrayOf<Byte> > byteArray = ArrayOf<Byte>::Alloc(10);
    AutoPtr<IDatagramPacket> send;
    CDatagramPacket::New(byteArray, 10, (IDatagramPacket**)&send);
    ds->Send(send);
    AutoPtr<ArrayOf<Byte> > byteArray2 = ArrayOf<Byte>::Alloc(20);
    AutoPtr<IDatagramPacket> receive;
    CDatagramPacket::New(byteArray2, 20, (IDatagramPacket**)&receive);
    ds->SetSoTimeout(1000);
    ds->Receive(receive);
    ds->Close();

    CDatagramSocket::New((IDatagramSocket**)&ds);
    portNumber = Support_PortManager::getNextPortForUDP();
    AutoPtr<IInetSocketAddress> isa;
    CInetSocketAddress::New(String("asdfasdf"), 1, (IInetSocketAddress**)&isa);
    socketAddress = ISocketAddress::Probe(isa);
    ds->Connect(socketAddress);
    ds->Close();

    // validate that we can send/receive with datagram sockets connected at
    // the native level
    // AutoPtr<IDatagramServer> server;
    // AutoPtr<ArrayOf<Int32> > ports;
    // *ports /*= Support_PortManager.getNextPortsForUDP(3)*/;
    // Int32 serverPortNumber = (*ports)[0];
    // AutoPtr<IInetAddress> localhost = inetAddress;
    // AutoPtr<IDatagramSocket> ds;
    // CDatagramSocket::New((*ports)[1], (IDatagramSocket**)&ds);
    // CDatagramSocket::New((*ports)[2], (IDatagramSocket**)&ds2);
}

//Num: Test23
int CTest::test_isBound(int argc, char* argv[])
{
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetLocalHost((IInetAddress**)&addr);
    AutoPtr<ArrayOf<Int32> > ports = ArrayOf<Int32>::Alloc(3);
    // Support_PortManager::GetNextPortsForUDP(3, (ArrayOf<Int32>**)&ports);
    (*ports)[0] = 57533;
    (*ports)[1] = 44792;
    (*ports)[0] = 53536;

    Int32 port = (*ports)[0];

    AutoPtr<IDatagramSocket> theSocket;
    CDatagramSocket::New((*ports)[1], (IDatagramSocket**)&theSocket);
    Boolean flag = FALSE;
    theSocket->IsBound(&flag);
    assert(flag == TRUE);
    theSocket->Close();

    AutoPtr<IInetSocketAddress> isa;
    CInetSocketAddress::New(addr, port, (IInetSocketAddress**)&isa);
    AutoPtr<ISocketAddress> socketAddress = (ISocketAddress*)ISocketAddress::Probe(isa);
    CDatagramSocket::New(socketAddress, (IDatagramSocket**)&theSocket);
    theSocket->IsBound(&flag);
    assert(flag == TRUE);
    theSocket->Close();

    AutoPtr<ISocketAddress> sa;
    CDatagramSocket::New(sa, (IDatagramSocket**)&theSocket);
    theSocket->IsBound(&flag);
    assert(flag == FALSE);
    theSocket->Close();

    // connect causes implicit bind
    CDatagramSocket::New(sa, (IDatagramSocket**)&theSocket);
    AutoPtr<IInetSocketAddress> inetsocketAddr;
    CInetSocketAddress::New(addr, port, (IInetSocketAddress**)&inetsocketAddr);
    theSocket->Connect(inetsocketAddr);
    theSocket->IsBound(&flag);
    theSocket->Close();

    // now test when we bind explicitely
    AutoPtr<IInetAddress> inetAddr;
    inetAddressHelper->GetLocalHost((IInetAddress**)&inetAddr);
    AutoPtr<IInetSocketAddress> theLocalAddress;
    CInetSocketAddress::New(inetAddr, (*ports)[2], (IInetSocketAddress**)&theLocalAddress);
    CDatagramSocket::New(sa, (IDatagramSocket**)&theSocket);
    theSocket->IsBound(&flag);
    assert(flag == FALSE);
    theSocket->Bind(theLocalAddress);
    theSocket->IsBound(&flag);
    assert(flag == TRUE);
    theSocket->Close();
    theSocket->IsBound(&flag);
    assert(flag == TRUE);
}

//Num: Test24
int CTest::test_isConnected(int argc, char* argv[])
{
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetLocalHost((IInetAddress**)&addr);
    AutoPtr<ArrayOf<Int32> > ports = ArrayOf<Int32>::Alloc(4);
    (*ports)[0] = 51264;
    (*ports)[1] = 41928;
    (*ports)[0] = 55359;
    (*ports)[0] = 41310;
    // Support_PortManager::GetNextPortsForUDP(4, (ArrayOf<Int32>**)&ports);
    Int32 port = (*ports)[0];

    // base test
    AutoPtr<IDatagramSocket> theSocket;
    CDatagramSocket::New((*ports)[1], (IDatagramSocket**)&theSocket);
    Boolean flag = FALSE;
    theSocket->IsConnected(&flag);
    assert(flag == FALSE);

    AutoPtr<IInetSocketAddress> isa;
    CInetSocketAddress::New(addr, port, (IInetSocketAddress**)&isa);
    theSocket->Connect(isa);
    theSocket->IsConnected(&flag);
    assert(flag == TRUE);

    // reconnect the socket and make sure we get the right answer
     CInetSocketAddress::New(addr, (*ports)[2], (IInetSocketAddress**)&isa);
     theSocket->Connect(isa);
     theSocket->IsConnected(&flag);
     assert(flag == TRUE);

     // now disconnect the socket and make sure we get the right answer
     theSocket->Disconnect();
     theSocket->IsConnected(&flag);
     assert(flag == FALSE);
     theSocket->Close();

     // now check behavior when socket is closed when connected
     CDatagramSocket::New((*ports)[3], (IDatagramSocket**)&theSocket);
     CInetSocketAddress::New(addr, port, (IInetSocketAddress**)&isa);
     theSocket->Connect(isa);
     theSocket->IsConnected(&flag);
     assert(flag == TRUE);
}

//Num: Test25
int CTest::test_getRemoteSocketAddress(int argc, char* argv[])
{
    AutoPtr<ArrayOf<Int32> > ports = ArrayOf<Int32>::Alloc(3);
    // Support_PortManager::GetNextPortsForUDP(3, (ArrayOf<Int32>**)&ports);
    (*ports)[0] = 39929;
    (*ports)[1] = 34836;
    (*ports)[0] = 41381;

    Int32 sport = (*ports)[0];
    Int32 portNumber = (*ports)[1];
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> iia;
    inetAddressHelper->GetLocalHost((IInetAddress**)&iia);
    AutoPtr<IInetSocketAddress> isa;
    CInetSocketAddress::New(iia, portNumber, (IInetSocketAddress**)&isa);
    AutoPtr<IDatagramSocket> s;
    CDatagramSocket::New(isa, (IDatagramSocket**)&s);

    AutoPtr<IInetSocketAddress> isa1;
    CInetSocketAddress::New(iia, sport, (IInetSocketAddress**)&isa1);
    s->Connect(isa1);

    Boolean flag = FALSE;
    AutoPtr<ISocketAddress> address;
    s->GetRemoteSocketAddress((ISocketAddress**)&address);
    AutoPtr<IObject> obj1 = IObject::Probe(address);
    AutoPtr<IObject> obj2 = IObject::Probe(isa1);
    obj1->Equals(obj2, &flag);
    assert(flag == TRUE);
    s->Close();

    // now create one that is not connected and validate that we get the
    // right answer
    AutoPtr<IDatagramSocket> theSocket;
    AutoPtr<ISocketAddress> isa2 = NULL;
    CDatagramSocket::New(isa2, (IDatagramSocket**)&theSocket);
    portNumber = (*ports)[2];
    theSocket->Bind(isa);
    AutoPtr<ISocketAddress> sktAddr;
    theSocket->GetRemoteSocketAddress((ISocketAddress**)&sktAddr);
    assert(NULL == sktAddr);

    // now connect and validate we get the right answer
    theSocket->Connect(isa1);
    theSocket->GetRemoteSocketAddress((ISocketAddress**)&sktAddr);
    obj1 = IObject::Probe(sktAddr);
    obj2 = IObject::Probe(isa1);
    obj1->Equals(obj2, &flag);
    assert(flag == TRUE);
    theSocket->Close();
}

//Num: Test26
int CTest::test_setReuseAddressZ(int argc, char* argv[])
{
    // test case were we set it to false
    AutoPtr<IDatagramSocket> theSocket1;
    AutoPtr<IDatagramSocket> theSocket2;

    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> localHost;
    inetAddressHelper->GetLocalHost((IInetAddress**)&localHost);

    Int32 port = Support_PortManager::getNextPortForUDP();
    AutoPtr<IInetSocketAddress> theAddress;
    CInetSocketAddress::New(localHost, port, (IInetSocketAddress**)&theAddress);

    AutoPtr<ISocketAddress> sAddress = NULL;
    CDatagramSocket::New(sAddress, (IDatagramSocket**)&theSocket1);
    CDatagramSocket::New(sAddress, (IDatagramSocket**)&theSocket2);
    theSocket1->SetReuseAddress(FALSE);
    theSocket2->SetReuseAddress(FALSE);
    theSocket1->Bind(theAddress);
    theSocket2->Bind(theAddress);

    if (theSocket1 != NULL)
        theSocket1->Close();

    if (theSocket2 != NULL)
        theSocket2->Close();

    // test case were we set it to true
    CInetSocketAddress::New(localHost, port,
     (IInetSocketAddress**)&theAddress);

    theSocket1->SetReuseAddress(TRUE);
    theSocket2->SetReuseAddress(TRUE);
    theSocket1->Bind(theAddress);
    theSocket2->Bind(theAddress);

    if (theSocket1 != NULL)
        theSocket1->Close();

    if (theSocket2 != NULL)
        theSocket2->Close();

    // test the default case which we expect to be the same on all
    // platforms
    CInetSocketAddress::New(localHost, port, (IInetSocketAddress**)&theAddress);
    AutoPtr<IDatagramSocket> theSocket3;
    AutoPtr<IDatagramSocket> theSocket4;
    CDatagramSocket::New(sAddress, (IDatagramSocket**)&theSocket3);
    CDatagramSocket::New(sAddress, (IDatagramSocket**)&theSocket4);
    theSocket3->Bind(theAddress);
    theSocket4->Bind(theAddress);

    if (theSocket3 != NULL)
        theSocket3->Close();

    if (theSocket4 != NULL)
        theSocket4->Close();

}

//Num: Test27
int CTest::test_getReuseAddress(int argc, char* argv[])
{
    AutoPtr<IDatagramSocket> theSocket;
    CDatagramSocket::New((IDatagramSocket**)&theSocket);
    theSocket->SetReuseAddress(TRUE);
    Boolean flag = FALSE;
    theSocket->GetReuseAddress(&flag);
    assert(flag == TRUE);
    theSocket->SetReuseAddress(FALSE);
    theSocket->GetReuseAddress(&flag);
    assert(flag == FALSE);
}

//Num: Test28
int CTest::test_setBroadcastZ(int argc, char* argv[])
{
    int* ports = Support_PortManager::getNextPortsForUDP(3);
    AutoPtr<IDatagramSocket> theSocket;
    CDatagramSocket::New(ports[0], (IDatagramSocket**)&theSocket);
    theSocket->SetBroadcast(FALSE);
    AutoPtr<ArrayOf<Byte> > theBytes;
    theBytes->Set(0, (Byte)-1);
    theBytes->Set(1, (Byte)-1);
    theBytes->Set(2, (Byte)-1);
    theBytes->Set(3, (Byte)-1);

    // validate we cannot connect to the broadcast address when
    // setBroadcast is false
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> inetAddress;
    inetAddressHelper->GetByAddress(theBytes, (IInetAddress**)&inetAddress);
    AutoPtr<IInetSocketAddress> inetsocketAddr;
    CInetSocketAddress::New(inetAddress, ports[1], (IInetSocketAddress**)&inetsocketAddr);
    Boolean flag = FALSE;
    theSocket->GetBroadcast(&flag);
    assert(flag == FALSE);

    // now validate that we can connect to the broadcast address when
    // setBroadcast is true
    theSocket->SetBroadcast(TRUE);
    CInetSocketAddress::New(inetAddress, ports[2], (IInetSocketAddress**)&inetsocketAddr);
    theSocket->Connect(inetsocketAddr);
}

//Num: Test29
int CTest::test_getBroadcast(int argc, char* argv[])
{
    AutoPtr<IDatagramSocket> theSocket;
    CDatagramSocket::New((IDatagramSocket**)&theSocket);
    theSocket->SetBroadcast(TRUE);
    Boolean flag = FALSE;
    theSocket->GetBroadcast(&flag);
    assert(flag == TRUE);

    theSocket->SetBroadcast(FALSE);
    theSocket->GetBroadcast(&flag);
    assert(flag == FALSE);
}

//Num: Test30
int CTest::test_setTrafficClassI(int argc, char* argv[])
{
    Int32 IPTOS_LOWCOST = 0x2;
    Int32 IPTOS_RELIABILTY = 0X4;
    Int32 IPTOS_THROUGHPUT = 0x8;
    Int32 IPTOS_LOWDELAY = 0x10;
    int* ports = Support_PortManager::getNextPortsForUDP(2);

    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> inetAddress;
    inetAddressHelper->GetLocalHost((IInetAddress**)&inetAddress);
    AutoPtr<IInetSocketAddress> inetSocketAddress;
    CInetSocketAddress::New(inetAddress, ports[0], (IInetSocketAddress**)&inetSocketAddress);
    AutoPtr<IDatagramSocket> theSocket;
    CDatagramSocket::New(ports[1], (IDatagramSocket**)&theSocket);

    // validate that value set must be between 0 and 255
    ECode ec = theSocket->SetTrafficClass(256);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    ec = theSocket->SetTrafficClass(-1);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    // now validate that we can set it to some good values
    theSocket->SetTrafficClass(IPTOS_LOWCOST);
    theSocket->SetTrafficClass(IPTOS_THROUGHPUT);
    theSocket->Close();

    ec = theSocket->SetTrafficClass(1);
}

//Num: Test31
int CTest::test_getTrafficClass(int argc, char* argv[])
{
    Int32 IPTOS_LOWCOST = 0x2;
    Int32 IPTOS_RELIABILTY = 0X4;
    Int32 IPTOS_THROUGHPUT = 0x8;
    Int32 IPTOS_LOWDELAY = 0x10;

    int* ports = Support_PortManager::getNextPortsForUDP(2);
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> localhost;
    inetAddressHelper->GetLocalHost((IInetAddress**)&localhost);
    AutoPtr<IInetSocketAddress> inetSocketAddress;
    CInetSocketAddress::New(localhost, ports[0], (IInetSocketAddress**)&inetSocketAddress);
    AutoPtr<IDatagramSocket> theSocket;
    CDatagramSocket::New(ports[1], (IDatagramSocket**)&theSocket);

    /*
     * we cannot actually check that the values are set as if a platform
     * does not support the option then it may come back unset even
     * though we set it so just get the value to make sure we can get it
     */
     Int32 trafficClass;
     ECode ec = theSocket->GetTrafficClass(&trafficClass);
}

//Num: Test32
int CTest::test_isClosed(int argc, char* argv[])
{
    AutoPtr<IDatagramSocket> theSocket;
    CDatagramSocket::New((IDatagramSocket**)&theSocket);

    // validate isClosed returns expected values
    Boolean flag = FALSE;
    theSocket->IsClosed(&flag);
    assert(flag == FALSE);
    theSocket->Close();
    theSocket->IsClosed(&flag);
    assert(flag == TRUE);

    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> localhost;
    inetAddressHelper->GetLocalHost((IInetAddress**)&localhost);
    AutoPtr<IInetSocketAddress> theAddress;
    Int32 port = Support_PortManager::getNextPortForUDP();
    CInetSocketAddress::New(localhost, port, (IInetSocketAddress**)&theAddress);
    CDatagramSocket::New(theAddress, (IDatagramSocket**)&theSocket);
    theSocket->IsClosed(&flag);
    assert(flag == FALSE);
    theSocket->Close();
    theSocket->IsClosed(&flag);
    assert(flag == TRUE);
}

//Num: Test33
int CTest::test_getChannel(int argc, char* argv[])
{
    AutoPtr<IDatagramSocket> datagramSocket;
    CDatagramSocket::New((IDatagramSocket**)&datagramSocket);
    AutoPtr<IDatagramChannel> channel;
    datagramSocket->GetChannel((IDatagramChannel**)&channel);
    assert(NULL == channel);
}
