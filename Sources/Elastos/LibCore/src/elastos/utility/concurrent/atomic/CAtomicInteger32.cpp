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

#include "CAtomicInteger32.h"
#include <cutils/atomic.h>

using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

Int64 CAtomicInteger32::mSerialVersionUID = 6214790243416807050ll;

Int64 CAtomicInteger32::mValueOffset = 0;

CAR_INTERFACE_IMPL_2(CAtomicInteger32, Object, IAtomicInteger32, ISerializable)

CAR_OBJECT_IMPL(CAtomicInteger32)

ECode CAtomicInteger32::constructor(
    /* [in] */ Int32 initialValue)
{
    mValue = initialValue;
    return NOERROR;
}

ECode CAtomicInteger32::constructor()
{
    mValue = 0;
    return NOERROR;
}

ECode CAtomicInteger32::Get(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    return NOERROR;
}

ECode CAtomicInteger32::Set(
    /* [in] */ Int32 newValue)
{
    mValue = newValue;
    return NOERROR;
}

ECode CAtomicInteger32::LazySet(
    /* [in] */ Int32 newValue)
{
    volatile Int32* address = &mValue;

    ANDROID_MEMBAR_STORE();
    *address = newValue;
    return NOERROR;
}

ECode CAtomicInteger32::GetAndSet(
    /* [in] */ Int32 newValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    for (;;) {
        Int32 current;
        Get(&current);
        Boolean result;
        if (CompareAndSet(current, newValue, &result), result) {
            *value = current;
            return NOERROR;
        }
    }

    // for (;;) {
    //     int current = get();
    //     if (compareAndSet(current, newValue))
    //         return current;
    // }
}

ECode CAtomicInteger32::CompareAndSet(
    /* [in] */ Int32 expect,
    /* [in] */ Int32 update,
    /* [out] */ Boolean* result)
{
    volatile int32_t* address = (volatile int32_t*)&mValue;

    // Note: android_atomic_release_cas() returns 0 on success, not failure.
    int ret = android_atomic_release_cas(expect, update, address);

    *result = (ret == 0);
    return NOERROR;
//  return unsafe.compareAndSwapInt(this, valueOffset, expect, update);
}

ECode CAtomicInteger32::WeakCompareAndSet(
    /* [in] */ Int32 expect,
    /* [in] */ Int32 update,
    /* [out] */ Boolean* result)
{
    volatile int32_t* address = (volatile int32_t*)&mValue;

    // Note: android_atomic_release_cas() returns 0 on success, not failure.
    int ret = android_atomic_release_cas(expect, update, address);

    *result = (ret == 0);
    return NOERROR;
//  return unsafe.compareAndSwapInt(this, valueOffset, expect, update);
}

ECode CAtomicInteger32::GetAndIncrement(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    for (;;) {
        Int32 current;
        Get(&current);
        Int32 next = current + 1;
        Boolean result;
        if (CompareAndSet(current, next, &result), result) {
            *value = current;
            return NOERROR;
        }
    }
}


ECode CAtomicInteger32::GetAndDecrement(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    for (;;) {
        Int32 current;
        Get(&current);
        Int32 next = current - 1;
        Boolean result;
        if (CompareAndSet(current, next, &result), result) {
            *value = current;
            return NOERROR;
        }
    }
}

ECode CAtomicInteger32::GetAndAdd(
    /* [in] */ Int32 delta,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    for (;;) {
        Int32 current;
        Get(&current);
        Int32 next = current + delta;
        Boolean result;
        if (CompareAndSet(current, next, &result), result) {
            *value = current;
            return NOERROR;
        }
    }
}

ECode CAtomicInteger32::IncrementAndGet(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    for (;;) {
        Int32 current;
        Get(&current);
        Int32 next = current + 1;
        Boolean result;
        if (CompareAndSet(current, next, &result), result) {
            *value = next;
            return NOERROR;
        }
    }
}

ECode CAtomicInteger32::DecrementAndGet(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    for (;;) {
        Int32 current;
        Get(&current);
        Int32 next = current - 1;
        Boolean result;
        if (CompareAndSet(current, next, &result), result) {
            *value = next;
            return NOERROR;
        }
    }
}

ECode CAtomicInteger32::AddAndGet(
    /* [in] */ Int32 delta,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    for (;;) {
        Int32 current;
        Get(&current);
        Int32 next = current + delta;
        Boolean result;
        if (CompareAndSet(current, next, &result), result) {
            *value = next;
            return NOERROR;
        }
    }
}

ECode CAtomicInteger32::Int32Value(
    /* [out] */ Int32* value)
{
    return Get(value);
}

ECode CAtomicInteger32::Int64Value(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 current;
    Get(&current);
    *value = (Int64)current;
    return NOERROR;
}

ECode CAtomicInteger32::FloatValue(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 current;
    Get(&current);
    *value = (Float)current;
    return NOERROR;
}

ECode CAtomicInteger32::DoubleValue(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 current;
    Get(&current);
    *value = (Double)current;
    return NOERROR;
}

} // namespace Atomic
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
