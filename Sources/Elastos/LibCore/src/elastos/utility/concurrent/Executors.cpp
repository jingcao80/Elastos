
#include "Elastos.CoreLibrary.Security.h"
#include "Executors.h"
#include "Thread.h"
#include "StringBuilder.h"
#include "StringUtils.h"
#include "CThread.h"
#include "CAtomicInteger32.h"
#include "CForkJoinPool.h"
#include "CLinkedBlockingQueue.h"
#include "CThreadPoolExecutor.h"
#include "TimeUnit.h"
#include "CScheduledThreadPoolExecutor.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CThread;
using Elastos::Core::Thread;
using Elastos::Utility::Concurrent::CForkJoinPool;
using Elastos::Utility::Concurrent::CLinkedBlockingQueue;
using Elastos::Utility::Concurrent::CThreadPoolExecutor;
using Elastos::Utility::Concurrent::CScheduledThreadPoolExecutor;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;

namespace Elastos {
namespace Utility {
namespace Concurrent {

//==============================================
//    Executors::RunnableAdapter
//===============================================
CAR_INTERFACE_IMPL(Executors::RunnableAdapter, Object, ICallable)

ECode Executors::RunnableAdapter::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    ECode ec = mTask->Run();
    *result = mResult;
    REFCOUNT_ADD(*result);
    return ec;
}

//==============================================
//    Executors::DefaultThreadFactory
//===============================================
static AutoPtr<IAtomicInteger32> InitsPoolNumber()
{
    AutoPtr<IAtomicInteger32> ai;
    CAtomicInteger32::New(1, (IAtomicInteger32**)&ai);
    return ai;
}

const AutoPtr<IAtomicInteger32> Executors::DefaultThreadFactory::sPoolNumber = InitsPoolNumber();

CAR_INTERFACE_IMPL(Executors::DefaultThreadFactory, Object, IThreadFactory)

Executors::DefaultThreadFactory::DefaultThreadFactory()
{
    CAtomicInteger32::New(1, (IAtomicInteger32**)&mThreadNumber);
    // SecurityManager s = System.getSecurityManager();
    // group = (s != null) ? s.getThreadGroup() :
    //                       Thread.currentThread().getThreadGroup();
    Thread::GetCurrentThread()->GetThreadGroup((IThreadGroup**)&mGroup);
    Int32 c;
    sPoolNumber->GetAndIncrement(&c);
    StringBuilder sb;
    sb += "pool-";
    sb += c;
    sb += "-thread-";
    mNamePrefix = sb.ToString();
}

ECode Executors::DefaultThreadFactory::NewThread(
    /* [in] */ IRunnable* r,
    /* [out] */ IThread** thread)
{
    VALIDATE_NOT_NULL(thread);
    Int32 c;
    mThreadNumber->GetAndIncrement(&c);
    FAIL_RETURN(CThread::New(mGroup, r,
            mNamePrefix + StringUtils::ToString(c), 0, thread));
    Boolean isDaemon;
    if ((*thread)->IsDaemon(&isDaemon), isDaemon) {
        (*thread)->SetDaemon(FALSE);
    }
    Int32 priority;
    if ((*thread)->GetPriority(&priority), priority != IThread::NORM_PRIORITY) {
        (*thread)->SetPriority(IThread::NORM_PRIORITY);
    }
    return NOERROR;
}

//==============================================
//    Executors::_PrivilegedCallable
//===============================================
CAR_INTERFACE_IMPL(Executors::_PrivilegedCallable, Object, ICallable)

Executors::_PrivilegedCallable::_PrivilegedCallable(
    /* [in] */ ICallable* task)
{
    mTask = task;
//    this.acc = AccessController.getContext();
}

ECode Executors::_PrivilegedCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // try {
    //     return AccessController.doPrivileged(
    //         new PrivilegedExceptionAction<T>() {
    //             public T run() throws Exception {
    //                 return task.call();
    //             }
    //         }, acc);
    // } catch (PrivilegedActionException e) {
    //     throw e.getException();
    // }
    return NOERROR;
}

//==============================================
//    Executors::_PrivilegedCallableUsingCurrentClassLoader::
//===============================================
CAR_INTERFACE_IMPL(Executors::_PrivilegedCallableUsingCurrentClassLoader, Object, ICallable)

Executors::_PrivilegedCallableUsingCurrentClassLoader::_PrivilegedCallableUsingCurrentClassLoader(
    /* [in] */ ICallable* task)
{
    assert(0 && "TODO");
    // BEGIN android-removed
    // SecurityManager sm = System.getSecurityManager();
    // if (sm != null) {
    //     // Calls to getContextClassLoader from this class
    //     // never trigger a security check, but we check
    //     // whether our callers have this permission anyways.
    //     sm.checkPermission(SecurityConstants.GET_CLASSLOADER_PERMISSION);
    //
    //     // Whether setContextClassLoader turns out to be necessary
    //     // or not, we fail fast if permission is not available.
    //     sm.checkPermission(new RuntimePermission("setContextClassLoader"));
    // }
    // END android-removed
    mTask = task;
//    this.acc = AccessController.getContext();
    Thread::GetCurrentThread()->GetContextClassLoader((IClassLoader**)&mCcl);
}

ECode Executors::_PrivilegedCallableUsingCurrentClassLoader::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    assert(0 && "TODO");
    // try {
    //     return AccessController.doPrivileged(
    //         new PrivilegedExceptionAction<T>() {
    //             public T run() throws Exception {
    //                 Thread t = Thread.currentThread();
    //                 ClassLoader cl = t.getContextClassLoader();
    //                 if (ccl == cl) {
    //                     return task.call();
    //                 } else {
    //                     t.setContextClassLoader(ccl);
    //                     try {
    //                         return task.call();
    //                     } finally {
    //                         t.setContextClassLoader(cl);
    //                     }
    //                 }
    //             }
    //         }, acc);
    // } catch (PrivilegedActionException e) {
    //     throw e.getException();
    // }
    return NOERROR;
}

