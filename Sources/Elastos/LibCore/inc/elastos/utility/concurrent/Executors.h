
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_EXECUTORS_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_EXECUTORS_H__

#include "Object.h"
#include "AbstractExecutorService.h"

using Elastos::Core::IRunnable;
using Elastos::Core::IThread;
using Elastos::Core::IThreadGroup;
using Elastos::Core::IClassLoader;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Security::IAccessControlContext;
using Elastos::Security::IPrivilegedAction;
using Elastos::Security::IPrivilegedExceptionAction;

namespace Elastos {
namespace Utility {
namespace Concurrent {

class Executors
    : public Object
{
public:
    /**
     * A callable that runs given task and returns given result
     */
    class RunnableAdapter
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        RunnableAdapter(
            /* [in] */ IRunnable* task,
            /* [in] */ IInterface* result)
            : mTask(task)
            , mResult(result)
        {}

        CARAPI Call(
            /* [out] */ IInterface** result);

    public:
        AutoPtr<IRunnable> mTask;
        AutoPtr<IInterface> mResult;
    };

    /**
     * The default thread factory
     */
    class DefaultThreadFactory
        : public Object
        , public IThreadFactory
    {
    public:
        CAR_INTERFACE_DECL()

        DefaultThreadFactory();

        CARAPI NewThread(
            /* [in] */ IRunnable* r,
            /* [out] */ IThread** thread);

    private:
        static const AutoPtr<IAtomicInteger32> sPoolNumber;
        AutoPtr<IThreadGroup> mGroup;
        const AutoPtr<IAtomicInteger32> mThreadNumber;
        String mNamePrefix;
    };

    /**
     * A callable that runs under established access control settings
     */
    class _PrivilegedCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        _PrivilegedCallable(
            /* [in] */ ICallable* task);

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        AutoPtr<ICallable> mTask;
        AutoPtr<IAccessControlContext> mAcc;
    };

    /**
     * A callable that runs under established access control settings and
     * current ClassLoader
     */
    class _PrivilegedCallableUsingCurrentClassLoader
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        _PrivilegedCallableUsingCurrentClassLoader(
            /* [in] */ ICallable* task);

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        AutoPtr<ICallable> mTask;
        AutoPtr<IAccessControlContext> mAcc;
        AutoPtr<IClassLoader> mCcl;
    };

    /**
     * Thread factory capturing access control context and class loader
     */
    class _PrivilegedThreadFactory
        : public DefaultThreadFactory
    {
    public:
        _PrivilegedThreadFactory();

        CARAPI NewThread(
            /* [in] */ IRunnable* r,
            /* [out] */ IThread** thread);

    private:
        AutoPtr<IAccessControlContext> mAcc;
        AutoPtr<IClassLoader> mCcl;
    };

    /**
     * A wrapper class that exposes only the ExecutorService methods
     * of an ExecutorService implementation.
     */
    class DelegatedExecutorService
        : public AbstractExecutorService
    {
    public:
        DelegatedExecutorService(
            /* [in] */ IExecutorService* executor);

        CARAPI Execute(
            /* [in] */ IRunnable* command);

        CARAPI_(void) Shutdown();

        CARAPI_(AutoPtr<IList>) ShutdownNow();

        CARAPI_(Boolean) IsShutdown();

        CARAPI_(Boolean) IsTerminated();

        CARAPI_(Boolean) AwaitTermination(
            /* [in] */ Int64 timeout,
            /* [in] */ ITimeUnit* unit);

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

        CARAPI InvokeAll(
            /* [in] */ ICollection* tasks,
            /* [out] */ IList** futures);

        CARAPI InvokeAll(
            /* [in] */ ICollection* tasks,
            /* [in] */ Int64 timeout,
            /* [in] */ ITimeUnit* unit,
            /* [out] */ IList** futures);

        CARAPI InvokeAny(
            /* [in] */ ICollection* tasks,
            /* [out] */ IInterface** result);

        CARAPI InvokeAny(
            /* [in] */ ICollection* tasks,
            /* [in] */ Int64 timeout,
            /* [in] */ ITimeUnit* unit,
            /* [out] */ IInterface** result);

    private:
        AutoPtr<IExecutorService> mE;
    };

    class FinalizableDelegatedExecutorService
        : public DelegatedExecutorService
    {
    public:
        FinalizableDelegatedExecutorService(
            /* [in] */ IExecutorService* executor);

        CARAPI_(void) Finalize();
    };

    /**
     * A wrapper class that exposes only the ScheduledExecutorService
     * methods of a ScheduledExecutorService implementation.
     */
    class DelegatedScheduledExecutorService
        : public DelegatedExecutorService
        , public IScheduledExecutorService
    {
    public:
        CAR_INTERFACE_DECL()

        DelegatedScheduledExecutorService(
            /* [in] */ IScheduledExecutorService* executor);

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
            /* [in] */  IRunnable* command,
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

    private:
        AutoPtr<IScheduledExecutorService> mE;
    };

