
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
