
#ifndef __MyRunnable_H__
#define __MyRunnable_H__

#include <elastos.h>
#include <elautoptr.h>
#include <eltypes.h>
#include <stdio.h>
#include <Elastos.CoreLibrary.h>
#include <elrefbase.h>
#include <elastos/Mutex.h>

using namespace Elastos;
using namespace Elastos::Core;

class MyRunnable
    : public ElRefBase
    , public IRunnable
{
public:
    MyRunnable();

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI Run();

public:
    Boolean mStop;

private:
    CARAPI_(Mutex*) GetSelfLock();

private:
    Mutex mLock;
};

#endif //__MyRunnable_H__