    class CallableObject_1
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        CallableObject_1(
            /* [in] */ IPrivilegedAction* act);

        CARAPI Call(
            /* [out] */ IInterface** result);

    public:
        AutoPtr<IPrivilegedAction> mAction;
    };

    class CallableObject_2
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        CallableObject_2(
            /* [in] */ IPrivilegedExceptionAction* act);

        CARAPI Call(
            /* [out] */ IInterface** result);

    public:
        AutoPtr<IPrivilegedExceptionAction> mAction;
    };

public:
    /**
     * Returns a default thread factory used to create new threads.
     * This factory creates all new threads used by an Executor in the
     * same {@link ThreadGroup}. Each new
     * thread is created as a non-daemon thread with priority set to
     * the smaller of <tt>Thread.NORM_PRIORITY</tt> and the maximum
     * priority permitted in the thread group.  New threads have names
     * accessible via {@link Thread#getName} of
     * <em>pool-N-thread-M</em>, where <em>N</em> is the sequence
     * number of this factory, and <em>M</em> is the sequence number
     * of the thread created by this factory.
     * @return a thread factory
     */
    static CARAPI_(AutoPtr<IThreadFactory>) GetDefaultThreadFactory();

    /**
     * Returns a {@link Callable} object that, when
     * called, runs the given task and returns the given result.  This
     * can be useful when applying methods requiring a
     * <tt>Callable</tt> to an otherwise resultless action.
     * @param task the task to run
     * @param result the result to return
     * @return a callable object
     * @throws NullPointerException if task null
     */
    static CARAPI Callable(
        /* [in] */ IRunnable* task,
        /* [in] */ IInterface* result,
        /* [out] */ ICallable** object);

    static CARAPI NewFixedThreadPool(
        /* [in] */ Int32 nThreads,
        /* [out] */ IExecutorService** result);

    static CARAPI NewWorkStealingPool(
        /* [in] */ Int32 parallelism,
        /* [out] */ IExecutorService** result);

    static CARAPI NewWorkStealingPool(
        /* [out] */ IExecutorService** result);

    static CARAPI NewFixedThreadPool(
        /* [in] */ Int32 nThreads,
        /* [in] */ IThreadFactory* threadFactory,
        /* [out] */ IExecutorService** result);

    static CARAPI NewSingleThreadExecutor(
        /* [out] */ IExecutorService** result);

    static CARAPI NewSingleThreadExecutor(
        /* [in] */ IThreadFactory* threadFactory,
        /* [out] */ IExecutorService** result);

    static CARAPI NewCachedThreadPool(
        /* [out] */ IExecutorService** result);

    static CARAPI NewCachedThreadPool(
        /* [in] */ IThreadFactory* threadFactory,
        /* [out] */ IExecutorService** result);

    static CARAPI NewSingleThreadScheduledExecutor(
        /* [out] */ IScheduledExecutorService** result);

    static CARAPI NewSingleThreadScheduledExecutor(
        /* [in] */ IThreadFactory* threadFactory,
        /* [out] */ IScheduledExecutorService** result);

    static CARAPI NewScheduledThreadPool(
        /* [in] */ Int32 corePoolSize,
        /* [out] */ IScheduledExecutorService** result);

    static CARAPI NewScheduledThreadPool(
        /* [in] */ Int32 corePoolSize,
        /* [in] */ IThreadFactory* threadFactory,
        /* [out] */ IScheduledExecutorService** result);

    static CARAPI UnconfigurableExecutorService(
        /* [in] */ IExecutorService* executor,
        /* [out] */ IExecutorService** result);

    static CARAPI UnconfigurableScheduledExecutorService(
        /* [in] */ IScheduledExecutorService* executor,
        /* [out] */ IScheduledExecutorService** result);

    static CARAPI PrivilegedThreadFactory(
        /* [out] */ IThreadFactory** result);

    static CARAPI Callable(
        /* [in] */ IRunnable* task,
        /* [out] */ ICallable** result);

    static CARAPI Callable(
        /* [in] */ IPrivilegedAction* action,
        /* [out] */ ICallable** result);

    static CARAPI Callable(
        /* [in] */ IPrivilegedExceptionAction* action,
        /* [out] */ ICallable** result);

    static CARAPI PrivilegedCallable(
        /* [in] */ ICallable* callable,
        /* [out] */ ICallable** result);

    static CARAPI PrivilegedCallableUsingCurrentClassLoader(
        /* [in] */ ICallable* callable,
        /* [out] */ ICallable** result);

private:
    /** Cannot instantiate. */
    Executors() {}
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_EXECUTORS_H__
