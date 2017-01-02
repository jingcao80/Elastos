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

#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_FORKJOINTASK_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_FORKJOINTASK_H__

#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include <elastos/core/Object.h>

using Elastos::Core::IThrowable;
using Elastos::Core::IRunnable;
using Elastos::Core::Object;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IObjectInputStream;
using Elastos::IO::ISerializable;
using Elastos::Utility::Concurrent::Locks::IReentrantLock;

namespace Elastos {
namespace Utility {
namespace Concurrent {

class ForkJoinTask
    : public Object
    , public IForkJoinTask
    , public ISerializable
    , public IFuture
{
public:
    /**
     * Key-value nodes for exception table.  The chained hash table
     * uses identity comparisons, full locking, and weak references
     * for keys. The table has a fixed capacity because it only
     * maintains task exceptions long enough for joiners to access
     * them, so should never become very large for sustained
     * periods. However, since we do not know when the last joiner
     * completes, we must use weak references and expunge them. We do
     * so on each operation (hence full locking). Also, some thread in
     * any ForkJoinPool will call helpExpungeStaleExceptions when its
     * pool becomes isQuiescent.
     */
    class ExceptionNode// extends WeakReference
        : public Object
    {
    public:
        ExceptionNode(
            /* [in] */ IForkJoinTask* task,
            /* [in] */ IThrowable* ex,
            /* [in] */ ExceptionNode* next);

    public:
        AutoPtr<IThrowable> mEx;
        AutoPtr<ExceptionNode> mNext;
        Int64 mThrower;  // use id not ref to avoid weak cycles
    };

public:
    CAR_INTERFACE_DECL()

    virtual CARAPI SetRawResult(
        /* [in] */ IInterface* value) = 0;

    virtual CARAPI Exec(
        /* [out] */ Boolean* res) = 0;

    virtual CARAPI GetRawResult(
        /* [out] */ IInterface** outface) = 0;

    virtual CARAPI DoExec(
        /* [out] */ Int32* result);

    // public methods

    virtual CARAPI Fork(
        /* [out] */ IForkJoinTask** outtask);

    virtual CARAPI Join(
        /* [out] */ IInterface** outface);

    virtual CARAPI Invoke(
        /* [out] */ IInterface** outface);

    virtual CARAPI Cancel(
        /* [in] */ Boolean mayInterruptIfRunning,
        /* [out] */ Boolean* res);

    virtual CARAPI IsDone(
        /* [out] */ Boolean* res);

    virtual CARAPI IsCancelled(
        /* [out] */ Boolean* res);

    virtual CARAPI IsCompletedAbnormally(
        /* [out] */ Boolean* value);

    virtual CARAPI IsCompletedNormally(
        /* [out] */ Boolean* value);

    virtual CARAPI GetException(
        /* [out] */ IThrowable** res);

    virtual CARAPI CompleteExceptionally(
        /* [in] */ IThrowable* ex);

    virtual CARAPI Complete(
        /* [in] */ IInterface* value);

    virtual CARAPI Get(
        /* [out] */ IInterface** res);

    virtual CARAPI Get(
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ IInterface** res);

    virtual CARAPI QuietlyJoin();

    virtual CARAPI QuietlyInvoke();

    virtual CARAPI Reinitialize();

    virtual CARAPI TryUnfork(
        /* [out] */ Boolean* value);

    virtual CARAPI QuietlyComplete();

    static CARAPI_(void) InvokeAll(
        /* [in] */ IForkJoinTask* t1,
        /* [in] */ IForkJoinTask* t2);

    static CARAPI_(void) InvokeAll(
        /* [in] */ ArrayOf<IForkJoinTask*>* tasks);

    static CARAPI_(AutoPtr<ICollection>) InvokeAll(
        /* [in] */ ICollection* tasks);

    static CARAPI_(void) HelpQuiesce();

    static CARAPI_(AutoPtr<IForkJoinPool>) GetPool();

    static CARAPI_(Boolean) InForkJoinPool();

    static CARAPI_(Int32) GetQueuedTaskCount();

    static CARAPI_(Int32) GetSurplusQueuedTaskCount();

    static CARAPI_(AutoPtr<IForkJoinTask>) Adapt(
        /* [in] */ IRunnable* runnable);

    static CARAPI_(AutoPtr<IForkJoinTask>) Adapt(
        /* [in] */ IRunnable* runnable,
        /* [in] */ IInterface* result);

    static CARAPI_(AutoPtr<IForkJoinTask>) Adapt(
        /* [in] */ ICallable* callable);

    /**
     * If lock is available, poll stale refs and remove them.
     * Called from ForkJoinPool when pools become quiescent.
     */
    static CARAPI_(void) HelpExpungeStaleExceptions();

    /**
     * Returns, but does not unschedule or execute, a task queued by
     * the current thread but not yet executed, if one is immediately
     * available. There is no guarantee that this task will actually
     * be polled or executed next. Conversely, this method may return
     * null even if a task exists but cannot be accessed without
     * contention with other threads.  This method is designed
     * primarily to support extensions, and is unlikely to be useful
     * otherwise.
     *
     * <p>This method may be invoked only from within {@code
     * ForkJoinPool} computations (as may be determined using method
     * {@link #inForkJoinPool}).  Attempts to invoke in other contexts
     * result in exceptions or errors, possibly including {@code
     * ClassCastException}.
     *
     * @return the next task, or {@code null} if none are available
     */
    static CARAPI_(AutoPtr<IForkJoinTask>) PeekNextLocalTask();

    /**
     * Unschedules and returns, without executing, the next task
     * queued by the current thread but not yet executed.  This method
     * is designed primarily to support extensions, and is unlikely to
     * be useful otherwise.
     *
     * <p>This method may be invoked only from within {@code
     * ForkJoinPool} computations (as may be determined using method
     * {@link #inForkJoinPool}).  Attempts to invoke in other contexts
     * result in exceptions or errors, possibly including {@code
     * ClassCastException}.
     *
     * @return the next task, or {@code null} if none are available
     */
    static CARAPI_(AutoPtr<IForkJoinTask>) PollNextLocalTask();

    /**
     * Unschedules and returns, without executing, the next task
     * queued by the current thread but not yet executed, if one is
     * available, or if not available, a task that was forked by some
     * other thread, if available. Availability may be transient, so a
     * {@code null} result does not necessarily imply quiescence
     * of the pool this task is operating in.  This method is designed
     * primarily to support extensions, and is unlikely to be useful
     * otherwise.
     *
     * <p>This method may be invoked only from within {@code
     * ForkJoinPool} computations (as may be determined using method
     * {@link #inForkJoinPool}).  Attempts to invoke in other contexts
     * result in exceptions or errors, possibly including {@code
     * ClassCastException}.
     *
     * @return a task, or {@code null} if none are available
     */
    static CARAPI_(AutoPtr<IForkJoinTask>) PollTask();

    /**
     * Tries to set SIGNAL status unless already completed. Used by
     * ForkJoinPool. Other variants are directly incorporated into
     * externalAwaitDone etc.
     *
     * @return true if successful
     */
    CARAPI_(Boolean) TrySetSignal();

    /**
     * Records exception and sets status.
     *
     * @return status on exit
     */
    CARAPI_(Int32) RecordExceptionalCompletion(
        /* [in] */ IThrowable* ex);

    /**
     * Hook for exception propagation support for tasks with completers.
     */
    CARAPI_(void) InternalPropagateException(
        /* [in] */ IThrowable* ex);

    /**
     * Cancels, ignoring any exceptions thrown by cancel. Used during
     * worker and pool shutdown. Cancel is spec'ed not to throw any
     * exceptions, but if it does anyway, we have no recourse during
     * shutdown, so guard against this case.
     */
    static CARAPI_(void) CancelIgnoringExceptions(
        /* [in] */ ForkJoinTask* t);

    /**
     * A version of "sneaky throw" to relay exceptions
     */
    static CARAPI_(void) Rethrow(
        /* [in] */ IThrowable* ex);

    /**
     * The sneaky part of sneaky throw, relying on generics
     * limitations to evade compiler complaints about rethrowing
     * unchecked exceptions
     */
    static CARAPI_(void) UncheckedThrow(
        /* [in] */ IThrowable* t);

    // tag operations

    /**
     * Returns the tag for this task.
     *
     * @return the tag for this task
     * @since 1.8
     * @hide
     */
    CARAPI_(Int16) GetForkJoinTaskTag();

    /**
     * Atomically sets the tag value for this task.
     *
     * @param tag the tag value
     * @return the previous value of the tag
     * @since 1.8
     * @hide
     */
    CARAPI_(Int16) SetForkJoinTaskTag(
        /* [in] */ Int16 tag);

    /**
     * Atomically conditionally sets the tag value for this task.
     * Among other applications, tags can be used as visit markers
     * in tasks operating on graphs, as in methods that check: {@code
     * if (task.compareAndSetForkJoinTaskTag((short)0, (short)1))}
     * before processing, otherwise exiting because the node has
     * already been visited.
     *
     * @param e the expected tag value
     * @param tag the new tag value
     * @return {@code true} if successful; i.e., the current value was
     * equal to e and is now tag.
     * @since 1.8
     * @hide
     */
    CARAPI_(Boolean) CompareAndSetForkJoinTaskTag(
        /* [in] */ Int16 e,
        /* [in] */ Int16 tag);

private:
    /**
     * Marks completion and wakes up threads waiting to join this task,
     * also clearing signal request bits.
     *
     * @param completion one of NORMAL, CANCELLED, EXCEPTIONAL
     * @return completion status on exit
     */
    CARAPI_(Int32) SetCompletion(
        /* [in] */ const Int32& completion);

    /**
     * Blocks a non-worker-thread until completion.
     * @return status upon completion
     */
    CARAPI_(Int32) ExternalAwaitDone();

    /**
     * Blocks a non-worker-thread until completion or interruption or timeout.
     */
    CARAPI_(Int32) ExternalInterruptibleAwaitDone();

    /**
     * Primary mechanics for join, get, quietlyJoin.
     * @return status upon completion
     */
    CARAPI_(Int32) DoJoin();

    /**
     * Primary mechanics for invoke, quietlyInvoke.
     * @return status upon completion
     */
    CARAPI_(Int32) DoInvoke();

    /**
     * Records exception and sets exceptional completion.
     *
     * @return status on exit
     */
    CARAPI_(Int32) SetExceptionalCompletion(
        /* [in] */ IThrowable* ex);

    /**
     * Removes exception node and clears status
     */
    CARAPI_(void) ClearExceptionalCompletion();

    /**
     * Returns a rethrowable exception for the given task, if
     * available. To provide accurate stack traces, if the exception
     * was not thrown by the current thread, we try to create a new
     * exception of the same type as the one thrown, but with the
     * recorded exception as its cause. If there is no such
     * constructor, we instead try to use a no-arg constructor,
     * followed by initCause, to the same effect. If none of these
     * apply, or any fail due to other exceptions, we return the
     * recorded exception, which is still correct, although it may
     * contain a misleading stack trace.
     *
     * @return the exception, or null if none
     */
    CARAPI_(AutoPtr<IThrowable>) GetThrowableException();

    /**
     * Poll stale refs and remove them. Call only while holding lock.
     */
    static CARAPI_(void) ExpungeStaleExceptions();

    /**
     * Report the result of invoke or join; called only upon
     * non-normal return of internal versions.
     */
    CARAPI_(AutoPtr<IInterface>) ReportResult();

    /**
     * Throws exception, if any, associated with the given status.
     */
    CARAPI_(void) ReportException(
        /* [in] */ Int32 s);

    /**
     * Saves the state to a stream (that is, serializes it).
     *
     * @serialData the current run status and the exception thrown
     * during execution, or {@code null} if none
     * @param s the stream
     */
    CARAPI_(void) WriteObject(
        /* [in] */ IObjectOutputStream* s);

    /**
     * Reconstitutes the instance from a stream (that is, deserializes it).
     *
     * @param s the stream
     */
    CARAPI_(void) ReadObject(
        /* [in] */ IObjectInputStream* s);

public:
    /** The run status of this task */
    volatile Int32 mStatus; // accessed directly by pool and workers

public:
    static Int32 DONE_MASK;
    static Int32 NORMAL;
    static Int32 CANCELLED;
    static Int32 EXCEPTIONAL;
    static Int32 SIGNAL;
    static Int32 SMASK;

    // Exception table support

    /**
     * Table of exceptions thrown by tasks, to enable reporting by
     * callers. Because exceptions are rare, we don't directly keep
     * them with task objects, but instead use a weak ref table.  Note
     * that cancellation exceptions don't appear in the table, but are
     * instead recorded as status values.
     *
     * Note: These statics are initialized below in static block.
     */
    static AutoPtr<ArrayOf<ExceptionNode*> > sExceptionTable;
    static AutoPtr<IReentrantLock> sExceptionTableLock;
//    static AutoPtr<IReferenceQueue> sExceptionTableRefQueue;

    /**
     * Fixed capacity for exceptionTable.
     */
    static Int32 EXCEPTION_MAP_CAPACITY;

    // Serialization support

    static Int64 sSerialVersionUID;

    // // Unsafe mechanics
    // private static final sun.misc.Unsafe U;
    // private static final long STATUS;

    // static {
    //     exceptionTableLock = new ReentrantLock();
    //     exceptionTableRefQueue = new ReferenceQueue<Object>();
    //     exceptionTable = new ExceptionNode[EXCEPTION_MAP_CAPACITY];
    //     try {
    //         U = sun.misc.Unsafe.getUnsafe();
    //         Class<?> k = ForkJoinTask.class;
    //         STATUS = U.objectFieldOffset
    //             (k.getDeclaredField("status"));
    //     } catch (Exception e) {
    //         throw new Error(e);
    //     }
    // }
};

/**
 * Adaptor for Runnables. This implements RunnableFuture
 * to be compliant with AbstractExecutorService constraints
 * when used in ForkJoinPool.
 */
class AdaptedRunnable
    : public ForkJoinTask
    , public IRunnableFuture
{
public:
    CAR_INTERFACE_DECL()

    AdaptedRunnable(
        /* [in] */ IRunnable* runnable,
        /* [in] */ IInterface* result);

    CARAPI GetRawResult(
        /* [out] */ IInterface** outface);

    CARAPI SetRawResult(
        /* [in] */ IInterface* v);

    CARAPI Exec(
        /* [out] */ Boolean* res);

    CARAPI_(void) Run();

public:
    AutoPtr<IRunnable> mRunnable;
    AutoPtr<IInterface> mResult;
//        static Int64 serialVersionUID = 5232453952276885070L;
};

/**
 * Adaptor for Runnables without results
 */
class AdaptedRunnableAction
    : public ForkJoinTask
    , public IRunnableFuture
{
public:
    CAR_INTERFACE_DECL()

    AdaptedRunnableAction(
        /* [in] */ IRunnable* runnable);

    CARAPI GetRawResult(
        /* [out] */ IInterface** outface);

    CARAPI SetRawResult(
        /* [in] */ IInterface* v);

    CARAPI Exec(
        /* [out] */ Boolean* res);

    CARAPI_(void) Run();

public:
    AutoPtr<IRunnable> mRunnable;
//    private static final long serialVersionUID = 5232453952276885070L;
};

/**
 * Adaptor for Runnables in which failure forces worker exception
 */
class RunnableExecuteAction
    : public ForkJoinTask
{
public:
    RunnableExecuteAction(
        /* [in] */ IRunnable* runnable);

    CARAPI GetRawResult(
        /* [out] */ IInterface** outface);

    CARAPI SetRawResult(
        /* [in] */ IInterface* v);

    CARAPI Exec(
        /* [out] */ Boolean* res);

    CARAPI_(void) InternalPropagateException(
        /* [in] */ IThrowable* ex);

public:
    AutoPtr<IRunnable> mRunnable;
//    private static final long serialVersionUID = 5232453952276885070L;
};

/**
 * Adaptor for Callables
 */
class AdaptedCallable
    : public ForkJoinTask
{
public:
    AdaptedCallable(
        /* [in] */ ICallable* callable);

    CARAPI GetRawResult(
        /* [out] */ IInterface** outface);

    CARAPI SetRawResult(
        /* [in] */ IInterface* v);

    CARAPI Exec(
        /* [out] */ Boolean* res);

    CARAPI_(void) Run();

public:
    AutoPtr<ICallable> mCallable;
    AutoPtr<IInterface> mResult;
//        static Int64 serialVersionUID = 2838392045355241008L;
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Utility::Concurrent::ForkJoinTask::ExceptionNode, IInterface)

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_FORKJOINTASK_H__
