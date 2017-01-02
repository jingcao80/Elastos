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

#include "Int16Buffer.h"
#include "CoreUtils.h"
#include "CArrayOf.h"
#include "Int16ArrayBuffer.h"
#include "Arrays.h"

using Elastos::Core::CArrayOf;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IInteger16;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace IO {

Int16Buffer::Int16Buffer()
{}

ECode Int16Buffer::constructor(
    /* [in] */ Int32 capacity,
    /* [in] */ Int64 effectiveDirectAddress)
{
    return Buffer::constructor(1, capacity, effectiveDirectAddress);
}

CAR_INTERFACE_IMPL_2(Int16Buffer, Object, IInt16Buffer, IBuffer)

ECode Int16Buffer::Allocate(
    /* [in] */ Int32 capacity,
    /* [out] */ IInt16Buffer** buf)
{
    VALIDATE_NOT_NULL(buf);
    *buf = NULL;

    if (capacity < 0) {
        // throw new IllegalArgumentException("capacity < 0: " + capacity);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr< ArrayOf<Int16> > res = ArrayOf<Int16>::Alloc(capacity);
    AutoPtr<Int16ArrayBuffer> iab = new Int16ArrayBuffer();
    FAIL_RETURN(iab->constructor(res))
    *buf = IInt16Buffer::Probe(iab);
    REFCOUNT_ADD(*buf)
    return NOERROR;
}

ECode Int16Buffer::Wrap(
    /* [in] */ ArrayOf<Int16>* array,
    /* [out] */ IInt16Buffer** buf)
{
    return Wrap(array, 0, array->GetLength(), buf);
}

ECode Int16Buffer::Wrap(
    /* [in] */ ArrayOf<Int16>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 int16Count,
    /* [out] */ IInt16Buffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);
    *buffer = NULL;

    FAIL_RETURN(Arrays::CheckOffsetAndCount(array->GetLength(), start, int16Count));
    AutoPtr<Int16ArrayBuffer> iab = new Int16ArrayBuffer();
    FAIL_RETURN(iab->constructor(array))
    iab->mPosition = start;
    iab->mLimit = start + int16Count;
    *buffer = IInt16Buffer::Probe(iab);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode Int16Buffer::GetArray(
    /* [out, callee] */ ArrayOf<Int16>** array)
{
    return ProtectedArray(array);
}

ECode Int16Buffer::GetArrayOffset(
    /* [out] */ Int32* offset)
{
    return ProtectedArrayOffset(offset);
}

ECode Int16Buffer::CompareTo(
    /* [in] */ IInt16Buffer* otherBuffer,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    Int32 remaining = 0;
    Int32 otherRemaining = 0;
    GetRemaining(&remaining);
    IBuffer::Probe(otherBuffer)->GetRemaining(&otherRemaining);
    Int32 compareRemaining = (remaining < otherRemaining) ? remaining : otherRemaining;
    Int32 thisPos = mPosition;
    Int32 otherPos = 0;
    IBuffer::Probe(otherBuffer)->GetPosition(&otherPos);
    Int16 thisInt16 = 0;
    Int16 otherInt16 = 0;
    while (compareRemaining > 0) {
        Get(thisPos, &thisInt16);
        otherBuffer->Get(otherPos, &otherInt16);
        // checks for Int16 and NaN inequality
        if (thisInt16 != otherInt16) {
            *result = thisInt16 < otherInt16 ? -1 : 1;
            return NOERROR;
        }
        thisPos++;
        otherPos++;
        compareRemaining--;
    }
    *result = remaining - otherRemaining;
    return NOERROR;
}

ECode Int16Buffer::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)
    *rst = FALSE;

    if (IInt16Buffer::Probe(other) == NULL) {
        return NOERROR;
    }

    AutoPtr<IInt16Buffer> otherBuffer = (IInt16Buffer*) other;
    Int32 thisRemaining = 0;
    Int32 otherRemaining = 0;
    GetRemaining(&thisRemaining);
    IBuffer::Probe(otherBuffer)->GetRemaining(&otherRemaining);
    if (thisRemaining != otherRemaining) {
        *rst = FALSE;
        return NOERROR;
    }

    Int32 myPosition = mPosition;
    Int32 otherPosition = 0;
    IBuffer::Probe(otherBuffer)->GetPosition(&otherPosition);
    Boolean equalSoFar = TRUE;
    Int16 thisValue = 0;
    Int16 otherValue = 0;
    while (equalSoFar && (myPosition < mLimit)) {
        FAIL_RETURN(Get(myPosition++, &thisValue))
        FAIL_RETURN(otherBuffer->Get(otherPosition++, &otherValue))
        equalSoFar = thisValue == otherValue;
    }
    *rst = equalSoFar;
    return NOERROR;
}

