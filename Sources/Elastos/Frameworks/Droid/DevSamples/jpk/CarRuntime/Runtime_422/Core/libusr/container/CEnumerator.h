//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CENUMERATOR_H__
#define __CENUMERATOR_H__

#include <elasys_server.h>
#include <container.h>

class __CObjectEnumerator :
    public IObject,
    public IObjectEnumerator
{
public:
    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI Aggregate(
        /* [in]  */ AggregateType type,
        /* [in]  */ PInterface pObject);

    CARAPI GetDomain(
        /* [out] */ PInterface *ppObject);

    CARAPI GetClassID(
        /* [out] */ ClassID *pCLSID);

    CARAPI Current(
        /* [out] */ PInterface *ppObj);

    CARAPI MoveNext(
        /* [out] */ Boolean *pSucceeded);

    CARAPI Reset();

    __CObjectEnumerator(
        IInterface *pOwner,
        ObjectNode *pHeader,
        CriticalSection *pLock,
        Int32 *pState);
    virtual ~__CObjectEnumerator();

private:
    Int32                m_cRef;
    IInterface           *m_pOwner;

    ObjectNode           *m_pHeader;
    ObjectNode           *m_pCurrent;
    CriticalSection      *m_pLock;
    Int32                *m_pState;
    Int32                m_nOrigState;
};

CAR_INLINE __CObjectEnumerator::__CObjectEnumerator(
    IInterface *pOwner,
    ObjectNode *pHeader,
    CriticalSection *pLock,
    Int32 *pState)
{
    m_pOwner = pOwner;
    m_pOwner->AddRef();

    m_pHeader = m_pCurrent = pHeader;
    m_pLock = pLock;
    m_pState = pState;
    m_nOrigState = *pState;
    m_cRef = 0;
}

CAR_INLINE __CObjectEnumerator::~__CObjectEnumerator()
{
    m_pOwner->Release();
}

#endif // __CENUMERATOR_H__
