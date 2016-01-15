//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CCallbackContext.h"

using namespace Elastos;

ELAPI _Impl_CallbackSink_TryToHandleEvents(
    /* [in] */ PInterface callbackContext)
{
    if (NULL == callbackContext) return NOERROR;

    CCallbackContext* cbCtxObj = (CCallbackContext*)callbackContext;
    ECode ec = cbCtxObj->HandleCallbackEvents(INFINITE, NULL, NULL, 0);
    cbCtxObj->mStatus = CallbackContextStatus_Finishing;

    return ec;
}

ELAPI _Impl_CallbackSink_GetCallbackContext(
    /* [out] */ PInterface* callbackContext)
{
    if (NULL == callbackContext) return E_INVALID_ARGUMENT;

    *callbackContext = (PInterface)pthread_getspecific(TL_CALLBACK_SLOT);
    if (NULL == *callbackContext) {
        return E_NOT_CALLBACK_THREAD;
    }
    else {
        (*callbackContext)->AddRef();
    }

    return NOERROR;
}

ELAPI _Impl_CallbackSink_InitCallbackContext(
    /* [out] */ PInterface* callbackContext)
{
    CCallbackContext* cbCtxObj = new CCallbackContext();
    if (NULL == cbCtxObj) return E_OUT_OF_MEMORY;

    cbCtxObj->AddRef();

    ECode ec = cbCtxObj->Initialize();
    if (FAILED(ec)) {
        cbCtxObj->Release();
        return ec;
    }

    if (NULL != callbackContext) {
        *callbackContext = cbCtxObj;
    }

    return NOERROR;
}

ELAPI _Impl_CallbackSink_AcquireCallbackContext(
    /* [out] */ PInterface* callbackContext)
{

    if (NULL == callbackContext) return E_INVALID_ARGUMENT;

    *callbackContext = (PInterface)pthread_getspecific(TL_ORG_CALLBACK_SLOT);
    if (*callbackContext) {
        (*callbackContext)->AddRef();
        return NOERROR;
    }

    CCallbackContext* cbCtxObj = new CCallbackContext();
    if (NULL == cbCtxObj) return E_OUT_OF_MEMORY;

    ECode ec = cbCtxObj->Initialize();
    if (FAILED(ec)) return ec;

    cbCtxObj->AddRef();
    *callbackContext = cbCtxObj;

    return NOERROR;
}

ELAPI _Impl_CallbackSink_PostCallbackEventAtTime(
    /* [in] */ PInterface callbackContext,
    /* [in] */ PCallbackEvent callbackEvent,
    /* [in] */ Int64 uptimeMillis)
{
    if (NULL == callbackContext || NULL == callbackEvent) {
        return E_INVALID_ARGUMENT;
    }

    callbackEvent->mWhen = uptimeMillis;

    return ((CCallbackContext *)callbackContext)->
                PostCallbackEvent(callbackEvent);
}

ELAPI _Impl_CallbackSink_PostCallbackEvent(
    /* [in] */ PInterface callbackContext,
    /* [in] */ PCallbackEvent callbackEvent)
{
    struct timeval now;
    Int64 when;

    gettimeofday(&now, NULL);
    when = (now.tv_sec * 1000 + now.tv_usec/1000.0) + 0.5;

    return _Impl_CallbackSink_PostCallbackEventAtTime(
            callbackContext, callbackEvent, when);
}

ELAPI _Impl_CallbackSink_SendCallbackEvent(
    /* [in] */ PInterface callbackContext,
    /* [in] */ PCallbackEvent callbackEvent,
    /* [in] */ Int32 msTimeOut)
{
    if (NULL == callbackContext || NULL == callbackEvent) {
        return E_INVALID_ARGUMENT;
    }

    return ((CCallbackContext *)callbackContext)->
                SendCallbackEvent(callbackEvent, msTimeOut);
}

ELAPI _Impl_CallbackSink_WaitForCallbackEvent(
    /* [in] */ PInterface callbackContext,
    /* [in] */ Int32 msTimeOut,
    /* [out] */ WaitResult* result,
    /* [out] */ Boolean* eventOccured,
    /* [in] */ UInt32 priority)
{
    if (NULL == callbackContext) {
        return E_INVALID_ARGUMENT;
    }

    return ((CCallbackContext *)callbackContext)->
        HandleCallbackEvents(msTimeOut, result, eventOccured, priority);
}

