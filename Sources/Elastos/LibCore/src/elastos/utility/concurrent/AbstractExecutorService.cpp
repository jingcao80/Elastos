
#include "AbstractExecutorService.h"
#include "CFutureTask.h"
#include "TimeUnit.h"
#include "CExecutorCompletionService.h"
#include "CSystem.h"
#include "CArrayList.h"

using Elastos::Core::ISystem;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CAR_INTERFACE_IMPL(AbstractExecutorService, Object, IAbstractExecutorService)

AutoPtr<IRunnableFuture> AbstractExecutorService::NewTaskFor(
    /* [in] */ IRunnable* runnable,
    /* [in] */ IInterface* value)
{
    AutoPtr<IRunnableFuture> task;
    CFutureTask::New(runnable, value, (IRunnableFuture**)&task);
    return task;
}

AutoPtr<IRunnableFuture> AbstractExecutorService::NewTaskFor(
    /* [in] */ ICallable* callable)
{
    AutoPtr<IRunnableFuture> task;
    CFutureTask::New(callable, (IRunnableFuture**)&task);
    return task;
}

ECode AbstractExecutorService::Submit(
    /* [in] */ IRunnable* task,
    /* [out] */ IFuture** future)
{
    VALIDATE_NOT_NULL(future);
    *future = NULL;
    if (task == NULL) return E_NULL_POINTER_EXCEPTION;

    AutoPtr<IRunnableFuture> ftask = NewTaskFor(task, NULL);
    Execute(IRunnable::Probe(ftask));
    *future = IFuture::Probe(ftask);
    REFCOUNT_ADD(*future);
    return NOERROR;
}

ECode AbstractExecutorService::Submit(
    /* [in] */ IRunnable* task,
    /* [in] */ IInterface* result,
    /* [out] */ IFuture** future)
{
    VALIDATE_NOT_NULL(future);
    *future = NULL;
    if (task == NULL) return E_NULL_POINTER_EXCEPTION;

    AutoPtr<IRunnableFuture> ftask = NewTaskFor(task, result);
    Execute(IRunnable::Probe(ftask));
    *future = IFuture::Probe(ftask);
    REFCOUNT_ADD(*future);
    return NOERROR;
}

ECode AbstractExecutorService::Submit(
    /* [in] */ ICallable* task,
    /* [out] */ IFuture** future)
{
    VALIDATE_NOT_NULL(future);
    *future = NULL;
    if (task == NULL) return E_NULL_POINTER_EXCEPTION;

    AutoPtr<IRunnableFuture> ftask = NewTaskFor(task);
    Execute(IRunnable::Probe(ftask));
    *future = IFuture::Probe(ftask);
    REFCOUNT_ADD(*future);
    return NOERROR;
}

ECode AbstractExecutorService::DoInvokeAny(
    /* [in] */ ICollection* tasks,
    /* [in] */ Boolean timed,
    /* [in] */ Int64 nanos,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    if (tasks == NULL) return E_NULL_POINTER_EXCEPTION;
    Int32 ntasks;
    tasks->GetSize(&ntasks);
    if (ntasks == 0) return E_ILLEGAL_ARGUMENT_EXCEPTION;

    AutoPtr<IArrayList> futures;
    CArrayList::New(ntasks, (IArrayList**)&futures);
    AutoPtr<ICompletionService> ecs;
    CExecutorCompletionService::New(THIS_PROBE(IExecutor), (ICompletionService**)&ecs);

    // For efficiency, especially in executors with limited
    // parallelism, check to see if previously submitted tasks are
    // done before submitting more of them. This interleaving
    // plus the exception mechanics account for messiness of main
    // loop.

    // try {
    // Record exceptions so that if we fail to obtain any
    // result, we can throw the last exception we got.
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    ECode ec = NOERROR;
    Int64 lastTime = 0;
    if (timed) {
        system->GetNanoTime(&lastTime);
    }
    AutoPtr<IIterator> it;
    tasks->GetIterator((IIterator**)&it);

    // Start one task for sure; the rest incrementally
    AutoPtr<IInterface> obj;
    it->GetNext((IInterface**)&obj);
    AutoPtr<IFuture> f;
    ecs->Submit(ICallable::Probe(obj), (IFuture**)&f);
    (IList::Probe(futures))->Add(f);
    --ntasks;
    Int32 active = 1;

    for (;;) {
        f = NULL;
        ecs->Poll((IFuture**)&f);
        if (f == NULL) {
            if (ntasks > 0) {
                --ntasks;
                obj = NULL;
                it->GetNext((IInterface**)&obj);
                ecs->Submit(ICallable::Probe(obj), (IFuture**)&f);
                (IList::Probe(futures))->Add(f);
                ++active;
            }
            else if (active == 0) break;
            else if (timed) {
                AutoPtr<ITimeUnit> unit = TimeUnit::GetNANOSECONDS();
                ecs->Poll(nanos, unit, (IFuture**)&f);
                if (f == NULL) {
                    ec = E_TIMEOUT_EXCEPTION;
                    goto EXIT;
                }
                Int64 now;
                system->GetNanoTime(&now);
                nanos -= now - lastTime;
                lastTime = now;
            }
            else {
                ecs->Take((IFuture**)&f);
            }
        }
        if (f != NULL) {
            --active;
            // try {
            ec = f->Get(result);
            goto EXIT;
            // } catch (ExecutionException eex) {
            //     ee = eex;
            // } catch (RuntimeException rex) {
            //     ee = new ExecutionException(rex);
            // }
        }
    }

    if (ec == NOERROR) ec = E_EXECUTION_EXCEPTION;
EXIT:
    // } finally {
    Int32 size;
    (ICollection::Probe(futures))->GetSize(&size);
    Boolean bval;
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> p;
        (IList::Probe(futures))->Get(i, (IInterface**)&p);
        AutoPtr<IFuture> n = IFuture::Probe(p);
        n->Cancel(TRUE, &bval);
    }
    // }
    return ec;
}

