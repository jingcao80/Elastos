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

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/Choreographer.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/hardware/display/DisplayManagerGlobal.h"
#include "elastos/droid/os/CMessage.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/utility/TimeUtils.h"
#include <elastos/core/Math.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::Hardware::Display::DisplayManagerGlobal;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::CMessage;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Utility::TimeUtils;

namespace Elastos {
namespace Droid {
namespace View {

const char* Choreographer::TAG = "Choreographer";
const Boolean Choreographer::DEBUG = FALSE;
const Int64 Choreographer::DEFAULT_FRAME_DELAY;
volatile Int64 Choreographer::sFrameDelay = DEFAULT_FRAME_DELAY;

static Boolean PropertiesGetBoolean(
    /* [in] */ const String& name,
    /* [in] */ Boolean def)
{
    Boolean rst;
    SystemProperties::GetBoolean(name, def, &rst);
    return rst;
}

static Int32 PropertiesGetInt32(
    /* [in] */ const String& name,
    /* [in] */ Int32 def)
{
    Int32 rst;
    SystemProperties::GetInt32(name, def, &rst);
    return rst;
}

pthread_key_t Choreographer::sKey;
Boolean Choreographer::sHaveKey = FALSE;
const Boolean Choreographer::USE_VSYNC = PropertiesGetBoolean(
        String("debug.choreographer.vsync"), TRUE);
const Boolean Choreographer::USE_FRAME_TIME = PropertiesGetBoolean(
        String("debug.choreographer.frametime"), TRUE);
const Int32 Choreographer::SKIPPED_FRAME_WARNING_LIMIT = PropertiesGetInt32(
        String("debug.choreographer.skipwarning"), 30);
const AutoPtr<IObject> Choreographer::FRAME_CALLBACK_TOKEN = new Choreographer::Token();
const Int32 Choreographer::CALLBACK_LAST = IChoreographer::CALLBACK_TRAVERSAL;

const Int32 Choreographer::MSG_DO_FRAME;// = 0;
const Int32 Choreographer::MSG_DO_SCHEDULE_VSYNC;// = 1;
const Int32 Choreographer::MSG_DO_SCHEDULE_CALLBACK;// = 2;

ECode Choreographer::Token::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)
    *info = "FRAME_CALLBACK_TOKEN";
    return NOERROR;
}

Choreographer::FrameHandler::FrameHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ Choreographer* host)
    : Handler(looper)
    , mHost(host)
{}

