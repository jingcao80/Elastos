
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/CMessengerImpl.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "elastos/droid/os/CLooperHelper.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::EIID_ISynchronize;

namespace Elastos {
namespace Droid {
namespace Os {

//===========================================================================
// Handler::BlockingRunnable
//===========================================================================
CAR_INTERFACE_IMPL(Handler::BlockingRunnable, Object, IRunnable)

Handler::BlockingRunnable::BlockingRunnable(
    /* [in] */ IRunnable* task)
    : mTask(task)
    , mDone(FALSE)
{}

ECode Handler::BlockingRunnable::Run()
{
    // try {
    mTask->Run();
    // } finally {
    {
        Lock();

        mDone = TRUE;
        NotifyAll();

        Unlock();
    }
    // }
    return NOERROR;
}

Boolean Handler::BlockingRunnable::PostAndWait(
    /* [in] */ Handler* handler,
    /* [in] */ Int64 timeout)
{
    Boolean result;
    if (handler->Post(this, &result), !result) {
        return FALSE;
    }

    {
        Lock();

        if (timeout > 0) {
            const Int64 expirationTime = SystemClock::GetUptimeMillis() + timeout;
            while (!mDone) {
                Int64 delay = expirationTime - SystemClock::GetUptimeMillis();
                if (delay <= 0) {
                    Unlock();
                    return FALSE; // timeout
                }
                // try {
                Wait(delay);
                // } catch (InterruptedException ex) {
                // }
            }
        }
        else {
            while (!mDone) {
                // try {
                Wait();
                // } catch (InterruptedException ex) {
                // }
            }
        }

        Unlock();
    }
    return TRUE;
}

//===========================================================================
// Handler
//===========================================================================
CAR_INTERFACE_IMPL(Handler, Object, IHandler)

Handler::Handler()
    : mTakeStrongRefOfCallback(TRUE)
    , mAsynchronous(FALSE)
{
    // sub class must call constructor() implicitly.
}

Handler::Handler(
    /* [in] */ Boolean async)
    : mTakeStrongRefOfCallback(TRUE)
    , mAsynchronous(async)
{
    constructor(async);
}

Handler::Handler(
    /* [in] */ IHandlerCallback* callback,
    /* [in] */ Boolean takeStrongRefOfCallback,
    /* [in] */ Boolean async)
    : mTakeStrongRefOfCallback(takeStrongRefOfCallback)
    , mAsynchronous(async)
{
    constructor(callback, takeStrongRefOfCallback, async);
}

Handler::Handler(
    /* [in] */ ILooper* looper,
    /* [in] */ Boolean async)
    : mTakeStrongRefOfCallback(TRUE)
    , mAsynchronous(async)
{
    constructor(looper, async);
}

Handler::Handler(
    /* [in] */ ILooper* looper,
    /* [in] */ IHandlerCallback* callback,
    /* [in] */ Boolean takeStrongRefOfCallback,
    /* [in] */ Boolean async)
    : mTakeStrongRefOfCallback(takeStrongRefOfCallback)
    , mAsynchronous(async)
{
    constructor(looper, callback, takeStrongRefOfCallback, async);
}


ECode Handler::constructor(
    /* [in] */ Boolean async)
{
    return constructor(NULL, TRUE, async);
}

ECode Handler::constructor(
    /* [in] */ ILooper* looper,
    /* [in] */ Boolean async)
{
    assert(looper != NULL);
    mLooper = looper;
    mLooper->GetQueue((IMessageQueue**)&mQueue);
    mTakeStrongRefOfCallback = TRUE;
    mAsynchronous = async;
    return NOERROR;
}

ECode Handler::constructor(
    /* [in] */ IHandlerCallback* callback,
    /* [in] */ Boolean takeStrongRefOfCallback,
    /* [in] */ Boolean async)
{
    AutoPtr<ILooperHelper> helper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&helper);
    helper->GetMyLooper((ILooper**)&mLooper);
    if (mLooper == NULL) {
        Logger::E("Handler", "Can't create handler inside thread that has not called Looper.prepare()");
        assert(0);
        return E_RUNTIME_EXCEPTION;
    }

    mLooper->GetQueue((IMessageQueue**)&mQueue);
    mTakeStrongRefOfCallback = takeStrongRefOfCallback;
    mAsynchronous = async;