ECode AbstractExecutorService::InvokeAny(
    /* [in] */ ICollection* tasks,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // try {
    ECode ec = DoInvokeAny(tasks, FALSE, 0, result);
    if (FAILED(ec)) *result = NULL;
    return ec;
    // } catch (TimeoutException cannotHappen) {
    //     assert false;
    //     return null;
    // }
}

ECode AbstractExecutorService::InvokeAny(
    /* [in] */ ICollection* tasks,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    return DoInvokeAny(tasks, TRUE, nanos, result);
}

ECode AbstractExecutorService::InvokeAll(
    /* [in] */ ICollection* tasks,
    /* [out] */ IList** futures)
{
    VALIDATE_NOT_NULL(futures);
    *futures = NULL;
    if (tasks == NULL) return E_NULL_POINTER_EXCEPTION;

    Int32 size = 0;
    tasks->GetSize(&size);
    AutoPtr<IArrayList> res;
    CArrayList::New(size, (IArrayList**)&res);
    Boolean done = FALSE;
    // try {
    AutoPtr< ArrayOf<IInterface*> > taskArray;
    Boolean b = FALSE;
    tasks->ToArray((ArrayOf<IInterface*>**)&taskArray);
    for (Int32 i = 0; i < taskArray->GetLength(); i++) {
        AutoPtr<ICallable> t = ICallable::Probe((*taskArray)[i]);
        AutoPtr<IRunnableFuture> f = NewTaskFor(t);
        (IList::Probe(res))->Add(f, &b);
        Execute(IRunnable::Probe(f));
    }

    Boolean isDone;
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        (IList::Probe(res))->Get(i, (IInterface**)&obj);
        AutoPtr<IFuture> f = IFuture::Probe(obj);
        if (f->IsDone(&isDone), !isDone) {
            // try {
            AutoPtr<IInterface> result;
            f->Get((IInterface**)&result);
            // } catch (CancellationException ignore) {
            // } catch (ExecutionException ignore) {
            // }
        }
    }
    done = TRUE;
    // } finally {
    if (!done) {
        Boolean result;
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            (IList::Probe(res))->Get(i, (IInterface**)&obj);
            AutoPtr<IFuture> f = IFuture::Probe(obj);
            f->Cancel(TRUE, &result);
        }
    }
    // }
    *futures = IList::Probe(res);;
    REFCOUNT_ADD(*futures);
    return NOERROR;
}

ECode AbstractExecutorService::InvokeAll(
    /* [in] */ ICollection* tasks,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IList** futures)
{
    VALIDATE_NOT_NULL(futures);
    *futures = NULL;
    if (tasks == NULL || unit == NULL) return E_NULL_POINTER_EXCEPTION;

    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    Int32 size = 0;
    tasks->GetSize(&size);
    AutoPtr<IArrayList> res;
    CArrayList::New(size, (IArrayList**)&res);
    Boolean done = FALSE;
    // try {
    AutoPtr< ArrayOf<IInterface*> > taskArray;
    Boolean b = FALSE;
    tasks->ToArray((ArrayOf<IInterface*>**)&taskArray);
    for (Int32 i = 0; i < taskArray->GetLength(); i++) {
        AutoPtr<ICallable> t = ICallable::Probe((*taskArray)[i]);
        AutoPtr<IRunnableFuture> f = NewTaskFor(t);
        (IList::Probe(res))->Add(f, &b);
    }

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    Int64 lastTime;
    system->GetNanoTime(&lastTime);

    // Interleave time checks and calls to execute in case
    // executor doesn't have any/much parallelism.
    AutoPtr<IIterator> it;
    (IIterable::Probe(res))->GetIterator((IIterator**)&it);
    Boolean hasNext = FALSE;
    Int64 now;
    while ((it->HasNext(&hasNext), hasNext)) {
        AutoPtr<IInterface> nxt;
        it->GetNext((IInterface**)&nxt);
        AutoPtr<IRunnable> p = IRunnable::Probe(nxt);;
        Execute(p);

        system->GetNanoTime(&now);
        nanos -= now - lastTime;
        lastTime = now;
        if (nanos <= 0) goto EXIT;
    }

    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        (IList::Probe(res))->Get(i, (IInterface**)&obj);
        AutoPtr<IFuture> f = IFuture::Probe(obj);
        Boolean isDone;
        if (f->IsDone(&isDone), !isDone) {
            if (nanos <= 0) goto EXIT;
            // try {
            AutoPtr<ITimeUnit> unit = TimeUnit::GetNANOSECONDS();
            AutoPtr<IInterface> result;
            ECode ec = f->Get(nanos, unit, (IInterface**)&result);
            if (FAILED(ec)) goto EXIT;
            // } catch (CancellationException ignore) {
            // } catch (ExecutionException ignore) {
            // } catch (TimeoutException toe) {
            //     return futures;
            // }
            system->GetNanoTime(&now);
            nanos -= now - lastTime;
            lastTime = now;
        }
    }
    done = TRUE;

EXIT:
    // } finally {
    if (!done) {
        Boolean result;
       for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            (IList::Probe(res))->Get(i, (IInterface**)&obj);
            AutoPtr<IFuture> f = IFuture::Probe(obj);
            f->Cancel(TRUE, &result);
        }
    }
    // }
    *futures = IList::Probe(res);;
    REFCOUNT_ADD(*futures);
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