//==============================================
//    Executors::_PrivilegedThreadFactory::
//===============================================
Executors::_PrivilegedThreadFactory::_PrivilegedThreadFactory()
{
    assert(0 && "TODO");
    // BEGIN android-removed
    // SecurityManager sm = System.getSecurityManager();
    // if (sm != null) {
    //     // Calls to getContextClassLoader from this class
    //     // never trigger a security check, but we check
    //     // whether our callers have this permission anyways.
    //     sm.checkPermission(SecurityConstants.GET_CLASSLOADER_PERMISSION);
    //
    //     // Fail fast
    //     sm.checkPermission(new RuntimePermission("setContextClassLoader"));
    // }
    // END android-removed
//    this.acc = AccessController.getContext();
    Thread::GetCurrentThread()->GetContextClassLoader((IClassLoader**)&mCcl);
}

ECode Executors::_PrivilegedThreadFactory::NewThread(
    /* [in] */ IRunnable* r,
    /* [out] */ IThread** thread)
{
    VALIDATE_NOT_NULL(thread)

    assert(0 && "TODO");
    // return super.newThread(new Runnable() {
    //     public void run() {
    //         AccessController.doPrivileged(new PrivilegedAction<Void>() {
    //             public Void run() {
    //                 Thread.currentThread().setContextClassLoader(ccl);
    //                 r.run();
    //                 return null;
    //             }
    //         }, acc);
    //     }
    // });
    return NOERROR;
}

//==============================================
//    Executors::DelegatedExecutorService::
//===============================================
Executors::DelegatedExecutorService::DelegatedExecutorService(
    /* [in] */ IExecutorService* executor)
{
    mE = executor;
}

ECode Executors::DelegatedExecutorService::Execute(
    /* [in] */ IRunnable* command)
{
    return (IExecutor::Probe(mE))->Execute(command);
}

ECode Executors::DelegatedExecutorService::Shutdown()
{
    return mE->Shutdown();
}

ECode Executors::DelegatedExecutorService::ShutdownNow(
    /* [out] */ IList** tasks)
{
    return mE->ShutdownNow(tasks);
}

ECode Executors::DelegatedExecutorService::IsShutdown(
    /* [out] */ Boolean* result)
{
    return mE->IsShutdown(result);
}

ECode Executors::DelegatedExecutorService::IsTerminated(
    /* [out] */ Boolean* result)
{
    return mE->IsTerminated(result);
}

ECode Executors::DelegatedExecutorService::AwaitTermination(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* result)
{
    return mE->AwaitTermination(timeout, unit, result);
}

ECode Executors::DelegatedExecutorService::Submit(
    /* [in] */ IRunnable* task,
    /* [out] */ IFuture** future)
{
    VALIDATE_NOT_NULL(future)

    return mE->Submit(task, future);
}

ECode Executors::DelegatedExecutorService::Submit(
    /* [in] */ ICallable* task,
    /* [out] */ IFuture** future)
{
    VALIDATE_NOT_NULL(future)

    return mE->Submit(task, future);
}

ECode Executors::DelegatedExecutorService::Submit(
    /* [in] */ IRunnable* task,
    /* [in] */ IInterface* result,
    /* [out] */ IFuture** future)
{
    VALIDATE_NOT_NULL(future)

    return mE->Submit(task, result, future);
}

