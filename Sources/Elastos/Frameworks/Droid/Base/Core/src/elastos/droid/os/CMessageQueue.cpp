
#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/CMessageQueue.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "elastos/droid/os/NativeMessageQueue.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Os {

const String CMessageQueue::TAG("CMessageQueue");
const Boolean CMessageQueue::DBG = TRUE;

CAR_INTERFACE_IMPL(CMessageQueue, Object, IMessageQueue)

CAR_OBJECT_IMPL(CMessageQueue)

ECode CMessageQueue::DebugMessage(
    /* [in] */ IMessage* msg,
    /* [in] */ const char* info)
{
    if (DBG) {
        Int32 what;
        msg->GetWhat(&what);
        AutoPtr<IRunnable> r;
        msg->GetCallback((IRunnable**)&r);
        AutoPtr<IHandler> t;
        msg->GetTarget((IHandler**)&t);
        Slogger::D(TAG, " >>>> Queue: %p, %s message: target: %p, what: %d, callback: %p",
            this, info, t.Get(), what, r.Get());
    }
    return NOERROR;
}

CMessageQueue::CMessageQueue()
    : mPtr(0)
    , mQuitAllowed(FALSE)
    , mQuitting(FALSE)
    , mBlocked(FALSE)
    , mNextBarrierToken(0)
{
    NativeInit();
}

CMessageQueue::~CMessageQueue()
{
    Dispose();
}

ECode CMessageQueue::constructor(
    /* [in] */ Boolean quitAllowed)
{
    mQuitAllowed = quitAllowed;
    return NOERROR;
}

