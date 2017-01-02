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

#include "CAtomicBoolean.h"
#include <cutils/atomic.h>
#include <cutils/atomic-inline.h>
#include "StringUtils.h"

using Elastos::Core::StringUtils;
using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

Int64 CAtomicBoolean::mSerialVersionUID = 4654671469794556979L;

Int64 CAtomicBoolean::mValueOffset = 0;

CAR_INTERFACE_IMPL_2(CAtomicBoolean, Object, IAtomicBoolean, ISerializable)

CAR_OBJECT_IMPL(CAtomicBoolean)

/**
 * Creates a new AtomicInteger with the given initial value.
 *
 * @param initialValue the initial value
 */
ECode CAtomicBoolean::constructor(
    /* [in] */ Boolean initialValue)
{
    mValue = initialValue ? 1 : 0;
    return NOERROR;
}

/**
 * Creates a new AtomicInteger with initial value {@code 0}.
 */
ECode CAtomicBoolean::constructor()
{
    mValue = 0;
    return NOERROR;
}

/**
 * Gets the current value.
 *
 * @return the current value
 */
ECode CAtomicBoolean::Get(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = (mValue != 0);
    return NOERROR;
}

/**
 * Sets to the given value.
 *
 * @param newValue the new value
 */
ECode CAtomicBoolean::Set(
    /* [in] */ Boolean newValue)
{
    mValue = newValue ? 1 : 0;
    return NOERROR;
}

/**
 * Eventually sets to the given value.
 *
 * @param newValue the new value
 * @since 1.6
 */
ECode CAtomicBoolean::LazySet(
    /* [in] */ Boolean newValue)
{
    Int32 v = newValue ? 1 : 0;
    ANDROID_MEMBAR_STORE();
    mValue = v;
    return NOERROR;
}

/**
 * Atomically sets to the given value and returns the old value.
 *
 * @param newValue the new value
 * @return the previous value
 */
ECode CAtomicBoolean::GetAndSet(
    /* [in] */ Boolean newValue,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    Boolean current, result;
    for (;;) {
        Get(&current);
        if (CompareAndSet(current, newValue, &result), result) {
            *value = current;
            return NOERROR;
        }
    }
}

/**
 * Atomically sets the value to the given updated value
 * if the current value {@code ==} the expected value.
 *
 * @param expect the expected value
 * @param update the new value
 * @return true if successful. False return indicates that
 * the actual value was not equal to the expected value.
 */
ECode CAtomicBoolean::CompareAndSet(
    /* [in] */ Boolean expect,
    /* [in] */ Boolean update,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    volatile int32_t* address = (volatile int32_t*)&mValue;
    int e = expect ? 1 : 0;
    int u = update ? 1 : 0;

    // Note: android_atomic_release_cas() returns 0 on success, not failure.
    int ret = android_atomic_release_cas(e, u, address);

    *result = (ret == 0);
    return NOERROR;

    // int e = expect ? 1 : 0;
    // int u = update ? 1 : 0;
    // return unsafe.compareAndSwapInt(this, valueOffset, e, u);
}

ECode CAtomicBoolean::WeakCompareAndSet(
    /* [in] */ Boolean expect,
    /* [in] */ Boolean update,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    volatile int32_t* address = (volatile int32_t*)&mValue;
    int e = expect ? 1 : 0;
    int u = update ? 1 : 0;

    // Note: android_atomic_release_cas() returns 0 on success, not failure.
    int ret = android_atomic_release_cas(e, u, address);

    *result = (ret == 0);
    return NOERROR;

    // int e = expect ? 1 : 0;
    // int u = update ? 1 : 0;
    // return unsafe.compareAndSwapInt(this, valueOffset, e, u);
}

ECode CAtomicBoolean::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    Boolean value;
    Get(&value);
    *str = StringUtils::BooleanToString(value);
    return NOERROR;
}

} // namespace Atomic
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
