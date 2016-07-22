
#include "CScheduledThreadPoolExecutor.h"
#include "TimeUnit.h"
#include "Arrays.h"
#include "Executors.h"
#include "CSystem.h"
#include "CAtomicInteger64.h"

using Elastos::Core::ISystem;
using Elastos::Utility::Concurrent::Locks::ILock;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger64;

namespace Elastos {
namespace Utility {
namespace Concurrent {

//==========================================================
// CScheduledThreadPoolExecutor::ScheduledFutureTask::
//==========================================================
CAR_INTERFACE_IMPL_2(CScheduledThreadPoolExecutor::ScheduledFutureTask, FutureTask, IRunnableScheduledFuture, IScheduledFutureTask)

CScheduledThreadPoolExecutor::ScheduledFutureTask::ScheduledFutureTask(
    /* [in] */ IRunnable* r,
    /* [in] */ IInterface* result,
    /* [in] */ Int64 ns,
    /* [in] */ CScheduledThreadPoolExecutor* owner) : FutureTask(r, result)
{
    mTime = ns;
    mPeriod = 0;
    mSequencer->GetAndIncrement(&mSequenceNumber);
    mOwner = owner;
}

CScheduledThreadPoolExecutor::ScheduledFutureTask::ScheduledFutureTask(
    /* [in] */ IRunnable* r,
    /* [in] */ IInterface* result,
    /* [in] */ Int64 ns,
    /* [in] */ Int64 period,
    /* [in] */ CScheduledThreadPoolExecutor* owner) : FutureTask(r, result)
{
    mTime = ns;
    mPeriod = period;
    mSequencer->GetAndIncrement(&mSequenceNumber);
    mOwner = owner;
}

CScheduledThreadPoolExecutor::ScheduledFutureTask::ScheduledFutureTask(
    /* [in] */ ICallable* callable,
    /* [in] */ Int64 ns,
    /* [in] */ CScheduledThreadPoolExecutor* owner) : FutureTask(callable)
{
    mTime = ns;
    mPeriod = 0;
    mSequencer->GetAndIncrement(&mSequenceNumber);
    mOwner = owner;
}

ECode CScheduledThreadPoolExecutor::ScheduledFutureTask::GetDelay(
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)