    if (callback) {
        if (mTakeStrongRefOfCallback) {
            mCallback = callback;
        }
        else {
            AutoPtr<IWeakReferenceSource> wrs = (IWeakReferenceSource*)callback->Probe(EIID_IWeakReferenceSource);
            if (wrs) {
                wrs->GetWeakReference((IWeakReference**)&mWeakCallback);
            }
            else {
                assert(0 && "IWeakReferenceSource was not impelemented!");
            }
        }
    }
    return NOERROR;
}

ECode Handler::constructor(
    /* [in] */ ILooper* looper,
    /* [in] */ IHandlerCallback* callback,
    /* [in] */ Boolean takeStrongRefOfCallback,
    /* [in] */ Boolean async)
{
    assert(looper != NULL);
    mLooper = looper;
    mLooper->GetQueue((IMessageQueue**)&mQueue);
    mTakeStrongRefOfCallback = takeStrongRefOfCallback;
    mAsynchronous = async;

    if (callback) {
        if (mTakeStrongRefOfCallback) {
            mCallback = callback;
        }
        else {
            AutoPtr<IWeakReferenceSource> wrs = (IWeakReferenceSource*)callback->Probe(EIID_IWeakReferenceSource);
            if (wrs) {
                wrs->GetWeakReference((IWeakReference**)&mWeakCallback);
            }
            else {
                assert(0 && "IWeakReferenceSource was not impelemented!");
            }
        }
    }
    return NOERROR;
}

ECode Handler::RunWithScissors(
    /* [in] */ IRunnable* r,
    /* [in] */ Int64 timeout,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (r == NULL) {
        Logger::E("Handler", "runnable must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (timeout < 0) {
        Logger::E("Handler", "timeout must be non-negative.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ILooperHelper> helper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&helper);
    AutoPtr<ILooper> myLooper;
    helper->GetMyLooper((ILooper**)&myLooper);
    if (myLooper == mLooper) {
        ECode ec = r->Run();
        *result = TRUE;
        return ec;
    }

    AutoPtr<BlockingRunnable> br = new BlockingRunnable(r);
    *result = br->PostAndWait(this, timeout);
    return NOERROR;
}

ECode Handler::ObtainMessage(
    /* [out] */ IMessage** msg)
{
    return ObtainMessage(0);
}

ECode Handler::ObtainMessage(
    /* [in] */ Int32 what,
    /* [out] */ IMessage** msg)
{
    return ObtainMessage(what, 0, 0, NULL, msg);
}

ECode Handler::ObtainMessage(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj,
    /* [out] */ IMessage** msg)
{
    return ObtainMessage(what, 0, 0, obj, msg);
}

ECode Handler::ObtainMessage(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [out] */ IMessage** msg)
{
    return ObtainMessage(what, arg1, arg2, NULL, msg);
}

ECode Handler::ObtainMessage(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    return helper->Obtain(THIS_PROBE(IHandler), what, arg1, arg2, obj, msg);
}

AutoPtr<IMessage> Handler::GetPostMessage(
    /* [in] */ IRunnable* r)
{
    return GetPostMessage(r, NULL);
}

AutoPtr<IMessage> Handler::GetPostMessage(
    /* [in] */ IRunnable* r,
    /* [in] */ IInterface* token)
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain(THIS_PROBE(IHandler), (IMessage**)&msg);
    msg->SetWhat((Int32)r);  // as Id
    msg->SetObj(token);
    msg->SetCallback(r);
    return msg;
}

ECode Handler::Post(
    /* [in] */ IRunnable* action,
    /* [out] */ Boolean* result)
{
   return PostDelayed(action, 0, result);
}

ECode Handler::PostAtTime(
    /* [in] */ IRunnable* action,
    /* [in] */ Int64 uptimeMillis,
    /* [out] */ Boolean* result)
{
   return PostAtTime(action, NULL, uptimeMillis, result);
}

ECode Handler::PostAtTime(
    /* [in] */ IRunnable* action,
    /* [in] */ IInterface* token,
    /* [in] */ Int64 uptimeMillis,
    /* [out] */ Boolean* result)
{
   VALIDATE_NOT_NULL(result);
   AutoPtr<IMessage> msg = GetPostMessage(action, token);
   return SendMessageAtTime(msg, uptimeMillis, result);
}

ECode Handler::PostDelayed(
    /* [in] */ IRunnable* action,
    /* [in] */ Int64 delayMillis,
    /* [out] */ Boolean* result)
{
   VALIDATE_NOT_NULL(result);
   AutoPtr<IMessage> msg = GetPostMessage(action);
   return SendMessageDelayed(msg, delayMillis, result);
}

ECode Handler::PostAtFrontOfQueue(
    /* [in] */ IRunnable* action,
    /* [out] */ Boolean* result)
{
   VALIDATE_NOT_NULL(result);
   AutoPtr<IMessage> msg = GetPostMessage(action);
   return SendMessageAtFrontOfQueue(msg, result);
}

ECode Handler::RemoveCallbacks(
    /* [in] */ IRunnable* action)
{
    return RemoveMessages((Int32)action);
}

ECode Handler::RemoveCallbacks(
    /* [in] */ IRunnable* action,
    /* [in] */ IInterface* token)
{
    return RemoveMessages((Int32)action, token);
}

ECode Handler::GetIMessenger(
    /* [out] */ IIMessenger** mgr)
{
    VALIDATE_NOT_NULL(mgr);

    AutoLock lock(mQueueLock);

    if (mMessenger == NULL) {
        CMessengerImpl::New(THIS_PROBE(IHandler), (IIMessenger**)&mMessenger);
    }
    *mgr = mMessenger;
    REFCOUNT_ADD(*mgr);
    return NOERROR;
}

ECode Handler::SendEmptyMessage(
    /* [in] */ Int32 what,
    /* [out] */ Boolean* result)
{
    return Handler::SendEmptyMessageDelayed(what, 0, result);
}

ECode Handler::SendEmptyMessageDelayed(
    /* [in] */ Int32 what,
    /* [in] */ Int64 delayMillis,
    /* [out] */ Boolean* result)
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain(THIS_PROBE(IHandler), (IMessage**)&msg);
    msg->SetWhat(what);
    return Handler::SendMessageDelayed(msg, delayMillis, result);
}

ECode Handler::SendEmptyMessageAtTime(
    /* [in] */ Int32 what,
    /* [in] */ Int64 uptimeMillis,
    /* [out] */ Boolean* result)
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain(THIS_PROBE(IHandler), (IMessage**)&msg);
    msg->SetWhat(what);
    return Handler::SendMessageAtTime(msg, uptimeMillis, result);
}

ECode Handler::SendMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    return Handler::SendMessageDelayed(msg, 0, result);
}

ECode Handler::SendMessageDelayed(
    /* [in] */ IMessage* msg,
    /* [in] */ Int64 delayMillis,
    /* [out] */ Boolean* result)
{
    if (delayMillis < 0) {
        delayMillis = 0;
    }

    return SendMessageAtTime(msg, SystemClock::GetUptimeMillis() + delayMillis, result);
}

ECode Handler::SendMessageAtTime(
    /* [in] */ IMessage* msg,
    /* [in] */ Int64 uptimeMillis,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(msg);

    AutoPtr<IMessageQueue> queue = mQueue;
    if (queue == NULL) {
        Logger::E("Handler", "Error: SendMessageAtTime called with no mQueue");
        return E_RUNTIME_EXCEPTION;
    }

    *result = EnqueueMessage(queue, msg, uptimeMillis);
    return NOERROR;
}

ECode Handler::SendMessageAtFrontOfQueue(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(msg);

    AutoPtr<IMessageQueue> queue = mQueue;
    if (queue == NULL) {
        Logger::E("Handler", "Error: SendMessageAtFrontOfQueue called with no mQueue");
        return E_RUNTIME_EXCEPTION;
    }

    *result = EnqueueMessage(queue, msg, 0);
    return NOERROR;
}

Boolean Handler::EnqueueMessage(
    /* [in] */ IMessageQueue* queue,
    /* [in] */ IMessage* msg,
    /* [in] */ Int64 uptimeMillis)
{
    msg->SetTarget(THIS_PROBE(IHandler));
    if (mAsynchronous) {
        msg->SetAsynchronous(true);
    }

    Boolean result;
    mQueue->EnqueueMessage(msg, uptimeMillis, &result);
    return result;
}

ECode Handler::HasCallbacks(
    /* [in] */ IRunnable* action,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoPtr<IMessageQueue> queue = mQueue;
    if (queue == NULL) {
        Logger::E("Handler", "Error: HasCallbacks called with no mQueue");
        return E_RUNTIME_EXCEPTION;
    }

    return queue->HasMessages(THIS_PROBE(IHandler), action, NULL, result);
}

ECode Handler::HasMessages(
    /* [in] */ Int32 what,
    /* [out] */ Boolean* result)
{
    return HasMessages(what, what, NULL, result);
}

ECode Handler::HasMessages(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return HasMessages(what, what, obj, result);
}

ECode Handler::HasMessages(
    /* [in] */ Int32 id,
    /* [in] */ Int32 what,
    /* [out] */ Boolean* result)
{
    return HasMessages(id, what, NULL, result);
}

ECode Handler::HasMessages(
    /* [in] */ Int32 id,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoPtr<IMessageQueue> queue = mQueue;
    if (queue == NULL) {
        Logger::E("Handler", "Error: HasMessagesEx3 called with no mQueue");
        return E_RUNTIME_EXCEPTION;
    }

    return queue->HasMessages(THIS_PROBE(IHandler), what, obj, result);
}

ECode Handler::RemoveMessages(
    /* [in] */ Int32 what)
{
    return RemoveMessages(what, what, NULL);
}

ECode Handler::RemoveMessages(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return RemoveMessages(what, what, obj);
}

ECode Handler::RemoveMessages(
    /* [in] */ Int32 id,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMessageQueue> queue = mQueue;
    if (queue == NULL) {
        Logger::E("Handler", "Error: RemoveMessagesEx2 called with no mQueue");
        return E_RUNTIME_EXCEPTION;
    }

    return queue->RemoveMessages(THIS_PROBE(IHandler), what, obj);
}

/**
 * Remove any pending posts of callbacks and sent messages whose
 * <var>obj</var> is <var>token</var>.  If <var>token</var> is null,
 * all callbacks and messages will be removed.
 */
ECode Handler::RemoveCallbacksAndMessages(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMessageQueue> queue = mQueue;
    if (queue == NULL) {
        Logger::E("Handler", "Error: RemoveCallbacksAndMessages called with no mQueue");
        return E_RUNTIME_EXCEPTION;
    }

    return queue->RemoveCallbacksAndMessages(THIS_PROBE(IHandler), obj);
}

ECode Handler::GetMessageName(
    /* [in] */ IMessage* msg,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = GetMessageNameImpl(msg);
    return NOERROR;
}

String Handler::GetMessageNameImpl(
    /* [in] */ IMessage* msg)
{
    if (msg == NULL) {
        return String("Null message");
    }

    AutoPtr<IRunnable> callback;
    msg->GetCallback((IRunnable**)&callback);
    if (callback != NULL) {
        // return message.callback.getClass().getName();
        String name("Runable:0x");
        name += StringUtils::ToHexString((Int32)callback.Get());
        return name;
    }

    Int32 what = 0;
    msg->GetWhat(&what);
    String name("Message:0x");
    name += StringUtils::ToHexString(what);
    return name;
}

/**
 * Subclasses must implement this to receive messages.
 */
ECode Handler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    return NOERROR;
}