ECode Executors::DelegatedExecutorService::InvokeAll(
    /* [in] */ ICollection* tasks,
    /* [out] */ IList** futures)
{
    VALIDATE_NOT_NULL(futures)

    return mE->InvokeAll(tasks, futures);
}

ECode Executors::DelegatedExecutorService::InvokeAll(
    /* [in] */ ICollection* tasks,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IList** futures)
{
    VALIDATE_NOT_NULL(futures)

    return mE->InvokeAll(tasks, timeout, unit, futures);
}

ECode Executors::DelegatedExecutorService::InvokeAny(
    /* [in] */ ICollection* tasks,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    return mE->InvokeAny(tasks, result);
}

ECode Executors::DelegatedExecutorService::InvokeAny(
    /* [in] */ ICollection* tasks,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    return mE->InvokeAny(tasks, timeout, unit, result);
}

//==============================================
//    Executors::FinalizableDelegatedExecutorService::
//===============================================
Executors::FinalizableDelegatedExecutorService::FinalizableDelegatedExecutorService(
    /* [in] */ IExecutorService* executor) : DelegatedExecutorService(executor)
{
}

void Executors::FinalizableDelegatedExecutorService::Finalize()
{
    DelegatedExecutorService::Shutdown();
}

//==============================================
//    Executors::DelegatedScheduledExecutorService::
//===============================================
CAR_INTERFACE_IMPL(Executors::DelegatedScheduledExecutorService, DelegatedExecutorService, IScheduledExecutorService)

Executors::DelegatedScheduledExecutorService::DelegatedScheduledExecutorService(
    /* [in] */ IScheduledExecutorService* executor) : DelegatedExecutorService(IExecutorService::Probe(executor))
{
    mE = executor;
}

ECode Executors::DelegatedScheduledExecutorService::Schedule(
    /* [in] */ IRunnable* command,
    /* [in] */ Int64 delay,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IFuture** result)
{
    return mE->Schedule(command, delay, unit, result);
}

ECode Executors::DelegatedScheduledExecutorService::Schedule(
    /* [in] */ ICallable* callable,
    /* [in] */ Int64 delay,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IFuture** result)
{
    return mE->Schedule(callable, delay, unit, result);
}

ECode Executors::DelegatedScheduledExecutorService::ScheduleAtFixedRate(
    /* [in] */ IRunnable* command,
    /* [in] */ Int64 initialDelay,
    /* [in] */ Int64 period,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IFuture** result)
{
    return mE->ScheduleAtFixedRate(command, initialDelay, period, unit, result);
}

ECode Executors::DelegatedScheduledExecutorService::ScheduleWithFixedDelay(
    /* [in] */ IRunnable* command,
    /* [in] */ Int64 initialDelay,
    /* [in] */ Int64 delay,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IFuture** result)
{
    return mE->ScheduleWithFixedDelay(command, initialDelay, delay, unit, result);
}

//==============================================
//    Executors::CallableObject_1::
//===============================================
CAR_INTERFACE_IMPL(Executors::CallableObject_1, Object, ICallable)

Executors::CallableObject_1::CallableObject_1(
    /* [in] */ IPrivilegedAction* act)
{
    mAction = act;
}

ECode Executors::CallableObject_1::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    return mAction->Run(result);
}

//==============================================
//    Executors::CallableObject_2::
//===============================================
CAR_INTERFACE_IMPL(Executors::CallableObject_2, Object, ICallable)

Executors::CallableObject_2::CallableObject_2(
    /* [in] */ IPrivilegedExceptionAction* act)
{
    mAction = act;
}

ECode Executors::CallableObject_2::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    return mAction->Run(result);
}

//==============================================
//    Executors::
//===============================================
AutoPtr<IThreadFactory> Executors::GetDefaultThreadFactory()
{
    AutoPtr<DefaultThreadFactory> p = new DefaultThreadFactory();
    return IThreadFactory::Probe(p);
}

