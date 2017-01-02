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

#include "elastos/droid/view/CInputQueue.h"
#include "elastos/droid/view/MotionEvent.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/NativeMessageQueue.h"
#include "elastos/droid/utility/CInt64SparseArray.h"

using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::NativeMessageQueue;
using Elastos::Droid::Utility::CInt64SparseArray;
using Elastos::Core::CCloseGuardHelper;
using Elastos::Core::ICloseGuardHelper;

namespace Elastos {
namespace Droid {
namespace View {

void CInputQueue::ActiveInputEvent::Recycle()
{
    mToken = NULL;
    mCallback = NULL;
}

static struct {
} gInputQueueClassInfo;

CAR_OBJECT_IMPL(CInputQueue);
CAR_INTERFACE_IMPL(CInputQueue, Object, IInputQueue);
CInputQueue::CInputQueue()
    : mPtr(0)
{
}

CInputQueue::~CInputQueue()
{
    Dispose(TRUE);
}

ECode CInputQueue::constructor()
{
    CInt64SparseArray::New((IInt64SparseArray**)&mActiveEventArray);
    mActiveInputEventPool = new Pools::SimplePool<ActiveInputEvent>(20);
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    ASSERT_SUCCEEDED(NativeInit(wr, Looper::GetMyQueue(), &mPtr));

    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mCloseGuard);
    mCloseGuard->Open(String("CInputQueue::Dispose"));
    return NOERROR;
}

ECode CInputQueue::Dispose()
{
    return Dispose(FALSE);
}

ECode CInputQueue::Dispose(
    /* [in] */ Boolean finalized)
{
    if (mCloseGuard != NULL) {
        if (finalized) {
            mCloseGuard->WarnIfOpen();
        }
        mCloseGuard->Close();
    }

    if (mPtr != 0) {
        NativeDispose(mPtr);
        mPtr = 0;
    }
    return NOERROR;
}

ECode CInputQueue::GetNativePtr(
    /* [out] */ Int64* native)
{
    VALIDATE_NOT_NULL(native);
    *native = mPtr;
    return NOERROR;
}

ECode CInputQueue::SendInputEvent(
    /* [in] */ IInputEvent* e,
    /* [in] */ IInterface* token,
    /* [in] */ Boolean predispatch,
    /* [in] */ IInputQueueFinishedInputEventCallback* callback)
{
    AutoPtr<ActiveInputEvent> event = ObtainActiveInputEvent(token, callback);
    Int64 id = 0;
    if (IKeyEvent::Probe(e)) {
        id = NativeSendKeyEvent(mPtr, IKeyEvent::Probe(e), predispatch);
    } else {
        NativeSendMotionEvent(mPtr, IMotionEvent::Probe(e), &id);
    }
    mActiveEventArray->Put(id, (IObject*)event->Probe(EIID_IObject));
    return NOERROR;
}

void CInputQueue::FinishInputEvent(
    /* [in] */ Int64 id,
    /* [in] */ Boolean handled)
{
    Int32 index = 0;
    mActiveEventArray->IndexOfKey(id, &index);
    if (index >= 0) {
        AutoPtr<IInterface> obj;
        mActiveEventArray->ValueAt(index, (IInterface**)&obj);
        AutoPtr<ActiveInputEvent> e = (ActiveInputEvent*)IObject::Probe(obj);
        mActiveEventArray->RemoveAt(index);
        e->mCallback->OnFinishedInputEvent(e->mToken, handled);
        RecycleActiveInputEvent(e);
    }
}

AutoPtr<CInputQueue::ActiveInputEvent> CInputQueue::ObtainActiveInputEvent(
    /* [in] */ IInterface* token,
    /* [in] */ IInputQueueFinishedInputEventCallback* callback)
{
    AutoPtr<ActiveInputEvent> e = mActiveInputEventPool->AcquireItem();
    if (e == NULL) {
        e = new ActiveInputEvent();
    }
    e->mToken = token;
    e->mCallback = callback;
    return e;
}

void CInputQueue::RecycleActiveInputEvent(
    /* [in] */ ActiveInputEvent* e)
{
    e->Recycle();
    mActiveInputEventPool->ReleaseItem(e);
}

ECode CInputQueue::NativeInit(
    /* [in] */ IWeakReference/*<InputQueue>*/* weakQueue,
    /* [in] */ IMessageQueue* msgQueue,
    /* [out] */ Int64* ptr)
{
    VALIDATE_NOT_NULL(ptr);
    AutoPtr<NativeMessageQueue> messageQueue = Elastos_Os_MessageQueue_GetMessageQueue(msgQueue);
    if (messageQueue == NULL) {
        // jniThrowRuntimeException(env, "NativeMessageQueue is not initialized.");
        *ptr = 0;
        return E_RUNTIME_EXCEPTION;
    }
    android::sp<InputQueue> queue = InputQueue::CreateQueue(weakQueue, messageQueue->GetLooper());
    if (!queue.get()) {
        // jniThrowRuntimeException(env, "InputQueue failed to initialize");
        *ptr = 0;
        return E_RUNTIME_EXCEPTION;
    }

    queue->incStrong(&gInputQueueClassInfo);
    *ptr = reinterpret_cast<Int64>(queue.get());
    return NOERROR;
}

Int64 CInputQueue::NativeSendKeyEvent(
    /* [in] */ Int64 ptr,
    /* [in] */ IKeyEvent* e,
    /* [in] */ Boolean predispatch)
{
    InputQueue* queue = reinterpret_cast<InputQueue*>(ptr);
    android::KeyEvent* event = queue->CreateKeyEvent();

    Int32 deviceId = 0;
    IInputEvent::Probe(e)->GetDeviceId(&deviceId);
    Int32 source = 0;
    IInputEvent::Probe(e)->GetSource(&source);
    Int32 metaState = 0;
    e->GetMetaState(&metaState);
    Int32 action = 0;
    e->GetAction(&action);
    Int32 keyCode = 0;
    e->GetKeyCode(&keyCode);
    Int32 scanCode = 0;
    e->GetScanCode(&scanCode);
    Int32 repeatCount = 0;
    e->GetRepeatCount(&repeatCount);
    Int32 flags = 0;
    e->GetFlags(&flags);
    Int64 downTime = 0;
    e->GetDownTime(&downTime);
    Int64 eventTime = 0;
    IInputEvent::Probe(e)->GetEventTime(&eventTime);

    event->initialize(deviceId, source, action, flags, keyCode, scanCode, metaState, repeatCount,
            milliseconds_to_nanoseconds(downTime),
            milliseconds_to_nanoseconds(eventTime));

    if (predispatch) {
        event->setFlags(event->getFlags() | AKEY_EVENT_FLAG_PREDISPATCH);
    }

    queue->EnqueueEvent(event);
    return reinterpret_cast<Int64>(event);
}

ECode CInputQueue::NativeSendMotionEvent(
    /* [in] */ Int64 ptr,
    /* [in] */ IMotionEvent* e,
    /* [out] */ Int64* id)
{
    android::sp<InputQueue> queue = reinterpret_cast<InputQueue*>(ptr);

    android::MotionEvent* originalEvent = NULL;
    if (e) {
        originalEvent = reinterpret_cast<android::MotionEvent*>(((MotionEvent*)e)->GetNativePtr());
    }

    if (!originalEvent) {
        // jniThrowRuntimeException(env, "Could not obtain MotionEvent pointer.");
        *id = -1;
        return E_RUNTIME_EXCEPTION;
    }
    android::MotionEvent* event = queue->CreateMotionEvent();
    event->copyFrom(originalEvent, true /* keepHistory */);
    queue->EnqueueEvent(event);
    *id = reinterpret_cast<Int64>(event);
    return NOERROR;
}

void CInputQueue::NativeDispose(
    /* [in] */ Int64 ptr)
{
    android::sp<InputQueue> queue = reinterpret_cast<InputQueue*>(ptr);
    queue->DetachLooper();
    queue->decStrong(&gInputQueueClassInfo);
}

enum {
    MSG_FINISH_INPUT = 1,
};

InputQueue::InputQueue(
    /* [in] */ IWeakReference* inputQueueObj,
    /* [in] */ const android::sp<android::Looper>& looper,
    /* [in] */ Int32 dispatchReadFd,
    /* [in] */ Int32 dispatchWriteFd)
    : mInputQueueWeakGlobal(inputQueueObj)
    , mDispatchReadFd(dispatchReadFd)
    , mDispatchWriteFd(dispatchWriteFd)
    , mDispatchLooper(looper)
    , mHandler(new android::WeakMessageHandler(this))
{
}

InputQueue::~InputQueue()
{
    mDispatchLooper->removeMessages(mHandler);
    mInputQueueWeakGlobal = NULL;
    close(mDispatchReadFd);
    close(mDispatchWriteFd);
}

void InputQueue::AttachLooper(
    /* [in] */ android::Looper* looper,
    /* [in] */ Int32 ident,
    /* [in] */ ALooper_callbackFunc callback,
    /* [in] */ void* data)
{
    Mutex::AutoLock _l(mLock);
    for (size_t i = 0; i < mAppLoopers.size(); i++) {
        if (looper == mAppLoopers[i]) {
            return;
        }
    }
    mAppLoopers.push(looper);
    looper->addFd(mDispatchReadFd, ident, ALOOPER_EVENT_INPUT, callback, data);
}

void InputQueue::DetachLooper()
{
    Mutex::AutoLock _l(mLock);
    DetachLooperLocked();
}

void InputQueue::DetachLooperLocked()
{
    for (size_t i = 0; i < mAppLoopers.size(); i++) {
        mAppLoopers[i]->removeFd(mDispatchReadFd);
    }
    mAppLoopers.clear();
}

Boolean InputQueue::HasEvents()
{
    Mutex::AutoLock _l(mLock);
    return mPendingEvents.size() > 0;
}

android::status_t InputQueue::GetEvent(
    /* [in] */ android::InputEvent** outEvent)
{
    Mutex::AutoLock _l(mLock);
    *outEvent = NULL;
    if (!mPendingEvents.isEmpty()) {
        *outEvent = mPendingEvents[0];
        mPendingEvents.removeAt(0);
    }

    if (mPendingEvents.isEmpty()) {
        char byteread[16];
        ssize_t nRead;
        do {
            nRead = TEMP_FAILURE_RETRY(read(mDispatchReadFd, &byteread, sizeof(byteread)));
            if (nRead < 0 && errno != EAGAIN) {
                ALOGW("Failed to read from native dispatch pipe: %s", strerror(errno));
            }
        } while (nRead > 0);
    }

    return *outEvent != NULL ? android::OK : android::WOULD_BLOCK;
}

Boolean InputQueue::PreDispatchEvent(
    /* [in] */ android::InputEvent* e)
{
    if (e->getType() == AINPUT_EVENT_TYPE_KEY) {
        android::KeyEvent* keyEvent = static_cast<android::KeyEvent*>(e);
        if (keyEvent->getFlags() & AKEY_EVENT_FLAG_PREDISPATCH) {
            FinishEvent(e, FALSE);
            return TRUE;
        }
    }
    return FALSE;
}

void InputQueue::FinishEvent(
    /* [in] */ android::InputEvent* event,
    /* [in] */ Boolean handled)
{
    Mutex::AutoLock _l(mLock);
    mFinishedEvents.push(android::key_value_pair_t<android::InputEvent*, Boolean>(event, handled));
    if (mFinishedEvents.size() == 1) {
        mDispatchLooper->sendMessage(this, android::Message(MSG_FINISH_INPUT));
    }
}

void InputQueue::handleMessage(
    /* [in] */ const android::Message& message)
{
    switch(message.what) {
    case MSG_FINISH_INPUT:
        AutoPtr<IInputQueue> inputQueueObj;
        mInputQueueWeakGlobal->Resolve(EIID_IInputQueue, (IInterface**)&inputQueueObj);
        if (!inputQueueObj) {
            ALOGW("InputQueue was finalized without being disposed");
            return;
        }
        while (TRUE) {
            android::InputEvent* event;
            Boolean handled = FALSE;
            {
                Mutex::AutoLock _l(mLock);
                if (mFinishedEvents.isEmpty()) {
                    break;
                }
                event = mFinishedEvents[0].getKey();
                handled = mFinishedEvents[0].getValue();
                mFinishedEvents.removeAt(0);
            }

            ((CInputQueue*)inputQueueObj.Get())->FinishInputEvent(reinterpret_cast<Int64>(event), handled);
            RecycleInputEvent(event);
        }
        break;
    }
}

void InputQueue::RecycleInputEvent(
    /* [in] */ android::InputEvent* event)
{
    mPooledInputEventFactory.recycle(event);
}

android::KeyEvent* InputQueue::CreateKeyEvent()
{
    return mPooledInputEventFactory.createKeyEvent();
}

android::MotionEvent* InputQueue::CreateMotionEvent()
{
    return mPooledInputEventFactory.createMotionEvent();
}

void InputQueue::EnqueueEvent(
    /* [in] */ android::InputEvent* event)
{
    Mutex::AutoLock _l(mLock);
    mPendingEvents.push(event);
    if (mPendingEvents.size() == 1) {
        char dummy = 0;
        int res = TEMP_FAILURE_RETRY(write(mDispatchWriteFd, &dummy, sizeof(dummy)));
        if (res < 0 && errno != EAGAIN) {
            ALOGW("Failed writing to dispatch fd: %s", strerror(errno));
        }
    }
}

InputQueue* InputQueue::CreateQueue(
    /* [in] */ IWeakReference* inputQueueObj,
    /* [in] */ const android::sp<android::Looper>& looper)
{
    int pipeFds[2];
    if (pipe(pipeFds)) {
        ALOGW("Could not create native input dispatching pipe: %s", strerror(errno));
        return NULL;
    }
    fcntl(pipeFds[0], F_SETFL, O_NONBLOCK);
    fcntl(pipeFds[1], F_SETFL, O_NONBLOCK);
    return new InputQueue(inputQueueObj, looper, pipeFds[0], pipeFds[1]);
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
