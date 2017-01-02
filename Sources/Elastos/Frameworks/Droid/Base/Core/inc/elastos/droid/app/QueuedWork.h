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

#ifndef __ELASTOS_DROID_APP_QUEUEDWORK_H__
#define __ELASTOS_DROID_APP_QUEUEDWORK_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IRunnable;
using Elastos::Utility::IQueue;
using Elastos::Utility::Concurrent::IExecutorService;
using Elastos::Utility::Concurrent::IConcurrentLinkedQueue;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Internal utility class to keep track of process-global work that's
 * outstanding and hasn't been finished yet.
 *
 * This was created for writing SharedPreference edits out
 * asynchronously so we'd have a mechanism to wait for the writes in
 * Activity.onPause and similar places, but we may use this mechanism
 * for other things in the future.
 *
 * @hide
 */
class QueuedWork
{
public:
    /**
     * Returns a single-thread Executor shared by the entire process,
     * creating it if necessary.
     */
    static CARAPI SingleThreadExecutor(
        /* [out] */ IExecutorService** singleThreadExecutor);

        /**
     * Add a runnable to finish (or wait for) a deferred operation
     * started in this context earlier.  Typically finished by e.g.
     * an Activity#onPause.  Used by SharedPreferences$Editor#startCommit().
     *
     * Note that this doesn't actually start it running.  This is just
     * a scratch set for callers doing async work to keep updated with
     * what's in-flight.  In the common case, caller code
     * (e.g. SharedPreferences) will pretty quickly call remove()
     * after an add().  The only time these Runnables are run is from
     * waitToFinish(), below.
     */
    static CARAPI Add(
        /* [in] */ IRunnable* finisher);

    static CARAPI Remove(
        /* [in] */ IRunnable* finisher);

    /**
     * Finishes or waits for async operations to complete.
     * (e.g. SharedPreferences$Editor#startCommit writes)
     *
     * Is called from the Activity base class's onPause(), after
     * BroadcastReceiver's onReceive, after Service command handling,
     * etc.  (so async work is never lost)
     */
    static CARAPI WaitToFinish();

    /**
     * Returns true if there is pending work to be done.  Note that the
     * result is out of data as soon as you receive it, so be careful how you
     * use it.
     */
    static CARAPI HasPendingWork(
        /* [out] */ Boolean* result);

public:
    // The set of Runnables that will finish or wait on any async
    // activities started by the application.
    //static ConcurrentLinkedQueue<Runnable> sPendingWorkFinishers;
    static AutoPtr<IQueue> sPendingWorkFinishers;//IConcurrentLinkedQueue
    static AutoPtr<IExecutorService> sSingleThreadExecutor; // lazy, guarded by class
    static Object sClassLock;
private:
    QueuedWork();
    QueuedWork(const QueuedWork&);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_QUEUEDWORK_H__