ECode Choreographer::FrameHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetNanoTime(&now);

    switch (what) {
        case Choreographer::MSG_DO_FRAME:
            mHost->DoFrame(now, 0);
            break;
        case Choreographer::MSG_DO_SCHEDULE_VSYNC:
            mHost->DoScheduleVsync();
            break;
        case Choreographer::MSG_DO_SCHEDULE_CALLBACK:
            mHost->DoScheduleCallback(arg1);
            break;
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(Choreographer::FrameDisplayEventReceiver, DisplayEventReceiver, IRunnable)

Choreographer::FrameDisplayEventReceiver::FrameDisplayEventReceiver(
    /* [in] */ ILooper* looper,
    /* [in] */ Choreographer* owner)
    : DisplayEventReceiver(looper)
    , mOwner(owner)
{
}

//@Override
ECode Choreographer::FrameDisplayEventReceiver::OnVsync(
    /* [in] */ Int64 timestampNanos,
    /* [in] */ Int32 builtInDisplayId,
    /* [in] */ Int32 frame)
{
    // Ignore vsync from secondary display.
    // This can be problematic because the call to scheduleVsync() is a one-shot.
    // We need to ensure that we will still receive the vsync from the primary
    // display which is the one we really care about.  Ideally we should schedule
    // vsync for a particular display.
    // At this time Surface Flinger won't send us vsyncs for secondary displays
    // but that could change in the future so let's log a message to help us remember
    // that we need to fix this.
    if (builtInDisplayId != ISurfaceControl::BUILT_IN_DISPLAY_ID_MAIN) {
       Logger::D(Choreographer::TAG,
           "Received vsync from secondary display, but we don't support "
           "this case yet.  Choreographer needs a way to explicitly request "
           "vsync for a specific display to ensure it doesn't lose track "
           "of its scheduled vsync.");
        ScheduleVsync();
        return NOERROR;
    }

    // Post the vsync event to the Handler.
    // The idea is to prevent incoming vsync events from completely starving
    // the message queue.  If there are no messages in the queue with timestamps
    // earlier than the frame time, then the vsync event will be processed immediately.
    // Otherwise, messages that predate the vsync event will be handled first.
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    Int64 now;
    system->GetNanoTime(&now);
    if (timestampNanos > now) {
       Logger::W(Choreographer::TAG,
           "Frame time is %f ms in the future! Check that"
           " graphics HAL is generating vsync timestamps using the"
           " correct timebase.", (timestampNanos - now) * 0.000001f);
        timestampNanos = now;
    }

    if (mHavePendingVsync) {
       Logger::W(Choreographer::TAG, "Already have a pending vsync event."
           "  There should only be one at a time.");
    }
    else {
        mHavePendingVsync = TRUE;
    }

    mTimestampNanos = timestampNanos;
    mFrame = frame;

    AutoPtr<IMessage> msg = CMessage::Obtain(mOwner->mHandler, this);
    msg->SetAsynchronous(TRUE);
    Boolean result;
    mOwner->mHandler->SendMessageAtTime(msg, timestampNanos / TimeUtils::NANOS_PER_MS, &result);
    return NOERROR;
}

ECode Choreographer::FrameDisplayEventReceiver::Run()
{
    mHavePendingVsync = FALSE;
    mOwner->DoFrame(mTimestampNanos, mFrame);
    return NOERROR;
}

void Choreographer::CallbackRecord::Run(
    /* [in] */ Int64 frameTimeNanos)
{
    if (mToken == FRAME_CALLBACK_TOKEN) {
        IFrameCallback::Probe(mAction)->DoFrame(frameTimeNanos);
    }
    else {
        IRunnable::Probe(mAction)->Run();
    }
}

Choreographer::CallbackQueue::CallbackQueue(
    /* [in] */ Choreographer* owner)
    : mOwner(owner)
{
}

Boolean Choreographer::CallbackQueue::HasDueCallbacksLocked(
    /* [in] */ Int64 now)
{
    return mHead != NULL && mHead->mDueTime <= now;
}

AutoPtr<Choreographer::CallbackRecord> Choreographer::CallbackQueue::ExtractDueCallbacksLocked(
    /* [in] */ Int64 now)
{
    AutoPtr<CallbackRecord> callbacks = mHead;
    if (callbacks == NULL || callbacks->mDueTime > now) {
        return NULL;
    }

    AutoPtr<CallbackRecord> last = callbacks;
    AutoPtr<CallbackRecord> next = last->mNext;
    while (next != NULL) {
        if (next->mDueTime > now) {
            last->mNext = NULL;
            break;
        }
        last = next;
        next = next->mNext;
    }
    mHead = next;
    return callbacks;
}

void Choreographer::CallbackQueue::AddCallbackLocked(
    /* [in] */ Int64 dueTime,
    /* [in] */ IInterface* action,
    /* [in] */ IObject* token)
{
    AutoPtr<CallbackRecord> callback =
        mOwner->ObtainCallbackLocked(dueTime, action, token);
    AutoPtr<CallbackRecord> entry = mHead;
    if (entry == NULL) {
        mHead = callback;
        return;
    }
    if (dueTime < entry->mDueTime) {
        callback->mNext = entry;
        mHead = callback;
        return;
    }
    while (entry->mNext != NULL) {
        if (dueTime < entry->mNext->mDueTime) {
            callback->mNext = entry->mNext;
            break;
        }
        entry = entry->mNext;
    }
    entry->mNext = callback;
}

void Choreographer::CallbackQueue::RemoveCallbacksLocked(
    /* [in] */ IInterface* action,
    /* [in] */ IObject* token)
{
    AutoPtr<CallbackRecord> predecessor;
    for (AutoPtr<CallbackRecord> callback = mHead; callback != NULL;) {
        AutoPtr<CallbackRecord> next = callback->mNext;
        if ((action == NULL || callback->mAction.Get() == action)
            && (token == NULL || callback->mToken.Get() == token)) {
            if (predecessor != NULL) {
                predecessor->mNext = next;
            }
            else {
                mHead = next;
            }
            mOwner->RecycleCallbackLocked(callback);
        }
        else {
            predecessor = callback;
        }
        callback = next;
    }
}

CAR_INTERFACE_IMPL(Choreographer, Object, IChoreographer);

Choreographer::Choreographer(
    /* [in] */ ILooper* looper)
    : mFrameScheduled(FALSE)
    , mCallbacksRunning(FALSE)
{
    mLooper = looper;
    mHandler = new FrameHandler(looper, this);

    mDisplayEventReceiver = USE_VSYNC ? new FrameDisplayEventReceiver(looper, this) : NULL;
    mLastFrameTimeNanos = Elastos::Core::Math::INT64_MIN_VALUE;

    mFrameIntervalNanos = (Int64)(1000000000 / GetRefreshRate());

    mCallbackQueues = ArrayOf<CallbackQueue*>::Alloc(CALLBACK_LAST + 1);
    for (Int32 i = 0; i <= CALLBACK_LAST; i++) {
        AutoPtr<CallbackQueue> cb = new CallbackQueue(this);
        mCallbackQueues->Set(i, cb);
    }
}

Float Choreographer::GetRefreshRate()
{
    AutoPtr<IDisplayInfo> di;
    DisplayManagerGlobal::GetInstance()->GetDisplayInfo(
        IDisplay::DEFAULT_DISPLAY, (IDisplayInfo**)&di);

    Float refreshRate;
    di->GetRefreshRate(&refreshRate);

    return refreshRate;
}

static void ChoreographerDestructor(void* st)
{
    Choreographer* obj = static_cast<Choreographer*>(st);
    if (obj) {
        obj->Release();
    }
}

AutoPtr<Choreographer> Choreographer::GetInstance()
{
    if (!sHaveKey) {
        ASSERT_TRUE(pthread_key_create(&sKey, ChoreographerDestructor) == 0);
        sHaveKey = TRUE;
    }

    AutoPtr<Choreographer> instance = (Choreographer*)pthread_getspecific(sKey);
    if (instance == NULL) {
        AutoPtr<ILooper> looper = Looper::GetMyLooper();
        if (looper == NULL) {
            Logger::E(TAG, "The current thread must have a looper!");
            // assert(0);
            //throw new IllegalStateException("The current thread must have a looper!");
        }
        instance = new Choreographer(looper);
        ASSERT_TRUE(pthread_setspecific(sKey, instance.Get()) == 0);
        instance->AddRef();
    }

    return instance;
}

Int64 Choreographer::GetFrameDelay()
{
    return sFrameDelay;
}

void Choreographer::SetFrameDelay(
    /* [in] */ Int64 frameDelay)
{
    sFrameDelay = frameDelay;
}

Int64 Choreographer::SubtractFrameDelay(
    /* [in] */ Int64 delayMillis)
{
    const Int64 frameDelay = sFrameDelay;
    return delayMillis <= frameDelay ? 0 : delayMillis - frameDelay;
}

ECode Choreographer::GetFrameIntervalNanos(
    /* [out] */ Int64* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mFrameIntervalNanos;
    return NOERROR;
}

ECode Choreographer::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IPrintWriter* writer)
{
    String innerPrefix = prefix + String("  ");
    writer->Print(prefix);
    writer->Println(String("Choreographer:"));
    writer->Print(innerPrefix);
    writer->Print(String("mFrameScheduled="));
    writer->Println(mFrameScheduled);
    writer->Print(innerPrefix);
    writer->Print(String("mLastFrameTime="));
    writer->Println(TimeUtils::FormatUptime(mLastFrameTimeNanos / 1000000));
    return NOERROR;
}

