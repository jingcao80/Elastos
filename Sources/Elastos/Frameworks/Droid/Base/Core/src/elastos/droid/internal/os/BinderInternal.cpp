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

#include "elastos/droid/internal/os/BinderInternal.h"
#include "elastos/droid/os/SystemClock.h"
#include <Elastos.Droid.Os.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Thread.h>
#include <elastos/core/AutoLock.h>

#include <binder/IInterface.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::SystemClock;
using Elastos::Core::Thread;
using Elastos::Core::IRunnable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Concurrent::IExecutors;
using Elastos::Utility::Concurrent::CExecutors;
using Elastos::Utility::Concurrent::EIID_ICallable;
using Elastos::Utility::Concurrent::CFutureTask;
using Elastos::Utility::Concurrent::IExecutor;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

// native code from android_util_Binder.cpp
static volatile int32_t gNumRefsCreated = 0;

AutoPtr<Elastos::Droid::Os::IBinder> ElastosObjectForIBinder(const android::sp<android::IBinder>& val)
{
    assert(0 && "Should not happen in Elastos.");
    return NULL;
}

// AutoPtr<IArrayList> InitGcWatchers()
// {
//     AutoPtr<IArrayList> list;
//     CArrayList::New((IArrayList**)&list);
//     return list;
// }

// AutoPtr<IWeakReference> BinderInternal::sGcWatcher; //new WeakReference<GcWatcher>(new GcWatcher());

// AutoPtr<IArrayList> BinderInternal::sGcWatchers = InitGcWatchers();
// AutoPtr<ArrayOf<IInterface*> > BinderInternal::sTmpWatchers = ArrayOf<IInterface*> ::Alloc(1);
// Int64 BinderInternal::sLastGcTime = 0;

Int32 BinderInternal::GC_DELAY_MAX_DURATION = 3000;

Int32 BinderInternal::POSTPONED_GC_MAX = 5;

Int64 BinderInternal::mLastGcDelayRequestTime = SystemClock::GetUptimeMillis();
AutoPtr<BinderInternal::TimerGc> BinderInternal::mTimerGcInstance = NULL ;
AutoPtr<IFuture> BinderInternal::mFutureTaskInstance = NULL ;

AutoPtr<IExecutorService> InitExecutorService()
{
    AutoPtr<IExecutors> hlp;
    CExecutors::AcquireSingleton((IExecutors**)&hlp);
    AutoPtr<IExecutorService> p;
    hlp->NewFixedThreadPool(1, (IExecutorService**)&p);
    return p;
}

AutoPtr<IExecutorService> BinderInternal::mExecutor = InitExecutorService();
Int32 BinderInternal::mPostponedGcCount = 0;
Object BinderInternal::mDelayGcMonitorObject;

//===============================================================
// BinderInternal::TimerGc::
//===============================================================
CAR_INTERFACE_IMPL(BinderInternal::TimerGc, Object, ICallable)

BinderInternal::TimerGc::TimerGc(
    /* [in] */ Int64 timeInMillis)
{
    mWaitTime = timeInMillis;
}

ECode BinderInternal::TimerGc::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    Thread::Sleep(mWaitTime);
    BinderInternal::ForceGc(String("Binder"));
    BinderInternal::mPostponedGcCount = 0;
    *result = NULL;
    return NOERROR;
}

//===============================================================
// BinderInternal::GcWatcher::
//===============================================================
BinderInternal::GcWatcher::~GcWatcher()
{
    Finalize();
}

ECode BinderInternal::GcWatcher::Finalize()
{
    // handleGc();
    // sLastGcTime = SystemClock.uptimeMillis();
    // {    AutoLock syncLock(sGcWatchers);
    //     sTmpWatchers = sGcWatchers.toArray(sTmpWatchers);
    // }
    // for (int i=0; i<sTmpWatchers.length; i++) {
    //     if (sTmpWatchers[i] != null) {
    //         sTmpWatchers[i].run();
    //     }
    // }
    // sGcWatcher = new WeakReference<GcWatcher>(new GcWatcher());
    return NOERROR;
}

