
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CEXECUTORCOMPLETIONSERVICE_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CEXECUTORCOMPLETIONSERVICE_H__

#include "_Elastos_Utility_Concurrent_CExecutorCompletionService.h"
#include "FutureTask.h"

using Elastos::Core::IRunnable;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CExecutorCompletionService)
    , public Object
    , public IExecutorCompletionService
{
private:
    /**
     * FutureTask extension to enqueue upon completion
     */
    class QueueingFuture
        : public FutureTask
    {
    public:
        QueueingFuture(
            /* [in] */ IRunnableFuture* task,
            /* [in] */ CExecutorCompletionService* owner)
            : FutureTask(IRunnable::Probe(task), NULL)
            , mOwner(owner)
        {
            mTask = IFuture::Probe(task);
        }

    protected:
        CARAPI_(void) Done();

    private:
        AutoPtr<IFuture> mTask;
        CExecutorCompletionService* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IExecutor* executor);

    CARAPI constructor(
        /* [in] */ IExecutor* executor,
        /* [in] */ IBlockingQueue* completionQueue);

    CARAPI Submit(
        /* [in] */ ICallable* task,
        /* [out] */ IFuture** future);

    CARAPI Submit(
        /* [in] */ IRunnable* task,
        /* [in] */ IInterface* result,
        /* [out] */ IFuture** future);

    CARAPI Take(
        /* [out] */ IFuture** future);

    CARAPI Poll(
        /* [out] */ IFuture** future);

    CARAPI Poll(
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ IFuture** future);

private:
    CARAPI_(AutoPtr<IRunnableFuture>) NewTaskFor(
        /* [in] */ ICallable* task);

    CARAPI_(AutoPtr<IRunnableFuture>) NewTaskFor(
        /* [in] */ IRunnable* task,
        /* [in] */ IInterface* result);

private:
    AutoPtr<IExecutor> mExecutor;
    AutoPtr<IAbstractExecutorService> mAes;
    AutoPtr<IBlockingQueue> mCompletionQueue;
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CEXECUTORCOMPLETIONSERVICE_H__
