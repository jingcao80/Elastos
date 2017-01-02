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

#include "LockSupport.h"
#include <Thread.h>
#include <cutils/atomic.h>

using Elastos::Core::Thread;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Locks {

ECode LockSupport::Unpark(
    /* [in] */ IThread* thread)
{
    if (thread != NULL) {
        return thread->Unpark();
    }
    return NOERROR;
}

ECode LockSupport::Park(
    /* [in] */ IInterface* blocker)
{
    AutoPtr<IThread> t = Thread::GetCurrentThread();
    SetBlocker(t, blocker);
    ECode ec = t->ParkFor(0);
    SetBlocker(t, NULL);
    return ec;
}

ECode LockSupport::ParkNanos(
    /* [in] */ IInterface* blocker,
    /* [in] */ Int64 nanos)
{
    ECode ec = NOERROR;
    if (nanos > 0) {
        AutoPtr<IThread> t = Thread::GetCurrentThread();
        SetBlocker(t, blocker);
        ec = t->ParkFor(nanos);
        SetBlocker(t, NULL);
    }
    return ec;
}

ECode LockSupport::ParkUntil(
    /* [in] */ IInterface* blocker,
    /* [in] */ Int64 deadline)
{
    AutoPtr<IThread> t = Thread::GetCurrentThread();
    SetBlocker(t, blocker);
    t->ParkUntil(deadline);
    SetBlocker(t, NULL);
    return NOERROR;
}

AutoPtr<IInterface> LockSupport::GetBlocker(
    /* [in] */ IThread* t)
{
    if (t == NULL) {
        // throw new NullPointerException();
        return NULL;
    }
    volatile int32_t* address = (volatile int32_t*) (&((Thread*)t)->mParkBlocker);

    AutoPtr<IInterface> blocker = reinterpret_cast<IInterface*>(android_atomic_acquire_load(address));
    return blocker;
}

ECode LockSupport::Park()
{
    Thread::GetCurrentThread()->ParkFor(0);
    return NOERROR;
}

ECode LockSupport::ParkNanos(
    /* [in] */ Int64 nanos)
{
    if (nanos > 0) {
        Thread::GetCurrentThread()->ParkFor(nanos);
    }
    return NOERROR;
}

ECode LockSupport::ParkUntil(
    /* [in] */ Int64 deadline)
{
    Thread::GetCurrentThread()->ParkUntil(deadline);
    return NOERROR;
}

ECode LockSupport::SetBlocker(
    /* [in] */ IThread* t,
    /* [in] */ IInterface* arg)
{
    // Even though volatile, hotspot doesn't need a write barrier here.
    ((Thread*)t)->mParkBlocker = arg;
    return NOERROR;
}

} // namespace Locks
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
