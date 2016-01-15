
#include "test.h"
#include "elastos/StringBuilder.h"
//#include "elastos/SocketChannel.h"

//====================================================================
// CTest::Thread_1::
//====================================================================
CTest::Thread_1::Thread_1(IServerSocket* serv, ISemaphore* p)
{
    mSem = p;
    mServ = serv;
}

ECode CTest::Thread_1::Run()
{
    //try {
    AutoPtr<ISocket> sock;
    mServ->Accept((ISocket**)&sock);
    AutoPtr<IInputStream> is;
    sock->GetInputStream((IInputStream**)&is);
    Int32 res;
    is->Read(&res);
    sock->Close();
    // } catch (IOException e) {
    //     serverError = e;
    // }
    mSem->Release();
    return NOERROR;
}

//====================================================================
// CTest::Thread_2::
//====================================================================
CTest::Thread_2::Thread_2(ISocket* p, Boolean* con)
{
    mClient = p;
    mCon = con;
}

ECode CTest::Thread_2::Run()
{
    AutoPtr<IInetSocketAddress> addr;
    CInetSocketAddress::New(PACKAGE_DROPPING_ADDRESS, 1357, (IInetSocketAddress**)&addr);
    mClient->Connect(addr);
    *mCon = TRUE;
    return NOERROR;
}

//====================================================================
// CTest::
//====================================================================

const String CTest::NON_EXISTING_ADDRESS = String("123.123.123.123");

const String CTest::KNOW_GOOD_ADDRESS = String("209.85.129.147");

const String CTest::PACKAGE_DROPPING_ADDRESS = String("191.167.0.1");

CTest::CTest()
{
}

CTest::~CTest()
{
}

// test 1
int CTest::testSocketSimple(int argc, char* argv[])
{
    AutoPtr<IServerSocket> ss;
    AutoPtr<ISocket> s, s1;
    Int32 port;

//    IOException lastEx = NULL;

    CServerSocket::New((IServerSocket**)&ss);

    for (port = 9900; port < 9999; port++) {
//        try {
        AutoPtr<IInetSocketAddress> addr;
        ECode ec = CInetSocketAddress::New(String("127.0.0.1"), port, (IInetSocketAddress**)&addr);
        if (ec == NOERROR) {
            ec = ss->Bind(addr);
            if (ec == NOERROR) {
                break;
            }
        }
//        lastEx = NULL;
        //     break;
        // } catch (IOException ex) {
        //     lastEx = ex;
        // }
    }

//    if (lastEx != NULL) {
//        throw lastEx;
//    }

    ECode ec = CSocket::New(String("127.0.0.1"), port, (ISocket**)&s);
    assert(ec == NOERROR);

    ss->Accept((ISocket**)&s1);

    AutoPtr<IOutputStream> os;
    s->GetOutputStream((IOutputStream**)&os);
    os->Write(0xa5);

    AutoPtr<IInputStream> is;
    s1->GetInputStream((IInputStream**)&is);
    Int32 val;
    is->Read(&val);
    assert(0xa5 == val);

    AutoPtr<IOutputStream> os2;
    s1->GetOutputStream((IOutputStream**)&os2);
    os2->Write(0x5a);
    AutoPtr<IInputStream> is2;
    s->GetInputStream((IInputStream**)&is2);
    is2->Read(&val);
    assert(0x5a == val);
    return 0;
}

// test 2
int CTest::testWildcardAddress(int argc, char* argv[])
{
    AutoPtr<ISocket> s2;
    CSocket::New((ISocket**)&s2);
    AutoPtr<IInetSocketAddress> netAddr;
    CInetSocketAddress::New((IInetAddress*) NULL, 12345, (IInetSocketAddress**)&netAddr);
    s2->Bind(netAddr);
    AutoPtr<IInetAddress> lAddr;
    s2->GetLocalAddress((IInetAddress**)&lAddr);
    AutoPtr<ArrayOf<Byte> > addr;
    lAddr->GetAddress((ArrayOf<Byte>**)&addr);
    for (int i = 0; i < 4; i++) {
        assert(0 == (*addr)[i]);
    }
    return 0;
}

// test 3
int CTest::testServerSocketClose(int argc, char* argv[])
{
    AutoPtr<IServerSocket> s3;
    CServerSocket::New(23456, (IServerSocket**)&s3);
    s3->Close();
    AutoPtr<IServerSocket> s4;
    CServerSocket::New(23456, (IServerSocket**)&s4);
    s4->Close();
    return 0;
}

