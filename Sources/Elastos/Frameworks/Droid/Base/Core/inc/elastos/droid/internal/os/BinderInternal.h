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

#ifndef __ELASTOS_DROID_INTERNAL_OS_BINDER_INTERNAL_H__
#define __ELASTOS_DROID_INTERNAL_OS_BINDER_INTERNAL_H__

#include "Elastos.Droid.Internal.h"
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Core::IRunnable;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Concurrent::ICallable;
using Elastos::Utility::Concurrent::IFuture;
using Elastos::Utility::Concurrent::IExecutorService;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

/**
 * Private and debugging Binder APIs.
 *
 * @see IBinder
 */
class ECO_PUBLIC BinderInternal
{
public:
    /**
     * TimerGc Callable : Wait for a certain time, and execute the BinderGc.
     * Set the postponed count to 0.
     */
    class ECO_LOCAL TimerGc
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        TimerGc(
            /* [in] */ Int64 timeInMillis);

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        Int64 mWaitTime;
    };

private:
    class ECO_LOCAL GcWatcher
        : public Object
    {
    public:
        ~GcWatcher();

        CARAPI Finalize();
    };

public:
    static CARAPI AddGcWatcher(
        /* [in] */ IRunnable* watcher);

    /**
     * Add the calling thread to the IPC thread pool.  This function does
     * not return until the current process is exiting.
     */
    static CARAPI JoinThreadPool();

    /**
     * Return the system time (as reported by {@link SystemClock#uptimeMillis
     * SystemClock.uptimeMillis()}) that the last garbage collection occurred
     * in this process.  This is not for general application use, and the
     * meaning of "when a garbage collection occurred" will change as the
     * garbage collector evolves.
     *
     * @return Returns the time as per {@link SystemClock#uptimeMillis
     * SystemClock.uptimeMillis()} of the last garbage collection.
     */
    static Int64 GetLastGcTime();

    /**
     * Return the global "context object" of the system.  This is usually
     * an implementation of IServiceManager, which you can use to find
     * other services.
     */
    static AutoPtr<IBinder> GetContextObject();

    /**
     * Special for system process to not allow incoming calls to run at
     * background scheduling priority.
     * @hide
     */
    static CARAPI DisableBackgroundScheduling(
        /* [in] */ Boolean disable);

    static CARAPI HandleGc();

    static CARAPI ForceGc(
        /* [in] */ const String& reason);

    /**
     * modifyDelayedGcParams : Call from the framework based on some special Ux event.
     * like appLaunch.
     *
     * 1. If this is the first time for the trigger event, or, if there is no scheduled
     *    task, create a new FutureTaskInstance, and set the lastGcDelayRequestTime.
     *    This will be used by forceBinderGc later.
     *
     * 2. If the postponed iterations hit a maximum limit, do nothing. Let the current
     *    task execute the gc. If not,
     *
     *    a. Set the start time.
     *    b. Increment the postponed count
     *    c. Cancel the current task and start a new one for GC_DELAY_MAX_DURATION.
     */
    static CARAPI ModifyDelayedGcParams();

    /**
     * Modified forceBinderGc. The brief algorithm is as follows --
     *
     * 1. If no futureTaskInstance has been initiated, directly force a BinderGc.
     * 2. Check for the duration since the last request, and see if it was within the
     *    last GC_DELAY_MAX_DURATION secs. If yes, we need to delay the GC until
     *    GC_DELAY_MAX_DURATION.
     * 3. If there is a task scheduled (postponedGcCount != 0), we merely prevent this GC,
     *    and let the GC scheduled execute.
     * 4. If no task is scheduled, we schedule one now for (GC_DELAY_MAX_DURATION - touch duration),
     *    and update postponedGcCount.
     */
    static CARAPI ForceBinderGc();

private:
    ECO_LOCAL BinderInternal();
    ECO_LOCAL BinderInternal(const BinderInternal&);

private:
    // static AutoPtr<IWeakReference> sGcWatcher; //new WeakReference<GcWatcher>(new GcWatcher());

    // static AutoPtr<IArrayList> sGcWatchers;// = new ArrayList<>();
    // static AutoPtr<ArrayOf<IInterface*> > sTmpWatchers;// = new Runnable[1];
    // static Int64 sLastGcTime;

    /* Maximum duration a GC can be delayed. */
    static Int32 GC_DELAY_MAX_DURATION;
    /**
     * Maximum number of times a GC can be delayed since the
     * original request.
     */
    static Int32 POSTPONED_GC_MAX;

    /**
     * lastGcDelayRequestTime records the time-stamp of the last time
     * a GC delay request was made.
     */
    static Int64 mLastGcDelayRequestTime;
    static AutoPtr<TimerGc> mTimerGcInstance;
    static AutoPtr<IFuture> mFutureTaskInstance;
    static AutoPtr<IExecutorService> mExecutor;
    static Int32 mPostponedGcCount;
    static Object mDelayGcMonitorObject;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_BINDER_INTERNAL_H__
