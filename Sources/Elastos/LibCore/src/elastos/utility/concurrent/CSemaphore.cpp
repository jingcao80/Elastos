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

#include "CSemaphore.h"
#include <cutils/log.h>

using Elastos::IO::EIID_ISerializable;
using namespace Elastos::Utility::Concurrent::Locks;

namespace Elastos {
namespace Utility {
namespace Concurrent {

//====================================================================
// CSemaphore::Sync
//====================================================================
CSemaphore::Sync::Sync(
    /* [in] */ Int32 permits)
{
    SetState(permits);
}

ECode CSemaphore::Sync::GetPermits(
    /* [out] */ Int32* out)
{
    VALIDATE_NOT_NULL(out)
    *out = GetState();
    return NOERROR;
}

ECode CSemaphore::Sync::NonfairTryAcquireShared(
    /* [in] */ Int32 acquires,
    /* [out] */ Int32* out)
{
    VALIDATE_NOT_NULL(out)
    for (;;) {
        Int32 available = GetState();
        Int32 remaining = available - acquires;
        if (remaining < 0 ||
            CompareAndSetState(available, remaining)) {
            *out = remaining;
            return NOERROR;
        }
    }
}

Boolean CSemaphore::Sync::TryReleaseShared(
    /* [in] */ Int32 releases)
{
    for (;;) {
        Int32 current = GetState();
        Int32 next = current + releases;
        if (next < current) {
            ALOGD("CSemaphore:Maximum permit count exceeded");
            return FALSE;
        }
        if (CompareAndSetState(current, next)) {
            return TRUE;
        }
    }
    return FALSE;
}

ECode CSemaphore::Sync::ReducePermits(
    /* [in] */ Int32 reductions)
{
    for (;;) {
        Int32 current = GetState();
        Int32 next = current - reductions;
        if (next > current) {
            ALOGD("CSemaphore:Permit count underflow");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (CompareAndSetState(current, next)) {
            return NOERROR;
        }
    }
}

ECode CSemaphore::Sync::DrainPermits(
    /* [out] */ Int32* out)
{
    VALIDATE_NOT_NULL(out)
    for (;;) {
        Int32 current = GetState();
        if (current == 0 || CompareAndSetState(current, 0)) {
            *out = current;
            return NOERROR;
        }
    }
}


//====================================================================
// CSemaphore::NonfairSync
//====================================================================
CSemaphore::NonfairSync::NonfairSync(
    /* [in] */ Int32 permits)
    : Sync(permits)
{
}

Int32 CSemaphore::NonfairSync::TryAcquireShared(
    /* [in] */ Int32 acquires)
{
    Int32 ival;
    NonfairTryAcquireShared(acquires, &ival);
    return ival;
}


//====================================================================
// CSemaphore::FairSync
//====================================================================
CSemaphore::FairSync::FairSync(
    /* [in] */ Int32 permits)
    : Sync(permits)
{
}

Int32 CSemaphore::FairSync::TryAcquireShared(
    /* [in] */ Int32 acquires)
{
    for (;;) {
        Boolean b = FALSE;
        if ((HasQueuedPredecessors(&b), b)) {
            return -1;
        }
        Int32 available = GetState();
        Int32 remaining = available - acquires;
        if (remaining < 0 ||
            CompareAndSetState(available, remaining)) {
            return remaining;
        }
    }
    return -1;
}


//====================================================================
// CSemaphore::
//====================================================================
CAR_INTERFACE_IMPL_2(CSemaphore, Object, ISemaphore, ISerializable)

CAR_OBJECT_IMPL(CSemaphore)

ECode CSemaphore::constructor(
    /* [in] */ Int32 permits)
{
    mSync = new NonfairSync(permits);
    return NOERROR;
}

ECode CSemaphore::constructor(
    /* [in] */ Int32 permits,
    /* [in] */ Boolean fair)
{
    mSync = fair ? (Sync*)new FairSync(permits) : (Sync*)new NonfairSync(permits);
    return NOERROR;
}

ECode CSemaphore::Acquire()
{
    return mSync->AcquireSharedInterruptibly(1);
}

ECode CSemaphore::AcquireUninterruptibly()
{
    return mSync->AcquireShared(1);
}

ECode CSemaphore::TryAcquire(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    Int32 res;
    mSync->NonfairTryAcquireShared(1, &res);
    *value = res >= 0;
    return NOERROR;
}

ECode CSemaphore::TryAcquire(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    Int64 res;
    unit->ToNanos(timeout, &res);
    return mSync->TryAcquireSharedNanos(1, res, value);
}

ECode CSemaphore::ReleasePermit()
{
    Boolean b = FALSE;
    return mSync->ReleaseShared(1, &b);
}

ECode CSemaphore::Acquire(
    /* [in] */ Int32 permits)
{
    if (permits < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return mSync->AcquireSharedInterruptibly(permits);
}

ECode CSemaphore::AcquireUninterruptibly(
    /* [in] */ Int32 permits)
{
    if (permits < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return mSync->AcquireShared(permits);
}

ECode CSemaphore::TryAcquire(
    /* [in] */ Int32 permits,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    if (permits < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 res;
    mSync->NonfairTryAcquireShared(permits, &res);
    *value = res >= 0;
    return NOERROR;
}

ECode CSemaphore::TryAcquire(
    /* [in] */ Int32 permits,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    if (permits < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int64 res;
    unit->ToNanos(timeout, &res);
    return mSync->TryAcquireSharedNanos(permits, res, value);
}

ECode CSemaphore::ReleasePermit(
    /* [in] */ Int32 permits)
{
    if (permits < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean b = FALSE;
    return mSync->ReleaseShared(permits, &b);
}

ECode CSemaphore::AvailablePermits(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    return mSync->GetPermits(value);
}

ECode CSemaphore::DrainPermits(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    return mSync->DrainPermits(value);
}

ECode CSemaphore::ReducePermits(
    /* [in] */ Int32 reduction)
{
    if (reduction < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return mSync->ReducePermits(reduction);
}

ECode CSemaphore::IsFair(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mSync->GetSyncID() == SyncID_FairSync;
    return NOERROR;
}

ECode CSemaphore::HasQueuedThreads(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    return mSync->HasQueuedThreads(value);
}

ECode CSemaphore::GetQueueLength(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    return mSync->GetQueueLength(value);
}

ECode CSemaphore::GetQueuedThreads(
    /* [out] */ ICollection** out)
{
    VALIDATE_NOT_NULL(out)
    return mSync->GetQueuedThreads(out);
}

ECode CSemaphore::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    Int32 mits;
    mSync->GetPermits(&mits);
//    *str = super.toString() + "[Permits = " + mits + "]";
    String res("[Permits = ");
    res.AppendFormat("%d]", mits);
    *str = res;
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
