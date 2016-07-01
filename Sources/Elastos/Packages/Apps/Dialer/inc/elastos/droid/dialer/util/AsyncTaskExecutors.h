#ifndef __ELASTOS_DROID_DIALER_UTIL_ASYNCTASKEXECUTORS_H__
#define __ELASTOS_DROID_DIALER_UTIL_ASYNCTASKEXECUTORS_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Utility::Concurrent::IExecutor;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Util {

/**
 * Factory methods for creating AsyncTaskExecutors.
 * <p>
 * All of the factory methods on this class check first to see if you have set a static
 * {@link AsyncTaskExecutorFactory} set through the
 * {@link #setFactoryForTest(AsyncTaskExecutorFactory)} method, and if so delegate to that instead,
 * which is one way of injecting dependencies for testing classes whose construction cannot be
 * controlled such as {@link android.app.Activity}.
 */
class AsyncTaskExecutors
    : public Object
{
private:
    class SimpleAsyncTaskExecutor
        : public Object
        , public IAsyncTaskExecutor
    {
    public:
        CAR_INTERFACE_DECL();

        SimpleAsyncTaskExecutor(
            /* [in] */ IExecutor* executor);

        CARAPI Submit(
            /* [in] */ IInterface* identifier,
            /* [in] */ IObject* task,
            /* [in] */ ArrayOf<IInteface*>* params,
            /* [out] */ IObject** resultTask);

    private:
        AutoPtr<IExecutor> mExecutor;
    };

public:
    /**
     * Creates an AsyncTaskExecutor that submits tasks to run with
     * {@link AsyncTask#SERIAL_EXECUTOR}.
     */
    static CARAPI_(AutoPtr<IAsyncTaskExecutor>) CreateAsyncTaskExecutor();

    /**
     * Creates an AsyncTaskExecutor that submits tasks to run with
     * {@link AsyncTask#THREAD_POOL_EXECUTOR}.
     */
    static CARAPI_(AutoPtr<IAsyncTaskExecutor>) CreateThreadPoolExecutor();

    // @NeededForTesting
    static CARAPI_(void) SetFactoryForTest(
        /* [in] */ IAsyncTaskExecutorFactory* factory);

    static CARAPI_(void) CheckCalledFromUiThread();

private:
    /**
     * A single instance of the {@link AsyncTaskExecutorFactory}, to which we delegate if it is
     * non-null, for injecting when testing.
     */
    static AutoPtr<IAsyncTaskExecutorFactory> mInjectedAsyncTaskExecutorFactory;
    static Object sSyncObject;
};

} // Util
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_UTIL_ASYNCTASKEXECUTORS_H__
