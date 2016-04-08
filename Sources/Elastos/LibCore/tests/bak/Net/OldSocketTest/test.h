#ifndef __WEAKHASHMAP_TEST_H
#define __WEAKHASHMAP_TEST_H

#include "Elastos.CoreLibrary.h"
#include "elastos/ThreadBase.h"

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Net;
using namespace Elastos::IO;
using namespace Elastos::IO::Channels;
using namespace Elastos::Utility::Concurrent;

#define TEST(a, x) a.test_##x

class CTest
{
public:
    class Thread_1
        : public ThreadBase
    {
    public:
        Thread_1(IServerSocket* serv, ISemaphore* p);

        CARAPI Run();

    public:
        AutoPtr<ISemaphore> mSem;
        AutoPtr<IServerSocket> mServ;
    };

    class Thread_2
        : public ThreadBase
    {
    public:
        Thread_2(ISocket* p, Boolean* con);

        CARAPI Run();

    public:
        AutoPtr<ISocket> mClient;
        Boolean* mCon;
    };

public:
    CTest();
    ~CTest();

    int testSocketSimple(int argc, char* argv[]);

    int testWildcardAddress(int argc, char* argv[]);

    int testServerSocketClose(int argc, char* argv[]);

    int testSetReuseAddress(int argc, char* argv[]);

    int testTimeoutException(int argc, char* argv[]);

    int testNativeSocketChannelOpen(int argc, char* argv[]);

    int disable_testConnectWithIP4IPAddr(int argc, char* argv[]);

    int disable_testSocketConnectClose(int argc, char* argv[]);

private:
    static const String NON_EXISTING_ADDRESS;

    static const String KNOW_GOOD_ADDRESS;

    static const String PACKAGE_DROPPING_ADDRESS;

//    Exception serverError = null;

    AutoPtr<ISocket> client;

//    Exception error;

    Boolean connected;
};

#endif //__WEAKHASHMAP_TEST_H
