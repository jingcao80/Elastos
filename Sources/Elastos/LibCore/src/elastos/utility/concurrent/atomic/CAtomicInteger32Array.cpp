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

#include "CAtomicInteger32Array.h"
#include <cutils/atomic.h>
#include <StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

CAR_INTERFACE_IMPL_2(CAtomicInteger32Array, Object, IAtomicInteger32Array, ISerializable)

CAR_OBJECT_IMPL(CAtomicInteger32Array)
/**
 * Creates a new AtomicIntegerArray of the given length, with all
 * elements initially zero.
 *
 * @param length the length of the array
 */
ECode CAtomicInteger32Array::constructor(
    /* [in] */ Int32 length)
{
    if (length < 0)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    mArray = ArrayOf<Int32>::Alloc(length);
    return NOERROR;
}

/**
 * Creates a new AtomicIntegerArray with the same length as, and
 * all elements copied from, the given array.
 *
 * @param array the array to copy elements from
 * @throws NullPointerException if array is null
 */
ECode CAtomicInteger32Array::constructor(
    /* [in] */ const ArrayOf<Int32>& other)
{
    mArray = other.Clone();
    return NOERROR;
}

ECode CAtomicInteger32Array::CheckedByteOffset(
    /* [in] */ Int32 index)
{
    if (index < 0 || index >= mArray->GetLength())
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    return NOERROR;
}

/**
 * Returns the length of the array.
 *
 * @return the length of the array
 */
ECode CAtomicInteger32Array::GetLength(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mArray->GetLength();
    return NOERROR;
}

/**
 * Gets the current value at position {@code i}.
 *
 * @param i the index
 * @return the current value
 */
ECode CAtomicInteger32Array::Get(
    /* [in] */ Int32 index,
    /* [out] */ Int32* value)
{
    FAIL_RETURN(CheckedByteOffset(index));

    *value = GetRaw(index);

    return NOERROR;
}

Int32 CAtomicInteger32Array::GetRaw(
    /* [in] */ Int32 index)
{
    volatile int32_t* address = (volatile int32_t*)(mArray->GetPayload() + index);

    int32_t value = android_atomic_acquire_load(address);

    return value;
}

/**
 * Sets the element at position {@code i} to the given value.
 *
 * @param i the index
 * @param newValue the new value
 */
ECode CAtomicInteger32Array::Set(
    /* [in] */ Int32 index,
    /* [in] */ Int32 newValue)
{
    FAIL_RETURN(CheckedByteOffset(index));

    volatile int32_t* address = (volatile int32_t*)(mArray->GetPayload() + index);

    android_atomic_release_store(newValue, address);

    return NOERROR;
}

/**
 * Eventually sets the element at position {@code i} to the given value.
 *
 * @param i the index
 * @param newValue the new value
 * @since 1.6
 */
ECode CAtomicInteger32Array::LazySet(
    /* [in] */ Int32 index,
    /* [in] */ Int32 newValue)
{
    FAIL_RETURN(CheckedByteOffset(index));

    volatile int32_t* address = (volatile int32_t*)(mArray->GetPayload() + index);

    ANDROID_MEMBAR_STORE();
    *address = newValue;
    return NOERROR;
}

/**
 * Atomically sets the element at position {@code i} to the given
 * value and returns the old value.
 *
 * @param i the index
 * @param newValue the new value
 * @return the previous value
 */
ECode CAtomicInteger32Array::GetAndSet(
    /* [in] */ Int32 index,
    /* [in] */ Int32 newValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    FAIL_RETURN(CheckedByteOffset(index));

    while (TRUE) {
        Int32 current = GetRaw(index);
        if (CompareAndSetRaw(index, current, newValue))
            return current;
    }
    return NOERROR;
}

/**
 * Atomically sets the element at position {@code i} to the given
 * updated value if the current value {@code ==} the expected value.
 *
 * @param i the index
 * @param expect the expected value
 * @param update the new value
 * @return true if successful. False return indicates that
 * the actual value was not equal to the expected value.
 */
ECode CAtomicInteger32Array::CompareAndSet(
    /* [in] */ Int32 index,
    /* [in] */ Int32 expect,
    /* [in] */ Int32 update,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckedByteOffset(index));

    *result = CompareAndSetRaw(index, expect, update);

    return NOERROR;
}

