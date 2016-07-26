
#ifndef __ELASTOS_DROID_INCALLUI_CALLTIMER_H__
#define __ELASTOS_DROID_INCALLUI_CALLTIMER_H__

#include <elastos/droid/os/Handler.h>
#include <elastos/droid/os/Runnable.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::Runnable;

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * Helper class used to keep track of events requiring regular intervals.
 */
class CallTimer
    : public Handler
{
private:
    class CallTimerCallback : public Runnable
    {
    public:
        CallTimerCallback(
            /* [in] */ CallTimer* host)
            : mHost(host)
        {}

        // @Override
        CARAPI Run();

    private:
        CallTimer* mHost;
    };

public:
    CallTimer(
        /* [in] */ IRunnable* callback);

    TO_STRING_IMPL("CallTimer")

    CARAPI_(Boolean) Start(
        /* [in] */ Int64 interval);

    CARAPI_(void) Cancel();

private:
    CARAPI_(void) PeriodicUpdateTimer();

private:
    AutoPtr<IRunnable> mInternalCallback;
    AutoPtr<IRunnable> mCallback;
    Int64 mLastReportedTime;
    Int64 mInterval;
    Boolean mRunning;

    friend class CallTimerCallback;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_CALLTIMER_H__
