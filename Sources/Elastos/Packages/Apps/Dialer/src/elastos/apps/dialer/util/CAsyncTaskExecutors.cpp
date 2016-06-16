
#include "util/CAsyncTaskExecutors.h"
#include "util/AsyncTaskExecutors.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Util {

CAR_INTERFACE_IMPL(CAsyncTaskExecutors, Singleton, IAsyncTaskExecutors);

CAR_SINGLETON_IMPL(CAsyncTaskExecutors);

ECode CAsyncTaskExecutors::CreateAsyncTaskExecutor(
    /* [out] */ IAsyncTaskExecutor** executor)
{
    VALUE_NOT_NULL(executor);
    AutoPtr<IAsyncTaskExecutor> result = AsyncTaskExecutors::CreateAsyncTaskExecutor();
    *executor = result;
    REFCOUNT_ADD(*executor);
    return NOERROR;
}

ECode CAsyncTaskExecutors::CreateThreadPoolExecutor(
    /* [out] */ IAsyncTaskExecutor** executor)
{
    VALUE_NOT_NULL(executor);
    AutoPtr<IAsyncTaskExecutor> result = AsyncTaskExecutors::CreateThreadPoolExecutor();
    *executor = result;
    REFCOUNT_ADD(*executor);
    return NOERROR;
}

ECode CAsyncTaskExecutors::SetFactoryForTest(
    /* [in] */ IAsyncTaskExecutorFactory* factory)
{
    AsyncTaskExecutors::SetFactoryForTest(factory);
    return NOERROR;
}

ECode CAsyncTaskExecutors::CheckCalledFromUiThread()
{
    AsyncTaskExecutors::CheckCalledFromUiThread(factory);
    return NOERROR;
}

} // Util
} // Dialer
} // Apps
} // Elastos
