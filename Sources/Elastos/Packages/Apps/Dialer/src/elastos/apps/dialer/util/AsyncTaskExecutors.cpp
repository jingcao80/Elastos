
#include "AsyncTaskExecutors.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Util {

//=================================================================
// AsyncTaskExecutors::SimpleAsyncTaskExecutor
//=================================================================
CAR_INTERFACE_IMPL(AsyncTaskExecutors::SimpleAsyncTaskExecutor, Object, IAsyncTaskExecutor)

AsyncTaskExecutors::SimpleAsyncTaskExecutor::SimpleAsyncTaskExecutor(
    /* [in] */ IExecutor* executor)
    : mExecutor(executor)
{}

ECode AsyncTaskExecutors::SimpleAsyncTaskExecutor::Submit(
    /* [in] */ IInterface* identifier,
    /* [in] */ IObject* task,
    /* [in] */ ArrayOf<IInteface*>* params,
    /* [out] */ IObject** resultTask)
{
    VALUE_NOT_NULL(resultTask);

    AsyncTaskExecutors::CheckCalledFromUiThread();
    return ((AsyncTask*)task)->ExecuteOnExecutor(mExecutor, params);
}

//=================================================================
// AsyncTaskExecutors
//=================================================================
AutoPtr<IAsyncTaskExecutorFactory> AsyncTaskExecutors::mInjectedAsyncTaskExecutorFactory;
Object AsyncTaskExecutors::sSyncObject;

AutoPtr<IAsyncTaskExecutor> AsyncTaskExecutors::CreateAsyncTaskExecutor()
{
    synchronized (sSyncObject) {
        if (mInjectedAsyncTaskExecutorFactory != NULL) {
            AutoPtr<IAsyncTaskExecutor> executor;
            mInjectedAsyncTaskExecutorFactory->CreateAsyncTaskExecutor(
                    (IAsyncTaskExecutor**)&executor);
            return executor;
        }
        AutoPtr<SimpleAsyncTaskExecutor> simpleExecutor = new SimpleAsyncTaskExecutor(
                AsyncTask::SERIAL_EXECUTOR);
        return (IAsyncTaskExecutor*)simpleExecutor;
    }
}

AutoPtr<IAsyncTaskExecutor> AsyncTaskExecutors::CreateThreadPoolExecutor()
{
    synchronized (sSyncObject) {
        if (mInjectedAsyncTaskExecutorFactory != null) {
            utoPtr<IAsyncTaskExecutor> executor;
            mInjectedAsyncTaskExecutorFactory->CreateAsyncTaskExecutor(
                    (IAsyncTaskExecutor**)&executor);
            return executor;
        }
        AutoPtr<SimpleAsyncTaskExecutor> simpleExecutor = new SimpleAsyncTaskExecutor(
                AsyncTask::THREAD_POOL_EXECUTOR);
        return (IAsyncTaskExecutor*)simpleExecutor;
    }
}

void AsyncTaskExecutors::SetFactoryForTest(
    /* [in] */ IAsyncTaskExecutorFactory* factory)
{
    synchronized (sSyncObject) {
        mInjectedAsyncTaskExecutorFactory = factory;
    }
}

void AsyncTaskExecutors::CheckCalledFromUiThread()
{
    AutoPtr<ILooperHelper> helper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&helper);
    AutoPtr<ILooper> looper;
    helper->GetMainLooper((ILooper**)&looper);
    AutoPtr<IThread> thread;
    looper->GetThread(&thread);
    AutoPtr<IThread> currentThread = Thread.GetCurrentThread();
    String str;
    currentThread->ToString(&str);

    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    preconditions->CheckState(currentThread == thread);
}

} // Util
} // Dialer
} // Apps
} // Elastos