//===============================================================
// BinderInternal::
//===============================================================
ECode BinderInternal::AddGcWatcher(
    /* [in] */ IRunnable* watcher)
{
    // {    AutoLock syncLock(sGcWatchers);
    //     sGcWatchers.add(watcher);
    // }
    return NOERROR;
}

ECode BinderInternal::JoinThreadPool()
{
    android::sp<android::IBinder> b = android::ProcessState::self()->getContextObject(NULL);
    android::IPCThreadState::self()->joinThreadPool();
    return NOERROR;
}

Int64 BinderInternal::GetLastGcTime()
{
    // return sLastGcTime;
    return 0;
}

AutoPtr<Elastos::Droid::Os::IBinder> BinderInternal::GetContextObject()
{
    android::sp<android::IBinder> b = android::ProcessState::self()->getContextObject(NULL);
    return ElastosObjectForIBinder(b);
}

ECode BinderInternal::DisableBackgroundScheduling(
    /* [in] */ Boolean disable)
{
    android::IPCThreadState::disableBackgroundScheduling(disable ? true : false);
    return NOERROR;
}

ECode BinderInternal::HandleGc()
{
    // ALOGV("Gc has executed, clearing binder ops");
    // android_atomic_and(0, &gNumRefsCreated);
    return NOERROR;
}

ECode BinderInternal::ForceGc(
    /* [in] */ const String& reason)
{
    // EventLog.writeEvent(2741, reason);
    // Runtime.getRuntime().gc();
    return NOERROR;
}

ECode BinderInternal::ModifyDelayedGcParams()
{
    Int64 nowTime = SystemClock::GetUptimeMillis();
    {    AutoLock syncLock(mDelayGcMonitorObject);
        if ((mFutureTaskInstance != NULL) && (mPostponedGcCount != 0)) {
            if (mPostponedGcCount <= POSTPONED_GC_MAX) {
                Boolean res = FALSE;
                mFutureTaskInstance->Cancel(TRUE, &res);
                Boolean bCal = FALSE;
                mFutureTaskInstance->IsCancelled(&bCal);
                if (bCal) {
                    mLastGcDelayRequestTime = nowTime;
                    mPostponedGcCount++;
                    mTimerGcInstance = new TimerGc(GC_DELAY_MAX_DURATION);
                    CFutureTask::New(mTimerGcInstance, (IFuture**)&mFutureTaskInstance);
                    IExecutor::Probe(mExecutor)->Execute(IRunnable::Probe(mFutureTaskInstance));
                }
            }
        }
        else {
            mLastGcDelayRequestTime = nowTime;
            mTimerGcInstance = new TimerGc(GC_DELAY_MAX_DURATION);
            CFutureTask::New(mTimerGcInstance, (IFuture**)&mFutureTaskInstance);
        }
    }
    return NOERROR;
}

ECode BinderInternal::ForceBinderGc()
{
    {    AutoLock syncLock(mDelayGcMonitorObject);
        if (mFutureTaskInstance != NULL) {
            Int64 lastGcDelayRequestDuration = (SystemClock::GetUptimeMillis() - mLastGcDelayRequestTime);
            if (lastGcDelayRequestDuration < GC_DELAY_MAX_DURATION) {
                if (mPostponedGcCount != 0)
                    return NOERROR;
                Boolean res = FALSE;
                mFutureTaskInstance->Cancel(TRUE, &res);
                mTimerGcInstance = new TimerGc(GC_DELAY_MAX_DURATION - lastGcDelayRequestDuration);
                CFutureTask::New(mTimerGcInstance, (IFuture**)&mFutureTaskInstance);
                mPostponedGcCount = 1;
                IExecutor::Probe(mExecutor)->Execute(IRunnable::Probe(mFutureTaskInstance));
                return NOERROR;
            }
        }
    }
    return ForceGc(String("Binder"));
}


} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
