//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CCallbackContext.h"

ELAPI _Impl_CallbackSink_TryToHandleEvents(
    IInterface* pCallbackContext)
{
    ECode ec;
    CCallbackContext* pCBCtx;

    if (NULL == pCallbackContext) return 0;

    pCBCtx = (CCallbackContext*)pCallbackContext;
    ec = pCBCtx->HandleCallbackEvents(INFINITE, NULL, NULL, 0);
    pCBCtx->m_Status = CallbackContextStatus_Finishing;

    return ec;
}

ELAPI _Impl_CallbackSink_GetCallbackContext(
    PInterface *ppCallbackContext)
{

    if (NULL == ppCallbackContext) return E_INVALID_ARGUMENT;

    *ppCallbackContext = (PInterface)pthread_getspecific(TL_CALLBACK_SLOT);
    if (NULL == *ppCallbackContext) {
        return E_NOT_CALLBACK_THREAD;
    }
    else {
        (*ppCallbackContext)->AddRef();
    }

    return NOERROR;
}

ELAPI _Impl_CallbackSink_InitCallbackContext(
    PInterface *ppCallbackContext)
{
    CCallbackContext *pCallbackContext;

    pCallbackContext = new CCallbackContext();
    if (NULL == pCallbackContext) return E_OUT_OF_MEMORY;

    pCallbackContext->AddRef();

    ECode ec = pCallbackContext->Initialize();
    if (FAILED(ec)) {
        pCallbackContext->Release();
        return ec;
    }

    if (NULL != ppCallbackContext) {
        *ppCallbackContext = pCallbackContext;
    }

    return NOERROR;
}

ELAPI _Impl_CallbackSink_AcquireCallbackContext(PInterface *ppCallbackContext)
{
    CCallbackContext *pCallbackContext;
    ECode ec;

    if (NULL == ppCallbackContext) return E_INVALID_ARGUMENT;

    *ppCallbackContext = (PInterface)pthread_getspecific(TL_ORG_CALLBACK_SLOT);
    if (*ppCallbackContext) {
        (*ppCallbackContext)->AddRef();
        return NOERROR;
    }

    pCallbackContext = new CCallbackContext();
    if (NULL == pCallbackContext) return E_OUT_OF_MEMORY;

    ec = pCallbackContext->Initialize();
    if (FAILED(ec)) return ec;

    pCallbackContext->AddRef();
    *ppCallbackContext = pCallbackContext;

    return NOERROR;
}

ELAPI _Impl_CallbackSink_PostCallbackEventAtTime(
    PInterface pCallbackContext,
    PCallbackEvent pCallbackEvent,
    _ELASTOS Millisecond64 uptimeMillis)
{
    if (NULL == pCallbackContext || NULL == pCallbackEvent) {
        return E_INVALID_ARGUMENT;
    }

    pCallbackEvent->m_when = uptimeMillis;

    return ((CCallbackContext *)pCallbackContext)->
                PostCallbackEvent(pCallbackEvent);
}

ELAPI _Impl_CallbackSink_PostCallbackEvent(
    IInterface *pCallbackContext,
    PCallbackEvent pCallbackEvent)
{
    struct timeval now;
    _ELASTOS Millisecond64 when;

    gettimeofday(&now, NULL);
    when = (now.tv_sec * 1000 + now.tv_usec/1000.0) + 0.5;

    return _Impl_CallbackSink_PostCallbackEventAtTime(
            pCallbackContext, pCallbackEvent, when);
}

ELAPI _Impl_CallbackSink_SendCallbackEvent(
    IInterface *pCallbackContext,
    PCallbackEvent pCallbackEvent,
    Millisecond32 msTimeOut)
{
    if (NULL == pCallbackContext || NULL == pCallbackEvent) {
        return E_INVALID_ARGUMENT;
    }

    return ((CCallbackContext *)pCallbackContext)->
                SendCallbackEvent(pCallbackEvent, msTimeOut);
}

ELAPI _Impl_CallbackSink_WaitForCallbackEvent(
    IInterface *pCallbackContext,
    Millisecond32 msTimeOut,
    WaitResult* pResult,
    Boolean * pbEventOccured,
    Flags32 fPriority)
{
    if (NULL == pCallbackContext) {
        return E_INVALID_ARGUMENT;
    }

    return ((CCallbackContext *)pCallbackContext)->
        HandleCallbackEvents(msTimeOut, pResult, pbEventOccured, fPriority);
}

