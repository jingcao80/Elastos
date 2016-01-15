
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "ForkJoinTask.h"
#include "Thread.h"
#include "ForkJoinWorkerThread.h"
#include "CSystem.h"
#include "AutoLock.h"

using Elastos::IO::EIID_ISerializable;
using Elastos::Core::ISystem;
using Elastos::Core::Thread;
using Elastos::Core::IThread;
using Elastos::Utility::Concurrent::ForkJoinWorkerThread;
using Elastos::Utility::Concurrent::Locks::ILock;

namespace Elastos {
namespace Utility {
namespace Concurrent {

//====================================================================
// ForkJoinTask::
//====================================================================
Int32 ForkJoinTask::DONE_MASK   = 0xf0000000;
Int32 ForkJoinTask::NORMAL      = 0xf0000000;
Int32 ForkJoinTask::CANCELLED   = 0xc0000000;
Int32 ForkJoinTask::EXCEPTIONAL = 0x80000000;
Int32 ForkJoinTask::SIGNAL      = 0x00010000;
Int32 ForkJoinTask::SMASK       = 0x0000ffff;

CAR_INTERFACE_IMPL_3(ForkJoinTask, Object, IForkJoinTask, IFuture, ISerializable)

Int32 ForkJoinTask::SetCompletion(
    /* [in] */ const Int32& completion)
{
    for (Int32 s;;) {
        if ((s = mStatus) < 0)
            return s;
        assert(0 && "TODO");
        // if (U.compareAndSwapInt(this, STATUS, s, s | completion)) {
        //     if ((s >>> 16) != 0)
        //         synchronized (this) { notifyAll(); }
        //     return completion;
        // }
    }
}

ECode ForkJoinTask::DoExec(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 s; Boolean completed;
    if ((s = mStatus) >= 0) {
//        try {
            Exec(&completed);
        // } catch (Throwable rex) {
        //     return setExceptionalCompletion(rex);
        // }
        if (completed)
            s = SetCompletion(NORMAL);
    }
    *result = s;
    return NOERROR;
}

Boolean ForkJoinTask::TrySetSignal()
{
    assert(0 && "TODO");
    return FALSE;
    // Int32 s = mStatus;
    // return s >= 0 && U.compareAndSwapInt(this, STATUS, s, s | SIGNAL);
}

Int32 ForkJoinTask::ExternalAwaitDone()
{
    assert(0 && "TODO");
    return 0;
    // Int32 s;
    // AutoPtr<IForkJoinPool> cp = ForkJoinPool::mCommon;
    // if ((s = mStatus) >= 0) {
    //     if (cp != NULL) {
    //         // if (this instanceof CountedCompleter)
    //         //     s = cp.externalHelpComplete((CountedCompleter<?>)this);
    //         // else if (cp.tryExternalUnpush(this))
    //         //     s = doExec();
    //     }
    //     if (s >= 0 && (s = mStatus) >= 0) {
    //         Boolean interrupted = FALSE;
    //         do {
    //             // if (U.compareAndSwapInt(this, STATUS, s, s | SIGNAL)) {
    //             //     synchronized (this) {
    //             //         if (mStatus >= 0) {
    //             //             try {
    //             //                 Wait();
    //             //             } catch (InterruptedException ie) {
    //             //                 interrupted = TRUE;
    //             //             }
    //             //         }
    //             //         else
    //             //             NotifyAll();
    //             //     }
    //             // }
    //         } while ((s = mStatus) >= 0);
    //         if (interrupted)
    //             Thread::GetCurrentThread()->Interrupt();
    //     }
    // }
    // return s;
}

Int32 ForkJoinTask::ExternalInterruptibleAwaitDone()
{
    assert(0 && "TODO");
    return 0;
    // int s;
    // ForkJoinPool cp = ForkJoinPool.common;
    // if (Thread.interrupted())
    //     throw new InterruptedException();
    // if ((s = status) >= 0 && cp != null) {
    //     if (this instanceof CountedCompleter)
    //         cp.externalHelpComplete((CountedCompleter<?>)this);
    //     else if (cp.tryExternalUnpush(this))
    //         doExec();
    // }
    // while ((s = status) >= 0) {
    //     if (U.compareAndSwapInt(this, STATUS, s, s | SIGNAL)) {
    //         synchronized (this) {
    //             if (status >= 0)
    //                 wait();
    //             else
    //                 notifyAll();
    //         }
    //     }
    // }
    // return s;
}

Int32 ForkJoinTask::DoJoin()
{
    Int32 s; AutoPtr<IThread> t;
    AutoPtr<ForkJoinWorkerThread> wt;
    AutoPtr<CForkJoinPool::WorkQueue> w;
    return (s = mStatus) < 0 ? s :
        ((t = Thread::GetCurrentThread())->Probe(EIID_IForkJoinWorkerThread) != NULL) ?
        (w = (wt = (ForkJoinWorkerThread*)(IForkJoinWorkerThread::Probe(t)))->mWorkQueue)->
        TryUnpush(this) && (DoExec(&s), s) < 0 ? s :
        ((CForkJoinPool*)(wt->mPool.Get()))->AwaitJoin(w, this) :
        ExternalAwaitDone();
}

Int32 ForkJoinTask::DoInvoke()
{

    Int32 s; AutoPtr<IThread> t; AutoPtr<ForkJoinWorkerThread> wt;
    return (DoExec(&s), s) < 0 ? s :
        ((t = Thread::GetCurrentThread())->Probe(EIID_IForkJoinWorkerThread) != NULL) ?
        ((CForkJoinPool*)((wt = ((ForkJoinWorkerThread*)(IForkJoinWorkerThread::Probe(t))))->mPool.Get()))->AwaitJoin(wt->mWorkQueue, this) :
        ExternalAwaitDone();
}

AutoPtr<ArrayOf<ForkJoinTask::ExceptionNode*> > ForkJoinTask::sExceptionTable;
AutoPtr<IReentrantLock> ForkJoinTask::sExceptionTableLock;
//AutoPtr<ReferenceQueue> ForkJoinTask::sExceptionTableRefQueue;

Int32 ForkJoinTask::EXCEPTION_MAP_CAPACITY = 32;

//====================================================================
// ForkJoinTask::ExceptionNode::
//====================================================================
ForkJoinTask::ExceptionNode::ExceptionNode(
    /* [in] */ IForkJoinTask* task,
    /* [in] */ IThrowable* ex,
    /* [in] */ ExceptionNode* next)
{
//    super(task, exceptionTableRefQueue);
    mEx = ex;
    mNext = next;
    Thread::GetCurrentThread()->GetId(&mThrower);
}

//====================================================================
// ForkJoinTask::
//====================================================================
Int32 ForkJoinTask::RecordExceptionalCompletion(
    /* [in] */ IThrowable* ex)
{
    Int32 s;
    if ((s = mStatus) >= 0) {
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int32 h;
        system->IdentityHashCode(THIS_PROBE(IInterface), &h);
        AutoPtr<IReentrantLock> lock = sExceptionTableLock;
        ILock::Probe(lock)->Lock();
//        try {
            ExpungeStaleExceptions();
            AutoPtr<ArrayOf<ExceptionNode*> > t = sExceptionTable;
            Int32 i = h & (t->GetLength() - 1);
            for (AutoPtr<ExceptionNode> e = (*t)[i]; ; e = e->mNext) {
                if (e == NULL) {
                    (*t)[i] = new ExceptionNode(this, ex, (*t)[i]);
                    break;
                }
                if (Object::Equals(e->Probe(EIID_IInterface), THIS_PROBE(IInterface))) // already present
                    break;
            }
//        } finally {
            ILock::Probe(lock)->UnLock();
//        }
        s = SetCompletion(EXCEPTIONAL);
    }
    return s;
}

Int32 ForkJoinTask::SetExceptionalCompletion(
    /* [in] */ IThrowable* ex)
{
    Int32 s = RecordExceptionalCompletion(ex);
    if ((s & DONE_MASK) == EXCEPTIONAL)
        InternalPropagateException(ex);
    return s;
}

void ForkJoinTask::InternalPropagateException(
    /* [in] */ IThrowable* ex)
{
}

void ForkJoinTask::CancelIgnoringExceptions(
    /* [in] */ ForkJoinTask* t)
{
    if (t != NULL && t->mStatus >= 0) {
//        try {
            Boolean bRes;
            t->Cancel(FALSE, &bRes);
        // } catch (Throwable ignore) {
        // }
    }
}

void ForkJoinTask::ClearExceptionalCompletion()
{
    AutoPtr<ISystem> sys;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&sys);
    Int32 h;
    sys->IdentityHashCode(THIS_PROBE(IInterface), &h);
    AutoPtr<IReentrantLock> lock = sExceptionTableLock;
    (ILock::Probe(lock))->Lock();
    // AutoPtr<ArrayOf<ExceptionNode*> > t = sExceptionTable;
    // Int32 i = h & (t->GetLength() - 1);
    // AutoPtr<ExceptionNode> e = (*t)[i];
    // AutoPtr<ExceptionNode> pred = NULL;
    // while (e != NULL) {
    //     AutoPtr<ExceptionNode> next = e->mNext;
    //     if (e->Get() == this) {
    //         if (pred == NULL)
    //             (*t)[i] = next;
    //         else
    //             pred->mNext = next;
    //         break;
    //     }
    //     pred = e;
    //     e = next;
    // }
    ExpungeStaleExceptions();
    mStatus = 0;
    (ILock::Probe(lock))->UnLock();
}

AutoPtr<IThrowable> ForkJoinTask::GetThrowableException()
{
    if ((mStatus & DONE_MASK) != EXCEPTIONAL)
        return NULL;
    AutoPtr<ISystem> sys;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&sys);
    Int32 h;
    sys->IdentityHashCode(THIS_PROBE(IInterface), &h);
    assert(0 && "TODO");
    return NULL;
    // AutoPtr<ExceptionNode> e;
    // AutoPtr<IReentrantLock> lock = sExceptionTableLock;
    // lock->Lock();
    // ExpungeStaleExceptions();
    // AutoPtr<ArrayOf<ExceptionNode*> > t = sExceptionTable;
    // e = (*t)[h & (t->GetLength() - 1)];
    // // while (e != NULL && e->Get() != this)
    // //     e = e->mNext;
    // lock->UnLock();
    // AutoPtr<IThrowable> ex;
    // if (e == NULL || (ex = e->mEx) == NULL)
    //     return NULL;
    // Int64 id = 0;
    // Thread::GetCurrentThread()->GetId(&id);
    // if (false && e->mThrower != id) {
    //     Class<? extends Throwable> ec = ex->GetClass();
    //     try {
    //         Constructor<?> noArgCtor = null;
    //         Constructor<?>[] cs = ec.getConstructors();// public ctors only
    //         for (Int32 i = 0; i < cs.length; ++i) {
    //             Constructor<?> c = cs[i];
    //             Class<?>[] ps = c.getParameterTypes();
    //             if (ps.length == 0)
    //                 noArgCtor = c;
    //             else if (ps.length == 1 && ps[0] == Throwable.class)
    //                 return (Throwable)(c.newInstance(ex));
    //         }
    //         if (noArgCtor != null) {
    //             Throwable wx = (Throwable)(noArgCtor.newInstance());
    //             wx.initCause(ex);
    //             return wx;
    //         }
    //     } catch (Exception ignore) {
    //     }
    // }
    // return ex;
}

void ForkJoinTask::ExpungeStaleExceptions()
{
    assert(0 && "TODO");
    // for (AutoPtr<IInterface> x; (x = sExceptionTableRefQueue->Poll()) != NULL;) {
    //     if (x->Probe(EIID_ExceptionNode) != NULL) {
    //         AutoPtr<ForkJoinTask> key = ((ExceptionNode*)x)->Get();
    //         AutoPtr<ArrayOf<ExceptionNode*> > t = sExceptionTable;
    //         Int32 i;// = System.identityHashCode(key) & (t.length - 1);
    //         AutoPtr<ExceptionNode> e = (*t)[i];
    //         AutoPtr<ExceptionNode> pred = NULL;
    //         while (e != NULL) {
    //             AutoPtr<ExceptionNode> next = e->mNext;
    //             if (Object::Equals(e, x)) {
    //                 if (pred == NULL)
    //                     (*t)[i] = next;
    //                 else
    //                     pred->mNext = next;
    //                 break;
    //             }
    //             pred = e;
    //             e = next;
    //         }
    //     }
    // }
}

void ForkJoinTask::HelpExpungeStaleExceptions()
{
    AutoPtr<IReentrantLock> lock = sExceptionTableLock;
    Boolean b = FALSE;
    if (((ILock::Probe(lock))->TryLock(&b), b)) {
        ExpungeStaleExceptions();
        (ILock::Probe(lock))->UnLock();
    }
}

void ForkJoinTask::Rethrow(
    /* [in] */ IThrowable* ex)
{
    if (ex != NULL)
        ForkJoinTask::UncheckedThrow(ex);
}

void ForkJoinTask::UncheckedThrow(
    /* [in] */ IThrowable* t)
{
//    throw t; // rely on vacuous cast
}

void ForkJoinTask::ReportException(
    /* [in] */ Int32 s)
{
    if (s == CANCELLED)
        return;
//        throw new CancellationException();
    if (s == EXCEPTIONAL)
        Rethrow(GetThrowableException());
}

ECode ForkJoinTask::Fork(
    /* [out] */ IForkJoinTask** outtask)
{
    VALIDATE_NOT_NULL(outtask)

    AutoPtr<IThread> t = Thread::GetCurrentThread();
    if (t->Probe(EIID_IForkJoinWorkerThread) != NULL)
        ((ForkJoinWorkerThread*)(IForkJoinWorkerThread::Probe(t)))->mWorkQueue->Push(this);
    else
        ((CForkJoinPool*)CForkJoinPool::mCommon.Get())->ExternalPush(this);
    *outtask = IForkJoinTask::Probe(this);
    REFCOUNT_ADD(*outtask)
    return NOERROR;
}

ECode ForkJoinTask::Join(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    Int32 s;
    if ((s = DoJoin() & DONE_MASK) != NORMAL)
        ReportException(s);
    return GetRawResult(outface);
}

ECode ForkJoinTask::Invoke(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);