Boolean CAtomicInteger32Array::CompareAndSetRaw(
    /* [in] */ Int32 index,
    /* [in] */ Int32 expect,
    /* [in] */ Int32 update)
{
    volatile int32_t* address = (volatile int32_t*)(mArray->GetPayload() + index);

    // Note: android_atomic_release_cas() returns 0 on success, not failure.
    int result = android_atomic_release_cas(expect, update, address);

    return (result != 0);
}

/**
 * Atomically sets the element at position {@code i} to the given
 * updated value if the current value {@code ==} the expected value.
 *
 * <p>May <a href="package-summary.html#Spurious">fail spuriously</a>
 * and does not provide ordering guarantees, so is only rarely an
 * appropriate alternative to {@code compareAndSet}.
 *
 * @param i the index
 * @param expect the expected value
 * @param update the new value
 * @return true if successful.
 */
ECode CAtomicInteger32Array::WeakCompareAndSet(
    /* [in] */ Int32 index,
    /* [in] */ Int32 expect,
    /* [in] */ Int32 update,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckedByteOffset(index));

    return CompareAndSet(index, expect, update, result);
}

/**
 * Atomically increments by one the element at index {@code i}.
 *
 * @param i the index
 * @return the previous value
 */
ECode CAtomicInteger32Array::GetAndIncrement(
    /* [in] */ Int32 index,
    /* [out] */ Int32* value)
{
    FAIL_RETURN(CheckedByteOffset(index));

    return GetAndAdd(index, 1, value);
}

/**
 * Atomically decrements by one the element at index {@code i}.
 *
 * @param i the index
 * @return the previous value
 */
ECode CAtomicInteger32Array::GetAndDecrement(
    /* [in] */ Int32 index,
    /* [out] */ Int32* value)
{
    FAIL_RETURN(CheckedByteOffset(index));

    return GetAndAdd(index, -1, value);
}

/**
 * Atomically adds the given value to the element at index {@code i}.
 *
 * @param i the index
 * @param delta the value to add
 * @return the previous value
 */
ECode CAtomicInteger32Array::GetAndAdd(
    /* [in] */ Int32 index,
    /* [in] */ Int32 delta,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    FAIL_RETURN(CheckedByteOffset(index));

    while (TRUE) {
        Int32 current = GetRaw(index);
        if (CompareAndSetRaw(index, current, current + delta))
            *value = current;
    }
    return NOERROR;
}

/**
 * Atomically increments by one the element at index {@code i}.
 *
 * @param i the index
 * @return the updated value
 */
ECode CAtomicInteger32Array::IncrementAndGet(
    /* [in] */ Int32 index,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    FAIL_RETURN(CheckedByteOffset(index));

    return AddAndGet(index, 1, value);
}

/**
 * Atomically decrements by one the element at index {@code i}.
 *
 * @param i the index
 * @return the updated value
 */
ECode CAtomicInteger32Array::DecrementAndGet(
    /* [in] */ Int32 index,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    FAIL_RETURN(CheckedByteOffset(index));

    return AddAndGet(index, -1, value);
}

/**
 * Atomically adds the given value to the element at index {@code i}.
 *
 * @param i the index
 * @param delta the value to add
 * @return the updated value
 */
ECode CAtomicInteger32Array::AddAndGet(
    /* [in] */ Int32 index,
    /* [in] */ Int32 delta,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    FAIL_RETURN(CheckedByteOffset(index));

    while (true) {
        Int32 current = GetRaw(index);
        Int32 next = current + delta;
        if (CompareAndSetRaw(index, current, next))
            *value = next;
    }
    return NOERROR;
}

ECode CAtomicInteger32Array::ToString(
    /* [out] */ String* str)
{
    Int32 iMax = mArray->GetLength() - 1;
    if (iMax == -1) {
        *str = String("[]");
        return NOERROR;
    }

    StringBuilder b("[");
    for (Int32 i = 0; ; i++) {
        b += GetRaw(i);
        if (i == iMax) {
            b += "]";
            b.ToString(str);
            return NOERROR;
        }
        else {
            b += ", ";
        }
    }
    return NOERROR;
}


} // namespace Atomic
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
