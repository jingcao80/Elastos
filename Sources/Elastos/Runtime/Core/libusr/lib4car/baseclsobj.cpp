//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <car.h>

_ELASTOS_NAMESPACE_USING

extern "C" Int32 g_dllLockCount;

PInterface _CBaseClassObject::Probe(
    /* [in] */ REIID riid)
{
    if (EIID_IInterface == riid) {
        return (IInterface *)(IClassObject *)this;
    }
    else if (EIID_IObject == riid) {
        return (IObject *)this;
    }
    else if (EIID_IClassObject == riid) {
        return (IClassObject *)this;
    }
    else {
        return NULL;
    }
}

UInt32 _CBaseClassObject::AddRef()
{
    atomic_inc(&g_dllLockCount);
    return 2;
}

UInt32 _CBaseClassObject::Release()
{
    atomic_dec(&g_dllLockCount);
    return 1;
}

ECode _CBaseClassObject::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    if (NULL == iid) return E_INVALID_ARGUMENT;

    if (object == (IInterface *)(IObject *)this) {
        *iid = EIID_IObject;
    }
    else if (object == (IInterface *)(IClassObject *)this) {
        *iid = EIID_IClassObject;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode _CBaseClassObject::Aggregate(
    /* [in] */ AggregateType type,
    /* [in] */ PInterface pObj)
{
    return E_NOT_IMPLEMENTED;
}

ECode _CBaseClassObject::GetDomain(
    /* [out] */ PInterface* object)
{
    return E_NOT_IMPLEMENTED;
}

ECode _CBaseClassObject::GetClassID(
    /* [out] */ ClassID* clsid)
{
    if (NULL == clsid) return E_INVALID_ARGUMENT;

    *clsid = ECLSID_CClassObject;
    return NOERROR;
}

ECode _CBaseClassObject::CreateObject(
    /* [in] */ IInterface* outer,
    /* [in] */ REIID riid,
    /* [out] */ IInterface** retObject)
{
    IInterface* object;
    if (NULL == mCreateObjectFunc) return E_CLASS_NOT_AVAILABLE;
    ECode ec = (*mCreateObjectFunc)(&object);
    if (FAILED(ec)) return ec;
    *retObject = object->Probe(riid);
    if (!(*retObject)) {
        object->Release();
        return E_NO_INTERFACE;
    }
    (*retObject)->AddRef();
    object->Release();
    return ec;
}

ECode _CBaseClassObject::StayResident(
    /* [in] */ Boolean isStayResident)
{
    if (isStayResident) {
        atomic_inc(&g_dllLockCount);
    }
    else {
        atomic_dec(&g_dllLockCount);
    }
    return NOERROR;
}