    return unit->Convert(mTime - mOwner->Now(), TimeUnit::GetNANOSECONDS(), value);
}

ECode CScheduledThreadPoolExecutor::ScheduledFutureTask::CompareTo(
    /* [in] */ IInterface* another,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (Object::Equals(another, TO_IINTERFACE(this))) { // compare zero if same object
        *result = 0;
        return NOERROR;
    }
    if (another->Probe(EIID_IScheduledFutureTask) != NULL) {
        AutoPtr<ScheduledFutureTask> x = (ScheduledFutureTask*)IScheduledFutureTask::Probe(another);
        Int64 diff = mTime - x->mTime;
        if (diff < 0) {
            *result = -1;
            return NOERROR;
        }
        else if (diff > 0) {
            *result = 1;
            return NOERROR;
        }
        else if (mSequenceNumber < x->mSequenceNumber) {
            *result = -1;
            return NOERROR;
        }
        else {
            *result = 1;
            return NOERROR;
        }
    }
    Int64 d1  = 0;
    GetDelay(TimeUnit::GetNANOSECONDS(), &d1);
    AutoPtr<IDelayed> p = IDelayed::Probe(another);
    Int64 d2 = 0;
    p->GetDelay(TimeUnit::GetNANOSECONDS(), &d2);
    Int64 diff = d1 - d2;
    return (diff < 0) ? -1 : (diff > 0) ? 1 : 0;
}

ECode CScheduledThreadPoolExecutor::ScheduledFutureTask::IsPeriodic(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mPeriod != 0;
    return NOERROR;
}

void CScheduledThreadPoolExecutor::ScheduledFutureTask::SetNextRunTime()
{
    Int64 p = mPeriod;
    if (p > 0)
        mTime += p;
    else
        mTime = mOwner->TriggerTime(-p);
}

ECode CScheduledThreadPoolExecutor::ScheduledFutureTask::Cancel(
    /* [in] */ Boolean mayInterruptIfRunning,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean cancelled;
    FutureTask::Cancel(mayInterruptIfRunning, &cancelled);
    if (cancelled && mOwner->mRemoveOnCancel && mHeapIndex >= 0) {
        Boolean b = FALSE;
        mOwner->Remove(this, &b);
    }
    *result = cancelled;
    return NOERROR;
}

ECode CScheduledThreadPoolExecutor::ScheduledFutureTask::Run()
{
    Boolean periodic;
    IsPeriodic(&periodic);
    if (!mOwner->CanRunInCurrentRunState(periodic)) {
        Boolean b;
        Cancel(FALSE, &b);
    }
    else if (!periodic)
        FutureTask::Run();
    else if (FutureTask::RunAndReset()) {
        SetNextRunTime();
        mOwner->ReExecutePeriodic(mOuterTask);
    }
    return NOERROR;
}

//==========================================================
// CScheduledThreadPoolExecutor::
//==========================================================
static AutoPtr<IAtomicInteger64> InitmSequencer()
{
    AutoPtr<IAtomicInteger64> ai;
    CAtomicInteger64::New(1, (IAtomicInteger64**)&ai);
    return ai;
}

const AutoPtr<IAtomicInteger64> CScheduledThreadPoolExecutor::mSequencer = InitmSequencer();

CAR_INTERFACE_IMPL_2(CScheduledThreadPoolExecutor, ThreadPoolExecutor, IScheduledExecutorService, IScheduledThreadPoolExecutor)

CAR_OBJECT_IMPL(CScheduledThreadPoolExecutor)

Int64 CScheduledThreadPoolExecutor::Now()
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 lastTime;
    system->GetNanoTime(&lastTime);
    return lastTime;
}

Boolean CScheduledThreadPoolExecutor::CanRunInCurrentRunState(
    /* [in] */ Boolean periodic)
{
    return IsRunningOrShutdown(periodic ?
                               mContinueExistingPeriodicTasksAfterShutdown :
                               mExecuteExistingDelayedTasksAfterShutdown);
}

void CScheduledThreadPoolExecutor::DelayedExecute(
    /* [in] */ IRunnableScheduledFuture* task)
{
    Boolean bIsShut = FALSE;
    if ((IsShutdown(&bIsShut), bIsShut))
        Reject(IRunnable::Probe(task));
    else {
        AutoPtr<IBlockingQueue> q;
        ThreadPoolExecutor::GetQueue((IBlockingQueue**)&q);
        IQueue::Probe(q)->Add(task);
        Boolean bIsPed = FALSE, bRm = FALSE;
        if ((IsShutdown(&bIsShut), bIsShut) &&
            !CanRunInCurrentRunState((task->IsPeriodic(&bIsPed), bIsPed)) &&
            (Remove(IRunnable::Probe(task), &bRm), bRm)) {
            Boolean b = FALSE;
            IFuture::Probe(task)->Cancel(FALSE, &b);
        }
        else
            EnsurePrestart();
    }
}

void CScheduledThreadPoolExecutor::ReExecutePeriodic(
    /* [in] */ IRunnableScheduledFuture* task)
{
    if (CanRunInCurrentRunState(TRUE)) {
        AutoPtr<IBlockingQueue> q;
        ThreadPoolExecutor::GetQueue((IBlockingQueue**)&q);
        IQueue::Probe(q)->Add(task);
        Boolean bR = FALSE;
        if (!CanRunInCurrentRunState(TRUE) && (Remove(IRunnable::Probe(task), &bR), bR)) {
            Boolean b = FALSE;
            IFuture::Probe(task)->Cancel(FALSE, &b);
        }
        else
            EnsurePrestart();
    }
}

void CScheduledThreadPoolExecutor::OnShutdown()
{
    AutoPtr<IBlockingQueue> q;
    ThreadPoolExecutor::GetQueue((IBlockingQueue**)&q);
    Boolean keepDelayed =
        GetExecuteExistingDelayedTasksAfterShutdownPolicy();
    Boolean keepPeriodic =
        GetContinueExistingPeriodicTasksAfterShutdownPolicy();
    if (!keepDelayed && !keepPeriodic) {
        AutoPtr<IIterator> it;
        IIterable::Probe(q)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            IRunnableScheduledFuture* p = IRunnableScheduledFuture::Probe(obj);
            if (p) {
                Boolean b;
                IFuture::Probe(p)->Cancel(FALSE, &b);
            }
        }
        IQueue::Probe(q)->Clear();
    }
    else {
        // Traverse snapshot to avoid iterator exceptions
        AutoPtr<IIterator> it;
        IIterable::Probe(q)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IRunnableScheduledFuture> t = IRunnableScheduledFuture::Probe(obj);
            if (t) {
                Boolean bPe = FALSE, bCal = FALSE;
                if (((t->IsPeriodic(&bPe), bPe) ? !keepPeriodic : !keepDelayed) ||
                    (IFuture::Probe(t)->IsCancelled(&bCal), bCal)) { // also remove if already cancelled
                    Boolean bR = FALSE;
                    if ((IQueue::Probe(q)->Remove(t, &bR), bR)) {
                        Boolean b;
                        IFuture::Probe(t)->Cancel(FALSE, &b);
                    }
                }
            }
        }
    }
    TryTerminate();
}