ECode Executors::Callable(
    /* [in] */ IRunnable* task,
    /* [in] */ IInterface* result,
    /* [out] */ ICallable** object)
{
    VALIDATE_NOT_NULL(object)

    if (task == NULL) return E_NULL_POINTER_EXCEPTION;
    AutoPtr<RunnableAdapter> p = new RunnableAdapter(task, result);
    *object = ICallable::Probe(p);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode Executors::NewFixedThreadPool(
    /* [in] */ Int32 nThreads,
    /* [out] */ IExecutorService** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IBlockingQueue> q;
    CLinkedBlockingQueue::New((IBlockingQueue**)&q);
    AutoPtr<IThreadPoolExecutor> p;
    CThreadPoolExecutor::New(nThreads, nThreads,
                             0L, TimeUnit::GetMILLISECONDS(),
                             q, (IThreadPoolExecutor**)&p);
    *result = (IExecutorService*)(p->Probe(EIID_IExecutorService));
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::NewWorkStealingPool(
    /* [in] */ Int32 parallelism,
    /* [out] */ IExecutorService** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IForkJoinPool> p;
    CForkJoinPool::New(parallelism,
         CForkJoinPool::mDefaultForkJoinWorkerThreadFactory,
         NULL, TRUE, (IForkJoinPool**)&p);
    *result = (IExecutorService*)(p->Probe(EIID_IExecutorService));
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::NewWorkStealingPool(
    /* [out] */ IExecutorService** result)
{
    VALIDATE_NOT_NULL(result)

    // return new ForkJoinPool
    //     (Runtime.getRuntime().availableProcessors(),
    //      ForkJoinPool.defaultForkJoinWorkerThreadFactory,
    //      null, true);

    AutoPtr<IForkJoinPool> p;
    CForkJoinPool::New(4,
         CForkJoinPool::mDefaultForkJoinWorkerThreadFactory,
         NULL, TRUE, (IForkJoinPool**)&p);
    *result = (IExecutorService*)(p->Probe(EIID_IExecutorService));
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::NewFixedThreadPool(
    /* [in] */ Int32 nThreads,
    /* [in] */ IThreadFactory* threadFactory,
    /* [out] */ IExecutorService** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IBlockingQueue> q;
    CLinkedBlockingQueue::New((IBlockingQueue**)&q);
    AutoPtr<IThreadPoolExecutor> tpe;
    CThreadPoolExecutor::New(nThreads, nThreads,
                             0L, TimeUnit::GetMILLISECONDS(),
                             q, threadFactory, (IThreadPoolExecutor**)&tpe);
    AutoPtr<IExecutorService> es = (IExecutorService*)(tpe->Probe(EIID_IExecutorService));
    AutoPtr<FinalizableDelegatedExecutorService> p = new FinalizableDelegatedExecutorService(es);
    *result = (IExecutorService*)(p->Probe(EIID_IExecutorService));
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::NewSingleThreadExecutor(
    /* [out] */ IExecutorService** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IBlockingQueue> q;
    CLinkedBlockingQueue::New((IBlockingQueue**)&q);
    AutoPtr<IThreadPoolExecutor> tpe;
    CThreadPoolExecutor::New(1, 1,
                             0L, TimeUnit::GetMILLISECONDS(),
                             q, (IThreadPoolExecutor**)&tpe);
    AutoPtr<IExecutorService> es = (IExecutorService*)(tpe->Probe(EIID_IExecutorService));
    AutoPtr<FinalizableDelegatedExecutorService> p = new FinalizableDelegatedExecutorService(es);
    *result = (IExecutorService*)(p->Probe(EIID_IExecutorService));
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::NewSingleThreadExecutor(
    /* [in] */ IThreadFactory* threadFactory,
    /* [out] */ IExecutorService** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IBlockingQueue> q;
    CLinkedBlockingQueue::New((IBlockingQueue**)&q);
    AutoPtr<IThreadPoolExecutor> tpe;
    CThreadPoolExecutor::New(1, 1,
                             0L, TimeUnit::GetMILLISECONDS(),
                             q, threadFactory, (IThreadPoolExecutor**)&tpe);
    AutoPtr<IExecutorService> es = (IExecutorService*)(tpe->Probe(EIID_IExecutorService));
    AutoPtr<FinalizableDelegatedExecutorService> p = new FinalizableDelegatedExecutorService(es);
    *result = (IExecutorService*)(p->Probe(EIID_IExecutorService));
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::NewCachedThreadPool(
    /* [out] */ IExecutorService** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ISynchronousQueue> q; //= new SynchronousQueue();
    AutoPtr<IThreadPoolExecutor> p;
    CThreadPoolExecutor::New(0, Elastos::Core::Math::INT32_MAX_VALUE,
                             60L, TimeUnit::GetSECONDS(),
                             IBlockingQueue::Probe(q), (IThreadPoolExecutor**)&p);
    *result = (IExecutorService*)(p->Probe(EIID_IExecutorService));
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::NewCachedThreadPool(
    /* [in] */ IThreadFactory* threadFactory,
    /* [out] */ IExecutorService** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ISynchronousQueue> q; //= new SynchronousQueue();
    AutoPtr<IThreadPoolExecutor> p;
    CThreadPoolExecutor::New(0, Elastos::Core::Math::INT32_MAX_VALUE,
                              60L, TimeUnit::GetSECONDS(),
                              IBlockingQueue::Probe(q), threadFactory, (IThreadPoolExecutor**)&p);
    *result = (IExecutorService*)(p->Probe(EIID_IExecutorService));
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::NewSingleThreadScheduledExecutor(
    /* [out] */ IScheduledExecutorService** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IScheduledThreadPoolExecutor> p;
    CScheduledThreadPoolExecutor::New(1, (IScheduledThreadPoolExecutor**)&p);
    AutoPtr<DelegatedScheduledExecutorService> res =
        new DelegatedScheduledExecutorService(IScheduledExecutorService::Probe(p));
    *result = IScheduledExecutorService::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::NewSingleThreadScheduledExecutor(
    /* [in] */ IThreadFactory* threadFactory,
    /* [out] */ IScheduledExecutorService** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IScheduledThreadPoolExecutor> p;
    CScheduledThreadPoolExecutor::New(1, threadFactory, (IScheduledThreadPoolExecutor**)&p);
    AutoPtr<DelegatedScheduledExecutorService> res =
        new DelegatedScheduledExecutorService(IScheduledExecutorService::Probe(p));
    *result = IScheduledExecutorService::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::NewScheduledThreadPool(
    /* [in] */ Int32 corePoolSize,
    /* [out] */ IScheduledExecutorService** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IScheduledThreadPoolExecutor> res;
    CScheduledThreadPoolExecutor::New(corePoolSize, (IScheduledThreadPoolExecutor**)&res);
    *result = IScheduledExecutorService::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::NewScheduledThreadPool(
    /* [in] */ Int32 corePoolSize,
    /* [in] */ IThreadFactory* threadFactory,
    /* [out] */ IScheduledExecutorService** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IScheduledThreadPoolExecutor> res;
    CScheduledThreadPoolExecutor::New(corePoolSize, threadFactory, (IScheduledThreadPoolExecutor**)&res);
    *result = IScheduledExecutorService::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::UnconfigurableExecutorService(
    /* [in] */ IExecutorService* executor,
    /* [out] */ IExecutorService** result)
{
    VALIDATE_NOT_NULL(result)

    if (executor == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<DelegatedExecutorService> p = new DelegatedExecutorService(executor);
    *result = (IExecutorService*)(p->Probe(EIID_IExecutorService));
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::UnconfigurableScheduledExecutorService(
    /* [in] */ IScheduledExecutorService* executor,
    /* [out] */ IScheduledExecutorService** result)
{
    VALIDATE_NOT_NULL(result)

    if (executor == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<DelegatedScheduledExecutorService> p = new DelegatedScheduledExecutorService(executor);
    *result = IScheduledExecutorService::Probe(p);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::PrivilegedThreadFactory(
    /* [out] */ IThreadFactory** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<_PrivilegedThreadFactory> p = new _PrivilegedThreadFactory();
    *result = IThreadFactory::Probe(p);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::Callable(
    /* [in] */ IRunnable* task,
    /* [out] */ ICallable** result)
{
    VALIDATE_NOT_NULL(result)

    if (task == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<RunnableAdapter> p = new RunnableAdapter(task, NULL);
    *result = ICallable::Probe(p);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::Callable(
    /* [in] */ IPrivilegedAction* action,
    /* [out] */ ICallable** result)
{
    VALIDATE_NOT_NULL(result)

    if (action == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<CallableObject_1> p = new CallableObject_1(action);
    *result = ICallable::Probe(p);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::Callable(
    /* [in] */ IPrivilegedExceptionAction* action,
    /* [out] */ ICallable** result)
{
    VALIDATE_NOT_NULL(result)

    if (action == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<CallableObject_2> p = new CallableObject_2(action);
    *result = ICallable::Probe(p);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::PrivilegedCallable(
    /* [in] */ ICallable* callable,
    /* [out] */ ICallable** result)
{
    VALIDATE_NOT_NULL(result)

    if (callable == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<_PrivilegedCallable> p = new _PrivilegedCallable(callable);
    *result = (ICallable*)(p->Probe(EIID_ICallable));
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Executors::PrivilegedCallableUsingCurrentClassLoader(
    /* [in] */ ICallable* callable,
    /* [out] */ ICallable** result)
{
    VALIDATE_NOT_NULL(result)

    if (callable == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<_PrivilegedCallableUsingCurrentClassLoader> res =
        new _PrivilegedCallableUsingCurrentClassLoader(callable);
    *result = (ICallable*)(res->Probe(EIID_ICallable));
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
