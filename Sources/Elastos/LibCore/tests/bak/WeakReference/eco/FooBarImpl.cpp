
#include "FooBarImpl.h"
// #include "WeakReferenceImpl.h"
#include <Elastos.CoreLibrary.h>

PInterface FooBarImpl::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IFoo*)this;
    }
    else if (riid == EIID_IFoo) {
        return (IFoo*)this;
    }
    else if (riid == EIID_IBar) {
        return (IBar*)this;
    }
    else if (riid == EIID_IFooBar) {
        return (IFooBar*)this;
    }
    else if (riid == EIID_IWeakReferenceSource) {
        return (IWeakReferenceSource*)this;
    }

    return NULL;
}

UInt32 FooBarImpl::AddRef()
{
    return ElRefBase::AddRef(); // return ElSWRefBase::AddRef();
}

UInt32 FooBarImpl::Release()
{
    return ElRefBase::Release(); // return ElSWRefBase::Release();
}

ECode FooBarImpl::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [in] */ InterfaceID* iid)
{
    if (object == (IInterface*)(IFoo*)this) {
        *iid = EIID_IFoo;
    }
    else if (object == (IInterface*)(IBar*)this) {
        *iid = EIID_IBar;
    }
    else if (object == (IInterface*)(IFooBar*)this) {
        *iid = EIID_IFooBar;
    }
    else if (object == (IInterface*)(IWeakReferenceSource*)this) {
        *iid = EIID_IWeakReferenceSource;
    }
    else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode FooBarImpl::Foo()
{
    PFL_EX("Foo");
    return NOERROR;
}

ECode FooBarImpl::Bar()
{
    PFL_EX("Bar");
    return NOERROR;
}

ECode FooBarImpl::FooBar()
{
    PFL_EX("FooBar");
    return NOERROR;
}

ECode FooBarImpl::GetWeakReference(
    /* [out] */ IWeakReference** weakReference)
{
    *weakReference = new WeakReferenceImpl(Probe(EIID_IInterface), CreateWeak(this));
    (*weakReference)->AddRef();
    return NOERROR;
}