ECode Int16Buffer::Get(
    /* [out] */ ArrayOf<Int16>* dst)
{
    return Get(dst, 0, dst->GetLength());
}

ECode Int16Buffer::Get(
    /* [out] */ ArrayOf<Int16>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 int16Count)
{
    VALIDATE_NOT_NULL(dst);
    Int32 arrayLength = dst->GetLength();
    if ((dstOffset | int16Count) < 0 || dstOffset > arrayLength || arrayLength - dstOffset < int16Count) {
        // throw new ArrayIndexOutOfBoundsException(arrayLength, offset,
        //         count);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 remaining = 0;
    GetRemaining(&remaining);
    if (int16Count > remaining) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }
    for (Int32 i = dstOffset; i < dstOffset + int16Count; ++i) {
        Get(&(*dst)[i]);
    }

    return NOERROR;
}

ECode Int16Buffer::HasArray(
    /* [out] */ Boolean* hasArray)
{
    return ProtectedHasArray(hasArray);
}

ECode Int16Buffer::Put(
    /* [in] */ ArrayOf<Int16>* src)
{
    return Put(src, 0, src->GetLength());
}

ECode Int16Buffer::Put(
    /* [in] */ ArrayOf<Int16>* src,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 int16Count)
{
    Int32 arrayLength = src->GetLength();
    if ((srcOffset | int16Count) < 0 || srcOffset > arrayLength || arrayLength - srcOffset < int16Count) {
        // throw new ArrayIndexOutOfBoundsException(arrayLength, offset,
        //         count);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 remaining = 0;
    GetRemaining(&remaining);
    if (int16Count > remaining) {
        // throw new BufferOverflowException();
        return E_BUFFER_OVER_FLOW_EXCEPTION;
    }
    for (Int32 i = srcOffset; i < srcOffset + int16Count; ++i) {
        Put((*src)[i]);
    }
    return NOERROR;
}

ECode Int16Buffer::Put(
    /* [in] */ IInt16Buffer* src)
{
    if (src == this) {
        // throw new IllegalArgumentException("src == this");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 srcRemaining = 0;
    Int32 remaining = 0;
    IBuffer::Probe(src)->GetRemaining(&srcRemaining);
    GetRemaining(&remaining);
    if (srcRemaining > remaining) {
        // throw new BufferOverflowException();
        return E_BUFFER_OVER_FLOW_EXCEPTION;
    }

    AutoPtr< ArrayOf<Int16> > contents = ArrayOf<Int16>::Alloc(srcRemaining);
    FAIL_RETURN(src->GetArray((ArrayOf<Int16>**)&contents))
    return Put(contents);
}

ECode Int16Buffer::GetArray(
    /* [out] */ IArrayOf** array)
{
    VALIDATE_NOT_NULL(array)
    *array = NULL;

    AutoPtr< ArrayOf<Int16> > res;
    GetArray((ArrayOf<Int16>**)&res);
    AutoPtr<IArrayOf> iarr;
    CArrayOf::New(EIID_IInteger16, res->GetLength(), (IArrayOf**)&iarr);
    for (Int32 i = 0; i < res->GetLength(); ++i) {
        iarr->Set(i, CoreUtils::Convert((*res)[i]));
    }
    *array = iarr;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