    Int32 s;
    if ((s = DoInvoke() & DONE_MASK) != NORMAL)
        ReportException(s);
    return GetRawResult(outface);
}

void ForkJoinTask::InvokeAll(
    /* [in] */ IForkJoinTask* t1,
    /* [in] */ IForkJoinTask* t2)
{
    Int32 s1, s2;
    AutoPtr<IForkJoinTask> p;
    t2->Fork((IForkJoinTask**)&p);
    if ((s1 = ((ForkJoinTask*)t1)->DoInvoke() & DONE_MASK) != NORMAL)
        ((ForkJoinTask*)t1)->ReportException(s1);
    if ((s2 = ((ForkJoinTask*)t2)->DoJoin() & DONE_MASK) != NORMAL)
        ((ForkJoinTask*)t2)->ReportException(s2);
}

void ForkJoinTask::InvokeAll(
    /* [in] */ ArrayOf<IForkJoinTask*>* tasks)
{
    AutoPtr<IThrowable> ex = NULL;
    Int32 last = tasks->GetLength() - 1;
    for (Int32 i = last; i >= 0; --i) {
        AutoPtr<IForkJoinTask> t = (*tasks)[i];
        AutoPtr<ForkJoinTask> ct = (ForkJoinTask*)t.Get();
        if (t == NULL) {
            // if (ex == NULL)
            //     ex = new NullPointerException();
        }
        else if (i != 0) {
            AutoPtr<IForkJoinTask> p;
            t->Fork((IForkJoinTask**)&p);
        }
        else if (ct->DoInvoke() < NORMAL && ex == NULL)
            t->GetException((IThrowable**)&ex);
    }
    for (Int32 i = 1; i <= last; ++i) {
        AutoPtr<IForkJoinTask> t = (*tasks)[i];
        AutoPtr<ForkJoinTask> ct = (ForkJoinTask*)t.Get();
        if (t != NULL) {
            if (ex != NULL) {
                Boolean b = FALSE;
                (IFuture::Probe(t))->Cancel(FALSE, &b);
            }
            else if (ct->DoJoin() < NORMAL)
                t->GetException((IThrowable**)&ex);
        }
    }
    // if (ex != NULL)
    //     rethrow(ex); // android-changed
}