AutoPtr<IRunnableScheduledFuture> CScheduledThreadPoolExecutor::DecorateTask(
    /* [in] */ IRunnable* runnable,
    /* [in] */ IRunnableScheduledFuture* task)
{
    return task;
}

AutoPtr<IRunnableScheduledFuture> CScheduledThreadPoolExecutor::DecorateTask(
    /* [in] */ ICallable* callable,
    /* [in] */ IRunnableScheduledFuture* task)
{
    return task;
}

ECode CScheduledThreadPoolExecutor::constructor(
    /* [in] */ Int32 corePoolSize)
{
    AutoPtr<DelayedWorkQueue> p = new DelayedWorkQueue();
    return ThreadPoolExecutor::constructor(corePoolSize,
          Elastos::Core::Math::INT32_MAX_VALUE, 0, TimeUnit::GetNANOSECONDS(),
          IBlockingQueue::Probe(p));
}

ECode CScheduledThreadPoolExecutor::constructor(
    /* [in] */ Int32 corePoolSize,
    /* [in] */ IThreadFactory* threadFactory)
{
    AutoPtr<DelayedWorkQueue> p = new DelayedWorkQueue();
    return ThreadPoolExecutor::constructor(corePoolSize,
          Elastos::Core::Math::INT32_MAX_VALUE, 0, TimeUnit::GetNANOSECONDS(),
          IBlockingQueue::Probe(p), threadFactory);
}

ECode CScheduledThreadPoolExecutor::constructor(
    /* [in] */ Int32 corePoolSize,
    /* [in] */ IRejectedExecutionHandler* handler)
{
    AutoPtr<DelayedWorkQueue> p = new DelayedWorkQueue();
    return ThreadPoolExecutor::constructor(corePoolSize,
          Elastos::Core::Math::INT32_MAX_VALUE, 0, TimeUnit::GetNANOSECONDS(),
          IBlockingQueue::Probe(p), handler);
}

ECode CScheduledThreadPoolExecutor::constructor(
    /* [in] */ Int32 corePoolSize,
    /* [in] */ IThreadFactory* threadFactory,
    /* [in] */ IRejectedExecutionHandler* handler)
{
    AutoPtr<DelayedWorkQueue> p = new DelayedWorkQueue();
    return ThreadPoolExecutor::constructor(corePoolSize,
          Elastos::Core::Math::INT32_MAX_VALUE, 0, TimeUnit::GetNANOSECONDS(),
          IBlockingQueue::Probe(p), threadFactory, handler);
}

Int64 CScheduledThreadPoolExecutor::TriggerTime(
    /* [in] */ Int64 delay,
    /* [in] */ ITimeUnit* unit)
{
    Int64 d = (delay < 0) ? 0 : delay;
    Int64 n;
    unit->ToNanos(d, &n);
    return TriggerTime(n);
}

