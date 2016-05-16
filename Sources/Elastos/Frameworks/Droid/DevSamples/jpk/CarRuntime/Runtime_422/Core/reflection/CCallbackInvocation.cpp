//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CCallbackInvocation.h"

UInt32 CCallbackInvocation::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CCallbackInvocation::Release()
{
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        delete this;
    }
    assert(nRef >= 0);
    return nRef;
}

PInterface  CCallbackInvocation::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_ICallbackInvocation) {
        return (ICallbackInvocation *)this;
    }
    else {
        return NULL;
    }
}

ECode CCallbackInvocation::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackInvocation::Invoke(
    /* [in] */ PVoid targetObject,
    /* [in] */ PVoid targetMethod,
    /* [in] */ ICallbackArgumentList * pCallbackArgumentList)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}
