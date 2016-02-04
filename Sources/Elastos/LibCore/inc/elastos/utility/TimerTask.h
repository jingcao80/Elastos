#ifndef __ELASTOS_UTILITY_TIMERTASK_H__
#define __ELASTOS_UTILITY_TIMERTASK_H__

#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Core::IRunnable;

namespace Elastos {
namespace Utility {

class ECO_PUBLIC TimerTask
    : public Object
    , public ITimerTask
    , public IRunnable
{
protected:
    /**
     * Creates a new {@code TimerTask}.
     */
    TimerTask();

public:
    CAR_INTERFACE_DECL();

    virtual ~TimerTask();

    /*
     * Method called from the Timer for synchronized getting of when field.
     */
    CARAPI GetWhen(
        /* [out] */ Int64* when);

    /*
     * Method called from the Timer object when scheduling an event @param time
     */
    CARAPI SetScheduledTime(
        /* [in] */ Int64 time);

    /*
     * Is TimerTask scheduled into any timer?
     *
     * @return {@code true} if the timer task is scheduled, {@code false}
     * otherwise.
     */
    CARAPI IsScheduled(
        /* [out] */ Boolean* scheduled);

    /**
     * Cancels the {@code TimerTask} and removes it from the {@code Timer}'s queue. Generally, it
     * returns {@code false} if the call did not prevent a {@code TimerTask} from running at
     * least once. Subsequent calls have no effect.
     *
     * @return {@code true} if the call prevented a scheduled execution
     *         from taking place, {@code false} otherwise.
     */
    CARAPI Cancel(
        /* [out] */ Boolean* result);

    /**
     * Returns the scheduled execution time. If the task execution is in
     * progress it returns the execution time of the ongoing task. Tasks which
     * have not yet run return an undefined value.
     *
     * @return the most recent execution time.
     */
    CARAPI ScheduledExecutionTime(
        /* [out] */ Int64* time);

    /**
     * The task to run should be specified in the implementation of the {@code run()}
     * method.
     */
    virtual CARAPI Run() = 0;

    CARAPI SetWhen(
        /* [in] */ Int64 when);

    CARAPI IsCancelled(
        /* [out] */ Boolean* cancelled);

    CARAPI GetPeriod(
        /* [out] */ Int64* period);

    CARAPI SetPeriod(
        /* [in] */ Int64 period);

    CARAPI IsFixedRate(
        /* [out] */ Boolean* fixed);

    CARAPI SetFixedRate(
        /* [in] */ Boolean fixed);

    CARAPI Lock();

    CARAPI Unlock();

public:
    /* Lock object for synchronization. It's also used by Timer class. */
    Object mLock;

    /* If timer was cancelled */
    Boolean mCancelled;

    /* Slots used by Timer */
    Int64 mWhen;

    Int64 mPeriod;

    Boolean mFixedRate;

private:
    /*
     * The time when task will be executed, or the time when task was launched
     * if this is task in progress.
     */
    Int64 mScheduledTime;
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_TIMERTASK_H__
