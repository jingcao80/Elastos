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

#include "CAtomicInteger64Array.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;
using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

const Int64 CAtomicInteger64Array::mSerialVersionUID = -2308431214976778248L;

const Int32 CAtomicInteger64Array::sBase = 0; // = unsafe.arrayBaseOffset(long[].class);

const Int32 CAtomicInteger64Array::sShift = 0;

CAR_INTERFACE_IMPL_2(CAtomicInteger64Array, Object, IAtomicInteger64Array, ISerializable)

CAR_OBJECT_IMPL(CAtomicInteger64Array)

ECode CAtomicInteger64Array::constructor(
    /* [in] */ Int32 length)
{
    mArray = ArrayOf<Int64>::Alloc(length);
    return NOERROR;
}

ECode CAtomicInteger64Array::constructor(
    /* [in] */ const ArrayOf<Int64>& other)
{
    // Visibility guaranteed by final field guarantees
    mArray = other.Clone();
    return NOERROR;
}

ECode CAtomicInteger64Array::GetLength(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mArray->GetLength();
    return NOERROR;
}

ECode CAtomicInteger64Array::Get(
    /* [in] */ Int32 i,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)

    *value = GetRaw(CheckedByteOffset(i));
    return NOERROR;
}

ECode CAtomicInteger64Array::Set(
    /* [in] */ Int32 i,
    /* [in] */ Int64 newValue)
{
    assert(0 && "TODO");
    // unsafe.putLongVolatile(array, checkedByteOffset(i), newValue);
    return NOERROR;
}

ECode CAtomicInteger64Array::LazySet(
    /* [in] */ Int32 i,
    /* [in] */ Int64 newValue)
{
    assert(0 && "TODO");
    // unsafe.putOrderedLong(array, checkedByteOffset(i), newValue);
    return NOERROR;
}

ECode CAtomicInteger64Array::GetAndSet(
    /* [in] */ Int32 i,
    /* [in] */ Int64 newValue,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)

    Int64 offset = CheckedByteOffset(i);
    while (TRUE) {
        Int64 current = GetRaw(offset);
        if (CompareAndSetRaw(offset, current, newValue)) {
            *value = current;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CAtomicInteger64Array::CompareAndSet(
    /* [in] */ Int32 i,
    /* [in] */ Int64 expect,
    /* [in] */ Int64 update,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = CompareAndSetRaw(CheckedByteOffset(i), expect, update);
    return NOERROR;
}

ECode CAtomicInteger64Array::WeakCompareAndSet(
    /* [in] */ Int32 i,
    /* [in] */ Int64 expect,
    /* [in] */ Int64 update,
    /* [out] */ Boolean* value)
{
    return CompareAndSet(i, expect, update, value);
}

ECode CAtomicInteger64Array::GetAndIncrement(
    /* [in] */ Int32 i,
    /* [out] */ Int64* value)
{
    return GetAndAdd(i, 1, value);
}

ECode CAtomicInteger64Array::GetAndDecrement(
    /* [in] */ Int32 i,
    /* [out] */ Int64* value)
{
    return GetAndAdd(i, -1, value);
}

ECode CAtomicInteger64Array::GetAndAdd(
    /* [in] */ Int32 i,
    /* [in] */ Int64 delta,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)

    Int64 offset = CheckedByteOffset(i);
    while (TRUE) {
        Int64 current = GetRaw(offset);
        if (CompareAndSetRaw(offset, current, current + delta)) {
            *value = current;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CAtomicInteger64Array::IncrementAndGet(
    /* [in] */ Int32 i,
    /* [out] */ Int64* value)
{
    return AddAndGet(i, 1, value);
}

ECode CAtomicInteger64Array::DecrementAndGet(
    /* [in] */ Int32 i,
    /* [out] */ Int64* value)
{
    return AddAndGet(i, -1, value);
}

ECode CAtomicInteger64Array::AddAndGet(
    /* [in] */ Int32 i,
    /* [in] */ Int64 delta,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)

    Int64 offset = CheckedByteOffset(i);
    while (TRUE) {
        Int64 current = GetRaw(offset);
        Int64 next = current + delta;
        if (CompareAndSetRaw(offset, current, next)) {
            *value = next;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CAtomicInteger64Array::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    Int32 iMax = mArray->GetLength() - 1;
    if (iMax == -1) {
        *str = String("[]");
        return NOERROR;
    }

    StringBuilder b;
    b.AppendChar('[');
    for (Int32 i = 0; ; i++) {
        b.Append(GetRaw(ByteOffset(i)));
        if (i == iMax) {
            b.AppendChar(']');
            *str = b.ToString();
            return NOERROR;
        }
        b.AppendChar(',');
        b.AppendChar(' ');
    }
    return NOERROR;
}

Int64 CAtomicInteger64Array::CheckedByteOffset(
    /* [in] */ Int32 i)
{
    if (i < 0 || i >= mArray->GetLength()) {
        // throw new IndexOutOfBoundsException("index " + i);
        return 0;
    }

    return ByteOffset(i);
}

Int64 CAtomicInteger64Array::ByteOffset(
    /* [in] */ Int32 i)
{
    return ((Int64) i << sShift) + sBase;
}

Int64 CAtomicInteger64Array::GetRaw(
    /* [in] */ Int64 offset)
{
    assert(0 && "TODO");
    // return unsafe.getLongVolatile(array, offset);
    return 0;
}

Boolean CAtomicInteger64Array::CompareAndSetRaw(
    /* [in] */ Int64 offset,
    /* [in] */ Int64 expect,
    /* [in] */ Int64 update)
{
    assert(0 && "TODO");
    // return unsafe.compareAndSwapLong(array, offset, expect, update);
    return FALSE;
}

} // namespace Atomic
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
