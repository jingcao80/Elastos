//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "CCallbackContext.h"
#include "CCallbackParcel.h"
#include <pthread.h>
#if !defined(_linux)
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
        for (p = (t)((h)->mNext); p != (t)h; p = (t)(p->mNext))

#define ForEachDLinkNodeReversely(t, p, h) \
        for (p = (t)((h)->mPrev); p != (t)h; p = (t)(p->mPrev))

EXTERN_C int CDECL printf(const char *fmt,...);
EXTERN_C int invokeCoalescer(void* pFunc, void* pOldParam, void* pNewParam, int size);
EXTERN_C int invokeCallback(CallbackEventFlags flags, void* sender, void* pThis, void* pFunc, void* pParam, int size);

ELAPI_(PCallbackEvent) _Impl_CallbackSink_AllocCallbackEvent(Elastos::Int32 size)
{
    // TODO: optimize
    return (PCallbackEvent)malloc(size);
}

ELAPI_(void) _Impl_CallbackSink_FreeCallbackEvent(Elastos::PVoid callbackEvent)
{
    // TODO: optimize

    ((PCallbackEvent)callbackEvent)->~CarCallbackEvent();
    free(callbackEvent);
}

_ELASTOS_NAMESPACE_BEGIN

CCallbackContext::CCallbackContext()
    : mExitRequested(FALSE)
    , mRequestToQuit(FALSE)
    , mStatus(CallbackContextStatus_Created)
    , mEventsCount(0)
{
    mEventQueue.Initialize();
    mCurrentEvents.Initialize();
}

CCallbackContext::~CCallbackContext()
{
    assert(mCurrentEvents.IsEmpty());

    CancelAllCallbackEvents();

    pthread_mutex_destroy(&mWorkingLock);
    pthread_mutex_destroy(&mQueueLock);

    sem_destroy(&mThreadEvent);
}

//
//  class CCallbackContext
//
UInt32 CCallbackContext::AddRef()
{
    return ElLightRefBase::AddRef();
}

UInt32 CCallbackContext::Release()
{
    return ElLightRefBase::Release();
}

PInterface CCallbackContext::Probe(REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    return NULL;
}

