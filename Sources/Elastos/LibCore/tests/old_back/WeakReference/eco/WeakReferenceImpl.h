
#ifndef __WEAKREFERENCEIMPL_H__
#define __WEAKREFERENCEIMPL_H__

#include "WeakReference_server.h"
// #include "ElSWRefBase.h"
#include <Elastos.CoreLibrary.h>
#include <elrefbase.h>

class WeakReferenceImpl
    : public ElLightRefBase
    , public IWeakReference
{
public:
    WeakReferenceImpl(
        /* [in] */ IInterface* object,
        /* [in] */ ElRefBase::WeakRefType* ref) //ElSWRefBase::WeakRefType* ref)
        : mObject(object)
        , mRef(ref)
    {}

    ~WeakReferenceImpl()
    {
        if (mObject) mRef->DecWeak(this);
    }

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid)
    {
        if (riid == EIID_IInterface) {
            return (PInterface)(IWeakReference*)this;
        }
        else if (riid == EIID_IWeakReference) {
            return (IWeakReference*)this;
        }

        return NULL;
    }

    CARAPI_(UInt32) AddRef()
    {
        return ElLightRefBase::AddRef();
    }

    CARAPI_(UInt32) Release()
    {
        return ElLightRefBase::Release();
    }

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [in] */ InterfaceID* iid)
    {
        if (object == (IInterface*)(IWeakReference*)this) {
            *iid = EIID_IWeakReference;
        }
        else {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        return NOERROR;
    }

    CARAPI Resolve(
        /* [in] */ const InterfaceID& riid,
        /* [out] */ IInterface** objectReference)
    {
        if (mObject && mRef->AttemptIncStrong(objectReference)) {
            *objectReference = mObject->Probe(riid);
            if (*objectReference != NULL) {
                (*objectReference)->AddRef();
            }
        }
        else {
            *objectReference = NULL;
        }
        return NOERROR;
    }

private:
    IInterface* mObject;
    ElRefBase::WeakRefType* mRef; // ElSWRefBase::WeakRefType* mRef;
};

#endif //__WEAKREFERENCEIMPL_H__