AutoPtr<ICollection> ForkJoinTask::InvokeAll(
    /* [in] */ ICollection* tasks)
{
    if ((tasks->Probe(EIID_IRandomAccess) == NULL) || (tasks->Probe(EIID_IList) == NULL)) {
        Int32 size;
        tasks->GetSize(&size);
        AutoPtr<ArrayOf<IForkJoinTask*> > arr = ArrayOf<IForkJoinTask*>::Alloc(size);
        AutoPtr<ArrayOf<IForkJoinTask*> > arrOut;
        tasks->ToArray((ArrayOf<IInterface*>*)arr.Get(), (ArrayOf<IInterface*>**)&arrOut);
        InvokeAll(arrOut);
        return tasks;
    }
    AutoPtr<IList> ts = (IList*)tasks->Probe(EIID_IList);
    AutoPtr<IThrowable> ex;
    Int32 last = 0;
    (ICollection::Probe(ts))->GetSize(&last);
    last -= 1;
    for (Int32 i = last; i >= 0; --i) {
        AutoPtr<IInterface> p;
        ts->Get(i, (IInterface**)&p);
        AutoPtr<IForkJoinTask> t = IForkJoinTask::Probe(p);
        AutoPtr<ForkJoinTask> ct = (ForkJoinTask*)t.Get();
        if (t == NULL) {
            // if (ex == null)
            //     ex = new NullPointerException();
        }
        else if (i != 0) {
            AutoPtr<IForkJoinTask> p;
            t->Fork((IForkJoinTask**)&p);
        }
        else if (ct->DoInvoke() < NORMAL && ex == NULL)
            t->GetException((IThrowable**)&ex);
    }
    for (Int32 i = 1; i <= last; ++i) {
        AutoPtr<IInterface> p;
        ts->Get(i, (IInterface**)&p);
        AutoPtr<IForkJoinTask> t = IForkJoinTask::Probe(p);
        AutoPtr<ForkJoinTask> ct = (ForkJoinTask*)t.Get();
        if (t != NULL) {
            if (ex != NULL) {
                Boolean b = FALSE;
                (IFuture::Probe(t))->Cancel(FALSE, &b);
            }
            else if (ct->DoJoin() < NORMAL)
                t->GetException((IThrowable**)&ex);
        }
    }
    // if (ex != NULL)
    //     rethrow(ex); // android-changed
    return tasks;
}

