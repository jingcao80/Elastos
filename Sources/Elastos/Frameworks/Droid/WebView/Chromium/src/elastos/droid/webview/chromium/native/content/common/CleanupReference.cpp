
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/webkit/webview/chromium/native/content/common/ContentSwitches.h"
#include "elastos/droid/webkit/webview/chromium/native/content/common/CleanupReference.h"
#include "elastos/droid/webkit/webview/chromium/native/base/TraceEvent.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Webkit::Webview::Chromium::Base::TraceEvent;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IHashSet;
using Elastos::Utility::EIID_IHashSet;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Common {

//===============================================================
//           CleanupReference::LazyHolder::InnerHandler
//===============================================================

//CAR_INTERFACE_IMPL(CleanupReference::LazyHolder::InnerHandler, Object, IHandler);
CleanupReference::LazyHolder::InnerHandler::InnerHandler(
    /* [in] */ ILooper* looper)
    : Handler(looper)
{
}

ECode CleanupReference::LazyHolder::InnerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);

    TraceEvent::Begin();
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    CleanupReference* ref = (CleanupReference*)(IObject::Probe(obj));
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case ADD_REF:
            sRefs->Add((IObject*)ref);
            break;
        case REMOVE_REF:
            ref->RunCleanupTaskInternal();
            break;
        default: {
                String log("Bad message=");
                log += StringUtils::ToString(what);
                Slogger::E(TAG, log);
            }
            break;
    }

    if (DEBUG) {
        Int32 size;
        sRefs->GetSize(&size);
        String log("will try and cleanup; max = ");
        log += StringUtils::ToString(size);
        Slogger::D(TAG, log);
    }

    synchronized(sCleanupMonitor) {
        // Always run the cleanup loop here even when adding or removing refs, to avoid
        // falling behind on rapid garbage allocation inner loops.
        assert(0);
        // TODO
        // while ((ref = (CleanupReference) sGcQueue.poll()) != NULL) {
        //     ref->runCleanupTaskInternal();
        // }
        sCleanupMonitor.NotifyAll();
    }

    TraceEvent::End();

    return NOERROR;
}

//===============================================================
//                 CleanupReference::LazyHolder
//===============================================================

AutoPtr<IHandler> CleanupReference_LazyHolder_sHandler_Init()
{
    AutoPtr<IHandler> handler = new CleanupReference::LazyHolder::InnerHandler(ThreadUtils::GetUiThreadLooper());
    return handler;
}

// TODO
AutoPtr<IHandler> CleanupReference::LazyHolder::sHandler;// = CleanupReference_LazyHolder_sHandler_Init();

//===============================================================
//                 CleanupReference::InnerThread
//===============================================================

CleanupReference::InnerThread::InnerThread(
    /* [in] */ const String& threadName)
{
    Thread::constructor(threadName);
}

ECode CleanupReference::InnerThread::Run()
{
    while (TRUE) {
        // try {
            AutoPtr<CleanupReference> ref;
            assert(0);
            // TODO
            // ref = (CleanupReference) sGcQueue->Remove();
            if (DEBUG) {
                Slogger::D(TAG, "removed one ref from GC queue");
            }

            synchronized(sCleanupMonitor) {
                AutoPtr<IMessageHelper> helper;
                CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
                AutoPtr<IMessage> msg;
                helper->Obtain(LazyHolder::sHandler, REMOVE_REF, (IObject*)ref, (IMessage**)&msg);
                msg->SendToTarget();
                // Give the UI thread chance to run cleanup before looping around and
                // taking the next item from the queue, to avoid Message bombing it.
                sCleanupMonitor.Wait(500);
            }
        // } catch (Exception e) {
        //     Log.e(TAG, "Queue remove exception:", e);
        // }
    }

    return NOERROR;
}

//===============================================================
//                      CleanupReference
//===============================================================

Boolean CleanupReferenceStart()
{
    CleanupReference::sReaperThread->SetDaemon(TRUE);
    CleanupReference::sReaperThread->Start();
    return TRUE;
}

static Boolean sStart = CleanupReferenceStart();

const String CleanupReference::TAG("CleanupReference");
const Boolean CleanupReference::DEBUG = FALSE;  // Always check in as false!

// TODO ReferenceQueue<Object> CleanupReference::sGcQueue;
Object CleanupReference::sCleanupMonitor;

AutoPtr<IThread> CleanupReference_sReaperThread_Init()
{
    AutoPtr<IThread> thread = new CleanupReference::InnerThread(CleanupReference::TAG);
    return thread;
}

AutoPtr<IThread> CleanupReference::sReaperThread = CleanupReference_sReaperThread_Init();

// Add a new reference to sRefs. |msg.obj| is the CleanupReference to add.
const Int32 CleanupReference::ADD_REF;
// Remove reference from sRefs. |msg.obj| is the CleanupReference to remove.
const Int32 CleanupReference::REMOVE_REF;

/**
 * Keep a strong reference to {@link CleanupReference} so that it will
 * actually get enqueued.
 * Only accessed on the UI thread.
 */
AutoPtr<ISet> sRefsInit()
{
    AutoPtr<IHashSet> hashSet;
    CHashSet::New((IHashSet**)&hashSet);
    AutoPtr<ISet> set = ISet::Probe(hashSet);
    return set;
}
AutoPtr<ISet> CleanupReference::sRefs = sRefsInit();

/**
 * @param obj the object whose loss of reachability should trigger the
 *            cleanup task.
 * @param cleanupTask the task to run once obj loses reachability.
 */
CleanupReference::CleanupReference(
    /* [in] */ Object* obj,
    /* [in] */ IRunnable* cleanupTask)
{
    assert(0);
    // TODO
    // super(obj, sGcQueue);

    if (DEBUG) {
        Slogger::D(TAG, "+++ CREATED ONE REF");
    }

    mCleanupTask = cleanupTask;
    HandleOnUiThread(ADD_REF);
}

/**
 * Clear the cleanup task {@link Runnable} so that nothing will be done
 * after garbage collection.
 */
void CleanupReference::CleanupNow()
{
    HandleOnUiThread(REMOVE_REF);
}

void CleanupReference::HandleOnUiThread(
    /* [in] */ Int32 what)
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain(LazyHolder::sHandler, what, TO_IINTERFACE(this), (IMessage**)&msg);
    AutoPtr<ILooperHelper> looperHelper;
    assert(0);
    CLooperHelper::AcquireSingleton((ILooperHelper**)&looperHelper);
    AutoPtr<ILooper> myLooper;
    looperHelper->GetMyLooper((ILooper**)&myLooper);
    AutoPtr<IHandler> target;
    msg->GetTarget((IHandler**)&target);
    AutoPtr<ILooper> looper;
    target->GetLooper((ILooper**)&looper);
    if (myLooper == looper) {
        target->HandleMessage(msg);
        msg->Recycle();
    }
    else {
        msg->SendToTarget();
    }
}

void CleanupReference::RunCleanupTaskInternal()
{
    if (DEBUG) {
        Slogger::D(TAG, "runCleanupTaskInternal");
    }

    sRefs->Remove(TO_IINTERFACE(this));
    if (mCleanupTask != NULL) {
        if (DEBUG) {
            Slogger::I(TAG, "--- CLEANING ONE REF");
        }
        mCleanupTask->Run();
        mCleanupTask = NULL;
    }

    assert(0);
    // TODO
    // Clear();
}

} // namespace Common
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
