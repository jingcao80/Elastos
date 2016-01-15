
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_COUNTEDCOMPLETER_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_COUNTEDCOMPLETER_H__

#include "ForkJoinTask.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

class CountedCompleter
    : public ForkJoinTask
    , public ICountedCompleter
{
public:
    CAR_INTERFACE_DECL()

    /**
     * The main computation performed by this task.
     */
    virtual CARAPI Compute() = 0;

    /**
     * Performs an action when method {@link #tryComplete} is invoked
     * and the pending count is zero, or when the unconditional
     * method {@link #complete} is invoked.  By default, this method
     * does nothing. You can distinguish cases by checking the
     * identity of the given caller argument. If not equal to {@code
     * this}, then it is typically a subtask that may contain results
     * (and/or links to other results) to combine.
     *
     * @param caller the task invoking this method (which may
     * be this task itself)
     */
    virtual CARAPI OnCompletion(
        /* [in] */ ICountedCompleter* caller);

    /**
     * Performs an action when method {@link
     * #completeExceptionally(Throwable)} is invoked or method {@link
     * #compute} throws an exception, and this task has not already
     * otherwise completed normally. On entry to this method, this task
     * {@link ForkJoinTask#isCompletedAbnormally}.  The return value
     * of this method controls further propagation: If {@code true}
     * and this task has a completer that has not completed, then that
     * completer is also completed exceptionally, with the same
     * exception as this completer.  The default implementation of
     * this method does nothing except return {@code true}.
     *
     * @param ex the exception
     * @param caller the task invoking this method (which may
     * be this task itself)
     * @return {@code true} if this exception should be propagated to this
     * task's completer, if one exists
     */
    virtual CARAPI OnExceptionalCompletion(
        /* [in] */ IThrowable* ex,
        /* [in] */ ICountedCompleter* caller,
        /* [out] */ Boolean* res);

    /**
     * Returns the completer established in this task's constructor,
     * or {@code null} if none.
     *
     * @return the completer
     */
    virtual CARAPI GetCompleter(
        /* [out] */ ICountedCompleter** res);

    /**
     * Returns the current pending count.
     *
     * @return the current pending count
     */
    virtual CARAPI GetPendingCount(
        /* [out] */ Int32* res);

    /**
     * Sets the pending count to the given value.
     *
     * @param count the count
     */
    virtual CARAPI SetPendingCount(
        /* [in] */ Int32 count);

    /**
     * Adds (atomically) the given value to the pending count.
     *
     * @param delta the value to add
     */
    virtual CARAPI AddToPendingCount(
        /* [in] */ Int32 delta);

    /**
     * Sets (atomically) the pending count to the given count only if
     * it currently holds the given expected value.
     *
     * @param expected the expected value
     * @param count the new value
     * @return {@code true} if successful
     */
    virtual CARAPI CompareAndSetPendingCount(
        /* [in] */ Int32 expected,
        /* [in] */ Int32 count,
        /* [out] */ Boolean* res);

    /**
     * If the pending count is nonzero, (atomically) decrements it.
     *
     * @return the initial (undecremented) pending count holding on entry
     * to this method
     */
    virtual CARAPI DecrementPendingCountUnlessZero(
        /* [out] */ Int32* res);

    /**
     * Returns the root of the current computation; i.e., this
     * task if it has no completer, else its completer's root.
     *
     * @return the root of the current computation
     */
    virtual CARAPI GetRoot(
        /* [out] */ ICountedCompleter** res);

    /**
     * If the pending count is nonzero, decrements the count;
     * otherwise invokes {@link #onCompletion(CountedCompleter)}
     * and then similarly tries to complete this task's completer,
     * if one exists, else marks this task as complete.
     */
    virtual CARAPI TryComplete();

    /**
     * Equivalent to {@link #tryComplete} but does not invoke {@link
     * #onCompletion(CountedCompleter)} along the completion path:
     * If the pending count is nonzero, decrements the count;
     * otherwise, similarly tries to complete this task's completer, if
     * one exists, else marks this task as complete. This method may be
     * useful in cases where {@code onCompletion} should not, or need
     * not, be invoked for each completer in a computation.
     */
    virtual CARAPI PropagateCompletion();

    /**
     * Regardless of pending count, invokes
     * {@link #onCompletion(CountedCompleter)}, marks this task as
     * complete and further triggers {@link #tryComplete} on this
     * task's completer, if one exists.  The given rawResult is
     * used as an argument to {@link #setRawResult} before invoking
     * {@link #onCompletion(CountedCompleter)} or marking this task
     * as complete; its value is meaningful only for classes
     * overriding {@code setRawResult}.  This method does not modify
     * the pending count.
     *
     * <p>This method may be useful when forcing completion as soon as
     * any one (versus all) of several subtask results are obtained.
     * However, in the common (and recommended) case in which {@code
     * setRawResult} is not overridden, this effect can be obtained
     * more simply using {@code quietlyCompleteRoot();}.
     *
     * @param rawResult the raw result
     */
    virtual CARAPI Complete(
        /* [in] */ IInterface* value);

    /**
     * If this task's pending count is zero, returns this task;
     * otherwise decrements its pending count and returns {@code
     * null}. This method is designed to be used with {@link
     * #nextComplete} in completion traversal loops.
     *
     * @return this task, if pending count was zero, else {@code null}
     */
    virtual CARAPI FirstComplete(
        /* [out] */ ICountedCompleter** res);

    /**
     * If this task does not have a completer, invokes {@link
     * ForkJoinTask#quietlyComplete} and returns {@code null}.  Or, if
     * the completer's pending count is non-zero, decrements that
     * pending count and returns {@code null}.  Otherwise, returns the
     * completer.  This method can be used as part of a completion
     * traversal loop for homogeneous task hierarchies:
     *
     * <pre> {@code
     * for (CountedCompleter<?> c = firstComplete();
     *      c != null;
     *      c = c.nextComplete()) {
     *   // ... process c ...
     * }}</pre>
     *
     * @return the completer, or {@code null} if none
     */
    virtual CARAPI NextComplete(
        /* [out] */ ICountedCompleter** res);

    /**
     * Equivalent to {@code getRoot().quietlyComplete()}.
     */
    virtual CARAPI QuietlyCompleteRoot();

    /**
     * Returns the result of the computation. By default
     * returns {@code null}, which is appropriate for {@code Void}
     * actions, but in other cases should be overridden, almost
     * always to return a field or function of a field that
     * holds the result upon completion.
     *
     * @return the result of the computation
     */
    virtual CARAPI GetRawResult(
        /* [out] */ IInterface** outface);

    /**
     * Supports ForkJoinTask exception propagation.
     */
    virtual CARAPI_(void) InternalPropagateException(
        /* [in] */ IThrowable* ex);

    /**
     * Creates a new CountedCompleter with the given completer
     * and initial pending count.
     *
     * @param completer this task's completer, or {@code null} if none
     * @param initialPendingCount the initial pending count
     */
    CountedCompleter(
        /* [in] */ ICountedCompleter* completer,
        /* [in] */ Int32 initialPendingCount);

    /**
     * Creates a new CountedCompleter with the given completer
     * and an initial pending count of zero.
     *
     * @param completer this task's completer, or {@code null} if none
     */
    CountedCompleter(
        /* [in] */ ICountedCompleter* completer);

    /**
     * Creates a new CountedCompleter with no completer
     * and an initial pending count of zero.
     */
    CountedCompleter();

    virtual CARAPI Exec(
        /* [out] */ Boolean* res);

    virtual CARAPI SetRawResult(
        /* [in] */ IInterface* value);

public:
//    private static final long serialVersionUID = 5232453752276485070L;

    /** This task's completer, or null if none */
    AutoPtr<ICountedCompleter> mCompleter;
    /** The number of pending tasks until completion */
    volatile Int32 mPending;

    // // Unsafe mechanics
    // private static final sun.misc.Unsafe U;
    // private static final long PENDING;
    // static {
    //     try {
    //         U = sun.misc.Unsafe.getUnsafe();
    //         PENDING = U.objectFieldOffset
    //             (CountedCompleter.class.getDeclaredField("pending"));
    //     } catch (Exception e) {
    //         throw new Error(e);
    //     }
    // }
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_COUNTEDCOMPLETER_H__