ECode CCallbackContext::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [in] */ InterfaceID* iid)
{
    if (NULL == iid) return E_INVALID_ARGUMENT;

    if (object == (IInterface*)this) {
        *iid = EIID_IInterface;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CCallbackContext::Initialize()
{
    pthread_mutexattr_t recursiveAttr;

    pthread_mutexattr_init(&recursiveAttr);
    pthread_mutexattr_settype(&recursiveAttr, PTHREAD_MUTEX_RECURSIVE);
    ECode ec = sem_init(&mThreadEvent, 0, 0);

    if (FAILED(ec)) return ec;
    if (pthread_mutex_init(&mWorkingLock, &recursiveAttr)) goto Exit;
    if (pthread_mutex_init(&mQueueLock, &recursiveAttr)) goto Exit;
Exit:
    pthread_mutexattr_destroy(&recursiveAttr);
    return NOERROR;
}

void CCallbackContext::PushEvent(
    /* [in] */ PCallbackEvent event)
{
    pthread_mutex_lock(&mQueueLock);
    mCurrentEvents.InsertNext(event);
    pthread_mutex_unlock(&mQueueLock);
}

PCallbackEvent CCallbackContext::PopEvent()
{
    pthread_mutex_lock(&mQueueLock);
    PCallbackEvent callbackEvent = mCurrentEvents.First();
    callbackEvent->Detach();
    pthread_mutex_unlock(&mQueueLock);
    return callbackEvent;
}

Boolean CCallbackContext::IsExistEvent(
    /* [in] */ PCallbackEvent event)
{
    return IsExistEvent(event->mSender, event->mID,
            event->mHandlerThis, event->mHandlerFunc);
}

Boolean CCallbackContext::IsExistEvent(
    /* [in] */ PInterface sender,
    /* [in] */ CallbackEventId id,
    /* [in] */ PVoid handlerThis,
    /* [in] */ PVoid handlerFunc)
{
    PCallbackEvent curtEvent;
    pthread_mutex_lock(&mQueueLock);
    ForEachDLinkNode(PCallbackEvent, curtEvent, &mCurrentEvents) {
        if ( (sender == curtEvent->mSender || sender == (PInterface)-1)
            && (id == curtEvent->mID || id == (CallbackEventId)-1)
            && (handlerThis == curtEvent->mHandlerThis || handlerThis == (PVoid)-1)
            && (handlerFunc == curtEvent->mHandlerFunc || handlerFunc == (PVoid)-1)) {
            pthread_mutex_unlock(&mQueueLock);
            return TRUE;
        }
    }

    pthread_mutex_unlock(&mQueueLock);
    return FALSE;
}

ECode CCallbackContext::PostCallbackEvent(
    /* [in] */ PCallbackEvent callbackEvent)
{
    assert(callbackEvent);

    pthread_mutex_lock(&mQueueLock);

    callbackEvent->AddRef();
    if (mExitRequested ||
            mRequestToQuit ||
            mStatus >= CallbackContextStatus_Finishing) {

        pthread_mutex_unlock(&mQueueLock);

        callbackEvent->mStatus = CallingStatus_Cancelled;
        callbackEvent->mCompleted = TRUE;
        sem_post(&callbackEvent->mSyncEvent);
        callbackEvent->Release();

        return NOERROR;
    }

    Boolean needNotify = mEventQueue.IsEmpty();

    ECode ec;
    PCallbackEvent prevCBEvent;
    PCallbackEvent cancelingCBEvent;

    if (callbackEvent->mFlags & CallbackEventFlag_Duplicated) {
        CallbackEventFlags highestFlag =
            callbackEvent->mFlags & CallbackEventFlag_PriorityMask;
        ForEachDLinkNodeReversely(PCallbackEvent, prevCBEvent, &mEventQueue) {
            if (callbackEvent->mID == prevCBEvent->mID
              && callbackEvent->mSender == prevCBEvent->mSender
              && callbackEvent->mHandlerThis == prevCBEvent->mHandlerThis
              && callbackEvent->mHandlerFunc == prevCBEvent->mHandlerFunc) {
                PVoid userFunc = callbackEvent->mCoalesceFunc;
                if (userFunc) {
                    Int32 size = 0;
                    Byte* newBuf = NULL;
                    Byte* oldBuf = NULL;

                    if ((NULL != callbackEvent->mParameters)
                        && (NULL != prevCBEvent->mParameters)) {
                        callbackEvent->mParameters->GetDataSize(&size);
                        callbackEvent->mParameters->GetDataPayload((Handle32*)&newBuf);
                        prevCBEvent->mParameters->GetDataPayload((Handle32*)&oldBuf);
                    }

                    ec = invokeCoalescer(userFunc, oldBuf, newBuf, size);
                    if (FAILED(ec)) {
                        if (ec == E_CANCLE_BOTH_EVENTS) {
                            callbackEvent->Release();
                            prevCBEvent->Detach();
                            prevCBEvent->Release();
                            mEventsCount--;
                            needNotify = FALSE;
                            goto Next;
                        }
                        continue;
                    }
                }
                highestFlag = prevCBEvent->mFlags & CallbackEventFlag_PriorityMask;
                cancelingCBEvent = prevCBEvent;
                prevCBEvent = (PCallbackEvent)prevCBEvent->Next();
                cancelingCBEvent->Detach();
                mEventsCount--;
                cancelingCBEvent->Release();
            }
        }
        if ((callbackEvent->mFlags & CallbackEventFlag_PriorityMask)
                > highestFlag) {
            callbackEvent->mFlags &= ~CallbackEventFlag_PriorityMask;
            callbackEvent->mFlags |= highestFlag;
        }
    }

    ForEachDLinkNodeReversely(PCallbackEvent, prevCBEvent, &mEventQueue) {
        CallbackEventFlags prevPri = prevCBEvent->mFlags & CallbackEventFlag_PriorityMask;
        CallbackEventFlags currPri = callbackEvent->mFlags & CallbackEventFlag_PriorityMask;
        if ((prevPri < currPri) || ((prevPri == currPri) &&
            (prevCBEvent->mWhen <= callbackEvent->mWhen))) {
            prevCBEvent->InsertNext(callbackEvent);
            mEventsCount++;
            goto Next;
        }
    }

    mEventQueue.InsertFirst(callbackEvent);
    mEventsCount++;

Next:
    pthread_mutex_unlock(&mQueueLock);

    if (needNotify) {
        sem_post(&mThreadEvent);
    }

    return NOERROR;
}

ECode CCallbackContext::SendCallbackEvent(
    /* [in] */ PCallbackEvent callbackEvent,
    /* [in] */ Int32 timeOut)
{
    ECode ec;
    Int32 CCStatus = 0;
    WaitResult wr = WaitResult_OK;
    PInterface orgCallbackContext;
    Boolean cancelled = FALSE;
    struct timespec ts;
    struct timeval tp;

    if (INFINITE != timeOut) {
        long long temp1, temp2;
        gettimeofday(&tp, NULL);
        ts.tv_sec  = tp.tv_sec;
        temp1 = (long long)tp.tv_usec * NanoSecPerMicrosec;
        temp2 = (long long)timeOut * NanoSecPerMilliSec;
        ts.tv_nsec = (long)((temp1 + temp2) % NanoSecPerSec);
        ts.tv_sec += (long)((temp1 + temp2) / NanoSecPerSec);
    }

    orgCallbackContext = (PInterface)pthread_getspecific(TL_ORG_CALLBACK_SLOT);

    if (orgCallbackContext) {
        CCStatus = _Impl_CallbackSink_GetStatus(orgCallbackContext);
    }

    if (orgCallbackContext && CCStatus > CallbackContextStatus_Created) {
        ec = _Impl_CallbackSink_GetThreadEvent(
                orgCallbackContext, &callbackEvent->mSyncEvent);
        if (NOERROR == ec) {
            ec = _Impl_CallbackSink_PostCallbackEvent(
                    this, callbackEvent);
            if (NOERROR == ec) {
                ec = _Impl_CallbackSink_WaitForCallbackEvent(
                        orgCallbackContext, timeOut, &wr,
                        (Boolean*)&callbackEvent->mCompleted,
                        CallbackEventFlag_SyncCall);
                if (NOERROR == ec) {
                    if (WaitResult_OK == wr) ec = NOERROR;
                    else if (WaitResult_Interrupted == wr) {
                        ec = E_INTERRUPTED;
                        cancelled = TRUE;
                    }
                    else if (WaitResult_TimedOut == wr) {
                        ec = E_TIMED_OUT;
                        cancelled = TRUE;
                    }
                }
            }
        }
    }
    else {
        ec = sem_init(&callbackEvent->mSyncEvent, 0, 0);
        if (NOERROR == ec) {
            ec = _Impl_CallbackSink_PostCallbackEvent(this, callbackEvent);
            if (NOERROR == ec) {
                if (INFINITE == timeOut) {
                    ec = sem_wait(&callbackEvent->mSyncEvent);
                }
                else {
                    ec = sem_timedwait(&callbackEvent->mSyncEvent, &ts);
                }

                if ( ec == -1) {
                    if (errno == ETIMEDOUT) {
                        ec = E_TIMED_OUT;
                        cancelled = TRUE;
                    }
                    else if (errno == EINTR) {
                        ec = E_INTERRUPTED;
                        cancelled = TRUE;
                    }
                }
                else {
                    ec = NOERROR;
                }
            }
        }
    }

    if (callbackEvent->mStatus == CallingStatus_Cancelled) {
        ec = E_CALLBACK_CANCELED;
    }

    if (cancelled) {
        callbackEvent->mStatus = CallingStatus_Cancelled;
    }

    return ec;
}

ECode CCallbackContext::RequestToFinish(
    /* [in] */ Int32 flag)
{
    if (flag == CallbackContextFinish_Nice) {
        mRequestToQuit = TRUE;
    }
    else if (flag == CallbackContextFinish_ASAP) {
        mRequestToQuit = FALSE;
    }
    else return E_NOT_IMPLEMENTED;

    mExitRequested = TRUE;
    sem_post(&mThreadEvent);

    return NOERROR;
}

ECode CCallbackContext::CancelAllPendingCallbacks(
    /* [in] */ PInterface sender)
{
    PCallbackEvent cbEvent;
    PCallbackEvent cancelingCBEvent;

    pthread_mutex_lock(&mQueueLock);
    ForEachDLinkNode(PCallbackEvent, cbEvent, &mEventQueue) {
        if (cbEvent->mSender.Get() == sender) {
            cancelingCBEvent = cbEvent;
            cbEvent = (PCallbackEvent)cancelingCBEvent->Prev();
            cancelingCBEvent->Detach();
            --mEventsCount;

            cancelingCBEvent->mStatus = CallingStatus_Cancelled;
            cancelingCBEvent->mCompleted = TRUE;
            sem_post(&cancelingCBEvent->mSyncEvent);
            cancelingCBEvent->Release();
        }
    }

    pthread_mutex_unlock(&mQueueLock);

    return NOERROR;
}

ECode CCallbackContext::CancelPendingCallback(
    /* [in] */ PInterface sender,
    /* [in] */ CallbackEventId id)
{
    PCallbackEvent cbEvent;
    PCallbackEvent cancelingCBEvent;

    pthread_mutex_lock(&mQueueLock);
    ForEachDLinkNode(PCallbackEvent, cbEvent, &mEventQueue) {
        if (cbEvent->mSender.Get() == sender
            && cbEvent->mID == id) {
            cancelingCBEvent = cbEvent;
            cbEvent = (PCallbackEvent)cancelingCBEvent->Prev();
            cancelingCBEvent->Detach();
            --mEventsCount;

            cancelingCBEvent->mStatus = CallingStatus_Cancelled;
            cancelingCBEvent->mCompleted = TRUE;
            sem_post(&cancelingCBEvent->mSyncEvent);
            cancelingCBEvent->Release();
        }
    }

    pthread_mutex_unlock(&mQueueLock);

    return NOERROR;
}

ECode CCallbackContext::CancelCallbackEvents(
    /* [in] */ PInterface sender,
    /* [in] */ CallbackEventId id,
    /* [in] */ PVoid handlerThis,
    /* [in] */ PVoid handlerFunc)
{
    PCallbackEvent cbEvent;
    PCallbackEvent cancelingCBEvent;

    pthread_mutex_lock(&mQueueLock);
    ForEachDLinkNode(PCallbackEvent, cbEvent, &mEventQueue) {
        if (cbEvent->mSender.Get() == sender
            && cbEvent->mID == id
            && cbEvent->mHandlerThis == handlerThis
            && cbEvent->mHandlerFunc == handlerFunc) {
            cancelingCBEvent = cbEvent;
            cbEvent = (PCallbackEvent)cancelingCBEvent->Prev();
            cancelingCBEvent->Detach();
            --mEventsCount;

            cancelingCBEvent->mStatus = CallingStatus_Cancelled;
            cancelingCBEvent->mCompleted = TRUE;
            sem_post(&cancelingCBEvent->mSyncEvent);
            cancelingCBEvent->Release();
        }
    }
    if (!IsExistEvent(sender, id, handlerThis, handlerFunc)) {
        pthread_mutex_unlock(&mQueueLock);
        return NOERROR;
    }
    pthread_mutex_unlock(&mQueueLock);

    // 'm_workingLock' be used waiting for msg loop idled
    if (!pthread_mutex_trylock(&mWorkingLock)) {
        pthread_mutex_unlock(&mWorkingLock);
    }
    else {
        return E_CALLBACK_IS_BUSY;
    }

    return NOERROR;
}

void CCallbackContext::CancelAllCallbackEvents()
{
    pthread_mutex_lock(&mQueueLock);
    while (!mEventQueue.IsEmpty()) {
        PCallbackEvent callbackEvent = (PCallbackEvent)mEventQueue.First();

        callbackEvent->Detach();
        --mEventsCount;

        callbackEvent->mStatus = CallingStatus_Cancelled;
        callbackEvent->mCompleted = TRUE;
        sem_post(&callbackEvent->mSyncEvent);
        callbackEvent->Release();

    }

    pthread_mutex_unlock(&mQueueLock);

}

PCallbackEvent CCallbackContext::GetEvent(
    /* [in] */ UInt32 priority)
{
again:
    struct timeval tp;
    long long now;
    gettimeofday(&tp, NULL);
    now = (long long)tp.tv_sec * MillisecPerSec;
    now += (long long)tp.tv_usec / (MicrosecPerSec / MillisecPerSec);

    PCallbackEvent callbackEvent = mEventQueue.First();
    if (priority) {
        while (callbackEvent != &mEventQueue) {
            if ((priority & CallbackEventFlag_PriorityMask) <
                ((UInt32)callbackEvent->mFlags & CallbackEventFlag_PriorityMask)) {
                if ((priority & ~CallbackEventFlag_PriorityMask) == 0
                        || ((priority & ~CallbackEventFlag_PriorityMask)
                        & callbackEvent->mFlags)) {
                    if (now < callbackEvent->mWhen) {
                        pthread_mutex_unlock(&mQueueLock);
                        usleep((callbackEvent->mWhen - now) *
                                (MicrosecPerSec / MillisecPerSec));
                        pthread_mutex_lock(&mQueueLock);
                        goto again;
                    }

                    callbackEvent->Detach();
                    return callbackEvent;
                }
            }

            callbackEvent = callbackEvent->Next();
        }

        return NULL;
    }
    else {
        if (now < callbackEvent->mWhen) {
            pthread_mutex_unlock(&mQueueLock);
            usleep((callbackEvent->mWhen - now) *
                    (MicrosecPerSec / MillisecPerSec));
            pthread_mutex_lock(&mQueueLock);
            goto again;
        }

        callbackEvent->Detach();
        return callbackEvent;
    }
}

Int32 CCallbackContext::HandleCallbackEvents(
    /* [in] */ Int32 msTimeOut,
    /* [in] */ WaitResult* result,
    /* [in] */ Boolean* occured,
    /* [in] */ UInt32 priority)
{
    Boolean empty = FALSE;
    PCallbackEvent callbackEvent = NULL;
    IObject* clientObj = NULL;
    PVoid lock = NULL;
    struct timespec ts;
    struct timeval tp;

    Int32 msBegPoint = clock();
    Int32 msLapsedTime = 0;

    PInterface callbackContext;
    callbackContext = (PInterface)pthread_getspecific(TL_CALLBACK_SLOT);
    this->AddRef();
    pthread_setspecific(TL_CALLBACK_SLOT, (PInterface)this);

    if (INFINITE != msTimeOut) {
        long long temp1, temp2;
        gettimeofday(&tp, NULL);
        ts.tv_sec = tp.tv_sec;
        temp1 = (long long)tp.tv_usec * NanoSecPerMicrosec;
        temp2 = (long long)(msTimeOut - msLapsedTime) * NanoSecPerMilliSec;
        ts.tv_nsec = (long)((temp1 + temp2) % NanoSecPerSec);
        ts.tv_sec += (long)((temp1 + temp2) / NanoSecPerSec);
    }

    while (TRUE) {
        pthread_mutex_lock(&mQueueLock);

        if (mExitRequested) {
            if (!mRequestToQuit) {
                pthread_mutex_unlock(&mQueueLock);
                if (result) *result = WaitResult_Interrupted;
                goto Exit;
            }
        }
        if (mEventQueue.IsEmpty()) {
            empty = TRUE;
        }
        else {
            callbackEvent = GetEvent(priority);
            if (callbackEvent == NULL) {
                empty = TRUE;
            }
        }
        if (empty) {
            empty = FALSE;
            if (mRequestToQuit) {
                pthread_mutex_unlock(&mQueueLock);
                if (result) *result = WaitResult_Interrupted;
                goto Exit;
            }
            mStatus = CallbackContextStatus_Idling;
            pthread_mutex_unlock(&mQueueLock);
            if (INFINITE == msTimeOut) {
                sem_wait(&mThreadEvent);
            }
            else {
                sem_timedwait(&mThreadEvent, &ts);
            }

            if (occured) {
                if (*occured) {
                    if (result) *result = WaitResult_OK;
                    *occured = FALSE;
                    goto Exit;
                }
            }
        }
        else {
            --mEventsCount;
            assert(callbackEvent);
            PushEvent(callbackEvent);
            pthread_mutex_lock(&mWorkingLock);
            mStatus = CallbackContextStatus_Working;
            pthread_mutex_unlock(&mQueueLock);

            if (NULL != callbackEvent->mHandlerFunc) {
                clientObj = NULL;
                if (CallbackType_CAR == callbackEvent->mTypeOfFunc) {
                    clientObj = (IObject*)callbackEvent->mReceiver->Probe(EIID_IObject);
                    assert(clientObj);
                    if (clientObj->AddRef() == 1) goto Cancel;
                    clientObj->Aggregate(AggrType_FriendEnter, (PInterface)&lock);
                }

                if (callbackEvent->mFlags & CallbackEventFlag_SyncCall) {
                    if (CallingStatus_Cancelled == callbackEvent->mStatus) {
                        callbackEvent->mCompleted = TRUE;
                        sem_post(&callbackEvent->mSyncEvent);
                        goto CallingCompleted;
                    }
                    callbackEvent->mStatus = CallingStatus_Running;
                }

                if (callbackEvent->mFlags & CallbackEventFlag_DirectCall) {
                        _Impl_SetHelperInfoFlag(HELPER_ASYNC_CALLING, TRUE);
                }
                {
                    Int32 size = 0;
                    Byte* buf = NULL;

                    if (callbackEvent->mParameters != NULL) {
                        callbackEvent->mParameters->GetDataPayload((Handle32*)&buf);
                        callbackEvent->mParameters->GetDataSize(&size);
                    }

                    CallbackEventFlags flags = callbackEvent->mFlags;
                    PVoid sender = callbackEvent->mSender;
                    PVoid pThis = callbackEvent->mHandlerThis;
                    PVoid pFunc = callbackEvent->mHandlerFunc;

                    callbackEvent->mRet = invokeCallback(flags, sender, pThis, pFunc, buf, size);
                }
                if (callbackEvent->mFlags & CallbackEventFlag_DirectCall) {
                    _Impl_SetHelperInfoFlag(HELPER_ASYNC_CALLING, FALSE);
                }

                if (callbackEvent->mFlags & CallbackEventFlag_SyncCall) {
                    callbackEvent->mStatus = CallingStatus_Completed;
                    callbackEvent->mCompleted = TRUE;
                    sem_post(&callbackEvent->mSyncEvent);
                }

CallingCompleted:
                if ((CallbackType_CAR == callbackEvent->mTypeOfFunc) && (clientObj)) {
                    clientObj->Aggregate(AggrType_FriendLeave, (PInterface)lock);
                    clientObj->Release();
                }
            }
Cancel:
            pthread_mutex_unlock(&mWorkingLock);

            PopEvent();
            callbackEvent->Release();

            if (occured) {
                if (*occured) {
                    if (result) *result = WaitResult_OK;
                    *occured = FALSE;
                    goto Exit;
                }
            }
        }

        if (msTimeOut != INFINITE) {
            msLapsedTime = clock() - msBegPoint;
            if (msLapsedTime > msTimeOut) {
                if (result) *result = WaitResult_TimedOut;
                goto Exit;
            }
        }
    }


Exit:
    this->Release();
    pthread_setspecific(TL_CALLBACK_SLOT, callbackContext);
    return 0;
}

_ELASTOS_NAMESPACE_END
