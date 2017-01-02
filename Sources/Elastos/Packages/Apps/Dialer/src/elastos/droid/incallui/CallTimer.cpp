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

#include "elastos/droid/incallui/CallTimer.h"
#include "elastos/droid/os/SystemClock.h"
#include "Elastos.Droid.Internal.h"

using Elastos::Droid::Os::SystemClock;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//================================================================
// CallTimer::CallTimerCallback
//================================================================
ECode CallTimer::CallTimerCallback::Run()
{
    mHost->PeriodicUpdateTimer();
    return NOERROR;
}


//================================================================
// CallTimer
//================================================================
CallTimer::CallTimer(
    /* [in] */ IRunnable* callback)
    : mLastReportedTime(0)
    , mInterval(0)
    , mRunning(FALSE)
{
    Handler::constructor(FALSE);

    assert(callback != NULL);

    mCallback = callback;
    mInternalCallback = (IRunnable*)new CallTimerCallback(this);
}

Boolean CallTimer::Start(
    /* [in] */ Int64 interval)
{
    if (interval <= 0) {
        return FALSE;
    }

    // cancel any previous timer
    Cancel();

    mInterval = interval;
    mLastReportedTime = SystemClock::GetUptimeMillis();

    mRunning = TRUE;
    PeriodicUpdateTimer();

    return TRUE;
}

void CallTimer::Cancel()
{
    RemoveCallbacks(mInternalCallback);
    mRunning = FALSE;
}

void CallTimer::PeriodicUpdateTimer()
{
    if (!mRunning) {
        return;
    }

    Int64 now = SystemClock::GetUptimeMillis();
    Int64 nextReport = mLastReportedTime + mInterval;
    while (now >= nextReport) {
        nextReport += mInterval;
    }

    Boolean result;
    PostAtTime(mInternalCallback, nextReport, &result);
    mLastReportedTime = nextReport;

    // Run the callback
    mCallback->Run();
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
