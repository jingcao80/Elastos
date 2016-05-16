//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "AObjectContainer.h"
#include "_AObjectContainer.cpp"
#include <elapi.h>
#include <elacs.h>

extern "C" _CBaseClassObject _g_AObjectContainer_ClsObj;
_CBaseClassObject _g_AObjectContainer_ClsObj(_AObjectContainerCreateObject);

ECode AObjectContainer::constructor()
{
    m_nCount = 0;
    m_nState = 0;
    return _CCriticalSection_Initialize(&m_lock);
}

ECode AObjectContainer::Add(
    /* [in] */ IInterface * pObject)
{
    ECode ec;

    Void * pContext = _Elastos_CriticalSection_Enter(&m_lock);

    ec = m_container.Add(pObject);
    if (NOERROR == ec) {
        m_nState++;
        m_nCount++;
        pObject->AddRef();
    }
    _Elastos_CriticalSection_Leave(&m_lock, pContext);

    return ec;
}

ECode AObjectContainer::Remove(
    /* [in] */ IInterface * pObject)
{
    ECode ec;

    Void * pContext = _Elastos_CriticalSection_Enter(&m_lock);

    ec = m_container.Remove(pObject);
    if (NOERROR == ec) {
        m_nState++;
        m_nCount--;
        pObject->Release();
    }

    _Elastos_CriticalSection_Leave(&m_lock, pContext);
    return ec;
}

ECode AObjectContainer::GetObjectEnumerator(
    /* [out] */ IObjectEnumerator ** ppObjects)
{
    __CObjectEnumerator *pEnum;

    pEnum = new __CObjectEnumerator(
                    (_IInterface *)this,
                    (ObjectNode *)&m_container.m_head,
                    &m_lock,
                    &m_nState);
    if (!pEnum) return E_OUT_OF_MEMORY;
    *ppObjects = (IObjectEnumerator *)pEnum;
    pEnum->AddRef();

    return NOERROR;
}

ECode AObjectContainer::GetObjectCount(
    /* [out] */ Int32 * pCount)
{
    *pCount = m_nCount;
    return NOERROR;
}

ECode AObjectContainer::Dispose()
{
    ObjectNode *pNode;

    Void * pContext = _Elastos_CriticalSection_Enter(&m_lock);

    m_nCount = 0;
    m_nState++;
    pNode = (ObjectNode *)m_container.m_head.Next();
    while (&m_container.m_head != (DLinkNode *)pNode) {
        pNode->Detach();
        pNode->m_pObject->Release();
        delete pNode;
        pNode = (ObjectNode *)m_container.m_head.Next();
    }
    _Elastos_CriticalSection_Leave(&m_lock, pContext);

    return NOERROR;
}