ECode ForkJoinTask::Cancel(
    /* [in] */ Boolean mayInterruptIfRunning,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (SetCompletion(CANCELLED) & DONE_MASK) == CANCELLED;
    return NOERROR;
}

ECode ForkJoinTask::IsDone(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = mStatus < 0;
    return NOERROR;
}

ECode ForkJoinTask::IsCancelled(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (mStatus & DONE_MASK) == CANCELLED;
    return NOERROR;
}

ECode ForkJoinTask::IsCompletedAbnormally(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = (mStatus & DONE_MASK) < NORMAL;
    return NOERROR;
}

ECode ForkJoinTask::IsCompletedNormally(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = (mStatus & DONE_MASK) == NORMAL;
    return NOERROR;
}

ECode ForkJoinTask::GetException(
    /* [out] */ IThrowable** res)
{
    VALIDATE_NOT_NULL(res);
    Int32 s = mStatus & DONE_MASK;
    assert(0 && "TODO");
    // *res = ((s >= NORMAL)    ? NULL :
    //         (s == CANCELLED) ? new CancellationException() :
    //         getThrowableException());
    return NOERROR;
}

ECode ForkJoinTask::CompleteExceptionally(
    /* [in] */ IThrowable* ex)
{
    assert(0 && "TODO");
    // SetExceptionalCompletion((ex instanceof RuntimeException) ||
    //                          (ex instanceof Error) ? ex :
    //                          new RuntimeException(ex));
    return NOERROR;
}