Int64 CScheduledThreadPoolExecutor::TriggerTime(
    /* [in] */ Int64 delay)
{
    return Now() +
        ((delay < (Elastos::Core::Math::INT64_MAX_VALUE >> 1)) ? delay : OverflowFree(delay));
}

Int64 CScheduledThreadPoolExecutor::OverflowFree(
    /* [in] */ Int64 delay)
{
    AutoPtr<IBlockingQueue> q;
    ThreadPoolExecutor::GetQueue((IBlockingQueue**)&q);
    AutoPtr<IInterface> e;
    IQueue::Probe(q)->Peek((IInterface**)&e);
    AutoPtr<IDelayed> head = IDelayed::Probe(e);
    if (head != NULL) {
        Int64 headDelay;
        head->GetDelay(TimeUnit::GetNANOSECONDS(), &headDelay);
        if (headDelay < 0 && (delay - headDelay < 0))
            delay = Elastos::Core::Math::INT64_MAX_VALUE + headDelay;
    }
    return delay;
}

ECode CScheduledThreadPoolExecutor::Schedule(
    /* [in] */ IRunnable* command,
    /* [in] */ Int64 delay,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IFuture** result)
{
    VALIDATE_NOT_NULL(result)

    if (command == NULL || unit == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<ScheduledFutureTask> task = new ScheduledFutureTask(command, NULL, TriggerTime(delay, unit), this);
    AutoPtr<IRunnableScheduledFuture> t = DecorateTask(command, IRunnableScheduledFuture::Probe(task));
    DelayedExecute(t);
    *result = IFuture::Probe(t);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CScheduledThreadPoolExecutor::Schedule(
    /* [in] */ ICallable* callable,
    /* [in] */ Int64 delay,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IFuture** result)
{
    VALIDATE_NOT_NULL(result)

    if (callable == NULL || unit == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<ScheduledFutureTask> task = new ScheduledFutureTask(callable, TriggerTime(delay, unit), this);
    AutoPtr<IRunnableScheduledFuture> t = DecorateTask(callable, IRunnableScheduledFuture::Probe(task));
    DelayedExecute(t);
    *result = IFuture::Probe(t);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CScheduledThreadPoolExecutor::ScheduleAtFixedRate(
    /* [in] */ IRunnable* command,
    /* [in] */ Int64 initialDelay,
    /* [in] */ Int64 period,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IFuture** result)
{
    VALIDATE_NOT_NULL(result)

    if (command == NULL || unit == NULL)
        return E_NULL_POINTER_EXCEPTION;
    if (period <= 0)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    Int64 n;
    unit->ToNanos(period, &n);
    AutoPtr<ScheduledFutureTask> sft =
        new ScheduledFutureTask(command, NULL, TriggerTime(initialDelay, unit), n, this);
    AutoPtr<IRunnableScheduledFuture> t = DecorateTask(command, IRunnableScheduledFuture::Probe(sft));
    sft->mOuterTask = t;
    DelayedExecute(t);
    *result = IFuture::Probe(t);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CScheduledThreadPoolExecutor::ScheduleWithFixedDelay(
    /* [in] */ IRunnable* command,
    /* [in] */ Int64 initialDelay,
    /* [in] */ Int64 delay,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IFuture** result)
{
    VALIDATE_NOT_NULL(result)

    if (command == NULL || unit == NULL)
        return E_NULL_POINTER_EXCEPTION;
    if (delay <= 0)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    Int64 n;
    unit->ToNanos(-delay, &n);
    AutoPtr<ScheduledFutureTask> sft =
        new ScheduledFutureTask(command,
                                  NULL,
                                  TriggerTime(initialDelay, unit),
                                  n, this);
    AutoPtr<IRunnableScheduledFuture> t = DecorateTask(command, IRunnableScheduledFuture::Probe(sft));
    sft->mOuterTask = t;
    DelayedExecute(t);
    *result = IFuture::Probe(t);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CScheduledThreadPoolExecutor::Execute(
    /* [in] */ IRunnable* command)
{
    AutoPtr<IFuture> res;
    return Schedule(command, 0, TimeUnit::GetNANOSECONDS(), (IFuture**)&res);
}

ECode CScheduledThreadPoolExecutor::Submit(
    /* [in] */ IRunnable* task,
    /* [out] */ IFuture** future)
{
    VALIDATE_NOT_NULL(future)

    return Schedule(task, 0, TimeUnit::GetNANOSECONDS(), future);
}

ECode CScheduledThreadPoolExecutor::Submit(
    /* [in] */ IRunnable* task,
    /* [in] */ IInterface* result,
    /* [out] */ IFuture** future)
{
    VALIDATE_NOT_NULL(future)

    AutoPtr<ICallable> p;
    Executors::Callable(task, result, (ICallable**)&p);
    return Schedule(p, 0, TimeUnit::GetNANOSECONDS(), future);
}

ECode CScheduledThreadPoolExecutor::Submit(
    /* [in] */ ICallable* task,
    /* [out] */ IFuture** future)
{
    VALIDATE_NOT_NULL(future)

    return Schedule(task, 0, TimeUnit::GetNANOSECONDS(), future);
}

void CScheduledThreadPoolExecutor::SetContinueExistingPeriodicTasksAfterShutdownPolicy(
    /* [in] */ Boolean value)
{
    mContinueExistingPeriodicTasksAfterShutdown = value;
    Boolean b = FALSE;
    if (!value && (IsShutdown(&b), b))
        OnShutdown();
}

Boolean CScheduledThreadPoolExecutor::GetContinueExistingPeriodicTasksAfterShutdownPolicy()
{
    return mContinueExistingPeriodicTasksAfterShutdown;
}

void CScheduledThreadPoolExecutor::SetExecuteExistingDelayedTasksAfterShutdownPolicy(
    /* [in] */ Boolean value)
{
    mExecuteExistingDelayedTasksAfterShutdown = value;
    Boolean b = FALSE;
    if (!value && (IsShutdown(&b), b))
        OnShutdown();
}

Boolean CScheduledThreadPoolExecutor::GetExecuteExistingDelayedTasksAfterShutdownPolicy()
{
    return mExecuteExistingDelayedTasksAfterShutdown;
}

void CScheduledThreadPoolExecutor::SetRemoveOnCancelPolicy(
    /* [in] */ Boolean value)
{
    mRemoveOnCancel = value;
}

Boolean CScheduledThreadPoolExecutor::GetRemoveOnCancelPolicy()
{
    return mRemoveOnCancel;
}

ECode CScheduledThreadPoolExecutor::Shutdown()
{
    return ThreadPoolExecutor::Shutdown();
}

ECode CScheduledThreadPoolExecutor::ShutdownNow(
    /* [out] */ IList** tasks)
{
    return ThreadPoolExecutor::ShutdownNow(tasks);
}

ECode CScheduledThreadPoolExecutor::GetQueue(
    /* [out] */ IBlockingQueue** queue)
{
    return ThreadPoolExecutor::GetQueue(queue);
}

//==========================================================
// CScheduledThreadPoolExecutor::DelayedWorkQueue::
//==========================================================
Int32 CScheduledThreadPoolExecutor::DelayedWorkQueue::INITIAL_CAPACITY = 16;

CAR_INTERFACE_IMPL(CScheduledThreadPoolExecutor::DelayedWorkQueue, AbstractQueue, IBlockingQueue)

void CScheduledThreadPoolExecutor::DelayedWorkQueue::SetIndex(
    /* [in] */ IRunnableScheduledFuture* f,
    /* [in] */ Int32 idx)
{
    if (f->Probe(EIID_IScheduledFutureTask) != NULL) {
        AutoPtr<ScheduledFutureTask> p = (ScheduledFutureTask*)IScheduledFutureTask::Probe(f);
        p->mHeapIndex = idx;
    }
}

void CScheduledThreadPoolExecutor::DelayedWorkQueue::SiftUp(
    /* [in] */ Int32 k,
    /* [in] */ IRunnableScheduledFuture* key)
{
    while (k > 0) {
        Int32 parent = (k - 1) >> 1;
        AutoPtr<IRunnableScheduledFuture> e = (*mQueue)[parent];
        Int32 n = 0;
        if ((key->CompareTo(e, &n), n) >= 0)
            break;
        (*mQueue)[k] = e;
        SetIndex(e, k);
        k = parent;
    }
    (*mQueue)[k] = key;
    SetIndex(key, k);
}

void CScheduledThreadPoolExecutor::DelayedWorkQueue::SiftDown(
    /* [in] */ Int32 k,
    /* [in] */ IRunnableScheduledFuture* key)
{
    Int32 half = mSize >> 1;
    while (k < half) {
        Int32 child = (k << 1) + 1;
        AutoPtr<IRunnableScheduledFuture> c = (*mQueue)[child];
        Int32 right = child + 1;
        Int32 n1 = 0;
        if (right < mSize && (c->CompareTo((*mQueue)[right], &n1), n1) > 0)
            c = (*mQueue)[child = right];
        Int32 n2 = 0;
        if ((key->CompareTo(c, &n2), n2) <= 0)
            break;
        (*mQueue)[k] = c;
        SetIndex(c, k);
        k = child;
    }
    (*mQueue)[k] = key;
    SetIndex(key, k);
}

void CScheduledThreadPoolExecutor::DelayedWorkQueue::Grow()
{
    Int32 oldCapacity = mQueue->GetLength();
    Int32 newCapacity = oldCapacity + (oldCapacity >> 1); // grow 50%
    if (newCapacity < 0) // overflow
        newCapacity = Elastos::Core::Math::INT32_MAX_VALUE;
    Arrays::CopyOf(mQueue, newCapacity, (ArrayOf<IRunnableScheduledFuture*>**)&mQueue);
}

Int32 CScheduledThreadPoolExecutor::DelayedWorkQueue::IndexOf(
    /* [in] */ IInterface* x)
{
    if (x != NULL) {
        if (x->Probe(EIID_IScheduledFutureTask) != NULL) {
            AutoPtr<ScheduledFutureTask> p = (ScheduledFutureTask*)IScheduledFutureTask::Probe(x);
            Int32 i = p->mHeapIndex;
            // Sanity check; x could conceivably be a
            // ScheduledFutureTask from some other pool.
            if (i >= 0 && i < mSize && Object::Equals((*mQueue)[i], x))
                return i;
        }
        else {
            for (Int32 i = 0; i < mSize; i++)
                if (Object::Equals((*mQueue)[i], x))
                    return i;
        }
    }
    return -1;
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    *result = IndexOf(object) != -1;
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    Int32 i = IndexOf(object);
    if (i < 0) {
        *modified = FALSE;
        ILock::Probe(lock)->UnLock();
        return NOERROR;
    }

    SetIndex((*mQueue)[i], -1);
    Int32 s = --mSize;
    AutoPtr<IRunnableScheduledFuture> replacement = (*mQueue)[s];
    (*mQueue)[s] = NULL;
    if (s != i) {
        SiftDown(i, replacement);
        if (Object::Equals((*mQueue)[i], replacement->Probe(EIID_IInterface)))
            SiftUp(i, replacement);
    }
    *modified = FALSE;
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    *size = mSize;
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 s;
    GetSize(&s);
    *result = s == 0;
    return NOERROR;
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::RemainingCapacity(
    /* [out] */ Int32* capacity)
{
    VALIDATE_NOT_NULL(capacity)

    *capacity = Elastos::Core::Math::INT32_MAX_VALUE;
    return NOERROR;
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::Peek(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    *e = (*mQueue)[0];
    REFCOUNT_ADD(*e)
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::Offer(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (e == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IRunnableScheduledFuture> p = IRunnableScheduledFuture::Probe(e);
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    Int32 i = mSize;
    if (i >= mQueue->GetLength())
        Grow();
    mSize = i + 1;
    if (i == 0) {
        (*mQueue)[0] = p;
        SetIndex(p, 0);
    }
    else {
        SiftUp(i, p);
    }
    if (Object::Equals((*mQueue)[0], p)) {
        mLeader = NULL;
        mAvailable->Signal();
    }
    *result = TRUE;
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::Put(
    /* [in] */ IInterface* e)
{
    Boolean b;
    return Offer(e, &b);
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified)

    return Offer(object, modified);
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::Offer(
    /* [in] */ IInterface* e,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return Offer(e, result);
}

AutoPtr<IRunnableScheduledFuture> CScheduledThreadPoolExecutor::DelayedWorkQueue::FinishPoll(
    /* [in] */ IRunnableScheduledFuture* f)
{
    Int32 s = --mSize;
    AutoPtr<IRunnableScheduledFuture> x = (*mQueue)[s];
    (*mQueue)[s] = NULL;
    if (s != 0)
        SiftDown(0, x);
    SetIndex(f, -1);
    return f;
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::Poll(
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)
    *e = NULL;

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    AutoPtr<IRunnableScheduledFuture> first = (*mQueue)[0];
    Int64 d = 0;
    if ((first == NULL) || ((first->GetDelay(TimeUnit::GetNANOSECONDS(), &d), d) > 0)) {
        ILock::Probe(lock)->UnLock();
        return NOERROR;
    }
    else {
        AutoPtr<IInterface> temp = FinishPoll(first);
        *e = temp;
        REFCOUNT_ADD(*e)
        ILock::Probe(lock)->UnLock();
        return NOERROR;
    }
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::Take(
    /* [out] */ IInterface** res)
{
    VALIDATE_NOT_NULL(res)

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->LockInterruptibly();
    for (;;) {
        AutoPtr<IRunnableScheduledFuture> first = (*mQueue)[0];
        if (first == NULL)
            mAvailable->Await();
        else {
            Int64 delay;
            first->GetDelay(TimeUnit::GetNANOSECONDS(), &delay);
            if (delay <= 0) {
                AutoPtr<IInterface> temp = FinishPoll(first);
                if (mLeader == NULL && (*mQueue)[0] != NULL)
                    mAvailable->Signal();
                ILock::Probe(lock)->UnLock();
                *res = temp;
                REFCOUNT_ADD(*res)
                return NOERROR;
            }
            first = NULL; // don't retain ref while waiting
            if (mLeader != NULL)
                mAvailable->Await();
            else {
                AutoPtr<IThread> thisThread = Thread::GetCurrentThread();
                mLeader = thisThread;
                Int64 num = 0;
                mAvailable->AwaitNanos(delay, &num);
                if (mLeader == thisThread)
                    mLeader = NULL;
            }
        }
    }
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::Poll(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IInterface** e)
{
    VALIDATE_NOT_NULL(e)
    *e = NULL;

    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->LockInterruptibly();
    for (;;) {
        AutoPtr<IRunnableScheduledFuture> first = (*mQueue)[0];
        if (first == NULL) {
            if (nanos <= 0) {
                if (mLeader == NULL && (*mQueue)[0] != NULL)
                    mAvailable->Signal();
                ILock::Probe(lock)->UnLock();
                return NOERROR;
            }
            else
                mAvailable->AwaitNanos(nanos, &nanos);
        }
        else {
            Int64 delay;
            first->GetDelay(TimeUnit::GetNANOSECONDS(), &delay);
            if (delay <= 0) {
                AutoPtr<IInterface> temp = FinishPoll(first);
                if (mLeader == NULL && (*mQueue)[0] != NULL)
                    mAvailable->Signal();
                ILock::Probe(lock)->UnLock();
                *e = temp;
                REFCOUNT_ADD(*e)
                return NOERROR;
            }
            if (nanos <= 0) {
                if (mLeader == NULL && (*mQueue)[0] != NULL)
                    mAvailable->Signal();
                ILock::Probe(lock)->UnLock();
                return NOERROR;
            }
            first = NULL; // don't retain ref while waiting
            if (nanos < delay || mLeader != NULL)
                mAvailable->AwaitNanos(nanos, &nanos);
            else {
                AutoPtr<IThread> thisThread = Thread::GetCurrentThread();
                mLeader = thisThread;
                Int64 timeLeft;
                mAvailable->AwaitNanos(delay, &timeLeft);
                nanos -= delay - timeLeft;
                if (mLeader == thisThread)
                    mLeader = NULL;
            }
        }
    }
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::Clear()
{
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    for (Int32 i = 0; i < mSize; i++) {
        AutoPtr<IRunnableScheduledFuture> t = (*mQueue)[i];
        if (t != NULL) {
            (*mQueue)[i] = NULL;
            SetIndex(t, -1);
        }
    }
    mSize = 0;
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

AutoPtr<IRunnableScheduledFuture> CScheduledThreadPoolExecutor::DelayedWorkQueue::PeekExpired()
{
    // assert lock.isHeldByCurrentThread();
    AutoPtr<IRunnableScheduledFuture> first = (*mQueue)[0];
    if (first == NULL)
        return NULL;
    Int64 num = 0;
    return (first == NULL || (first->GetDelay(TimeUnit::GetNANOSECONDS(), &num), num) > 0) ?
            NULL : first;
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::DrainTo(
    /* [in] */ ICollection* c,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)

    if (c == NULL)
        return E_NULL_POINTER_EXCEPTION;
    if (Object::Equals(c->Probe(EIID_IInterface), TO_IINTERFACE(this)))
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    AutoPtr<IRunnableScheduledFuture> first;
    Int32 n = 0;
    while ((first = PeekExpired()) != NULL) {
        c->Add(first);   // In this order, in case add() throws.
        FinishPoll(first);
        ++n;
    }
    *number = n;
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::DrainTo(
    /* [in] */ ICollection* c,
    /* [in] */ Int32 maxElements,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)

    if (c == NULL)
        return E_NULL_POINTER_EXCEPTION;
    if (Object::Equals(c->Probe(EIID_IInterface), TO_IINTERFACE(this)))
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    if (maxElements <= 0) {
        *number = 0;
        return NOERROR;
    }
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    AutoPtr<IRunnableScheduledFuture> first;
    Int32 n = 0;
    while (n < maxElements && (first = PeekExpired()) != NULL) {
        c->Add(first);   // In this order, in case add() throws.
        FinishPoll(first);
        ++n;
    }
    *number = n;
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array)

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
//    Arrays.copyOf(queue, size, Object[].class);
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray)

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    if (inArray->GetLength() < mSize) {
        ILock::Probe(lock)->UnLock();
//        return (T[]) Arrays::CopyOf(mQueue, mSize, a.getClass());
    }
    inArray->Copy(0, (ArrayOf<IInterface*>*)(mQueue.Get()), 0, mSize);
    if (inArray->GetLength() > mSize)
        (*inArray)[mSize] = NULL;
    *outArray = inArray;
    REFCOUNT_ADD(*outArray)
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)

    AutoPtr<ArrayOf<IRunnableScheduledFuture*> > arr;
    Arrays::CopyOf(mQueue, mSize, (ArrayOf<IRunnableScheduledFuture*>**)&arr);
    AutoPtr<Itr> p = new Itr(arr, this);
    *it = IIterator::Probe(p);
    REFCOUNT_ADD(*it)
    return NOERROR;
}

//==========================================================
// CScheduledThreadPoolExecutor::DelayedWorkQueue::Itr::
//==========================================================
CAR_INTERFACE_IMPL(CScheduledThreadPoolExecutor::DelayedWorkQueue::Itr, Object, IIterator)

CScheduledThreadPoolExecutor::DelayedWorkQueue::Itr::Itr(
    /* [in] */ ArrayOf<IRunnableScheduledFuture*>* array,
    /* [in] */ DelayedWorkQueue* owner)
{
    mArray = array;
    mOwner = owner;
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::Itr::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mCursor < mArray->GetLength();
    return NOERROR;
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::Itr::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    if (mCursor >= mArray->GetLength())
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    mLastRet = mCursor;
    *object = (*mArray)[mCursor++];
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CScheduledThreadPoolExecutor::DelayedWorkQueue::Itr::Remove()
{
    if (mLastRet < 0)
        return E_ILLEGAL_STATE_EXCEPTION;
    Boolean b;
    mOwner->Remove((*mArray)[mLastRet], &b);
    mLastRet = -1;
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