ELAPI _Impl_CallbackSink_CleanupAllCallbacks(
    IInterface *pCallbackContext)
{
    if (NULL == pCallbackContext) return E_INVALID_ARGUMENT;

    ((CCallbackContext *)pCallbackContext)->CancelAllCallbackEvents();
    return NOERROR;
}

ELAPI _Impl_CallbackSink_CancelAllPendingCallbacks(
    IInterface *pCallbackContext,
    PInterface pSender)
{
    if (NULL == pCallbackContext) return E_INVALID_ARGUMENT;

    return ((CCallbackContext *)pCallbackContext)->
                CancelAllPendingCallbacks(pSender);
}

ELAPI _Impl_CallbackSink_CancelPendingCallback(
    IInterface *pCallbackContext,
    PInterface pSender,
    CallbackEventId id)
{
    if (NULL == pCallbackContext) return E_INVALID_ARGUMENT;

    return ((CCallbackContext *)pCallbackContext)->
                CancelPendingCallback(pSender, id);
}

ELAPI _Impl_CallbackSink_CancelCallbackEvents(
    IInterface *pCallbackContext,
    PInterface pSender,
    CallbackEventId id,
    PVoid pHandlerThis,
    PVoid pHandlerFunc)
{
    if (NULL == pCallbackContext) return E_INVALID_ARGUMENT;

    return ((CCallbackContext *)pCallbackContext)->
                CancelCallbackEvents(pSender, id, pHandlerThis, pHandlerFunc);
}

ELAPI _Impl_CallbackSink_RequestToFinish(
    PInterface pCallbackContext, Int32 Flag)
{
    CCallbackContext* pContext;
    ECode ec;
    if (pCallbackContext == NULL) {
        ec = _Impl_CallbackSink_GetCallbackContext((PInterface*)&pContext);
        if (FAILED(ec)) return ec;
        ec = pContext->RequestToFinish(Flag);
        pContext->Release();
    }
    else {
        pContext = (CCallbackContext*)pCallbackContext;
        ec = pContext->RequestToFinish(Flag);
    }
    return ec;
}

ELAPI _Impl_CallbackSink_GetThreadEvent(
    PInterface pCallbackContext,
    sem_t* ppEvent)
{
    if (NULL == pCallbackContext || NULL == ppEvent) {
        return E_INVALID_ARGUMENT;
    }

    ppEvent =  ((CCallbackContext *)pCallbackContext)->m_ppThreadEvent;

    return NOERROR;
}

ELAPI_(Int32) _Impl_CallbackSink_GetStatus(
    PInterface pCallbackContext)
{
    if (pCallbackContext) {
        return ((CCallbackContext *)pCallbackContext)->m_Status;
    }
    return 0;
}

ECode _Impl_CheckClsId(
        PInterface pServerObj,
        const ClassID* pClassiD,
        PInterface *ppServerObj)
{
    IObject *pObject;
	char str[80];
    ClassID clsid;
	clsid.pUunm = str;

    pObject = (IObject*)pServerObj->Probe(EIID_IObject);
    if (NULL == pObject) return E_INVALID_ARGUMENT;

    pObject->GetClassID(&clsid);
    while (*(EMuid *)&clsid != *(EMuid *)pClassiD) {
        pObject = (IObject*)pObject->Probe(EIID_SUPER_OBJECT);
        if (NULL == pObject) return E_INVALID_ARGUMENT;

        pObject->GetClassID(&clsid);
    }
    *ppServerObj = pObject;  // don't AddRef, Caller don't Release either.
    return NOERROR;
}

ECode _Impl_AcquireCallbackHandler(
        PInterface pServerObj,
        REIID iid,
        PInterface *ppHandler)
{
    ICallbackSink *pSink = NULL;
    PInterface pHandler = NULL;
    ECode ec;

    do {
        if (pServerObj) {
            ec = _CObject_AcquireCallbackSink(pServerObj, &pSink);
            if (FAILED(ec)) return ec;
            pHandler = pSink->Probe(iid);
            if (pHandler) break;
        }
        else {
            return E_NO_INTERFACE;
        }

        pServerObj = pServerObj->Probe(EIID_SUPER_OBJECT);
        pSink->Release();

    } while (NULL == pHandler);

    *ppHandler = pHandler;
    return NOERROR;
}
