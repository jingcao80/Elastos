//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <car.h>
#include <elaatomics.h>

_ELASTOS_NAMESPACE_USING

extern "C" Int32 g_dllLockCount;

PInterface  _CBaseClassObject::Probe(REIID riid)
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
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (NULL == pIID) return E_INVALID_ARGUMENT;

    if (pObject == (IInterface *)(IObject *)this) {
        *pIID = EIID_IObject;
    }
    else if (pObject == (IInterface *)(IClassObject *)this) {
        *pIID = EIID_IClassObject;
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
    /* [out] */ PInterface *ppObj)
{
    return E_NOT_IMPLEMENTED;
}

ECode _CBaseClassObject::GetClassID(
    ClassID *pCLSID)
{
    if (NULL == pCLSID) return E_INVALID_ARGUMENT;

    *pCLSID = ECLSID_CClassObject;
    return NOERROR;
}

ECode _CBaseClassObject::CreateObject(
    IInterface *pOuter, REIID riid, IInterface **ppObj)
{
    IInterface *pObj;
    if (NULL == m_fnCreateObject) return E_CLASS_NOT_AVAILABLE;
    ECode ec = (*m_fnCreateObject)(&pObj);
    if (FAILED(ec)) return ec;
    *ppObj = pObj->Probe(riid);
    if (!(* ppObj)) {
        pObj->Release();
        return E_NO_INTERFACE;
    }
	(*ppObj)->AddRef();
	pObj->Release();
    return ec;
}

ECode _CBaseClassObject::StayResident(_ELASTOS Boolean bIsStayResident)
{
    if (bIsStayResident) {
        atomic_inc(&g_dllLockCount);
    }
    else {
        atomic_dec(&g_dllLockCount);
    }
    return NOERROR;
}
