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

#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_ABSTRACTEXECUTORSERVICE_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_ABSTRACTEXECUTORSERVICE_H__

#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include <elastos/core/Object.h>

using Elastos::Core::IRunnable;
using Elastos::Core::Object;

namespace Elastos {
namespace Utility {
namespace Concurrent {

/**
 * Provides default implementations of {@link ExecutorService}
 * execution methods. This class implements the <tt>submit</tt>,
 * <tt>invokeAny</tt> and <tt>invokeAll</tt> methods using a
 * {@link RunnableFuture} returned by <tt>newTaskFor</tt>, which defaults
 * to the {@link FutureTask} class provided in this package.  For example,
 * the implementation of <tt>submit(Runnable)</tt> creates an
 * associated <tt>RunnableFuture</tt> that is executed and
 * returned. Subclasses may override the <tt>newTaskFor</tt> methods
 * to return <tt>RunnableFuture</tt> implementations other than
 * <tt>FutureTask</tt>.
 *
 * <p> <b>Extension example</b>. Here is a sketch of a class
 * that customizes {@link ThreadPoolExecutor} to use
 * a <tt>CustomTask</tt> class instead of the default <tt>FutureTask</tt>:
 *  <pre> {@code
 * public class CustomThreadPoolExecutor extends ThreadPoolExecutor {
 *
 *   static class CustomTask<V> implements RunnableFuture<V> {...}
 *
 *   protected <V> RunnableFuture<V> newTaskFor(Callable<V> c) {
 *       return new CustomTask<V>(c);
 *   }
 *   protected <V> RunnableFuture<V> newTaskFor(Runnable r, V v) {
 *       return new CustomTask<V>(r, v);
 *   }
 *   // ... add constructors, etc.
 * }}</pre>
 *
 * @since 1.5
 * @author Doug Lea
 */
class AbstractExecutorService
    : public Object
    , public IAbstractExecutorService
    , public IExecutorService
    , public IExecutor
{
    friend class CExecutorCompletionService;

public:
    CAR_INTERFACE_DECL()

    virtual CARAPI Submit(
        /* [in] */ IRunnable* task,
        /* [out] */ IFuture** future);

    virtual CARAPI Submit(
        /* [in] */ IRunnable* task,
        /* [in] */ IInterface* result,
        /* [out] */ IFuture** future);

    virtual CARAPI Submit(
        /* [in] */ ICallable* task,
        /* [out] */ IFuture** future);

    virtual CARAPI InvokeAny(
        /* [in] */ ICollection* tasks,
        /* [out] */ IInterface** result);

    virtual CARAPI InvokeAny(
        /* [in] */ ICollection* tasks,
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ IInterface** result);

    virtual CARAPI InvokeAll(
        /* [in] */ ICollection* tasks,
        /* [out] */ IList** futures);

    virtual CARAPI InvokeAll(
        /* [in] */ ICollection* tasks,
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ IList** futures);

    virtual CARAPI Execute(
        /* [in] */ IRunnable* command) = 0;

protected:
    /**
     * Returns a <tt>RunnableFuture</tt> for the given runnable and default
     * value.
     *
     * @param runnable the runnable task being wrapped
     * @param value the default value for the returned future
     * @return a <tt>RunnableFuture</tt> which when run will run the
     * underlying runnable and which, as a <tt>Future</tt>, will yield
     * the given value as its result and provide for cancellation of
     * the underlying task.
     * @since 1.6
     */
    virtual CARAPI_(AutoPtr<IRunnableFuture>) NewTaskFor(
        /* [in] */ IRunnable* runnable,
        /* [in] */ IInterface* value);

    /**
     * Returns a <tt>RunnableFuture</tt> for the given callable task.
     *
     * @param callable the callable task being wrapped
     * @return a <tt>RunnableFuture</tt> which when run will call the
     * underlying callable and which, as a <tt>Future</tt>, will yield
     * the callable's result as its result and provide for
     * cancellation of the underlying task.
     * @since 1.6
     */
    virtual CARAPI_(AutoPtr<IRunnableFuture>) NewTaskFor(
        /* [in] */ ICallable* callable);

private:
    /**
     * the main mechanics of invokeAny.
     */
    CARAPI DoInvokeAny(
        /* [in] */ ICollection* tasks,
        /* [in] */ Boolean timed,
        /* [in] */ Int64 nanos,
        /* [out] */ IInterface** result);
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_ABSTRACTEXECUTORSERVICE_H__
