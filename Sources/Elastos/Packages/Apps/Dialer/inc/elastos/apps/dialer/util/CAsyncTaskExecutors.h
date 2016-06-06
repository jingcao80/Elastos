#ifndef __ELASTOS_APPS_DIALER_UTIL_CASYNCTASKEXECUTORS_H__
#define __ELASTOS_APPS_DIALER_UTIL_CASYNCTASKEXECUTORS_H__

#include "_Elastos_Apps_Dialer_Util_CAsyncTaskExecutors.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Util {

CarClass(CAsyncTaskExecutors)
    , public Singleton
    , public IAsyncTaskExecutors
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Creates an AsyncTaskExecutor that submits tasks to run with
     * {@link AsyncTask#SERIAL_EXECUTOR}.
     */
    CARAPI CreateAsyncTaskExecutor(
        /* [out] */ IAsyncTaskExecutor** executor);

    /**
     * Creates an AsyncTaskExecutor that submits tasks to run with
     * {@link AsyncTask#THREAD_POOL_EXECUTOR}.
     */
    CARAPI CreateThreadPoolExecutor(
        /* [out] */ IAsyncTaskExecutor** executor);

    // @NeededForTesting
    CARAPI SetFactoryForTest(
        /* [in] */ IAsyncTaskExecutorFactory* factory);

    CARAPI CheckCalledFromUiThread();
};

} // Util
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_UTIL_CASYNCTASKEXECUTORS_H__
