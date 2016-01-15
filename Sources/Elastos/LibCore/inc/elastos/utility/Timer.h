#ifndef __ELASTOS_UTILITY_TIMER_H__
#define __ELASTOS_UTILITY_TIMER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Thread.h"

using Elastos::Core::Thread;
using Elastos::Core::IClassLoader;
using Elastos::Core::IRunnable;
using Elastos::Core::IThread;

namespace Elastos {
namespace Utility {

class Timer
    : public Object
    , public ITimer
{
private:
    class TimerImpl
        : public Thread
    {
        friend class Timer;

    private:
        class TimerHeap : public ElRefBase
        {
            friend class Timer;

        public:
            TimerHeap();

            ~TimerHeap();

            CARAPI_(AutoPtr<ITimerTask>) Minimum();

            CARAPI_(Boolean) IsEmpty();

            CARAPI_(void) Insert(
                /* [in] */ ITimerTask* task);

            CARAPI_(void) Delete(
                /* [in] */ Int32 pos);

            CARAPI_(void) Reset();

            CARAPI_(void) AdjustMinimum();

            CARAPI_(void) DeleteIfCancelled();

        private:
            CARAPI_(void) UpHeap();

            CARAPI_(void) DownHeap(
                /* [in] */ Int32 pos);

            CARAPI_(Int32) GetTask(
                /* [in] */ ITimerTask* task);

        private:
            Int32 DEFAULT_HEAP_SIZE;
            AutoPtr< ArrayOf<ITimerTask*> > mTimers;
            Int32 mSize;
            Int32 mDeletedCancelledNumber;
        };

    public:
        TimerImpl(
            /* [in] */ const String& name,
            /* [in] */ Boolean isDaemon);

        ~TimerImpl();

        CARAPI Run();

        CARAPI_(void) Cancel();

        CARAPI_(Int32) Purge();

    private:
        CARAPI_(void) InsertTask(
            /* [in] */ ITimerTask* newTask);

    public:
        /**
         * True if the method cancel() of the Timer was called or the !!!stop()
         * method was invoked
         */
        Boolean mCancelled;

        /**
         * True if the Timer has become garbage
         */
        Boolean mFinished;

        /**
         * Contains scheduled events, sorted according to
         * {@code when} field of TaskScheduled object.
         */
        AutoPtr<TimerHeap> mTasks;
    };

    class FinalizerHelper : public ElRefBase
    {
    public:
        FinalizerHelper(
            /* [in] */ TimerImpl* impl);

        ~FinalizerHelper();

    private:
        AutoPtr<TimerImpl> mImpl;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a new named {@code Timer} which may be specified to be run as a
     * daemon thread.
     *
     * @param name the name of the {@code Timer}.
     * @param isDaemon true if {@code Timer}'s thread should be a daemon thread.
     * @throws NullPointerException is {@code name} is {@code null}
     */
     Timer(
        /* [in] */ const String& name,
        /* [in] */ Boolean isDaemon);

    /**
     * Creates a new named {@code Timer} which does not run as a daemon thread.
     *
     * @param name the name of the Timer.
     * @throws NullPointerException is {@code name} is {@code null}
     */
    Timer(
        /* [in] */ const String& name);

    /**
     * Creates a new {@code Timer} which may be specified to be run as a daemon thread.
     *
     * @param isDaemon {@code true} if the {@code Timer}'s thread should be a daemon thread.
     */
    Timer(
        /* [in] */ Boolean isDaemon);

    /**
     * Creates a new non-daemon {@code Timer}.
     */
    Timer();

    virtual ~Timer();

    /**
     * Cancels the {@code Timer} and all scheduled tasks. If there is a
     * currently running task it is not affected. No more tasks may be scheduled
     * on this {@code Timer}. Subsequent calls do nothing.
     */
    CARAPI Cancel();

    /**
     * Removes all canceled tasks from the task queue. If there are no
     * other references on the tasks, then after this call they are free
     * to be garbage collected.
     *
     * @return the number of canceled tasks that were removed from the task
     *         queue.
     */
    CARAPI Purge(
        /* [out] */ Int32* number);

    /**
     * Schedule a task for single execution. If {@code when} is less than the
     * current time, it will be scheduled to be executed as soon as possible.
     *
     * @param task
     *            the task to schedule.
     * @param when
     *            time of execution.
     * @throws IllegalArgumentException
     *                if {@code when.getTime() < 0}.
     * @throws IllegalStateException
     *                if the {@code Timer} has been canceled, or if the task has been
     *                scheduled or canceled.
     */
    CARAPI Schedule(
        /* [in] */ ITimerTask* task,
        /* [in] */ IDate* when);

    /**
     * Schedule a task for single execution after a specified delay.
     *
     * @param task
     *            the task to schedule.
     * @param delay
     *            amount of time in milliseconds before execution.
     * @throws IllegalArgumentException
     *                if {@code delay < 0}.
     * @throws IllegalStateException
     *                if the {@code Timer} has been canceled, or if the task has been
     *                scheduled or canceled.
     */
    CARAPI Schedule(
        /* [in] */ ITimerTask* task,
        /* [in] */ Int64 delay);

    /**
     * Schedule a task for repeated fixed-delay execution after a specific delay.
     *
     * @param task
     *            the task to schedule.
     * @param delay
     *            amount of time in milliseconds before first execution.
     * @param period
     *            amount of time in milliseconds between subsequent executions.
     * @throws IllegalArgumentException
     *                if {@code delay < 0} or {@code period < 0}.
     * @throws IllegalStateException
     *                if the {@code Timer} has been canceled, or if the task has been
     *                scheduled or canceled.
     */
    CARAPI Schedule(
        /* [in] */ ITimerTask* task,
        /* [in] */ Int64 delay,
        /* [in] */ Int64 period);

    /**
     * Schedule a task for repeated fixed-delay execution after a specific time
     * has been reached.
     *
     * @param task
     *            the task to schedule.
     * @param when
     *            time of first execution.
     * @param period
     *            amount of time in milliseconds between subsequent executions.
     * @throws IllegalArgumentException
     *                if {@code when.getTime() < 0} or {@code period < 0}.
     * @throws IllegalStateException
     *                if the {@code Timer} has been canceled, or if the task has been
     *                scheduled or canceled.
     */
    CARAPI Schedule(
        /* [in] */ ITimerTask* task,
        /* [in] */ IDate* when,
        /* [in] */ Int64 period);

    /**
     * Schedule a task for repeated fixed-rate execution after a specific delay
     * has passed.
     *
     * @param task
     *            the task to schedule.
     * @param delay
     *            amount of time in milliseconds before first execution.
     * @param period
     *            amount of time in milliseconds between subsequent executions.
     * @throws IllegalArgumentException
     *                if {@code delay < 0} or {@code period < 0}.
     * @throws IllegalStateException
     *                if the {@code Timer} has been canceled, or if the task has been
     *                scheduled or canceled.
     */
    CARAPI ScheduleAtFixedRate(
        /* [in] */ ITimerTask* task,
        /* [in] */ Int64 delay,
        /* [in] */ Int64 period);

    /**
     * Schedule a task for repeated fixed-rate execution after a specific time
     * has been reached.
     *
     * @param task
     *            the task to schedule.
     * @param when
     *            time of first execution.
     * @param period
     *            amount of time in milliseconds between subsequent executions.
     * @throws IllegalArgumentException
     *                if {@code when.getTime() < 0} or {@code period < 0}.
     * @throws IllegalStateException
     *                if the {@code Timer} has been canceled, or if the task has been
     *                scheduled or canceled.
     */
    CARAPI ScheduleAtFixedRate(
        /* [in] */ ITimerTask* task,
        /* [in] */ IDate* when,
        /* [in] */ Int64 period);

public:
    /**
     * Creates a new named {@code Timer} which may be specified to be run as a
     * daemon thread.
     *
     * @param name the name of the {@code Timer}.
     * @param isDaemon true if {@code Timer}'s thread should be a daemon thread.
     * @throws NullPointerException is {@code name} is {@code null}
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ Boolean isDaemon);

    CARAPI constructor(
        /* [in] */ const String& name);

    /**
     * Creates a new {@code Timer} which may be specified to be run as a daemon thread.
     *
     * @param isDaemon {@code true} if the {@code Timer}'s thread should be a daemon thread.
     */
    CARAPI constructor(
        /* [in] */ Boolean isDaemon);

    /**
     * Creates a new non-daemon {@code Timer}.
     */
    CARAPI constructor();

private:
    static CARAPI_(Int64) NextId();

    /*
     * Schedule a task.
     */
    CARAPI ScheduleImpl(
        /* [in] */ ITimerTask* task,
        /* [in] */ Int64 delay,
        /* [in] */ Int64 period,
        /* [in] */ Boolean fixed);

private:
    static Int64 sTimerId;
    static Object sTimerIdLock;

    /* This object will be used in synchronization purposes */
    AutoPtr<TimerImpl> mImpl;

    // Used to finalize thread
    AutoPtr<FinalizerHelper> mFinalizer;
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_TIMER_H__