// test 4
int CTest::testSetReuseAddress(int argc, char* argv[])
{
    AutoPtr<IInetSocketAddress> addr;
    CInetSocketAddress::New(8383, (IInetSocketAddress**)&addr);

    AutoPtr<IServerSocket> serverSock;
    CServerSocket::New((IServerSocket**)&serverSock);
    serverSock->SetReuseAddress(TRUE);
    serverSock->Bind(addr);

    AutoPtr<ISemaphore> semThreadEnd;
    CSemaphore::New(0, (ISemaphore**)&semThreadEnd);
    AutoPtr<IThread> h = new Thread_1(serverSock, semThreadEnd);
    h->Start();

    // // Give the server a bit of time for startup
    Thread::Sleep(2000);

    AutoPtr<ISocket> client;
    CSocket::New(String("localhost"), 8383, (ISocket**)&client);
    AutoPtr<IOutputStream> os;
    client->GetOutputStream((IOutputStream**)&os);
    os->Write(1);
    // Just leave this connection open from the client side. It will be
    // closed from the server side so the server stays in the TIME_WAIT
    // state for a while. setReuseAddress() should be able to handle this.

    semThreadEnd->Acquire();
    serverSock->Close();

    AutoPtr<IServerSocket> serverSock2;
    CServerSocket::New((IServerSocket**)&serverSock2);
    serverSock2->SetReuseAddress(TRUE);
    serverSock2->Bind(addr);
    serverSock2->Close();

    // if (serverError != NULL) {
    //     throw new RuntimeException("Server must complete without error", serverError);
    // }
    return 0;
}

// test 5
int CTest::testTimeoutException(int argc, char* argv[])
{
    AutoPtr<IServerSocket> s;
    CServerSocket::New(9800, (IServerSocket**)&s);
    s->SetSoTimeout(2000);
    AutoPtr<ISocket> socket;
    ECode ec = s->Accept((ISocket**)&socket);
    return ec;
}

// test 6
int CTest::testNativeSocketChannelOpen(int argc, char* argv[])
{
//    SocketChannel::Open();
    return 0;
}

//    public void testSocketSetSOTimeout() throws IOException {
//        Socket sock = new Socket();
//        int timeout = 5000;
//        long start = System.currentTimeMillis();
//        try {
//            sock.connect(new InetSocketAddress(NON_EXISTING_ADDRESS, 80), timeout);
//        } catch (SocketTimeoutException e) {
//            // expected
//            long delay = System.currentTimeMillis() - start;
//            if (Math.abs(delay - timeout) > 1000) {
//                fail("timeout was not accurate. expected: " + timeout
//                        + " actual: " + delay + " miliseconds.");
//            }
//        } finally {
//            try {
//                sock.close();
//            } catch (IOException ioe) {
//                // ignore
//            }
//        }
//    }

// test 7
int CTest::disable_testConnectWithIP4IPAddr(int argc, char* argv[])
{
    // call a Google Web server
    AutoPtr<IInetSocketAddress> scktAddrss;
    CInetSocketAddress::New(KNOW_GOOD_ADDRESS, 80, (IInetSocketAddress**)&scktAddrss);
    AutoPtr<ISocket> clntSckt;
    CSocket::New((ISocket**)&clntSckt);
    ECode ec = clntSckt->ConnectEx(scktAddrss, 5000);
    clntSckt->Close();
    return ec;
}

// test 8
// Regression test for #1058962: Socket.close() does not cause
// socket.connect() to return immediately.
int CTest::disable_testSocketConnectClose(int argc, char* argv[])
{
    // try {
    client = NULL;
    CSocket::New((ISocket**)&client);

    AutoPtr<IThread> h = new Thread_2(client, &connected);
    h->Start();

    Thread::Sleep(1000);

    //     Assert.assertNull(error == NULL);
    assert(connected == FALSE);

    client->Close();

    Thread::Sleep(1000);

    //     if (error == NULL) {
    //         fail("Socket connect still ongoing");
    //     }
    //     else if (!(error instanceof SocketException)) {
    //         fail("Socket connect interrupted with wrong error: " + error.toString());
    //     }

    // } catch (Exception ex) {
    //     throw new RuntimeException(ex);
    // }
    return 0;
}