ECode Choreographer::PostCallback(
    /* [in] */ Int32 callbackType,
    /* [in] */ IRunnable* action,
    /* [in] */ IObject* token)
{
    return PostCallbackDelayed(callbackType, action, token, 0);
}

ECode Choreographer::PostCallbackDelayed(
    /* [in] */ Int32 callbackType,
    /* [in] */ IRunnable* action,
    /* [in] */ IObject* token,
    /* [in] */ Int64 delayMillis)
{
    if (action == NULL) {
        Logger::E(TAG, "action must not be NULL");
        assert(0 && "PostCallbackDelayed");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (callbackType < 0 || callbackType > CALLBACK_LAST) {
        Logger::E(TAG, "callbackType is invalid");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    PostCallbackDelayedInternal(callbackType, action, token, delayMillis);
    return NOERROR;
}

void Choreographer::PostCallbackDelayedInternal(
    /* [in] */ Int32 callbackType,
    /* [in] */ IInterface* action,
    /* [in] */ IObject* token,
    /* [in] */ Int64 delayMillis)
{
    if (DEBUG) {
        Logger::D(
            TAG, "PostCallback: type=%d, action=0x%08x, token=0x%08x, delayMillis=%d",
            callbackType, action, token, delayMillis);
    }

    AutoLock lock(mLock);
    Int64 now = SystemClock::GetUptimeMillis();
    Int64 dueTime = now + delayMillis;
    (*mCallbackQueues)[callbackType]->AddCallbackLocked(dueTime, action, token);

    if (dueTime <= now) {
        ScheduleFrameLocked(now);
    }
    else {
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_DO_SCHEDULE_CALLBACK, action, (IMessage**)&msg);
        msg->SetArg1(callbackType);
        msg->SetAsynchronous(TRUE);
        Boolean result;
        mHandler->SendMessageAtTime(msg, dueTime, &result);
    }
}

ECode Choreographer::RemoveCallbacks(
    /* [in] */ Int32 callbackType,
    /* [in] */ IRunnable* action,
    /* [in] */ IObject* token)
{
    if (callbackType < 0 || callbackType > CALLBACK_LAST) {
        Logger::E(TAG, "callbackType is invalid");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    RemoveCallbacksInternal(callbackType, action, token);
    return NOERROR;
}

void Choreographer::RemoveCallbacksInternal(
    /* [in] */ Int32 callbackType,
    /* [in] */ IInterface* action,
    /* [in] */ IObject* token)
{
    if (DEBUG) {
        Logger::D(TAG, "RemoveCallbacks: type=%d, action=0x%08x, token=0x%08x",
            callbackType, action, token);
    }

    AutoLock lock(mLock);
    (*mCallbackQueues)[callbackType]->RemoveCallbacksLocked(action, token);
    if (action != NULL && token == NULL) {
        mHandler->RemoveMessages(MSG_DO_SCHEDULE_CALLBACK, action->Probe(EIID_IInterface));
    }
}

ECode Choreographer::PostFrameCallback(
    /* [in] */ IFrameCallback* callback)
{
    return PostFrameCallbackDelayed(callback, 0);
}

ECode Choreographer::PostFrameCallbackDelayed(
    /* [in] */ IFrameCallback* callback,
    /* [in] */ Int64 delayMillis)
{
    if (callback == NULL) {
        Logger::E(TAG, "callback must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    PostCallbackDelayedInternal(CALLBACK_ANIMATION,
            callback, FRAME_CALLBACK_TOKEN, delayMillis);
    return NOERROR;
}

ECode Choreographer::RemoveFrameCallback(
    /* [in] */ IFrameCallback* callback)
{
    if (callback == NULL) {
        Logger::E(TAG, "callback must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    RemoveCallbacksInternal(CALLBACK_ANIMATION, callback, FRAME_CALLBACK_TOKEN);
    return NOERROR;
}

ECode Choreographer::GetFrameTime(
    /* [out] */ Int64* frameTime)
{
    VALIDATE_NOT_NULL(frameTime);
    FAIL_RETURN(GetFrameTimeNanos(frameTime));
    *frameTime = *frameTime / TimeUtils::NANOS_PER_MS;

    return NOERROR;
}

ECode Choreographer::GetFrameTimeNanos(
    /* [out] */ Int64* frameTimeNanos)
{
    AutoLock lock(mLock);
    if (!mCallbacksRunning) {
        Logger::E(TAG, "This method must only be called as "
            "part of a callback while a frame is in progress.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 nanos;
    system->GetNanoTime(&nanos);
    *frameTimeNanos = USE_FRAME_TIME ? mLastFrameTimeNanos : nanos;
    return NOERROR;
}


void Choreographer::ScheduleFrameLocked(
    /* [in] */ Int64 now)
{
    if (!mFrameScheduled) {
        mFrameScheduled = TRUE;

        if (USE_VSYNC) {
            if (DEBUG) {
                Logger::D(TAG, "Scheduling next frame on vsync.");
            }

            // If running on the Looper thread, then schedule the vsync immediately,
            // otherwise post a message to schedule the vsync from the UI thread
            // as soon as possible.
            if (IsRunningOnLooperThreadLocked()) {
                ScheduleVsyncLocked();
            }
            else {
                AutoPtr<IMessage> msg;
                mHandler->ObtainMessage(MSG_DO_SCHEDULE_VSYNC, (IMessage**)&msg);
                msg->SetAsynchronous(true);
                Boolean result;
                mHandler->SendMessageAtFrontOfQueue(msg, &result);
            }
        }
        else {
            Int64 nextFrameTime = Elastos::Core::Math::Max(
                    mLastFrameTimeNanos / TimeUtils::NANOS_PER_MS + sFrameDelay, now);
            if (DEBUG) {
                Logger::D(TAG, "Scheduling next frame in %lld ms.", nextFrameTime - now);
            }

            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(MSG_DO_FRAME, (IMessage**)&msg);
            msg->SetAsynchronous(true);
            Boolean result;
            mHandler->SendMessageAtTime(msg, nextFrameTime, &result);
        }
    }
}

void Choreographer::DoFrame(
    /* [in] */ Int64 frameTimeNanos,
    /* [in] */ Int32 frame)
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 startNanos;
    {
        AutoLock lock(mLock);
        if (!mFrameScheduled) {
            return; // no work to do
        }

        system->GetNanoTime(&startNanos);
        const Int64 jitterNanos = startNanos - frameTimeNanos;
        if (jitterNanos >= mFrameIntervalNanos) {
            const Int64 skippedFrames = jitterNanos / mFrameIntervalNanos;
            if (skippedFrames >= SKIPPED_FRAME_WARNING_LIMIT) {
//                Logger::I(TAG, "Skipped %d frames! The application may be"
//                    "doing too much work on its main thread.", skippedFrames);
            }

            const Int64 lastFrameOffset = jitterNanos % mFrameIntervalNanos;
            if (DEBUG) {
                Logger::D(TAG, "Missed vsync by %f ms which is more than "
                    "the frame interval of %f ms! Skipping %d frames and "
                    "setting frame time to %f ms in the past.",
                    jitterNanos * 0.000001f, mFrameIntervalNanos * 0.000001f,
                    skippedFrames, lastFrameOffset * 0.000001f);
            }
            frameTimeNanos = startNanos - lastFrameOffset;
        }

        if (frameTimeNanos < mLastFrameTimeNanos) {
            if (DEBUG) {
                Logger::D(TAG, "Frame time appears to be going backwards. May be due"
                    " to a previously skipped frame.  Waiting for next vsync.");
            }
            ScheduleVsyncLocked();
            return;
        }

        mFrameScheduled = FALSE;
        mLastFrameTimeNanos = frameTimeNanos;
    }

    DoCallbacks(Choreographer::CALLBACK_INPUT, frameTimeNanos);
    DoCallbacks(Choreographer::CALLBACK_ANIMATION, frameTimeNanos);
    DoCallbacks(Choreographer::CALLBACK_TRAVERSAL, frameTimeNanos);

    if (DEBUG) {
        Int64 endNanos;
        system->GetNanoTime(&endNanos);
        Logger::D(TAG, "Frame %d: Finished, took %f ms, latency %f ms.",
            frame, (endNanos - startNanos) * 0.000001f,
            (startNanos - frameTimeNanos) * 0.000001f);
    }
}

void Choreographer::DoCallbacks(
    /* [in] */ Int32 callbackType,
    /* [in] */ Int64 frameTimeNanos)
{
    AutoPtr<CallbackRecord> callbacks;
    {
        AutoLock lock(mLock);
        // We use "now" to determine when callbacks become due because it's possible
        // for earlier processing phases in a frame to post callbacks that should run
        // in a following phase, such as an input event that causes an animation to start.
        const Int64 now = SystemClock::GetUptimeMillis();
        callbacks = (*mCallbackQueues)[callbackType]->ExtractDueCallbacksLocked(now);
        if (callbacks == NULL) {
            return;
        }
        mCallbacksRunning = TRUE;
    }

    for (CallbackRecord* c = callbacks; c != NULL; c = c->mNext) {
        if (DEBUG) {
            Logger::D(TAG, "RunCallback: type=%d, action=0x%08x, token=0x%08x"
                ", latencyMillis=%lld", callbackType, c->mAction.Get(), c->mToken.Get(),
                SystemClock::GetUptimeMillis() - c->mDueTime);
        }
        c->Run(frameTimeNanos);
    }

    {
        AutoLock lock(mLock);
        mCallbacksRunning = FALSE;
        do {
            AutoPtr<CallbackRecord> next = callbacks->mNext;
            RecycleCallbackLocked(callbacks);
            callbacks = next;
        } while (callbacks != NULL);
    }
}

void Choreographer::DoScheduleVsync()
{
    AutoLock lock(mLock);
    if (mFrameScheduled) {
        ScheduleVsyncLocked();
    }
}

void Choreographer::DoScheduleCallback(
    /* [in] */ Int32 callbackType)
{
    AutoLock lock(mLock);
    if (!mFrameScheduled) {
        const Int64 now = SystemClock::GetUptimeMillis();
        if ((*mCallbackQueues)[callbackType]->HasDueCallbacksLocked(now)) {
            ScheduleFrameLocked(now);
        }
    }
}


void Choreographer::ScheduleVsyncLocked()
{
    mDisplayEventReceiver->ScheduleVsync();
}

Boolean Choreographer::IsRunningOnLooperThreadLocked()
{
    return Looper::GetMyLooper() == mLooper;
}

AutoPtr<Choreographer::CallbackRecord> Choreographer::ObtainCallbackLocked(
    /* [in] */ Int64 dueTime,
    /* [in] */ IInterface* action,
    /* [in] */ IObject* token)
{
    AutoPtr<CallbackRecord> callback = mCallbackPool;
    if (callback == NULL) {
        callback = new CallbackRecord();
    }
    else {
        mCallbackPool = callback->mNext;
        callback->mNext = NULL;
    }
    callback->mDueTime = dueTime;
    callback->mAction = action;
    callback->mToken = token;
    return callback;
}

void Choreographer::RecycleCallbackLocked(
    /* [in] */ CallbackRecord* callback)
{
    callback->mAction = NULL;
    callback->mToken = NULL;
    callback->mNext = mCallbackPool;
    mCallbackPool = callback;
}

void Choreographer::HandleDoFrame()
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetNanoTime(&now);
    DoFrame(now, 0);
}

} // namespace View
} // namespace Droid
} // namespace Elastos