ECode CMessageQueue::AddIdleHandler(
    /* [in] */ IIdleHandler* handler)
{
    if (handler == NULL) {
        Slogger::W(TAG, "Can't add a null IdleHandler!");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoLock lock(mLock);
    mIdleHandlers.PushBack(handler);
    return NOERROR;
}

ECode CMessageQueue::RemoveIdleHandler(
    /* [in] */ IIdleHandler* handler)
{
    AutoLock lock(mLock);
    mIdleHandlers.Remove(handler);
    return NOERROR;
}

ECode CMessageQueue::Dispose()
{
    if (mPtr != 0) {
        NativeDestroy();
        mPtr = 0;
    }
    return NOERROR;
}

ECode CMessageQueue::GetNext(
    /* [out] */ IMessage** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    // Return here if the message loop has already quit and been disposed.
    // This can happen if the application tries to restart a looper after quit
    // which is not supported.
    Int64 ptr = mPtr;
    if (ptr == 0) {
        return NOERROR;
    }

    using Elastos::Core::Math;
    Int32 pendingIdleHandlerCount = -1; // -1 only during first iteration
    Int32 nextPollTimeoutMillis = 0;

    for (;;) {
        if (nextPollTimeoutMillis != 0) {
            Binder::FlushPendingCommands();
        }
        NativePollOnce(nextPollTimeoutMillis);

        {
            AutoLock lock(mLock);

             // Try to retrieve the next message.  Return if found.
            Int64 now = SystemClock::GetUptimeMillis();
            AutoPtr<IMessage> prevMsg;
            AutoPtr<IMessage> msg = mMessages;
            AutoPtr<IHandler> target;
            if (msg != NULL && (msg->GetTarget((IHandler**)&target), target == NULL)) {
                target = NULL;
                Boolean isAsync;
                // Stalled by a barrier.  Find the next asynchronous message in the queue.
                do {
                    prevMsg = msg;
                    msg = NULL;
                    prevMsg->GetNext((IMessage**)&msg);
                }
                while (msg != NULL && (msg->IsAsynchronous(&isAsync), !isAsync));
            }

            Int64 when;
            if (msg != NULL) {
                msg->GetWhen(&when);
                if (now < when) {
                    // Next message is not ready.  Set a timeout to wake up when it is ready.
                    nextPollTimeoutMillis = (Int32)Min(when - now, (Int64)Math::INT32_MAX_VALUE);
                }
                else {
                    // Got a message.
                    mBlocked = FALSE;
                    AutoPtr<IMessage> next;
                    msg->GetNext((IMessage**)&next);
                    if (prevMsg != NULL) {
                        prevMsg->SetNext(next);
                    }
                    else {
                        mMessages = next;
                    }

                    // if (DBG) DebugMessage(msg, "Returning");

                    msg->SetNext(NULL);
                    *result = msg;
                    REFCOUNT_ADD(*result);
                    return NOERROR;
                }
            }
            else {
                // No more messages.
                nextPollTimeoutMillis = -1;
            }

            // Process the quit message now that all pending messages have been handled.
            if (mQuitting) {
                Dispose();
                return NOERROR;
            }

            // If first time idle, then get the number of idlers to run.
            // Idle handles only run if the queue is empty or if the first message
            // in the queue (possibly a barrier) is due to be handled in the future.
            if (pendingIdleHandlerCount < 0
                    && (mMessages == NULL || (mMessages->GetWhen(&when),  now < when))) {
                pendingIdleHandlerCount = mIdleHandlers.GetSize();
            }
            if (pendingIdleHandlerCount <= 0) {
                // No idle handlers to run.  Loop and wait some more.
                mBlocked = TRUE;
                continue;
            }

            if (mPendingIdleHandlers == NULL) {
                mPendingIdleHandlers = ArrayOf<IIdleHandler*>::Alloc(Math::Max(pendingIdleHandlerCount, 4));
            }
            if (mPendingIdleHandlers->GetLength() < mIdleHandlers.GetSize()) {
                mPendingIdleHandlers = ArrayOf<IIdleHandler*>::Alloc(mIdleHandlers.GetSize());
            }
            Int32 i = 0;
            List< AutoPtr<IIdleHandler> >::Iterator it;
            for (it = mIdleHandlers.Begin(); it != mIdleHandlers.End(); ++it, ++i) {
                mPendingIdleHandlers->Set(i, (*it).Get());
            }
        }

        // Run the idle handlers.
        // We only ever reach this code block during the first iteration.
        for (Int32 i = 0; i < pendingIdleHandlerCount; i++) {
            AutoPtr<IIdleHandler> idler = (*mPendingIdleHandlers)[i];
            mPendingIdleHandlers->Set(i, NULL); // release the reference to the handler

            Boolean keep = FALSE;
            // try {
            idler->QueueIdle(&keep);
            // } catch (Throwable t) {
            //     Log.wtf("MessageQueue", "IdleHandler threw exception", t);
            // }

            if (!keep) {
                AutoLock lock(mLock);
                mIdleHandlers.Remove(idler);
            }
        }

        // Reset the idle handler count to 0 so we do not run them again.
        pendingIdleHandlerCount = 0;

        // While calling an idle handler, a new message could have been delivered
        // so go back and look again for a pending message without waiting.
        nextPollTimeoutMillis = 0;
    }

    return NOERROR;
}

ECode CMessageQueue::Quit(
    /* [in] */ Boolean safe)
{
    if (!mQuitAllowed) {
        Slogger::W(TAG, "Main thread not allowed to quit.");
        assert(0);
        return E_RUNTIME_EXCEPTION;
    }

    {
        AutoLock lock(mLock);

        if (mQuitting) {
            return NOERROR;
        }
        mQuitting = TRUE;

        if (safe) {
            RemoveAllFutureMessagesLocked();
        }
        else {
            RemoveAllMessagesLocked();
        }

        // We can assume mPtr != 0 because mQuitting was previously false.
        NativeWake();
    }

    return NOERROR;
}

ECode CMessageQueue::EnqueueMessage(
    /* [in] */ IMessage* msg,
    /* [in] */ Int64 when,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(msg);

    AutoPtr<IHandler> target;
    msg->GetTarget((IHandler**)&target);
    if (target.Get() == NULL) {
        Slogger::W(TAG, "Message must have a target.");
        assert(0);
        return E_RUNTIME_EXCEPTION;
    }

    Boolean inUse;
    msg->IsInUse(&inUse);
    if (inUse) {
        Slogger::W(TAG, "This message is already in use.");
        assert(0);
        return E_RUNTIME_EXCEPTION;
    }

    Boolean isAsync;
    {
        AutoLock lock(mLock);

        if (mQuitting) {
            Slogger::W(TAG, "sending message to a Handler on a dead thread");
            msg->Recycle();
            return NOERROR;
        }

        msg->MarkInUse();
        Int64 tempWhen;
        msg->SetWhen(when);
        AutoPtr<IMessage> p = mMessages;
        Boolean needWake;
        if (p == NULL || when == 0 || (p->GetWhen(&tempWhen), when < tempWhen)) {
            // New head, wake up the event queue if blocked.
            msg->SetNext(p);
            mMessages = msg;
            needWake = mBlocked;
        }
        else {
            // Inserted within the middle of the queue.  Usually we don't have to wake
            // up the event queue unless there is a barrier at the head of the queue
            // and the message is the earliest asynchronous message in the queue.
            target = NULL;
            needWake = mBlocked
                && (p->GetTarget((IHandler**)&target), target.Get() == NULL)
                && (msg->IsAsynchronous(&isAsync), isAsync);

            AutoPtr<IMessage> prev;
            for (;;) {
                prev = p;
                p = NULL;
                prev->GetNext((IMessage**)&p);
                if (p == NULL || (p->GetWhen(&tempWhen), when < tempWhen)) {
                    break;
                }

                if (needWake && (p->IsAsynchronous(&isAsync), isAsync)) {
                    needWake = FALSE;
                }
            }
            msg->SetNext(p);    // invariant: p == prev.next
            prev->SetNext(msg);
        }

        // We can assume mPtr != 0 because mQuitting is false.
        if (needWake) {
            NativeWake();
        }
    }

    *result = TRUE;
    return NOERROR;
}

ECode CMessageQueue::HasMessages(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (h == NULL) {
        return NOERROR;
    }

    AutoLock lock(mLock);

    AutoPtr<IHandler> pH;
    AutoPtr<IInterface> pObj;
    Int32 pWhat;

    AutoPtr<IMessage> p = mMessages;
    while (p != NULL) {
        p->GetWhat(&pWhat);
        pH = NULL;
        p->GetTarget((IHandler**)&pH);
        if (pWhat == what && pH.Get() == h) {
            if (obj == NULL) {
                // DebugMessage(p, "Has");
                *result = TRUE;
                return NOERROR;
            }

            pObj = NULL;
            p->GetObj((IInterface**)&pObj);
            if (pObj.Get() == obj) {
                *result = TRUE;
                // DebugMessage(p, "Has");
                return NOERROR;
            }
        }

        AutoPtr<IMessage> next;
        p->GetNext((IMessage**)&next);
        p = next;
    }

    return NOERROR;
}

ECode CMessageQueue::HasMessages(
    /* [in] */ IHandler* h,
    /* [in] */ IRunnable* r,
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (h == NULL) {
        return NOERROR;
    }

    AutoLock lock(mLock);

    AutoPtr<IHandler> pH;
    AutoPtr<IInterface> pObj;
    AutoPtr<IRunnable> pR;

    AutoPtr<IMessage> p = mMessages;
    while (p != NULL) {
        pH = NULL;
        p->GetTarget((IHandler**)&pH);
        pR = NULL;
        p->GetCallback((IRunnable**)&pR);
        if (pH.Get() == h && pR.Get() == r) {
            if (obj == NULL) {
                *result = TRUE;
                // DebugMessage(p, "Has");
                return NOERROR;
            }

            pObj = NULL;
            p->GetObj((IInterface**)&pObj);
            if (pObj.Get() == obj) {
                *result = TRUE;
                // DebugMessage(p, "Has");
                return NOERROR;
            }
        }

        AutoPtr<IMessage> next;
        p->GetNext((IMessage**)&next);
        p = next;
    }

    return NOERROR;
}

ECode CMessageQueue::IsIdling(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    synchronized(this) {
        *result = IsIdlingLocked();
    }
    return NOERROR;
}

Boolean CMessageQueue::IsIdlingLocked()
{
    // If the loop is quitting then it must not be idling.
    // We can assume mPtr != 0 when mQuitting is false.
    return !mQuitting && NativeIsIdling();
}

ECode CMessageQueue::RemoveMessages(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    if (h == NULL) {
        return NOERROR;
    }

    AutoLock lock(mLock);

    AutoPtr<IHandler> pH;
    AutoPtr<IInterface> pObj;
    Int32 pWhat;

    AutoPtr<IMessage> p = mMessages;

    // Remove all messages at front.
    while (p != NULL) {
        p->GetWhat(&pWhat);
        pH = NULL;
        p->GetTarget((IHandler**)&pH);
        if (pWhat == what && pH.Get() == h) {
            if (obj != NULL) {
                pObj = NULL;
                p->GetObj((IInterface**)&pObj);
                if (pObj.Get() != obj) {
                    break;
                }
            }

            // DebugMessage(p, "Remove");
            AutoPtr<IMessage> next;
            p->GetNext((IMessage**)&next);
            mMessages = next;
            p->RecycleUnchecked();
            p = next;
            continue;
        }

        break;
    }

    // Remove all messages after front.
    while (p != NULL) {
        AutoPtr<IMessage> next;
        p->GetNext((IMessage**)&next);
        if (next != NULL) {
            next->GetWhat(&pWhat);
            pH = NULL;
            next->GetTarget((IHandler**)&pH);
            if (pWhat == what && pH.Get() == h) {
                Boolean isRecycle = FALSE;
                if (obj == NULL) {
                    isRecycle = TRUE;
                }
                else {
                    pObj = NULL;
                    next->GetObj((IInterface**)&pObj);
                    if (pObj.Get() == obj) {
                        isRecycle = TRUE;
                    }
                }

                if (isRecycle) {
                    AutoPtr<IMessage> nn;
                    next->GetNext((IMessage**)&nn);
                    // DebugMessage(next, "Remove");
                    next->RecycleUnchecked();
                    p->SetNext(nn);
                    continue;
                }
            }
        }
        p = next;
    }

    return NOERROR;
}

ECode CMessageQueue::RemoveMessages(
    /* [in] */ IHandler* h,
    /* [in] */ IRunnable* r,
    /* [in] */ IInterface* obj)
{
    if (h == NULL || r == NULL) {
        return NOERROR;
    }

    AutoLock lock(mLock);

    AutoPtr<IHandler> pH;
    AutoPtr<IRunnable> pR;
    AutoPtr<IInterface> pObj;

    AutoPtr<IMessage> p = mMessages;

    // Remove all messages at front.
    while (p != NULL) {
        pH = NULL; pR = NULL;
        p->GetTarget((IHandler**)&pH);
        p->GetCallback((IRunnable**)&pR);
        if (pH.Get() == h && pR.Get() == r) {
            if (obj != NULL) {
                pObj = NULL;
                p->GetObj((IInterface**)&pObj);
                if (pObj.Get() != obj) {
                    break;
                }
            }

            // DebugMessage(p, "Remove");
            AutoPtr<IMessage> n;
            p->GetNext((IMessage**)&n);
            mMessages = n;
            p->RecycleUnchecked();
            p = n;
            continue;
        }

        break;
    }

    // Remove all messages after front.
    while (p != NULL) {
        AutoPtr<IMessage> n;
        p->GetNext((IMessage**)&n);
        if (n != NULL) {
            pH = NULL; pR = NULL;
            n->GetTarget((IHandler**)&pH);
            n->GetCallback((IRunnable**)&pR);
            if (pH.Get() == h && pR.Get() == r) {
                Boolean isRecycle = FALSE;
                if (obj == NULL) {
                    isRecycle = TRUE;
                }
                else {
                    pObj = NULL;
                    n->GetObj((IInterface**)&pObj);
                    if (pObj.Get() == obj) {
                        isRecycle = TRUE;
                    }
                }

                if (isRecycle) {
                    AutoPtr<IMessage> nn;
                    n->GetNext((IMessage**)&nn);
                    // DebugMessage(n, "Remove");
                    n->RecycleUnchecked();
                    p->SetNext(nn);
                    continue;
                }
            }
        }
        p = n;
    }

    return NOERROR;
}

ECode CMessageQueue::RemoveCallbacksAndMessages(
    /* [in] */ IHandler* h,
    /* [in] */ IInterface* obj)
{
    if (h == NULL) {
        return NOERROR;
    }

    AutoLock lock(mLock);

    AutoPtr<IHandler> pH;
    AutoPtr<IInterface> pObj;

    AutoPtr<IMessage> p = mMessages;

    // Remove all messages at front.
    while (p != NULL) {
        pH = NULL;
        p->GetTarget((IHandler**)&pH);
        if (pH.Get() == h) {
            if (obj != NULL) {
                pObj = NULL;
                p->GetObj((IInterface**)&pObj);
                if (pObj.Get() != obj) {
                    break;
                }
            }

            // DebugMessage(p, "RemoveCallbacksAndMessages");
            AutoPtr<IMessage> n;
            p->GetNext((IMessage**)&n);
            mMessages = n;
            p->RecycleUnchecked();
            p = n;
            continue;
        }

        break;
    }

    // Remove all messages after front.
    while (p != NULL) {
        AutoPtr<IMessage> n;
        p->GetNext((IMessage**)&n);
        if (n != NULL) {
            pH = NULL;
            n->GetTarget((IHandler**)&pH);
            if (pH.Get() == h) {
                Boolean isRecycle = FALSE;
                if (obj == NULL) {
                    isRecycle = TRUE;
                }
                else {
                    pObj = NULL;
                    n->GetObj((IInterface**)&pObj);
                    if (pObj.Get() == obj) {
                        isRecycle = TRUE;
                    }
                }

                if (isRecycle) {
                    AutoPtr<IMessage> nn;
                    n->GetNext((IMessage**)&nn);
                    // DebugMessage(n, "RemoveCallbacksAndMessages");
                    n->RecycleUnchecked();
                    p->SetNext(nn);
                    continue;
                }
            }
        }
        p = n;
    }

    return NOERROR;
}

ECode CMessageQueue::GetNativeMessageQueue(
    /* [out] */ Handle64* queue)
{
    VALIDATE_NOT_NULL(queue);
    *queue = mPtr;
    return NOERROR;
}

ECode CMessageQueue::EnqueueSyncBarrier(
    /* [in] */ Int64 when,
    /* [in] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    // Enqueue a new sync barrier token.
    // We don't need to wake the queue because the purpose of a barrier is to stall it.
    AutoLock lock(mLock);

    const Int32 token = mNextBarrierToken++;
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain((IMessage**)&msg);
    msg->MarkInUse();
    msg->SetWhen(when);
    msg->SetArg1(token);

    AutoPtr<IMessage> prev;
    AutoPtr<IMessage> p = mMessages;
    if (when != 0) {
        Int64 tempWhen;
        while (p != NULL && (p->GetWhen(&tempWhen), tempWhen <= when)) {
            prev = p;
            p = NULL;
            prev->GetNext((IMessage**)&p);
        }
    }
    if (prev != NULL) { // invariant: p == prev.next
        msg->SetNext(p);
        prev->SetNext(msg);
    }
    else {
        msg->SetNext(p);
        mMessages = msg;
    }

    *result = token;
    return NOERROR;
}

ECode CMessageQueue::RemoveSyncBarrier(
    /* [in] */ Int32 token)
{
    // Remove a sync barrier token from the queue.
    // If the queue is no longer stalled by a barrier then wake it.
    {
        AutoLock lock(mLock);

        Int32 arg1;
        AutoPtr<IMessage> prev;
        AutoPtr<IMessage> p = mMessages;
        AutoPtr<IHandler> target;
        while (p != NULL
            && (p->GetTarget((IHandler**)&target), target != NULL)
            && (p->GetArg1(&arg1), arg1 != token)
        ) {
            target = NULL;
            prev = p;
            prev->GetNext((IMessage**)&p);
        }

        if (p == NULL) {
            Slogger::W(TAG, "The specified message queue synchronization " \
                " barrier token has not been posted or has already been removed.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        Boolean needWake;
        if (prev != NULL) {
            AutoPtr<IMessage> n;
            p->GetNext((IMessage**)&n);
            prev->SetNext(n);;
            needWake = FALSE;
        }
        else {
            mMessages = NULL;
            p->GetNext((IMessage**)&mMessages);

            target = NULL;
            needWake = (mMessages == NULL
                || (mMessages->GetTarget((IHandler**)&target), target.Get() != NULL));
        }
        p->RecycleUnchecked();

        // If the loop is quitting then it is already awake.
        // We can assume mPtr != 0 when mQuitting is false.
        if (needWake && !mQuitting) {
            NativeWake();
        }
    }

    return NOERROR;
}

ECode CMessageQueue::RemoveAllMessagesLocked()
{
    AutoPtr<IMessage> p = mMessages;
    while (p != NULL) {
        AutoPtr<IMessage> n;
        p->GetNext((IMessage**)&n);
        p->RecycleUnchecked();
        p = n;
    }
    mMessages = NULL;
    return NOERROR;
}

ECode CMessageQueue::RemoveAllFutureMessagesLocked()
{
    Int64 now = SystemClock::GetUptimeMillis();
    AutoPtr<IMessage> p = mMessages;
    if (p != NULL) {
        Int64 when;
        p->GetWhen(&when);
        if (when > now) {
            RemoveAllMessagesLocked();
        }
        else {
            AutoPtr<IMessage> n;
            for (;;) {
                n = NULL;
                p->GetNext((IMessage**)&n);
                if (n == NULL) {
                    return NOERROR;
                }
                n->GetWhen(&when);
                if (when > now) {
                    break;
                }
                p = n;
            }
            p->SetNext(NULL);
            do {
                p = n;
                n = NULL;
                p->GetNext((IMessage**)&n);
                p->RecycleUnchecked();
            }
            while (n != NULL);
        }
    }
    return NOERROR;
}

ECode CMessageQueue::NativeInit()
{
    NativeMessageQueue* nativeMessageQueue = new NativeMessageQueue();
    if (!nativeMessageQueue) {
        Slogger::E(TAG, "Unable to allocate native queue");
        return E_RUNTIME_EXCEPTION;
    }

    nativeMessageQueue->AddRef();
    mPtr = reinterpret_cast<Handle64>(nativeMessageQueue);
    return NOERROR;
}

void CMessageQueue::NativeDestroy()
{
    NativeMessageQueue* nativeMessageQueue = (NativeMessageQueue*)mPtr;
    if (nativeMessageQueue) {
        mPtr = 0;
        nativeMessageQueue->Release();
    }
}

void CMessageQueue::NativePollOnce(
    /* [in] */ Int32 timeoutMillis)
{
    NativeMessageQueue* nativeMessageQueue = (NativeMessageQueue*)mPtr;
    nativeMessageQueue->PollOnce(timeoutMillis);
}

void CMessageQueue::NativeWake()
{
    NativeMessageQueue* nativeMessageQueue = (NativeMessageQueue*)mPtr;
    return nativeMessageQueue->Wake();
}

Boolean CMessageQueue::NativeIsIdling()
{
    NativeMessageQueue* nativeMessageQueue = (NativeMessageQueue*)mPtr;
    return (Boolean)nativeMessageQueue->GetLooper()->isIdling();
}

} // namespace Os
} // namespace Droid
} // Elastos
