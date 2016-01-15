
#include "test.h"


//====================================================================
// CTest::MyRunnable::
//====================================================================
CAR_INTERFACE_IMPL(CTest::MyRunnable, IRunnable);

CTest::MyRunnable::MyRunnable(IServerSocket* serv, ArrayOf<ISocket*>* arr)
{
    mServ = serv;
    mArr = arr;
}

ECode CTest::MyRunnable::Run()
{
    return mServ->Accept(&((*mArr)[0]));
}

//====================================================================
// CTest::
//====================================================================

CTest::CTest()
{
}

CTest::~CTest()
{
}

// test 1
int CTest::testTimeoutAfterAccept(int argc, char* argv[])// throws Exception
{
    AutoPtr<IServerSocket> ss;
    CServerSocket::New(0, (IServerSocket**)&ss);
    ss->SetReuseAddress(TRUE);
    // On Unix, the receive timeout is inherited by the result of accept(2).
    // Java specifies that it should always be 0 instead.
    ss->SetSoTimeout(1234);
    AutoPtr<ArrayOf<ISocket*> > result = ArrayOf<ISocket*>::Alloc(1);
    AutoPtr<IRunnable> r = new MyRunnable(ss, result);
    AutoPtr<IThread> t;
    CThread::New(r, (IThread**)&t);
    t->Start();
    AutoPtr<IInetAddress> addr;
    ss->GetInetAddress((IInetAddress**)&addr);
    Int32 port;
    ss->GetLocalPort(&port);
    AutoPtr<ISocket> sock;
    CSocket::New(addr, port, (ISocket**)&sock);
    t->Join();
    Int32 tim;
    (*result)[0]->GetSoTimeout(&tim);
    assert(0 == tim);
    return 0;
}