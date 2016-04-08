
#ifndef __FOOBARIMPL_H__
#define __FOOBARIMPL_H__

#include <elrefbase.h> //#include "ElSWRefBase.h"
#include "WeakReference_server.h"

class FooBarImpl
    : public ElRefBase// ElSWRefBase
    , public IFoo
    , public IBar
    , public IFooBar
    , public IWeakReferenceSource
{
public:
    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [in] */ InterfaceID* iid);

    CARAPI Foo();

    CARAPI Bar();

    CARAPI FooBar();

    CARAPI GetWeakReference(
        /* [out] */ IWeakReference** weakReference);
};

#endif //__FOOBARIMPL_H__
