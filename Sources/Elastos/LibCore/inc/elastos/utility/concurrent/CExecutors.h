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

#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CEXECUTORS_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CEXECUTORS_H__

#include "_Elastos_Utility_Concurrent_CExecutors.h"
#include "Executors.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CExecutors)
    , public Singleton
    , public IExecutors
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Callable(
        /* [in] */ IRunnable* task,
        /* [in] */ IInterface* result,
        /* [out] */ ICallable** object);

    CARAPI NewFixedThreadPool(
        /* [in] */ Int32 nThreads,
        /* [out] */ IExecutorService** result);

    CARAPI NewWorkStealingPool(
        /* [in] */ Int32 parallelism,
        /* [out] */ IExecutorService** result);

    CARAPI NewWorkStealingPool(
        /* [out] */ IExecutorService** result);

    CARAPI NewFixedThreadPool(
        /* [in] */ Int32 nThreads,
        /* [in] */ IThreadFactory* threadFactory,
        /* [out] */ IExecutorService** result);

    CARAPI NewSingleThreadExecutor(
        /* [out] */ IExecutorService** result);

    CARAPI NewSingleThreadExecutor(
        /* [in] */ IThreadFactory* threadFactory,
        /* [out] */ IExecutorService** result);

    CARAPI NewCachedThreadPool(
        /* [out] */ IExecutorService** result);

    CARAPI NewCachedThreadPool(
        /* [in] */ IThreadFactory* threadFactory,
        /* [out] */ IExecutorService** result);

    CARAPI NewSingleThreadScheduledExecutor(
        /* [out] */ IScheduledExecutorService** result);

    CARAPI NewSingleThreadScheduledExecutor(
        /* [in] */ IThreadFactory* threadFactory,
        /* [out] */ IScheduledExecutorService** result);

    CARAPI NewScheduledThreadPool(
        /* [in] */ Int32 corePoolSize,
        /* [out] */ IScheduledExecutorService** result);

    CARAPI NewScheduledThreadPool(
        /* [in] */ Int32 corePoolSize,
        /* [in] */ IThreadFactory* threadFactory,
        /* [out] */ IScheduledExecutorService** result);

    CARAPI UnconfigurableExecutorService(
        /* [in] */ IExecutorService* executor,
        /* [out] */ IExecutorService** result);

    CARAPI UnconfigurableScheduledExecutorService(
        /* [in] */ IScheduledExecutorService* executor,
        /* [out] */ IScheduledExecutorService** result);

    CARAPI PrivilegedThreadFactory(
        /* [out] */ IThreadFactory** result);

    CARAPI Callable(
        /* [in] */ IRunnable* task,
        /* [out] */ ICallable** result);

    CARAPI Callable(
        /* [in] */ IPrivilegedAction* action,
        /* [out] */ ICallable** result);

    CARAPI Callable(
        /* [in] */ IPrivilegedExceptionAction* action,
        /* [out] */ ICallable** result);

    CARAPI PrivilegedCallable(
        /* [in] */ ICallable* callable,
        /* [out] */ ICallable** result);

    CARAPI PrivilegedCallableUsingCurrentClassLoader(
        /* [in] */ ICallable* callable,
        /* [out] */ ICallable** result);
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CEXECUTORS_H__
