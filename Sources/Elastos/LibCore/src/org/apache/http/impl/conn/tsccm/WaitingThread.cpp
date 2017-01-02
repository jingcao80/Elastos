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

#include "org/apache/http/impl/conn/tsccm/WaitingThread.h"
#include "elastos/core/Thread.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::Thread;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

WaitingThread::WaitingThread(
    /* [in] */ ICondition* cond,
    /* [in] */ RouteSpecificPool* pool)
    : mAborted(FALSE)
{
    if (cond == NULL) {
        Logger::E("WaitingThread", "Condition must not be null.");
        assert(0);
        // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mCond = cond;
    mPool = pool;
}

AutoPtr<ICondition> WaitingThread::GetCondition()
{
    return mCond;
}

AutoPtr<RouteSpecificPool> WaitingThread::GetPool()
{
    return mPool;
}

AutoPtr<IThread> WaitingThread::GetThread()
{
    return mWaiter;
}

ECode WaitingThread::Await(
    /* [in] */ IDate* deadline,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    // This is only a sanity check. We cannot synchronize here,
    // the lock would not be released on calling cond.await() below.
    if (mWaiter != NULL) {
        Logger::E("WaitingThread", "A thread is already waiting on this object.\ncaller: %p\nwaiter: %p"
                , Thread::GetCurrentThread().Get(), mWaiter.Get());
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (mAborted) {
        Logger::E("WaitingThread", "Operation interrupted");
        return E_INTERRUPTED_EXCEPTION;
    }

    mWaiter = Thread::GetCurrentThread();

    Boolean success = FALSE;
    // try {
    if (deadline != NULL) {
        mCond->AwaitUntil(deadline, &success);
    }
    else {
        mCond->Await();
        success = TRUE;
    }
    if (mAborted) {
        Logger::E("WaitingThread", "Operation interrupted");
        return E_INTERRUPTED_EXCEPTION;
    }
    // } finally {
    //     this.waiter = null;
    // }
    mWaiter = NULL;
    *result = success;
    return NOERROR;
}

ECode WaitingThread::Wakeup()
{
    // If external synchronization and pooling works properly,
    // this cannot happen. Just a sanity check.
    if (mWaiter == NULL) {
        Logger::E("WaitingThread", "Nobody waiting on this object.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // One condition might be shared by several WaitingThread instances.
    // It probably isn't, but just in case: wake all, not just one.
    mCond->SignalAll();
    return NOERROR;
}

void WaitingThread::Interrupt()
{
    mAborted = TRUE;
    mCond->SignalAll();
}

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org