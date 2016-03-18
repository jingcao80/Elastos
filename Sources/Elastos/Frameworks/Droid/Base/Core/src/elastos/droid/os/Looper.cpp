
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/CMessageQueue.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Thread.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Core::IRunnable;
using Elastos::Core::Thread;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Os {

const String Looper::TAG("Looper");
AutoPtr<ILooper> Looper::sMainLooper;
Object Looper::sClassLock;

pthread_key_t Looper::sKey;
pthread_once_t Looper::sKeyOnce = PTHREAD_ONCE_INIT;

static void LooperDestructor(void* st)
{
    Looper* obj = static_cast<Looper*>(st);
    if (obj) {
        obj->Release();
    }
}

static void MakeKey()
{
    ASSERT_TRUE(pthread_key_create(&Looper::sKey, LooperDestructor) == 0);
}

//==========================================================================
// Looper
//==========================================================================
CAR_INTERFACE_IMPL(Looper, Object, ILooper)

Looper::Looper(
    /* [in] */ Boolean quitAllowed)
{
    CMessageQueue::New(quitAllowed, (IMessageQueue**)&mQueue);
    assert(mQueue != NULL);
    mThread = Thread::GetCurrentThread();
}

ECode Looper::Prepare()
{
    return Prepare(TRUE);
}

ECode Looper::Prepare(
    /* [in] */ Boolean quitAllowed)
{
    pthread_once(&sKeyOnce, MakeKey);
    if (pthread_getspecific(sKey) != NULL) {
        Slogger::E(TAG, "Only one Looper may be created per thread");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<Looper> l = new Looper(quitAllowed);
    ASSERT_TRUE(pthread_setspecific(sKey, l.Get()) == 0);
    l->AddRef();
    return NOERROR;
}

ECode Looper::PrepareMainLooper()
{
    FAIL_RETURN(Prepare(FALSE))

    {
        AutoLock lock(sClassLock);
        if (sMainLooper != NULL) {
            Slogger::E(TAG, "The main Looper has already been prepared.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        sMainLooper = GetMyLooper();
        assert(sMainLooper != NULL);
    }
    return NOERROR;
}

AutoPtr<ILooper> Looper::GetMainLooper()
{
    AutoLock lock(sClassLock);

    return sMainLooper;
}

ECode Looper::Loop()
{
    AutoPtr<Looper> me = (Looper*)GetMyLooper().Get();
    if (me == NULL) {
        Slogger::E(TAG, "No Looper; Looper::Prepare() wasn't called on this thread.");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IMessageQueue> queue = me->mQueue;

    // Make sure the identity of this thread is that of the local process,
    // and keep track of what that identity token actually is.
    Binder::ClearCallingIdentity();
    const Int64 ident = Binder::ClearCallingIdentity();

    for (;;) {
        AutoPtr<IMessage> msg;
        queue->GetNext((IMessage**)&msg); // might block
        if (msg == NULL) {
            // No message indicates that the message queue is quitting.
            return NOERROR;
        }

        // This must be in a local variable, in case a UI event sets the logger
        // Printer logging = me.mLogging;
        // if (logging != null) {
        //     logging.println(">>>>> Dispatching to " + msg.target + " " +
        //             msg.callback + ": " + msg.what);
        // }

        AutoPtr<IHandler> target;
        msg->GetTarget((IHandler**)&target);
        assert(target != NULL);
        if (target != NULL) {
#if 0
            Int32 what;
            msg->GetWhat(&what);
            AutoPtr<IRunnable> r;
            msg->GetCallback((IRunnable**)&r);
            Slogger::D("Looper", " >>>>> Dispatching to %p, msg: %d, callback: %p", target.Get(), what, r.Get());
#endif
            target->DispatchMessage(msg);
        }
        // if (logging != null) {
        //     logging.println("<<<<< Finished to " + msg.target + " " + msg.callback);
        // }

        // Make sure that during the course of dispatching the
        // identity of the thread wasn't corrupted.
        const Int64 newIdent = Binder::ClearCallingIdentity();
        if (ident != newIdent) {
            // Log.wtf(TAG, "Thread identity changed from 0x"
            //         + Long.toHexString(ident) + " to 0x"
            //         + Long.toHexString(newIdent) + " while dispatching to "
            //         + msg.target.getClass().getName() + " "
            //         + msg.callback + " what=" + msg.what);
        }

        msg->RecycleUnchecked();
    }
}

AutoPtr<ILooper> Looper::GetMyLooper()
{
    AutoPtr<ILooper> l = (Looper*)pthread_getspecific(sKey);
    return l;
}

ECode Looper::SetMessageLogging(
    /* [in] */ IPrinter* printer)
{
    mLogging = printer;
    return NOERROR;
}

AutoPtr<IMessageQueue> Looper::GetMyQueue()
{
    AutoPtr<ILooper> looper = GetMyLooper();
    return ((Looper*)looper.Get())->mQueue;
}

Boolean Looper::IsCurrentThread()
{
    return Thread::GetCurrentThread() == mThread;
}

ECode Looper::IsCurrentThread(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = Thread::GetCurrentThread() == mThread;
    return NOERROR;
}

ECode Looper::Quit()
{
    return mQueue->Quit(FALSE);
}

ECode Looper::QuitSafely()
{
    return mQueue->Quit(TRUE);
}

ECode Looper::PostSyncBarrier(
    /* [in] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mQueue->EnqueueSyncBarrier(SystemClock::GetUptimeMillis(), result);
}

ECode Looper::RemoveSyncBarrier(
    /* [in] */ Int32 token)
{
    return mQueue->RemoveSyncBarrier(token);
}

ECode Looper::GetThread(
    /* [out] */ IThread** thread)
{
    VALIDATE_NOT_NULL(thread);

    *thread = mThread;
    REFCOUNT_ADD(*thread);
    return NOERROR;
}

AutoPtr<IThread> Looper::GetThread()
{
    return mThread;
}

AutoPtr<IMessageQueue> Looper::GetQueue()
{
    return mQueue;
}

Boolean Looper::IsIdling()
{
    Boolean bval;
    mQueue->IsIdling(&bval);
    return bval;
}

ECode Looper::IsIdling(
    /* [out] */ Boolean* result)
{
    return mQueue->IsIdling(result);
}

ECode Looper::GetQueue(
    /* [out] */ IMessageQueue** queue)
{
    VALIDATE_NOT_NULL(queue);
    *queue = mQueue;
    REFCOUNT_ADD(*queue);
    return NOERROR;
}

ECode Looper::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String name;
    mThread->GetName(&name);
    Int64 id;
    mThread->GetId(&id);

    StringBuilder sb("Looper (");
    sb.Append(name);
    sb.Append(id);
    sb.Append(")");
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