ECode ForkJoinTask::Complete(
    /* [in] */ IInterface* value)
{
//    try {
        SetRawResult(value);
    // } catch (Throwable rex) {
    //     SetExceptionalCompletion(rex);
    //     return;
    // }
    SetCompletion(NORMAL);
    return NOERROR;
}

ECode ForkJoinTask::QuietlyComplete()
{
    SetCompletion(NORMAL);
    return NOERROR;
}

ECode ForkJoinTask::Get(
    /* [out] */ IInterface** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IThread> t = Thread::GetCurrentThread();
    Int32 s = ( t->Probe(EIID_IForkJoinWorkerThread) != NULL ) ?
        DoJoin() : ExternalInterruptibleAwaitDone();
    AutoPtr<IThrowable> ex;
    if ((s &= DONE_MASK) == CANCELLED)
        return E_CANCELLATION_EXCEPTION;
    if (s == EXCEPTIONAL && (ex = GetThrowableException()) != NULL)
        return E_EXECUTION_EXCEPTION;
    return GetRawResult(res);
}

ECode ForkJoinTask::Get(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IInterface** res)
{
    VALIDATE_NOT_NULL(res);

    if (Thread::Interrupted())
        return E_INTERRUPTED_EXCEPTION;
    // Messy in part because we measure in nanosecs, but wait in millisecs
    Int32 s; Int64 ms;
    Int64 ns;
    unit->ToNanos(timeout, &ns);
    AutoPtr<IForkJoinPool> cp;
    if ((s = mStatus) >= 0 && ns > 0L) {
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 nas;
        system->GetNanoTime(&nas);
        Int64 deadline = nas + ns;
        AutoPtr<IForkJoinPool> p;
        AutoPtr<CForkJoinPool::WorkQueue> w;
        AutoPtr<IThread> t = Thread::GetCurrentThread();
        if (t->Probe(EIID_IForkJoinWorkerThread) != NULL) {
            AutoPtr<ForkJoinWorkerThread> wt = (ForkJoinWorkerThread*)IForkJoinWorkerThread::Probe(t);
            p = wt->mPool;
            w = wt->mWorkQueue;
            ((CForkJoinPool*)p.Get())->HelpJoinOnce(w, this); // no retries on failure
        }
        else if ((cp = CForkJoinPool::mCommon) != NULL) {
            // if (this instanceof CountedCompleter)
            //     cp.externalHelpComplete((CountedCompleter<?>)this);
            // else if (cp.tryExternalUnpush(this))
            //     doExec();
        }
        Boolean canBlock = FALSE;
        Boolean interrupted = FALSE;
//        try {
            while ((s = mStatus) >= 0) {
                if (w != NULL && w->mQlock < 0)
                    CancelIgnoringExceptions(this);
                else if (!canBlock) {
                    AutoPtr<CForkJoinPool> cp = (CForkJoinPool*)p.Get();
                    if (p == NULL || cp->TryCompensate(cp->mCtl))
                        canBlock = TRUE;
                }
                else {
                    // if ((ms = TimeUnit.NANOSECONDS.toMillis(ns)) > 0L &&
                    //     U.compareAndSwapInt(this, STATUS, s, s | SIGNAL)) {
                    //     synchronized (this) {
                    //         if (status >= 0) {
                    //             try {
                    //                 wait(ms);
                    //             } catch (InterruptedException ie) {
                    //                 if (p == NULL)
                    //                     interrupted = TRUE;
                    //             }
                    //         }
                    //         else
                    //             notifyAll();
                    //     }
                    // }
                    // if ((s = status) < 0 || interrupted ||
                    //     (ns = deadline - System.nanoTime()) <= 0L)
                    //     break;
                }
            }
//        } finally {
            if (p != NULL && canBlock)
                ((CForkJoinPool*)p.Get())->IncrementActiveCount();
//        }
        if (interrupted)
            return E_INTERRUPTED_EXCEPTION;
    }
    if ((s &= DONE_MASK) != NORMAL) {
        AutoPtr<IThrowable> ex;
        if (s == CANCELLED)
            return E_CANCELLATION_EXCEPTION;
        if (s != EXCEPTIONAL)
            return E_TIMEOUT_EXCEPTION;
        if ((ex = GetThrowableException()) != NULL)
            return E_EXECUTION_EXCEPTION;
    }
    return GetRawResult(res);
}

