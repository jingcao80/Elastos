//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
            /* [in] */ IInterface* task,
            /* [in] */ ArrayOf<IInterface*>* params);

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
    static AutoPtr<IAsyncTaskExecutorFactory> sInjectedAsyncTaskExecutorFactory;
    static Object sSyncObject;
};

} // Util
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_UTIL_ASYNCTASKEXECUTORS_H__
