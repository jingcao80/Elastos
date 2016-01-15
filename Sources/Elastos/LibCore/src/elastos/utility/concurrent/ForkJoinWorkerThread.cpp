
#include "ForkJoinWorkerThread.h"
#include "CForkJoinPool.h"

using Elastos::Utility::Concurrent::CForkJoinPool;

namespace Elastos {
namespace Utility {
namespace Concurrent {

//====================================================================
// ForkJoinWorkerThread::
//====================================================================
CAR_INTERFACE_IMPL(ForkJoinWorkerThread, Thread, IForkJoinWorkerThread)

ForkJoinWorkerThread::ForkJoinWorkerThread(
    /* [in] */ IForkJoinPool* pool)
{
    // Use a placeholder until a useful name can be set in registerWorker
    Thread::constructor(String("aForkJoinWorkerThread"));
    mPool = pool;
    AutoPtr<CForkJoinPool> cpool = (CForkJoinPool*)pool;
    mWorkQueue = cpool->RegisterWorker(this);
}

ECode ForkJoinWorkerThread::GetPool(
    /* [out] */ IForkJoinPool** outpool)
{
    VALIDATE_NOT_NULL(outpool)
    *outpool = mPool;
    REFCOUNT_ADD(*outpool)
    return NOERROR;
}

ECode ForkJoinWorkerThread::GetPoolIndex(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mWorkQueue->mPoolIndex >> 1; // ignore odd/even tag bit
    return NOERROR;
}

void ForkJoinWorkerThread::OnStart()
{
}

void ForkJoinWorkerThread::OnTermination(
    /* [in] */ IThrowable* exception)
{
}

ECode ForkJoinWorkerThread::Run()
{
    AutoPtr<IThrowable> exception = NULL;
//    try {
        OnStart();
        ((CForkJoinPool*)mPool.Get())->RunWorker(mWorkQueue);
//    } catch (Throwable ex) {
//        exception = ex;
//    } finally {
//        try {
            OnTermination(exception);
//        } catch (Throwable ex) {
//            if (exception == null)
//                exception = ex;
//        } finally {
            ((CForkJoinPool*)mPool.Get())->DeregisterWorker(this, exception);
//        }
//    }
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
