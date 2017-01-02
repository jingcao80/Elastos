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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_COMMON_CLEANUPREFERENCE_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_COMMON_CLEANUPREFERENCE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Thread.h>

using Elastos::Core::Thread;
using Elastos::Core::IThread;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Utility::ISet;

// import org.chromium.base.ThreadUtils;
// import org.chromium.base.TraceEvent;

// import java.lang.ref.ReferenceQueue;
// import java.lang.ref.WeakReference;
// import java.util.HashSet;
// import java.util.Set;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Common {

/**
 * Handles running cleanup tasks when an object becomes eligible for GC. Cleanup tasks
 * are always executed on the main thread. In general, classes should not have
 * finalizers and likewise should not use this class for the same reasons. The
 * exception is where public APIs exist that require native side resources to be
 * cleaned up in response to java side GC of API objects. (Private/internal
 * interfaces should always favor explicit resource releases / destroy()
 * protocol for this rather than depend on GC to trigger native cleanup).
 * NOTE this uses WeakReference rather than PhantomReference, to avoid delaying the
 * cleanup processing until after finalizers (if any) have run. In general usage of
 * this class indicates the client does NOT use finalizers anyway (Good), so this should
 * not be a visible difference in practice.
 */
class CleanupReference : public Object
{
    friend Boolean CleanupReferenceStart();
    friend AutoPtr<IHandler> CleanupReference_LazyHolder_sHandler_Init();
    friend AutoPtr<IThread> CleanupReference_sReaperThread_Init();
private:
    /**
     * This {@link Handler} polls {@link #sRefs}, looking for cleanup tasks that
     * are ready to run.
     * This is lazily initialized as ThreadUtils.getUiThreadLooper() may not be
     * set yet early in startup.
     */
    class LazyHolder
    {
        friend class CleanupReference;
        friend AutoPtr<IHandler> CleanupReference_LazyHolder_sHandler_Init();
    private:
        class InnerHandler
            : public Handler
        {
        public:
            //CAR_INTERFACE_DECL()
            InnerHandler(
                /* [in] */ ILooper* looper);

            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);
        };

    private:
        static AutoPtr<IHandler> sHandler;
    };

    class InnerThread : public Thread
    {
    public:
        InnerThread(
            /* [in] */ const String& threadName);

        CARAPI Run();

    private:
        CleanupReference* mOwner;
    };

public:
    /**
     * @param obj the object whose loss of reachability should trigger the
     *            cleanup task.
     * @param cleanupTask the task to run once obj loses reachability.
     */
    CleanupReference(
        /* [in] */ Object* obj,
        /* [in] */ IRunnable* cleanupTask);

    /**
     * Clear the cleanup task {@link Runnable} so that nothing will be done
     * after garbage collection.
     */
    CARAPI_(void) CleanupNow();

private:
    CARAPI_(void) HandleOnUiThread(
        /* [in] */ Int32 what);

    CARAPI_(void) RunCleanupTaskInternal();

private:
    static const String TAG;

    static const Boolean DEBUG;  // Always check in as false!

    // The VM will enqueue CleanupReference instance onto sGcQueue when it becomes eligible for
    // garbage collection (i.e. when all references to the underlying object are nullified).
    // |sReaperThread| processes this queue by forwarding the references on to the UI thread
    // (via REMOVE_REF message) to perform cleanup.

    // TODO private static ReferenceQueue<Object> sGcQueue = new ReferenceQueue<Object>();

    static Object sCleanupMonitor;

    static AutoPtr<IThread> sReaperThread;

    // Message's sent in the |what| field to |sHandler|.

    // Add a new reference to sRefs. |msg.obj| is the CleanupReference to add.
    static const Int32 ADD_REF = 1;
    // Remove reference from sRefs. |msg.obj| is the CleanupReference to remove.
    static const Int32 REMOVE_REF = 2;

    /**
     * Keep a strong reference to {@link CleanupReference} so that it will
     * actually get enqueued.
     * Only accessed on the UI thread.
     */
    static AutoPtr<ISet> sRefs;

    AutoPtr<IRunnable> mCleanupTask;
};

} // namespace Common
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_COMMON_CLEANUPREFERENCE_H__