/**
 * Handle system messages here.
 */
ECode Handler::DispatchMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IRunnable> callback;
    msg->GetCallback((IRunnable**)&callback);
    if (callback != NULL) {
        return callback->Run();
    }
    else {
        AutoPtr<IHandlerCallback> handlerCallBack = mCallback;
        if (handlerCallBack == NULL && mWeakCallback != NULL) {
            AutoPtr<IInterface> obj;
            mWeakCallback->Resolve(EIID_IInterface, (IInterface**)&obj);
            handlerCallBack = IHandlerCallback::Probe(obj);
            assert(handlerCallBack != NULL);
        }

        if (handlerCallBack != NULL) {
            Boolean result = FALSE;
            ECode ec = handlerCallBack->HandleMessage(msg, &result);
            if (result) {
                return ec;
            }
        }

        return HandleMessage(msg);
    }
}

ECode Handler::GetLooper(
    /* [out] */ ILooper** looper)
{
    VALIDATE_NOT_NULL(looper)
    *looper = mLooper;
    REFCOUNT_ADD(*looper);
    return NOERROR;
}

ECode Handler::GetMessageQueue(
    /* [out] */ IMessageQueue** cq)
{
    VALIDATE_NOT_NULL(cq)
    *cq = mQueue;
    REFCOUNT_ADD(*cq)
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
