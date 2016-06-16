#ifndef  __ELASTOS_DROID_PHONE_CALLTIME_H__
#define  __ELASTOS_DROID_PHONE_CALLTIME_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Helper class used to keep track of various "elapsed time" indications
 * in the Phone app, and also to start and stop tracing / profiling.
 */
class CallTime
    : public Handler
{
private:
    class PeriodicTimerCallback
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CallTime::PeriodicTimerCallback")

        PeriodicTimerCallback(
            /* [in] */ CallTime* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CallTime* mHost;
    }

public:
    TO_STRING_IMPL("CallTime")

    CallTime(
        /* [in] */ ICallTimeOnTickListener* listener);

    /**
     * Sets the call timer to "active call" mode, where the timer will
     * periodically update the UI to show how long the specified call
     * has been active.
     *
     * After calling this you should also call reset() and
     * periodicUpdateTimer() to get the timer started.
     */
    CARAPI SetActiveCallMode(
        /* [in] */ ICall* call);

    CARAPI Reset();

    CARAPI PeriodicUpdateTimer();

    CARAPI CancelTimer();

    /**
     * Returns a "call duration" value for the specified Call, in msec,
     * suitable for display in the UI.
     */
    static CARAPI_(Int64) GetCallDuration(
        /* [in] */ ICall* call);

    static CARAPI_(void) SetTraceReady();

    CARAPI_(Boolean) IsTraceReady();

    CARAPI_(Boolean) IsTraceRunning();

    CARAPI_(void) StartTrace();

    CARAPI_(void) StopTrace();

private:
    CARAPI_(void) UpdateElapsedTime(
        /* [in] */ ICall* call);

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String LOG_TAG;
    static const Boolean DBG;
    static const Boolean PROFILE;

    static const Int32 PROFILE_STATE_NONE;
    static const Int32 PROFILE_STATE_READY;
    static const Int32 PROFILE_STATE_RUNNING;

    static Int32 sProfileState;

    AutoPtr<ICall> mCall;
    Int64 mLastReportedTime;
    Boolean mTimerRunning;
    Int64 mInterval;
    AutoPtr<PeriodicTimerCallback> mTimerCallback;
    AutoPtr<ICallTimeOnTickListener> mListener;

};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CALLTIME_H__