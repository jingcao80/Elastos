//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CCallbackContext.h"
#include "CCallbackParcel.h"
#include <pthread.h>
#if !defined(_android) && !defined(_linux)
#include <e_time.h>
#endif
#include <errno.h>
#include <unistd.h>

#define MillisecPerSec 1000
#define MicrosecPerSec 1000000

#define NanoSecPerMicrosec 1000
#define NanoSecPerMilliSec 1000000
#define NanoSecPerSec 1000000000

#define ForEachDLinkNode(t, p, h) \
        for (p = (t)((h)->m_pNext); p != (t)h; p = (t)(p->m_pNext))

#define ForEachDLinkNodeReversely(t, p, h) \
        for (p = (t)((h)->m_pPrev); p != (t)h; p = (t)(p->m_pPrev))

EXTERN_C int CDECL printf(const char *fmt,...);
EXTERN_C int invokeCoalescer(void* pFunc, void* pOldParam, void* pNewParam, int nSize);
EXTERN_C int invokeCallback(CallbackEventFlags cFlags, void* pSender, void* pThis, void* pFunc, void* pParam, int size);

ELAPI_(PCallbackEvent) _Impl_CallbackSink_AllocCallbackEvent(MemorySize size)
{
    // TODO: optimize
    return (PCallbackEvent)malloc(size);
}

ELAPI_(void) _Impl_CallbackSink_FreeCallbackEvent(PVoid pCallbackEvent)
{
    // TODO: optimize

    ((PCallbackEvent)pCallbackEvent)->~_EzCallbackEvent();
    free(pCallbackEvent);
}

//
//  class CCallbackContext
//
UInt32 CCallbackContext::AddRef()
{
    Int32 ref = atomic_inc(&m_cRef);

    return (UInt32)ref;
}

UInt32 CCallbackContext::Release()
{
    Int32 ref = atomic_dec(&m_cRef);

    if (0 == ref) {
        assert(m_currentEvents.IsEmpty());
        delete this;
    }
    assert(ref >= 0);
    return ref;
}

PInterface CCallbackContext::Probe(REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    return NULL;
}

