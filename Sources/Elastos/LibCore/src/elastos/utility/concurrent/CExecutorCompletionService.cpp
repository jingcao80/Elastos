
#include "CExecutorCompletionService.h"
#include "CLinkedBlockingQueue.h"
#include "CFutureTask.h"
#include "AbstractExecutorService.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

void CExecutorCompletionService::QueueingFuture::Done()
{
    ICollection::Probe(mOwner->mCompletionQueue)->Add(mTask);
}

CAR_INTERFACE_IMPL(CExecutorCompletionService, Object, IExecutorCompletionService)

CAR_OBJECT_IMPL(CExecutorCompletionService)

ECode CExecutorCompletionService::constructor(
    /* [in] */ IExecutor* executor)
{
    if (executor == NULL) return E_NULL_POINTER_EXCEPTION;
    mExecutor = executor;
    mAes = (IAbstractExecutorService::Probe(executor)) ?
            IAbstractExecutorService::Probe(executor) : NULL;
    return CLinkedBlockingQueue::New((IBlockingQueue**)&mCompletionQueue);
}

ECode CExecutorCompletionService::constructor(
    /* [in] */ IExecutor* executor,
    /* [in] */ IBlockingQueue* completionQueue)
{
    if (executor == NULL || completionQueue == NULL) return E_NULL_POINTER_EXCEPTION;
    mExecutor = executor;
    mAes = (IAbstractExecutorService::Probe(executor)) ?
            IAbstractExecutorService::Probe(executor) : NULL;
    mCompletionQueue = completionQueue;
    return NOERROR;
}

AutoPtr<IRunnableFuture> CExecutorCompletionService::NewTaskFor(
    /* [in] */ ICallable* task)
{
    if (mAes == NULL) {
        AutoPtr<IRunnableFuture> f;
        CFutureTask::New(task, (IRunnableFuture**)&f);
        return f;
    }
    else {
        AbstractExecutorService* aes = (AbstractExecutorService*)mAes.Get();
        assert(aes != NULL);
        return aes->NewTaskFor(task);
    }
}

AutoPtr<IRunnableFuture> CExecutorCompletionService::NewTaskFor(
    /* [in] */ IRunnable* task,
    /* [in] */ IInterface* result)
{
    if (mAes == NULL) {
        AutoPtr<IRunnableFuture> f;
        CFutureTask::New(task, result, (IRunnableFuture**)&f);
        return f;
    }
    else {
        AbstractExecutorService* aes = (AbstractExecutorService*)mAes.Get();
        assert(aes != NULL);
        return aes->NewTaskFor(task, result);
    }
}

ECode CExecutorCompletionService::Submit(
    /* [in] */ ICallable* task,
    /* [out] */ IFuture** future)
{
    VALIDATE_NOT_NULL(future);
    if (task == NULL) return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IRunnableFuture> f = NewTaskFor(task);
    AutoPtr<IRunnable> r = new QueueingFuture(f, this);
    mExecutor->Execute(r);
    *future = (IFuture*)f.Get();
    REFCOUNT_ADD(*future);
    return NOERROR;
}

ECode CExecutorCompletionService::Submit(
    /* [in] */ IRunnable* task,
    /* [in] */ IInterface* result,
    /* [out] */ IFuture** future)
{
    VALIDATE_NOT_NULL(future);
    if (task == NULL) return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IRunnableFuture> f = NewTaskFor(task, result);
    AutoPtr<IRunnable> r = new QueueingFuture(f, this);
    mExecutor->Execute(r);
    *future = (IFuture*)f.Get();
    REFCOUNT_ADD(*future);
    return NOERROR;
}

ECode CExecutorCompletionService::Take(
    /* [out] */ IFuture** future)
{
    VALIDATE_NOT_NULL(future)
    AutoPtr<IInterface> e;
    FAIL_RETURN(mCompletionQueue->Take((IInterface**)&e));
    *future = IFuture::Probe(e);
    REFCOUNT_ADD(*future);
    return NOERROR;
}

ECode CExecutorCompletionService::Poll(
    /* [out] */ IFuture** future)
{
    VALIDATE_NOT_NULL(future)
    AutoPtr<IInterface> e;
    FAIL_RETURN(IQueue::Probe(mCompletionQueue)->Poll((IInterface**)&e));
    *future = IFuture::Probe(e);
    REFCOUNT_ADD(*future);
    return NOERROR;
}

ECode CExecutorCompletionService::Poll(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IFuture** future)
{
    VALIDATE_NOT_NULL(future)
    AutoPtr<IInterface> e;
    FAIL_RETURN(mCompletionQueue->Poll(timeout, unit, (IInterface**)&e));
    *future = IFuture::Probe(e);
    REFCOUNT_ADD(*future);
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