ECode ForkJoinTask::QuietlyJoin()
{
    DoJoin();
    return NOERROR;
}

ECode ForkJoinTask::QuietlyInvoke()
{
    DoInvoke();
    return NOERROR;
}

void ForkJoinTask::HelpQuiesce()
{
    AutoPtr<IThread> t = Thread::GetCurrentThread();
    if (t->Probe(EIID_IForkJoinWorkerThread) != NULL) {
        AutoPtr<ForkJoinWorkerThread> wt = (ForkJoinWorkerThread*)(IForkJoinWorkerThread::Probe(t));
        ((CForkJoinPool*)(wt->mPool.Get()))->HelpQuiescePool(wt->mWorkQueue);
    }
    else
        CForkJoinPool::QuiesceCommonPool();
}

ECode ForkJoinTask::Reinitialize()
{
    if ((mStatus & DONE_MASK) == EXCEPTIONAL)
        ClearExceptionalCompletion();
    else
        mStatus = 0;
    return NOERROR;
}

AutoPtr<IForkJoinPool> ForkJoinTask::GetPool()
{
    AutoPtr<IThread> t = Thread::GetCurrentThread();
    AutoPtr<IForkJoinWorkerThread> ft = (IForkJoinWorkerThread*)t->Probe(EIID_IForkJoinWorkerThread);
    AutoPtr<IForkJoinPool> p;
    if (ft != NULL) {
        AutoPtr<ForkJoinWorkerThread> cft = (ForkJoinWorkerThread*)ft.Get();
        p = cft->mPool;
    }
    else
        p = NULL;
    return p;
}

Boolean ForkJoinTask::InForkJoinPool()
{
    AutoPtr<IThread> t = Thread::GetCurrentThread();
    AutoPtr<IForkJoinWorkerThread> ft = (IForkJoinWorkerThread*)t->Probe(EIID_IForkJoinWorkerThread);
    return ft == NULL ? FALSE : TRUE;
}

