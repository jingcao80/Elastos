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

#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CSCHEDULEDTHREADPOOLEXECUTOR_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CSCHEDULEDTHREADPOOLEXECUTOR_H__

#include "_Elastos_Utility_Concurrent_CScheduledThreadPoolExecutor.h"
#include "ThreadPoolExecutor.h"
#include "AbstractQueue.h"
#include "FutureTask.h"

using Elastos::Utility::Concurrent::Locks::IReentrantLock;
using Elastos::Utility::Concurrent::Locks::ICondition;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger64;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CScheduledThreadPoolExecutor)
    , public ThreadPoolExecutor
    , public IScheduledExecutorService
    , public IScheduledThreadPoolExecutor
{
public:
    /**
     * Specialized delay queue. To mesh with TPE declarations, this
     * class must be declared as a BlockingQueue<Runnable> even though
     * it can only hold RunnableScheduledFutures.
     */
    class DelayedWorkQueue
        : public AbstractQueue
        , public IBlockingQueue
    {
        /*
         * A DelayedWorkQueue is based on a heap-based data structure
         * like those in DelayQueue and PriorityQueue, except that
         * every ScheduledFutureTask also records its index into the
         * heap array. This eliminates the need to find a task upon
         * cancellation, greatly speeding up removal (down from O(n)
         * to O(log n)), and reducing garbage retention that would
         * otherwise occur by waiting for the element to rise to top
         * before clearing. But because the queue may also hold
         * RunnableScheduledFutures that are not ScheduledFutureTasks,
         * we are not guaranteed to have such indices available, in
         * which case we fall back to linear search. (We expect that
         * most tasks will not be decorated, and that the faster cases
         * will be much more common.)
         *
         * All heap operations must record index changes -- mainly
         * within siftUp and siftDown. Upon removal, a task's
         * heapIndex is set to -1. Note that ScheduledFutureTasks can
         * appear at most once in the queue (this need not be true for
         * other kinds of tasks or work queues), so are uniquely
         * identified by heapIndex.
         */
     private:
        /**
         * Snapshot iterator that works off copy of underlying q array.
         */
        class Itr
            : public Object
            , public IIterator
        {
        public:
            CAR_INTERFACE_DECL()

            Itr(
                /* [in] */ ArrayOf<IRunnableScheduledFuture*>* array,
                /* [in] */ DelayedWorkQueue* owner);

            CARAPI HasNext(
                /* [out] */ Boolean* result);

            CARAPI GetNext(
                /* [out] */ IInterface** object);

            CARAPI Remove();

        private:
            AutoPtr<ArrayOf<IRunnableScheduledFuture*> > mArray;
            Int32 mCursor;     // index of next element to return
            Int32 mLastRet;   // index of last element, or -1 if no such
            DelayedWorkQueue* mOwner;
        };

    public:
        CAR_INTERFACE_DECL()

        CARAPI Contains(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI Remove(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified);

        CARAPI GetSize(
            /* [out] */ Int32* size);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI RemainingCapacity(
            /* [out] */ Int32* capacity);

        CARAPI Peek(
            /* [out] */ IInterface** e);

        CARAPI Offer(
            /* [in] */ IInterface* e,
            /* [out] */ Boolean* result);

        CARAPI Put(
            /* [in] */ IInterface* e);

        CARAPI Add(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified);

        CARAPI Offer(
            /* [in] */ IInterface* e,
            /* [in] */ Int64 timeout,
            /* [in] */ ITimeUnit* unit,
            /* [out] */ Boolean* result);

        CARAPI Poll(
            /* [out] */ IInterface** e);

        CARAPI Take(
            /* [out] */ IInterface** res);

        CARAPI Poll(
            /* [in] */ Int64 timeout,
            /* [in] */ ITimeUnit* unit,
            /* [out] */ IInterface** e);

        CARAPI Clear();

        CARAPI DrainTo(
            /* [in] */ ICollection* c,
            /* [out] */ Int32* number);

        CARAPI DrainTo(
            /* [in] */ ICollection* c,
            /* [in] */ Int32 maxElements,
            /* [out] */ Int32* number);

        CARAPI ToArray(
            /* [out, callee] */ ArrayOf<IInterface*>** array);

        CARAPI ToArray(
            /* [in] */ ArrayOf<IInterface*>* inArray,
            /* [out, callee] */ ArrayOf<IInterface*>** outArray);

        CARAPI GetIterator(
            /* [out] */ IIterator** it);

    protected:
        CARAPI_(String) GetClassName() { return String("CScheduledThreadPoolExecutor::DelayedWorkQueue"); }

    private:
        /**
         * Sets f's heapIndex if it is a ScheduledFutureTask.
         */
        CARAPI_(void) SetIndex(
            /* [in] */ IRunnableScheduledFuture* f,
            /* [in] */ Int32 idx);

        /**
         * Sifts element added at bottom up to its heap-ordered spot.
         * Call only when holding lock.
         */
        CARAPI_(void) SiftUp(
            /* [in] */ Int32 k,
            /* [in] */ IRunnableScheduledFuture* key);

        /**
         * Sifts element added at top down to its heap-ordered spot.
         * Call only when holding lock.
         */
        CARAPI_(void) SiftDown(
            /* [in] */ Int32 k,
            /* [in] */ IRunnableScheduledFuture* key);

        /**
         * Resizes the heap array.  Call only when holding lock.
         */
        CARAPI_(void) Grow();

        /**
         * Finds index of given object, or -1 if absent.
         */
        CARAPI_(Int32) IndexOf(
            /* [in] */ IInterface* x);

        /**
         * Performs common bookkeeping for poll and take: Replaces
         * first element with last and sifts it down.  Call only when
         * holding lock.
         * @param f the task to remove and return
         */
        CARAPI_(AutoPtr<IRunnableScheduledFuture>) FinishPoll(
            /* [in] */ IRunnableScheduledFuture* f);

        /**
         * Returns first element only if it is expired.
         * Used only by drainTo.  Call only when holding lock.
         */
        CARAPI_(AutoPtr<IRunnableScheduledFuture>) PeekExpired();

    private:
        static Int32 INITIAL_CAPACITY;
        AutoPtr<ArrayOf<IRunnableScheduledFuture*> > mQueue;
        AutoPtr<IReentrantLock> mLock;
        Int32 mSize;

        /**
         * Thread designated to wait for the task at the head of the
         * queue.  This variant of the Leader-Follower pattern
         * (http://www.cs.wustl.edu/~schmidt/POSA/POSA2/) serves to
         * minimize unnecessary timed waiting.  When a thread becomes
         * the leader, it waits only for the next delay to elapse, but
         * other threads await indefinitely.  The leader thread must
         * signal some other thread before returning from take() or
         * poll(...), unless some other thread becomes leader in the
         * interim.  Whenever the head of the queue is replaced with a
         * task with an earlier expiration time, the leader field is
         * invalidated by being reset to null, and some waiting
         * thread, but not necessarily the current leader, is
         * signalled.  So waiting threads must be prepared to acquire
         * and lose leadership while waiting.
         */
        AutoPtr<IThread> mLeader;

        /**
         * Condition signalled when a newer task becomes available at the
         * head of the queue or a new thread may need to become leader.
         */
        AutoPtr<ICondition> mAvailable;
    };

private:
    class ScheduledFutureTask
        : public FutureTask
        , public IRunnableScheduledFuture
        , public IScheduledFutureTask
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Creates a one-shot action with given nanoTime-based trigger time.
         */
        ScheduledFutureTask(
            /* [in] */ IRunnable* r,
            /* [in] */ IInterface* result,
            /* [in] */ Int64 ns,
            /* [in] */ CScheduledThreadPoolExecutor* owner);

        /**
         * Creates a periodic action with given nano time and period.
         */
        ScheduledFutureTask(
            /* [in] */ IRunnable* r,
            /* [in] */ IInterface* result,
            /* [in] */ Int64 ns,
            /* [in] */ Int64 period,
            /* [in] */ CScheduledThreadPoolExecutor* owner);

        /**
         * Creates a one-shot action with given nanoTime-based trigger time.
         */
        ScheduledFutureTask(
            /* [in] */ ICallable* callable,
            /* [in] */ Int64 ns,
            /* [in] */ CScheduledThreadPoolExecutor* owner);

        CARAPI GetDelay(
            /* [in] */ ITimeUnit* unit,
            /* [out] */ Int64* value);

        CARAPI CompareTo(
            /* [in] */ IInterface* another,
            /* [out] */ Int32* result);

        CARAPI IsPeriodic(
            /* [out] */ Boolean* value);

        CARAPI Cancel(
            /* [in] */ Boolean mayInterruptIfRunning,
            /* [out] */ Boolean* result);

        /**
         * Overrides FutureTask version so as to reset/requeue if periodic.
         */
        CARAPI Run();

    private:
        /**
         * Sets the next time to run for a periodic task.
         */
        CARAPI_(void) SetNextRunTime();

    public:
        /** The actual task to be re-enqueued by reExecutePeriodic */
        AutoPtr<IRunnableScheduledFuture> mOuterTask;

        /**
         * Index into delay queue, to support faster cancellation.
         */
        Int32 mHeapIndex;

    private:
        /** Sequence number to break ties FIFO */
        Int64 mSequenceNumber;

        /** The time the task is enabled to execute in nanoTime units */
        Int64 mTime;

        /**
         * Period in nanoseconds for repeating tasks.  A positive
         * value indicates fixed-rate execution.  A negative value
         * indicates fixed-delay execution.  A value of 0 indicates a
         * non-repeating task.
         */
        Int64 mPeriod;

        CScheduledThreadPoolExecutor* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Creates a new {@code ScheduledThreadPoolExecutor} with the
     * given core pool size.
     *
     * @param corePoolSize the number of threads to keep in the pool, even
     *        if they are idle, unless {@code allowCoreThreadTimeOut} is set
     * @throws IllegalArgumentException if {@code corePoolSize < 0}
     */
    CARAPI constructor(
        /* [in] */ Int32 corePoolSize);

    /**
     * Creates a new {@code ScheduledThreadPoolExecutor} with the
     * given initial parameters.
     *
     * @param corePoolSize the number of threads to keep in the pool, even
     *        if they are idle, unless {@code allowCoreThreadTimeOut} is set
     * @param threadFactory the factory to use when the executor
     *        creates a new thread
     * @throws IllegalArgumentException if {@code corePoolSize < 0}
     * @throws NullPointerException if {@code threadFactory} is null
     */
    CARAPI constructor(
        /* [in] */ Int32 corePoolSize,
        /* [in] */ IThreadFactory* threadFactory);

    /**
     * Creates a new ScheduledThreadPoolExecutor with the given
     * initial parameters.
     *
     * @param corePoolSize the number of threads to keep in the pool, even
     *        if they are idle, unless {@code allowCoreThreadTimeOut} is set
     * @param handler the handler to use when execution is blocked
     *        because the thread bounds and queue capacities are reached
     * @throws IllegalArgumentException if {@code corePoolSize < 0}
     * @throws NullPointerException if {@code handler} is null
     */
    CARAPI constructor(
        /* [in] */ Int32 corePoolSize,
        /* [in] */ IRejectedExecutionHandler* handler);

    /**
     * Creates a new ScheduledThreadPoolExecutor with the given
     * initial parameters.
     *
     * @param corePoolSize the number of threads to keep in the pool, even
     *        if they are idle, unless {@code allowCoreThreadTimeOut} is set
     * @param threadFactory the factory to use when the executor
     *        creates a new thread
     * @param handler the handler to use when execution is blocked
     *        because the thread bounds and queue capacities are reached
     * @throws IllegalArgumentException if {@code corePoolSize < 0}
     * @throws NullPointerException if {@code threadFactory} or
     *         {@code handler} is null
     */
    CARAPI constructor(
        /* [in] */ Int32 corePoolSize,
        /* [in] */ IThreadFactory* threadFactory,
        /* [in] */ IRejectedExecutionHandler* handler);

    CARAPI Schedule(
        /* [in] */ IRunnable* command,
        /* [in] */ Int64 delay,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ IFuture** result);

    CARAPI Schedule(
        /* [in] */ ICallable* callable,
        /* [in] */ Int64 delay,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ IFuture** result);

    CARAPI ScheduleAtFixedRate(
        /* [in] */ IRunnable* command,
        /* [in] */ Int64 initialDelay,
        /* [in] */ Int64 period,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ IFuture** result);

    CARAPI ScheduleWithFixedDelay(
        /* [in] */ IRunnable* command,
        /* [in] */ Int64 initialDelay,
        /* [in] */ Int64 delay,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ IFuture** result);

    CARAPI Execute(
        /* [in] */ IRunnable* command);

    CARAPI Submit(
        /* [in] */ IRunnable* task,
        /* [out] */ IFuture** future);

    CARAPI Submit(
        /* [in] */ IRunnable* task,
        /* [in] */ IInterface* result,
        /* [out] */ IFuture** future);

    CARAPI Submit(
        /* [in] */ ICallable* task,
        /* [out] */ IFuture** future);

    /**
     * Sets the policy on whether to continue executing existing
     * periodic tasks even when this executor has been {@code shutdown}.
     * In this case, these tasks will only terminate upon
     * {@code shutdownNow} or after setting the policy to
     * {@code false} when already shutdown.
     * This value is by default {@code false}.
     *
     * @param value if {@code true}, continue after shutdown, else don't
     * @see #getContinueExistingPeriodicTasksAfterShutdownPolicy
     */
    CARAPI_(void) SetContinueExistingPeriodicTasksAfterShutdownPolicy(
        /* [in] */ Boolean value);

    /**
     * Gets the policy on whether to continue executing existing
     * periodic tasks even when this executor has been {@code shutdown}.
     * In this case, these tasks will only terminate upon
     * {@code shutdownNow} or after setting the policy to
     * {@code false} when already shutdown.
     * This value is by default {@code false}.
     *
     * @return {@code true} if will continue after shutdown
     * @see #setContinueExistingPeriodicTasksAfterShutdownPolicy
     */
    CARAPI_(Boolean) GetContinueExistingPeriodicTasksAfterShutdownPolicy();

    /**
     * Sets the policy on whether to execute existing delayed
     * tasks even when this executor has been {@code shutdown}.
     * In this case, these tasks will only terminate upon
     * {@code shutdownNow}, or after setting the policy to
     * {@code false} when already shutdown.
     * This value is by default {@code true}.
     *
     * @param value if {@code true}, execute after shutdown, else don't
     * @see #getExecuteExistingDelayedTasksAfterShutdownPolicy
     */
    CARAPI_(void) SetExecuteExistingDelayedTasksAfterShutdownPolicy(
        /* [in] */ Boolean value);

    /**
     * Gets the policy on whether to execute existing delayed
     * tasks even when this executor has been {@code shutdown}.
     * In this case, these tasks will only terminate upon
     * {@code shutdownNow}, or after setting the policy to
     * {@code false} when already shutdown.
     * This value is by default {@code true}.
     *
     * @return {@code true} if will execute after shutdown
     * @see #setExecuteExistingDelayedTasksAfterShutdownPolicy
     */
    CARAPI_(Boolean) GetExecuteExistingDelayedTasksAfterShutdownPolicy();

    /**
     * Sets the policy on whether cancelled tasks should be immediately
     * removed from the work queue at time of cancellation.  This value is
     * by default {@code false}.
     *
     * @param value if {@code true}, remove on cancellation, else don't
     * @see #getRemoveOnCancelPolicy
     * @since 1.7
     */
    CARAPI_(void) SetRemoveOnCancelPolicy(
        /* [in] */ Boolean value);

    /**
     * Gets the policy on whether cancelled tasks should be immediately
     * removed from the work queue at time of cancellation.  This value is
     * by default {@code false}.
     *
     * @return {@code true} if cancelled tasks are immediately removed
     *         from the queue
     * @see #setRemoveOnCancelPolicy
     * @since 1.7
     */
    CARAPI_(Boolean) GetRemoveOnCancelPolicy();

    /**
     * Initiates an orderly shutdown in which previously submitted
     * tasks are executed, but no new tasks will be accepted.
     * Invocation has no additional effect if already shut down.
     *
     * <p>This method does not wait for previously submitted tasks to
     * complete execution.  Use {@link #awaitTermination awaitTermination}
     * to do that.
     *
     * <p>If the {@code ExecuteExistingDelayedTasksAfterShutdownPolicy}
     * has been set {@code false}, existing delayed tasks whose delays
     * have not yet elapsed are cancelled.  And unless the {@code
     * ContinueExistingPeriodicTasksAfterShutdownPolicy} has been set
     * {@code true}, future executions of existing periodic tasks will
     * be cancelled.
     */
    CARAPI Shutdown();

    /**
     * Attempts to stop all actively executing tasks, halts the
     * processing of waiting tasks, and returns a list of the tasks
     * that were awaiting execution.
     *
     * <p>This method does not wait for actively executing tasks to
     * terminate.  Use {@link #awaitTermination awaitTermination} to
     * do that.
     *
     * <p>There are no guarantees beyond best-effort attempts to stop
     * processing actively executing tasks.  This implementation
     * cancels tasks via {@link Thread#interrupt}, so any task that
     * fails to respond to interrupts may never terminate.
     *
     * @return list of tasks that never commenced execution.
     *         Each element of this list is a {@link ScheduledFuture},
     *         including those tasks submitted using {@code execute},
     *         which are for scheduling purposes used as the basis of a
     *         zero-delay {@code ScheduledFuture}.
     */
    CARAPI ShutdownNow(
        /* [out] */ IList** tasks);

    /**
     * Returns the task queue used by this executor.  Each element of
     * this queue is a {@link ScheduledFuture}, including those
     * tasks submitted using {@code execute} which are for scheduling
     * purposes used as the basis of a zero-delay
     * {@code ScheduledFuture}.  Iteration over this queue is
     * <em>not</em> guaranteed to traverse tasks in the order in
     * which they will execute.
     *
     * @return the task queue
     */
    CARAPI GetQueue(
        /* [out] */ IBlockingQueue** queue);

    /**
     * Returns current nanosecond time.
     */
    CARAPI_(Int64) Now();

    /**
     * Returns true if can run a task given current run state
     * and run-after-shutdown parameters.
     *
     * @param periodic true if this task periodic, false if delayed
     */
    CARAPI_(Boolean) CanRunInCurrentRunState(
        /* [in] */ Boolean periodic);

    /**
     * Requeues a periodic task unless current run state precludes it.
     * Same idea as delayedExecute except drops task rather than rejecting.
     *
     * @param task the task
     */
    CARAPI_(void) ReExecutePeriodic(
        /* [in] */ IRunnableScheduledFuture* task);

    /**
     * Cancels and clears the queue of all tasks that should not be run
     * due to shutdown policy.  Invoked within super.shutdown.
     */
    CARAPI_(void) OnShutdown();

    /**
     * Modifies or replaces the task used to execute a runnable.
     * This method can be used to override the concrete
     * class used for managing internal tasks.
     * The default implementation simply returns the given task.
     *
     * @param runnable the submitted Runnable
     * @param task the task created to execute the runnable
     * @return a task that can execute the runnable
     * @since 1.6
     */
    CARAPI_(AutoPtr<IRunnableScheduledFuture>) DecorateTask(
        /* [in] */ IRunnable* runnable,
        /* [in] */ IRunnableScheduledFuture* task);

    /**
     * Modifies or replaces the task used to execute a callable.
     * This method can be used to override the concrete
     * class used for managing internal tasks.
     * The default implementation simply returns the given task.
     *
     * @param callable the submitted Callable
     * @param task the task created to execute the callable
     * @return a task that can execute the callable
     * @since 1.6
     */
    CARAPI_(AutoPtr<IRunnableScheduledFuture>) DecorateTask(
        /* [in] */ ICallable* callable,
        /* [in] */ IRunnableScheduledFuture* task);

    /**
     * Returns the trigger time of a delayed action.
     */
    CARAPI_(Int64) TriggerTime(
        /* [in] */ Int64 delay);

private:
    /**
     * Main execution method for delayed or periodic tasks.  If pool
     * is shut down, rejects the task. Otherwise adds task to queue
     * and starts a thread, if necessary, to run it.  (We cannot
     * prestart the thread to run the task because the task (probably)
     * shouldn't be run yet.)  If the pool is shut down while the task
     * is being added, cancel and remove it if required by state and
     * run-after-shutdown parameters.
     *
     * @param task the task
     */
    CARAPI_(void) DelayedExecute(
        /* [in] */ IRunnableScheduledFuture* task);

    /**
     * Returns the trigger time of a delayed action.
     */
    CARAPI_(Int64) TriggerTime(
        /* [in] */ Int64 delay,
        /* [in] */ ITimeUnit* unit);

    /**
     * Constrains the values of all delays in the queue to be within
     * Long.MAX_VALUE of each other, to avoid overflow in compareTo.
     * This may occur if a task is eligible to be dequeued, but has
     * not yet been, while some other task is added with a delay of
     * Long.MAX_VALUE.
     */
    CARAPI_(Int64) OverflowFree(
        /* [in] */ Int64 delay);

private:
    /**
     * False if should cancel/suppress periodic tasks on shutdown.
     */
    volatile Boolean mContinueExistingPeriodicTasksAfterShutdown;

    /**
     * False if should cancel non-periodic tasks on shutdown.
     */
    volatile Boolean mExecuteExistingDelayedTasksAfterShutdown;

    /**
     * True if ScheduledFutureTask.cancel should remove from queue
     */
    volatile Boolean mRemoveOnCancel;

    /**
     * Sequence number to break scheduling ties, and in turn to
     * guarantee FIFO order among tied entries.
     */
    static const AutoPtr<IAtomicInteger64> mSequencer;
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CSCHEDULEDTHREADPOOLEXECUTOR_H__
