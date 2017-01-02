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
