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

#include "CExecutors.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

CAR_SINGLETON_IMPL(CExecutors)

CAR_INTERFACE_IMPL(CExecutors, Singleton, IExecutors)

ECode CExecutors::Callable(
    /* [in] */ IRunnable* task,
    /* [in] */ IInterface* result,
    /* [out] */ ICallable** object)
{
    return Executors::Callable(task, result, object);
}

ECode CExecutors::NewFixedThreadPool(
    /* [in] */ Int32 nThreads,
    /* [out] */ IExecutorService** result)
{
    return Executors::NewFixedThreadPool(nThreads, result);
}

ECode CExecutors::NewWorkStealingPool(
    /* [in] */ Int32 parallelism,
    /* [out] */ IExecutorService** result)
{
    return Executors::NewWorkStealingPool(parallelism, result);
}

ECode CExecutors::NewWorkStealingPool(
    /* [out] */ IExecutorService** result)
{
    return Executors::NewWorkStealingPool(result);
}

ECode CExecutors::NewFixedThreadPool(
    /* [in] */ Int32 nThreads,
    /* [in] */ IThreadFactory* threadFactory,
    /* [out] */ IExecutorService** result)
{
    return Executors::NewFixedThreadPool(nThreads, threadFactory, result);
}

ECode CExecutors::NewSingleThreadExecutor(
    /* [out] */ IExecutorService** result)
{
    return Executors::NewSingleThreadExecutor(result);
}

ECode CExecutors::NewSingleThreadExecutor(
    /* [in] */ IThreadFactory* threadFactory,
    /* [out] */ IExecutorService** result)
{
    return Executors::NewSingleThreadExecutor(threadFactory, result);
}

ECode CExecutors::NewCachedThreadPool(
    /* [out] */ IExecutorService** result)
{
    return Executors::NewCachedThreadPool(result);
}

ECode CExecutors::NewCachedThreadPool(
    /* [in] */ IThreadFactory* threadFactory,
    /* [out] */ IExecutorService** result)
{
    return Executors::NewCachedThreadPool(threadFactory, result);
}

ECode CExecutors::NewSingleThreadScheduledExecutor(
    /* [out] */ IScheduledExecutorService** result)
{
    return Executors::NewSingleThreadScheduledExecutor(result);
}

ECode CExecutors::NewSingleThreadScheduledExecutor(
    /* [in] */ IThreadFactory* threadFactory,
    /* [out] */ IScheduledExecutorService** result)
{
    return Executors::NewSingleThreadScheduledExecutor(threadFactory, result);
}

ECode CExecutors::NewScheduledThreadPool(
    /* [in] */ Int32 corePoolSize,
    /* [out] */ IScheduledExecutorService** result)
{
    return Executors::NewScheduledThreadPool(corePoolSize, result);
}

ECode CExecutors::NewScheduledThreadPool(
    /* [in] */ Int32 corePoolSize,
    /* [in] */ IThreadFactory* threadFactory,
    /* [out] */ IScheduledExecutorService** result)
{
    return Executors::NewScheduledThreadPool(corePoolSize, threadFactory, result);
}

ECode CExecutors::UnconfigurableExecutorService(
    /* [in] */ IExecutorService* executor,
    /* [out] */ IExecutorService** result)
{
    return Executors::UnconfigurableExecutorService(executor, result);
}

ECode CExecutors::UnconfigurableScheduledExecutorService(
    /* [in] */ IScheduledExecutorService* executor,
    /* [out] */ IScheduledExecutorService** result)
{
    return Executors::UnconfigurableScheduledExecutorService(executor, result);
}

ECode CExecutors::PrivilegedThreadFactory(
    /* [out] */ IThreadFactory** result)
{
    return Executors::PrivilegedThreadFactory(result);
}

ECode CExecutors::Callable(
    /* [in] */ IRunnable* task,
    /* [out] */ ICallable** result)
{
    return Executors::Callable(task, result);
}

ECode CExecutors::Callable(
    /* [in] */ IPrivilegedAction* action,
    /* [out] */ ICallable** result)
{
    return Executors::Callable(action, result);
}

ECode CExecutors::Callable(
    /* [in] */ IPrivilegedExceptionAction* action,
    /* [out] */ ICallable** result)
{
    return Executors::Callable(action, result);
}

ECode CExecutors::PrivilegedCallable(
    /* [in] */ ICallable* callable,
    /* [out] */ ICallable** result)
{
    return Executors::PrivilegedCallable(callable, result);
}

ECode CExecutors::PrivilegedCallableUsingCurrentClassLoader(
    /* [in] */ ICallable* callable,
    /* [out] */ ICallable** result)
{
    return Executors::PrivilegedCallableUsingCurrentClassLoader(callable, result);
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