ECode CCallbackContext::GetInterfaceID(
    IInterface *pObject,
    InterfaceID *pIID)
{
    if (NULL == pIID) return E_INVALID_ARGUMENT;

    if (pObject == (IInterface*)this) {
        *pIID = EIID_IInterface;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

void CCallbackContext::PushEvent(PCallbackEvent pEvent)
{
    pthread_mutex_lock(&m_queueLock);
    m_currentEvents.InsertNext(pEvent);
    pthread_mutex_unlock(&m_queueLock);
}

PCallbackEvent CCallbackContext::PopEvent()
{
    pthread_mutex_lock(&m_queueLock);
    PCallbackEvent pCallbackEvent = m_currentEvents.First();
    pCallbackEvent->Detach();
    pthread_mutex_unlock(&m_queueLock);
    return pCallbackEvent;
}

Boolean CCallbackContext::IsExistEvent(PCallbackEvent pEvent)
{
    return IsExistEvent(pEvent->m_pSender, pEvent->m_id,
                            pEvent->m_pHandlerThis, pEvent->m_pHandlerFunc);
}

Boolean CCallbackContext::IsExistEvent(
    PInterface pSender,
    CallbackEventId id,
    PVoid pHandlerThis,
    PVoid pHandlerFunc)
{
    PCallbackEvent pCurtEvent;
    pthread_mutex_lock(&m_queueLock);
    ForEachDLinkNode(PCallbackEvent, pCurtEvent, &m_currentEvents) {
        if ( (pSender == pCurtEvent->m_pSender || pSender == (PInterface)-1)
            && (id == pCurtEvent->m_id || id == (CallbackEventId)-1)
            && (pHandlerThis == pCurtEvent->m_pHandlerThis
                                               || pHandlerThis == (PVoid)-1)
            && (pHandlerFunc == pCurtEvent->m_pHandlerFunc
                                               || pHandlerFunc == (PVoid)-1) ) {
            pthread_mutex_unlock(&m_queueLock);
            return TRUE;
        }
    }

    pthread_mutex_unlock(&m_queueLock);
    return FALSE;
}

ECode CCallbackContext::PostCallbackEvent(PCallbackEvent pCallbackEvent)
{
    assert(pCallbackEvent);

    pthread_mutex_lock(&m_queueLock);

    pCallbackEvent->AddRef();
    if (m_bExitRequested ||
            m_bRequestToQuit ||
            m_Status >= CallbackContextStatus_Finishing) {

        pthread_mutex_unlock(&m_queueLock);

        pCallbackEvent->m_Status = CallingStatus_Cancelled;
        pCallbackEvent->m_bCompleted = TRUE;
        sem_post(&pCallbackEvent->m_pSyncEvent);
        pCallbackEvent->Release();

        return NOERROR;
    }

    Boolean bNeedNotify = m_eventQueue.IsEmpty();

    ECode ec;
    PCallbackEvent pPrevCBEvent;
    PCallbackEvent pCancelingCBEvent;

    if (pCallbackEvent->m_flags & CallbackEventFlag_Duplicated) {
        CallbackEventFlags highestFlag =
            pCallbackEvent->m_flags & CallbackEventFlag_PriorityMask;
        ForEachDLinkNodeReversely(PCallbackEvent, pPrevCBEvent, &m_eventQueue) {
            if (pCallbackEvent->m_id == pPrevCBEvent->m_id
              && pCallbackEvent->m_pSender == pPrevCBEvent->m_pSender
              && pCallbackEvent->m_pHandlerThis == pPrevCBEvent->m_pHandlerThis
              && pCallbackEvent->m_pHandlerFunc == pPrevCBEvent->m_pHandlerFunc) {
                PVoid pUserFunc = pCallbackEvent->m_pCoalesceFunc;
                if (pUserFunc) {
                    Int32 nSize = 0;
                    Byte *pNewBuf = NULL;
                    Byte *pOldBuf = NULL;

                    if ((NULL != pCallbackEvent->m_pParameters)
                        && (NULL != pPrevCBEvent->m_pParameters)) {
                        pCallbackEvent->m_pParameters->GetElementSize(&nSize);
                        pCallbackEvent->m_pParameters->GetElementPayload((Handle32*)&pNewBuf);
                        pPrevCBEvent->m_pParameters->GetElementPayload((Handle32*)&pOldBuf);
                    }

                    ec = invokeCoalescer(pUserFunc, pOldBuf, pNewBuf, nSize);
                    if (FAILED(ec)) {
                        if (ec == E_CANCLE_BOTH_EVENTS) {
                            pCallbackEvent->Release();
                            pPrevCBEvent->Detach();
                            pPrevCBEvent->Release();
                            m_nEventsCount--;
                            bNeedNotify = FALSE;
                            goto Next;
                        }
                        continue;
                    }
                }
                highestFlag =
                    pPrevCBEvent->m_flags & CallbackEventFlag_PriorityMask;
                pCancelingCBEvent = pPrevCBEvent;
                pPrevCBEvent = (PCallbackEvent)pPrevCBEvent->Next();
                pCancelingCBEvent->Detach();
                m_nEventsCount--;
                pCancelingCBEvent->Release();
            }
        }
        if ((pCallbackEvent->m_flags & CallbackEventFlag_PriorityMask)
                > highestFlag) {
            pCallbackEvent->m_flags &= ~CallbackEventFlag_PriorityMask;
            pCallbackEvent->m_flags |= highestFlag;
        }
    }

    ForEachDLinkNodeReversely(PCallbackEvent, pPrevCBEvent, &m_eventQueue) {
        CallbackEventFlags prevPri = pPrevCBEvent->m_flags & CallbackEventFlag_PriorityMask;
        CallbackEventFlags currPri = pCallbackEvent->m_flags & CallbackEventFlag_PriorityMask;
        if ((prevPri < currPri) || ((prevPri == currPri) &&
            (pPrevCBEvent->m_when <= pCallbackEvent->m_when))) {
            pPrevCBEvent->InsertNext(pCallbackEvent);
            m_nEventsCount++;
            goto Next;
        }
    }

    m_eventQueue.InsertFirst(pCallbackEvent);
    m_nEventsCount++;

Next:
    pthread_mutex_unlock(&m_queueLock);

    if (bNeedNotify) {
        sem_post(&m_pThreadEvent);
    }

    return NOERROR;
}

ECode CCallbackContext::SendCallbackEvent(
    PCallbackEvent pCallbackEvent,
    Millisecond32 timeOut)
{
    ECode _ec_;
    Int32 CCStatus = 0;
    WaitResult _wr_ = WaitResult_OK;
    PInterface _pOrgCallbackContext_;
    Boolean bCancelled = FALSE;
    struct timespec ts;
    struct timeval  tp;

    if (INFINITE != timeOut) {
        long long temp1, temp2;
        gettimeofday(&tp, NULL);
        ts.tv_sec  = tp.tv_sec;
        temp1 = (long long)tp.tv_usec * NanoSecPerMicrosec;
        temp2 = (long long)timeOut * NanoSecPerMilliSec;
        ts.tv_nsec = (long)((temp1 + temp2) % NanoSecPerSec);
        ts.tv_sec += (long)((temp1 + temp2) / NanoSecPerSec);
    }

    _pOrgCallbackContext_ = (PInterface)pthread_getspecific(TL_ORG_CALLBACK_SLOT);

    if (_pOrgCallbackContext_) {
        CCStatus = _Impl_CallbackSink_GetStatus(_pOrgCallbackContext_);
    }

    if (_pOrgCallbackContext_ && CCStatus > CallbackContextStatus_Created) {
        _ec_ = _Impl_CallbackSink_GetThreadEvent(
                        _pOrgCallbackContext_, &pCallbackEvent->m_pSyncEvent);
        if (NOERROR == _ec_) {
            _ec_ = _Impl_CallbackSink_PostCallbackEvent(
                            this, pCallbackEvent);
            if (NOERROR == _ec_) {
                _ec_ = _Impl_CallbackSink_WaitForCallbackEvent(
                            _pOrgCallbackContext_, timeOut, &_wr_,
                            (Boolean*)&pCallbackEvent->m_bCompleted,
                            CallbackEventFlag_SyncCall);
                if (NOERROR == _ec_) {
                    if (WaitResult_OK == _wr_) _ec_ = NOERROR;
                    else if (WaitResult_Interrupted == _wr_) {
                        _ec_ = E_INTERRUPTED;
                        bCancelled = TRUE;
                    }
                    else if (WaitResult_TimedOut == _wr_) {
                        _ec_= E_TIMED_OUT;
                        bCancelled = TRUE;
                    }
                }
            }
        }
    }
    else {
        _ec_ = sem_init(&pCallbackEvent->m_pSyncEvent, 0, 0);
        if (NOERROR == _ec_) {
            _ec_ = _Impl_CallbackSink_PostCallbackEvent(this, pCallbackEvent);
            if (NOERROR == _ec_) {
                if (INFINITE == timeOut) {
                    _ec_ = sem_wait(&pCallbackEvent->m_pSyncEvent);
                } else {
                    _ec_ = sem_timedwait(&pCallbackEvent->m_pSyncEvent, &ts);
                }

                if ( _ec_ == -1) {
                    if (errno == ETIMEDOUT) {
                        _ec_= E_TIMED_OUT;
                        bCancelled = TRUE;
                    } else if (errno == EINTR) {
                        _ec_ = E_INTERRUPTED;
                        bCancelled = TRUE;
                    }
                } else {
                    _ec_ = NOERROR;
                }
            }
        }
    }

    if (pCallbackEvent->m_Status == CallingStatus_Cancelled) {
        _ec_ = E_CALLBACK_CANCELED;
    }

    if (bCancelled) {
        pCallbackEvent->m_Status = CallingStatus_Cancelled;
    }

    return _ec_;
}

ECode CCallbackContext::RequestToFinish(Int32 flag)
{
    //ThreadEvent eventType = ThreadEvent_RequestToQuit;
    if (flag ==CallbackContextFinish_Nice) {
        m_bRequestToQuit = TRUE;
        //eventType = ThreadEvent_RequestToQuit;
    }
    else if (flag == CallbackContextFinish_ASAP) {
        m_bRequestToQuit = FALSE;
        //eventType = ThreadEvent_ForceToQuit;
    }
    else return E_NOT_IMPLEMENTED;

    m_bExitRequested = TRUE;
    sem_post(&m_pThreadEvent);

    return NOERROR;
}

ECode CCallbackContext::CancelAllPendingCallbacks(
    PInterface pSender)
{
    register PCallbackEvent pCBEvent;
    PCallbackEvent pCancelingCBEvent;

    pthread_mutex_lock(&m_queueLock);
    ForEachDLinkNode(PCallbackEvent, pCBEvent, &m_eventQueue) {
        if (pCBEvent->m_pSender == pSender) {
            pCancelingCBEvent = pCBEvent;
            pCBEvent = (PCallbackEvent)pCancelingCBEvent->Prev();
            pCancelingCBEvent->Detach();
            --m_nEventsCount;

            pCancelingCBEvent->m_Status = CallingStatus_Cancelled;
            pCancelingCBEvent->m_bCompleted = TRUE;
            sem_post(&pCancelingCBEvent->m_pSyncEvent);
            pCancelingCBEvent->Release();
        }
    }

    pthread_mutex_unlock(&m_queueLock);

    return NOERROR;
}

ECode CCallbackContext::CancelPendingCallback(
    PInterface pSender,
    CallbackEventId id)
{
    register PCallbackEvent pCBEvent;
    PCallbackEvent pCancelingCBEvent;

    pthread_mutex_lock(&m_queueLock);
    ForEachDLinkNode(PCallbackEvent, pCBEvent, &m_eventQueue) {
        if (pCBEvent->m_pSender == pSender
            && pCBEvent->m_id == id) {
            pCancelingCBEvent = pCBEvent;
            pCBEvent = (PCallbackEvent)pCancelingCBEvent->Prev();
            pCancelingCBEvent->Detach();
            --m_nEventsCount;

            pCancelingCBEvent->m_Status = CallingStatus_Cancelled;
            pCancelingCBEvent->m_bCompleted = TRUE;
            sem_post(&pCancelingCBEvent->m_pSyncEvent);
            pCancelingCBEvent->Release();
        }
    }

    pthread_mutex_unlock(&m_queueLock);

    return NOERROR;
}

ECode CCallbackContext::CancelCallbackEvents(
    PInterface pSender,
    CallbackEventId id,
    PVoid pHandlerThis,
    PVoid pHandlerFunc)
{
    register PCallbackEvent pCBEvent;
    PCallbackEvent pCancelingCBEvent;

    pthread_mutex_lock(&m_queueLock);
    ForEachDLinkNode(PCallbackEvent, pCBEvent, &m_eventQueue) {
        if (pCBEvent->m_pSender == pSender
            && pCBEvent->m_id == id
            && pCBEvent->m_pHandlerThis == pHandlerThis
            && pCBEvent->m_pHandlerFunc == pHandlerFunc) {
            pCancelingCBEvent = pCBEvent;
            pCBEvent = (PCallbackEvent)pCancelingCBEvent->Prev();
            pCancelingCBEvent->Detach();
            --m_nEventsCount;

            pCancelingCBEvent->m_Status = CallingStatus_Cancelled;
            pCancelingCBEvent->m_bCompleted = TRUE;
            sem_post(&pCancelingCBEvent->m_pSyncEvent);
            pCancelingCBEvent->Release();
        }
    }
    if (!IsExistEvent(pSender, id, pHandlerThis, pHandlerFunc)) {
        pthread_mutex_unlock(&m_queueLock);
        return NOERROR;
    }
    pthread_mutex_unlock(&m_queueLock);

    // 'm_workingLock' be used waiting for msg loop idled
    if (!pthread_mutex_trylock(&m_workingLock)) {
        pthread_mutex_unlock(&m_workingLock);
    }
    else {
        return E_CALLBACK_IS_BUSY;
    }

    return NOERROR;
}

void CCallbackContext::CancelAllCallbackEvents()
{
    pthread_mutex_lock(&m_queueLock);
    while (!m_eventQueue.IsEmpty()) {
        PCallbackEvent pCallbackEvent = (PCallbackEvent)m_eventQueue.First();

        pCallbackEvent->Detach();
        --m_nEventsCount;

        pCallbackEvent->m_Status = CallingStatus_Cancelled;
        pCallbackEvent->m_bCompleted = TRUE;
        sem_post(&pCallbackEvent->m_pSyncEvent);
        pCallbackEvent->Release();

    }

    pthread_mutex_unlock(&m_queueLock);

}

PCallbackEvent CCallbackContext::GetEvent(Flags32 fPriority)
{
again:
    struct timeval  tp;
    long long now;
    gettimeofday(&tp, NULL);
    now = (long long)tp.tv_sec * MillisecPerSec;
    now += (long long)tp.tv_usec / (MicrosecPerSec / MillisecPerSec);

    PCallbackEvent pCallbackEvent = m_eventQueue.First();
    if (fPriority) {
        while (pCallbackEvent != &m_eventQueue) {
            if ((fPriority & CallbackEventFlag_PriorityMask) <
                ((Flags32)pCallbackEvent->m_flags & CallbackEventFlag_PriorityMask)) {
                if ((fPriority & ~CallbackEventFlag_PriorityMask)==0
                    || ((fPriority & ~CallbackEventFlag_PriorityMask)
                            & pCallbackEvent->m_flags)){
                    if (now < pCallbackEvent->m_when) {
                        pthread_mutex_unlock(&m_queueLock);
                        usleep((pCallbackEvent->m_when - now) *
                                (MicrosecPerSec / MillisecPerSec));
                        pthread_mutex_lock(&m_queueLock);
                        goto again;
                    }

                    pCallbackEvent->Detach();
                    return pCallbackEvent;
                }
            }

            pCallbackEvent = pCallbackEvent->Next();
        }

        return NULL;
    }
    else {
        if (now < pCallbackEvent->m_when) {
            pthread_mutex_unlock(&m_queueLock);
            usleep((pCallbackEvent->m_when - now) *
                    (MicrosecPerSec / MillisecPerSec));
            pthread_mutex_lock(&m_queueLock);
            goto again;
        }

        pCallbackEvent->Detach();
        return pCallbackEvent;
    }
}

Int32 CCallbackContext::HandleCallbackEvents(
    Millisecond32 msTimeOut,
    WaitResult *pResult,
    Boolean* pbOccured,
    Flags32 fPriority)
{
    Boolean bEmpty = FALSE;
    PCallbackEvent pCallbackEvent = NULL;
    IObject* pClientObj = NULL;
    PVoid pLock = NULL;
    struct timespec ts;
    struct timeval  tp;

    Millisecond32 msBegPoint = clock();
    Millisecond32 msLapsedTime = 0;

    PInterface pCallbackContext;
    pCallbackContext = (PInterface)pthread_getspecific(TL_CALLBACK_SLOT);
    this->AddRef();
    pthread_setspecific(TL_CALLBACK_SLOT, (PInterface)this);

    if (INFINITE != msTimeOut) {
        long long temp1, temp2;
        gettimeofday(&tp, NULL);
        ts.tv_sec  = tp.tv_sec;
        temp1 = (long long)tp.tv_usec * NanoSecPerMicrosec;
        temp2 = (long long)(msTimeOut - msLapsedTime) * NanoSecPerMilliSec;
        ts.tv_nsec = (long)((temp1 + temp2) % NanoSecPerSec);
        ts.tv_sec += (long)((temp1 + temp2) / NanoSecPerSec);
    }

    while (TRUE) {
        pthread_mutex_lock(&m_queueLock);

        if (m_bExitRequested) {
            if (!m_bRequestToQuit) {
                pthread_mutex_unlock(&m_queueLock);
                if (pResult) *pResult = WaitResult_Interrupted;
                goto Exit;
            }
        }
        if (m_eventQueue.IsEmpty()) {
            bEmpty = TRUE;
        }
        else {
            pCallbackEvent = GetEvent(fPriority);
            if (pCallbackEvent == NULL) {
                bEmpty = TRUE;
            }
        }
        if (bEmpty) {
            bEmpty = FALSE;
            if (m_bRequestToQuit) {
                pthread_mutex_unlock(&m_queueLock);
                if (pResult) *pResult = WaitResult_Interrupted;
                goto Exit;
            }
            m_Status = CallbackContextStatus_Idling;
            pthread_mutex_unlock(&m_queueLock);
            if (INFINITE == msTimeOut) {
                sem_wait(&m_pThreadEvent);
            } else {
                sem_timedwait(&m_pThreadEvent, &ts);
            }

            if (pbOccured) {
                if (*pbOccured) {
                    if (pResult) *pResult = WaitResult_OK;
                    *pbOccured = FALSE;
                    goto Exit;
                }
            }
        }
        else {
            --m_nEventsCount;
            assert(pCallbackEvent);
            PushEvent(pCallbackEvent);
            pthread_mutex_lock(&m_workingLock);
            m_Status = CallbackContextStatus_Working;
            pthread_mutex_unlock(&m_queueLock);

            if (NULL != pCallbackEvent->m_pHandlerFunc) {
                pClientObj = NULL;
                if (CallbackType_CAR == pCallbackEvent->m_typeOfFunc) {
                    pClientObj = (IObject*)pCallbackEvent->m_pReceiver->Probe(EIID_IObject);
                    assert(pClientObj);
                    if (pClientObj->AddRef() == 1) goto Cancel;
                    pClientObj->Aggregate(AggrType_FriendEnter, (PInterface)&pLock);
                }

                if (pCallbackEvent->m_flags & CallbackEventFlag_SyncCall) {
                    if (CallingStatus_Cancelled == pCallbackEvent->m_Status) {
                        pCallbackEvent->m_bCompleted = TRUE;
                        sem_post(&pCallbackEvent->m_pSyncEvent);
                        goto CallingCompleted;
                    }
                    pCallbackEvent->m_Status = CallingStatus_Running;
                }

                if (pCallbackEvent->m_flags & CallbackEventFlag_DirectCall) {
                        _Impl_SetHelperInfoFlag(HELPER_ASYNC_CALLING, TRUE);
                }
                {
                    Int32 nSize = 0;
                    Byte* pBuf = NULL;

                    if (pCallbackEvent->m_pParameters != NULL) {
                        pCallbackEvent->m_pParameters->GetElementPayload((Handle32*)&pBuf);
                        pCallbackEvent->m_pParameters->GetElementSize(&nSize);
                    }

                    CallbackEventFlags cFlags = pCallbackEvent->m_flags;
                    PVoid pSender = pCallbackEvent->m_pSender;
                    PVoid pThis = pCallbackEvent->m_pHandlerThis;
                    PVoid pFunc = pCallbackEvent->m_pHandlerFunc;

                    pCallbackEvent->m_ecRet = invokeCallback(cFlags, pSender, pThis, pFunc, pBuf, nSize);
                }
                if (pCallbackEvent->m_flags & CallbackEventFlag_DirectCall) {
                    _Impl_SetHelperInfoFlag(HELPER_ASYNC_CALLING, FALSE);
                }

                if (pCallbackEvent->m_flags & CallbackEventFlag_SyncCall) {
                    pCallbackEvent->m_Status = CallingStatus_Completed;
                    pCallbackEvent->m_bCompleted = TRUE;
                    sem_post(&pCallbackEvent->m_pSyncEvent);
                }

CallingCompleted:
                if ((CallbackType_CAR == pCallbackEvent->m_typeOfFunc) && (pClientObj)) {
                    pClientObj->Aggregate(AggrType_FriendLeave, (PInterface)pLock);
                    pClientObj->Release();
                }
            }
Cancel:
            pthread_mutex_unlock(&m_workingLock);

            PopEvent();
            pCallbackEvent->Release();

            if (pbOccured) {
                if (*pbOccured) {
                    if (pResult) *pResult = WaitResult_OK;
                    *pbOccured = FALSE;
                    goto Exit;
                }
            }
        }

        if (msTimeOut != INFINITE) {
            msLapsedTime = clock() - msBegPoint;
            if (msLapsedTime > msTimeOut) {
                if (pResult) *pResult = WaitResult_TimedOut;
                goto Exit;
            }
        }
    }


Exit:
    this->Release();
    pthread_setspecific(TL_CALLBACK_SLOT, pCallbackContext);
    return 0;
}
