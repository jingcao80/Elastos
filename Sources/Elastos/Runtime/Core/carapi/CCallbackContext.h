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

#ifndef __CCALLBACKCONTEXT_H__
#define __CCALLBACKCONTEXT_H__

#include <elapi.h>
#include <clsdef.h>
#include <semaphore.h>
#include <sys/time.h>

EXTERN void _Impl_CallbackSink_FreeCallbackEvent(PCallbackEvent pCallbackEvent);

_ELASTOS_NAMESPACE_BEGIN

class CCallbackContext
    : public ElLightRefBase
    , public IInterface
{
public:
    CCallbackContext();

    virtual ~CCallbackContext();

    CARAPI Initialize();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [in] */ InterfaceID* iid);

    CARAPI PostCallbackEvent(
        /* [in] */ PCallbackEvent callbackEvent);

    CARAPI SendCallbackEvent(
        /* [in] */ PCallbackEvent callbackEvent,
        /* [in] */ Int32 timeOut);

    CARAPI RequestToFinish(
        /* [in] */ Int32 flag);

    CARAPI CancelAllPendingCallbacks(
        /* [in] */ PInterface sender);

    CARAPI CancelPendingCallback(
        /* [in] */ PInterface sender,
        /* [in] */ CallbackEventId id);

    CARAPI CancelCallbackEvents(
        /* [in] */ PInterface sender,
        /* [in] */ CallbackEventId id,
        /* [in] */ PVoid handlerThis,
        /* [in] */ PVoid handlerFunc);

    CARAPI_(Int32) HandleCallbackEvents(
        /* [in] */ Int32 msTimeOut = INFINITE,
        /* [in] */ WaitResult* result = NULL,
        /* [in] */ Boolean* occured = NULL,
        /* [in] */ UInt32 priority = 0);

    CARAPI_(void) CancelAllCallbackEvents();

private:
    CARAPI_(PCallbackEvent) PopEvent();

    CARAPI_(void) PushEvent(
        /* [in] */ PCallbackEvent event);

    CARAPI_(Boolean) IsExistEvent(
        /* [in] */ PCallbackEvent event);

    CARAPI_(Boolean) IsExistEvent(
        /* [in] */ PInterface sender,
        /* [in] */ CallbackEventId id,
        /* [in] */ PVoid handlerThis,
        /* [in] */ PVoid handlerFunc);

    CARAPI_(PCallbackEvent) GetEvent(
        /* [in] */ UInt32 priority);

public:
    pthread_mutex_t mWorkingLock;
    pthread_mutex_t mQueueLock;
    Boolean mExitRequested;
    Boolean mRequestToQuit;
    CarCallbackEvent mEventQueue;
    CarCallbackEvent mCurrentEvents;
    sem_t mThreadEvent;
    Int32 mStatus;
    Int32 mEventsCount;
};

_ELASTOS_NAMESPACE_END

#endif // __CCALLBACKCONTEXT_H__