ECode ForkJoinTask::TryUnfork(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    AutoPtr<IThread> t = Thread::GetCurrentThread();
    return ((t->Probe(EIID_IForkJoinWorkerThread) != NULL) ?
            ((ForkJoinWorkerThread*)(IForkJoinWorkerThread::Probe(t)))->mWorkQueue->TryUnpush(this) :
            ((CForkJoinPool*)(CForkJoinPool::mCommon.Get()))->TryExternalUnpush(this));
}

Int32 ForkJoinTask::GetQueuedTaskCount()
{
    AutoPtr<IThread> t = Thread::GetCurrentThread();
    AutoPtr<CForkJoinPool::WorkQueue> q;
    if (t->Probe(EIID_IForkJoinWorkerThread) != NULL)
        q = ((ForkJoinWorkerThread*)(IForkJoinWorkerThread::Probe(t)))->mWorkQueue;
    else
        q = CForkJoinPool::CommonSubmitterQueue();
    return (q == NULL) ? 0 : q->QueueSize();
}

Int32 ForkJoinTask::GetSurplusQueuedTaskCount()
{
    return CForkJoinPool::GetSurplusQueuedTaskCount();
}

AutoPtr<IForkJoinTask> ForkJoinTask::PeekNextLocalTask()
{
    AutoPtr<IThread> t = Thread::GetCurrentThread();
    AutoPtr<CForkJoinPool::WorkQueue> q;
    if (t->Probe(EIID_IForkJoinWorkerThread) != NULL)
        q = ((ForkJoinWorkerThread*)(IForkJoinWorkerThread::Probe(t)))->mWorkQueue;
    else
        q = CForkJoinPool::CommonSubmitterQueue();
    return (q == NULL) ? NULL : q->Peek();
}

AutoPtr<IForkJoinTask> ForkJoinTask::PollNextLocalTask()
{
    AutoPtr<IThread> t = Thread::GetCurrentThread();
    return (t->Probe(EIID_IForkJoinWorkerThread) != NULL) ?
        ((ForkJoinWorkerThread*)(IForkJoinWorkerThread::Probe(t)))->mWorkQueue->NextLocalTask() :
        NULL;
}

AutoPtr<IForkJoinTask> ForkJoinTask::PollTask()
{
    AutoPtr<IThread> t = Thread::GetCurrentThread();
    AutoPtr<ForkJoinWorkerThread> wt;
    return (t->Probe(EIID_IForkJoinWorkerThread) != NULL) ?
        ((CForkJoinPool*)((wt = (ForkJoinWorkerThread*)(IForkJoinWorkerThread::Probe(t)))->mPool.Get()))->NextTaskFor(wt->mWorkQueue) :
        NULL;
}

Int16 ForkJoinTask::GetForkJoinTaskTag()
{
    return (Int16)mStatus;
}

Int16 ForkJoinTask::SetForkJoinTaskTag(
    /* [in] */ Int16 tag)
{
    assert(0 && "TODO");
    return 0;
    // for (Int32 s;;) {
    //     if (U.compareAndSwapInt(this, STATUS, s = status,
    //                             (s & ~SMASK) | (tag & SMASK)))
    //         return (short)s;
    // }
}

Boolean ForkJoinTask::CompareAndSetForkJoinTaskTag(
    /* [in] */ Int16 e,
    /* [in] */ Int16 tag)
{
    for (Int32 s;;) {
        if ((Int16)(s = mStatus) != e)
            return FALSE;
        // if (U.compareAndSwapInt(this, STATUS, s,
        //                         (s & ~SMASK) | (tag & SMASK)))
        //     return true;
    }
}

AutoPtr<IForkJoinTask> ForkJoinTask::Adapt(
    /* [in] */ IRunnable* runnable)
{
    AutoPtr<AdaptedRunnableAction> p = new AdaptedRunnableAction(runnable);
    AutoPtr<IForkJoinTask> res = (IForkJoinTask*)p->Probe(EIID_IForkJoinTask);
    return res;
}

AutoPtr<IForkJoinTask> ForkJoinTask::Adapt(
    /* [in] */ IRunnable* runnable,
    /* [in] */ IInterface* result)
{
    AutoPtr<AdaptedRunnable> p = new AdaptedRunnable(runnable, result);
    AutoPtr<IForkJoinTask> res = (IForkJoinTask*)p->Probe(EIID_IForkJoinTask);
    return res;
}

