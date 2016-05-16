//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CEnumerator.h"
#include <elastos.h>
#include <elapi.h>
#include <elacs.h>

UInt32 __CObjectEnumerator::AddRef()
{
    Int32 ref = atomic_inc(&m_cRef);
    return (UInt32)ref;
}

UInt32 __CObjectEnumerator::Release()
{
    Int32 ref = atomic_dec(&m_cRef);

    if (0 == ref) {
        delete this;
        DecrementDllLockCount();
    }
    assert(ref >= 0);
    return ref;
}

PInterface __CObjectEnumerator::Probe(
    REIID riid)
{
    if (EIID_IInterface == riid) {
        return (IInterface *)(IObjectEnumerator *)this;
    }
    else if (riid == EIID_IObject) {
        return (IObject *)this;
    }
    else if (EIID_IObjectEnumerator == riid) {
        return (IObjectEnumerator *)this;
    }

    return NULL;
}

ECode __CObjectEnumerator::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (NULL == pIID) return E_INVALID_ARGUMENT;

    if (pObject == (IInterface *)(IObject *)this) {
        *pIID = EIID_IObject;
    }
    else if (pObject == (IInterface *)(IObjectEnumerator *)this) {
        *pIID = EIID_IObjectEnumerator;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode __CObjectEnumerator::Aggregate(
    /* [in]  */ AggregateType type,
    /* [in]  */ PInterface pObject)
{
    return E_NOT_IMPLEMENTED;
}

ECode __CObjectEnumerator::GetDomain(
    /* [out] */ PInterface * ppObject)
{
    return E_NOT_IMPLEMENTED;
}

ECode __CObjectEnumerator::GetClassID(
    /* [out] */ ClassID *pCLSID)
{
    if (NULL == pCLSID) return E_INVALID_ARGUMENT;

    *pCLSID = ECLSID_CObjectEnumerator;
    return NOERROR;
}

ECode __CObjectEnumerator::Current(
    /* [out] */ PInterface *ppObj)
{
    if (NULL == ppObj) return E_INVALID_ARGUMENT;
    if (NULL == m_pCurrent || m_pHeader == m_pCurrent) {
        return E_INVALID_OPERATION;
    }

    Void * pContext = _Elastos_CriticalSection_Enter(m_pLock);

    if (m_nOrigState != *m_pState) {
        m_pCurrent = NULL;
        _Elastos_CriticalSection_Leave(m_pLock, pContext);
        return E_INVALID_OPERATION;
    }

    *ppObj = m_pCurrent->m_pObject;
    m_pCurrent->m_pObject->AddRef();
    _Elastos_CriticalSection_Leave(m_pLock, pContext);
    return NOERROR;
}

ECode __CObjectEnumerator::MoveNext(Boolean *pSucceeded)
{
    if (NULL == m_pCurrent) {
        *pSucceeded = FALSE;
        return E_INVALID_OPERATION;
    }

    Void * pContext = _Elastos_CriticalSection_Enter(m_pLock);

    if (m_nOrigState != *m_pState) {
        m_pCurrent = NULL;
        _Elastos_CriticalSection_Leave(m_pLock, pContext);
        *pSucceeded = FALSE;
        return E_INVALID_OPERATION;
    }

    if (m_pCurrent == m_pHeader->Prev()) {
        m_pCurrent = NULL;
        _Elastos_CriticalSection_Leave(m_pLock, pContext);
        *pSucceeded = FALSE;
        return NOERROR;
    }

    m_pCurrent = (ObjectNode *)m_pCurrent->Next();
    _Elastos_CriticalSection_Leave(m_pLock, pContext);
    *pSucceeded = TRUE;
    return NOERROR;
}

ECode __CObjectEnumerator::Reset()
{
    m_pCurrent = m_pHeader;
    m_nOrigState = *m_pState;
    return NOERROR;
}