ELAPI _Impl_CallbackSink_CleanupAllCallbacks(
    /* [in] */ PInterface callbackContext)
{
    if (NULL == callbackContext) return E_INVALID_ARGUMENT;

    ((CCallbackContext *)callbackContext)->CancelAllCallbackEvents();
    return NOERROR;
}

ELAPI _Impl_CallbackSink_CancelAllPendingCallbacks(
    /* [in] */ PInterface callbackContext,
    /* [in] */ PInterface sender)
{
    if (NULL == callbackContext) return E_INVALID_ARGUMENT;

    return ((CCallbackContext *)callbackContext)->
                CancelAllPendingCallbacks(sender);
}

ELAPI _Impl_CallbackSink_CancelPendingCallback(
    /* [in] */ PInterface callbackContext,
    /* [in] */ PInterface sender,
    /* [in] */ CallbackEventId id)
{
    if (NULL == callbackContext) return E_INVALID_ARGUMENT;

    return ((CCallbackContext *)callbackContext)->
                CancelPendingCallback(sender, id);
}

ELAPI _Impl_CallbackSink_CancelCallbackEvents(
    /* [in] */ PInterface callbackContext,
    /* [in] */ PInterface sender,
    /* [in] */ CallbackEventId id,
    /* [in] */ PVoid handlerThis,
    /* [in] */ PVoid handlerFunc)
{
    if (NULL == callbackContext) return E_INVALID_ARGUMENT;

    return ((CCallbackContext *)callbackContext)->
                CancelCallbackEvents(sender, id, handlerThis, handlerFunc);
}

ELAPI _Impl_CallbackSink_RequestToFinish(
    /* [in] */ PInterface callbackContext,
    /* [in] */ Int32 Flag)
{
    CCallbackContext* cbCtxObj;
    ECode ec;
    if (callbackContext == NULL) {
        ec = _Impl_CallbackSink_GetCallbackContext((PInterface*)&cbCtxObj);
        if (FAILED(ec)) return ec;
        ec = cbCtxObj->RequestToFinish(Flag);
        cbCtxObj->Release();
    }
    else {
        cbCtxObj = (CCallbackContext*)callbackContext;
        ec = cbCtxObj->RequestToFinish(Flag);
    }
    return ec;
}

ELAPI _Impl_CallbackSink_GetThreadEvent(
    /* [in] */ PInterface callbackContext,
    /* [out] */ sem_t* event)
{
    if (NULL == callbackContext || NULL == event) {
        return E_INVALID_ARGUMENT;
    }

    event = &((CCallbackContext *)callbackContext)->mThreadEvent;

    return NOERROR;
}

ELAPI_(Int32) _Impl_CallbackSink_GetStatus(
    /* [in] */ PInterface callbackContext)
{
    if (callbackContext) {
        return ((CCallbackContext *)callbackContext)->mStatus;
    }
    return 0;
}

ELAPI _Impl_CheckClsId(
    /* [in] */ PInterface serverObj,
    /* [in] */ const ClassID* classiD,
    /* [out] */ PInterface* outServerObj)
{
    IObject* object;
    char str[80];
    ClassID clsid;
    clsid.mUunm = str;

    object = (IObject*)serverObj->Probe(EIID_IObject);
    if (NULL == object) return E_INVALID_ARGUMENT;

    object->GetClassID(&clsid);
    while (*(EMuid *)&clsid != *(EMuid *)classiD) {
        object = (IObject*)object->Probe(EIID_SUPER_OBJECT);
        if (NULL == object) return E_INVALID_ARGUMENT;

        object->GetClassID(&clsid);
    }
    *outServerObj = object;  // don't AddRef, Caller don't Release either.
    return NOERROR;
}

ELAPI _Impl_AcquireCallbackHandler(
    /* [in] */ PInterface serverObj,
    /* [in] */ REIID iid,
    /* [out] */ PInterface* outHandler)
{
    ICallbackSink* sink = NULL;
    PInterface handler = NULL;

    do {
        if (serverObj) {
            ECode ec = _CObject_AcquireCallbackSink(serverObj, &sink);
            if (FAILED(ec)) return ec;
            handler = sink->Probe(iid);
            if (handler) break;
        }
        else {
            return E_NO_INTERFACE;
        }

        serverObj = serverObj->Probe(EIID_SUPER_OBJECT);
        sink->Release();

    } while (NULL == handler);

    *outHandler = handler;
    return NOERROR;
}