AutoPtr<IForkJoinTask> ForkJoinTask::Adapt(
    /* [in] */ ICallable* callable)
{
    AutoPtr<AdaptedCallable> p = new AdaptedCallable(callable);
    AutoPtr<IForkJoinTask> res = (IForkJoinTask*)p->Probe(EIID_IForkJoinTask);
    return res;
}

void ForkJoinTask::WriteObject(
    /* [in] */ IObjectOutputStream* s)
{
    s->DefaultWriteObject();
//    s->WriteObject(GetException());
}

void ForkJoinTask::ReadObject(
    /* [in] */ IObjectInputStream* s)
{
    s->DefaultReadObject();
    AutoPtr<IInterface> ex;// = s->ReadObject();
    if (ex != NULL)
        SetExceptionalCompletion((IThrowable*)ex.Get());
}

//====================================================================
// AdaptedRunnable::
//====================================================================
CAR_INTERFACE_IMPL(AdaptedRunnable, ForkJoinTask, IRunnableFuture)

AdaptedRunnable::AdaptedRunnable(
    /* [in] */ IRunnable* runnable,
    /* [in] */ IInterface* result)
{
    if (runnable == NULL) return;// throw new NullPointerException();
    mRunnable = runnable;
    mResult = result;
}

ECode AdaptedRunnable::GetRawResult(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    *outface = mResult;
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode AdaptedRunnable::SetRawResult(
    /* [in] */ IInterface* v)
{
    mResult = v;
    return NOERROR;
}

ECode AdaptedRunnable::Exec(
    /* [out] */ Boolean* res)
{
    mRunnable->Run();
    *res = TRUE;
    return NOERROR;
}

void AdaptedRunnable::Run()
{
    AutoPtr<IInterface> p;
    Invoke((IInterface**)&p);
}

//====================================================================
// AdaptedRunnableAction::
//====================================================================
CAR_INTERFACE_IMPL(AdaptedRunnableAction, ForkJoinTask, IRunnableFuture)

AdaptedRunnableAction::AdaptedRunnableAction(
    /* [in] */ IRunnable* runnable)
{
    if (runnable == NULL) return;// E_NULL_POINTER_EXCEPTION;
    mRunnable = runnable;
}

ECode AdaptedRunnableAction::GetRawResult(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)
    *outface = NULL;
    return NOERROR;
}

ECode AdaptedRunnableAction::SetRawResult(
    /* [in] */ IInterface* v)
{
    return NOERROR;
}

ECode AdaptedRunnableAction::Exec(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    mRunnable->Run();
    *res = TRUE;
    return NOERROR;
}

void AdaptedRunnableAction::Run()
{
    AutoPtr<IInterface> p;
    Invoke((IInterface**)&p);
}

//====================================================================
// RunnableExecuteAction::
//====================================================================
RunnableExecuteAction::RunnableExecuteAction(
    /* [in] */ IRunnable* runnable)
{
    if (runnable == NULL) return;// E_NULL_POINTER_EXCEPTION;
    mRunnable = runnable;
}

ECode RunnableExecuteAction::GetRawResult(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)
    *outface = NULL;
    return NOERROR;
}

ECode RunnableExecuteAction::SetRawResult(
    /* [in] */ IInterface* v)
{
    return NOERROR;
}

ECode RunnableExecuteAction::Exec(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    mRunnable->Run();
    *res = TRUE;
    return NOERROR;
}

void RunnableExecuteAction::InternalPropagateException(
    /* [in] */ IThrowable* ex)
{
    Rethrow(ex); // rethrow outside exec() catches.
}

//====================================================================
// AdaptedCallable::
//====================================================================
AdaptedCallable::AdaptedCallable(
    /* [in] */ ICallable* callable)
{
    if (callable == NULL) return;//throw new NullPointerException();
    mCallable = callable;
}

ECode AdaptedCallable::GetRawResult(
    /* [out] */ IInterface** outface)
{
    *outface = mResult;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode AdaptedCallable::SetRawResult(
    /* [in] */ IInterface* v)
{
    mResult = v;
    return NOERROR;
}

ECode AdaptedCallable::Exec(
    /* [out] */ Boolean* res)
{
    ECode ec = mCallable->Call((IInterface**)&mResult);
    *res = FAILED(ec) ? FALSE : TRUE;
    return NOERROR;
}

void AdaptedCallable::Run()
{
    AutoPtr<IInterface> p;
    Invoke((IInterface**)&p);
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
