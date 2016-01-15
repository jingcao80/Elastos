#ifndef __WEAKHASHMAP_TEST_H
#define __WEAKHASHMAP_TEST_H

#include "cmdef.h"
#include "Elastos.CoreLibrary.h"

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Net;

#define TEST(a, x) a.test_##x

class CTest
{
public:
    class MyRunnable
        : public IRunnable
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        MyRunnable(IServerSocket* serv, ArrayOf<ISocket*>* arr);

        CARAPI Run();

    public:
        AutoPtr<IServerSocket> mServ;
        AutoPtr<ArrayOf<ISocket*> > mArr;
    };

public:
    CTest();
    ~CTest();

    int testTimeoutAfterAccept(int argc, char* argv[]);
};

#endif //__WEAKHASHMAP_TEST_H
