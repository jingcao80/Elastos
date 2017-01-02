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

#include "DoubleArrayBuffer.h"
#include "CByteOrderHelper.h"

namespace Elastos {
namespace IO {

DoubleArrayBuffer::DoubleArrayBuffer()
    : mArrayOffset(0)
    , mIsReadOnly(FALSE)
{
}

ECode DoubleArrayBuffer::constructor(
    /* [in] */ ArrayOf<Double>* array)
{
    FAIL_RETURN(DoubleBuffer::constructor(array->GetLength(), 0))
    mBackingArray = array;
    return NOERROR;
}

ECode DoubleArrayBuffer::constructor(
    /* [in] */ Int32 capacity,
    /* [in] */ ArrayOf<Double>* backingArray,
    /* [in] */ Int32 offset,
    /* [in] */ Boolean isReadOnly)
{
    FAIL_RETURN(DoubleBuffer::constructor(capacity, 0))
    mArrayOffset = offset;
    mIsReadOnly = isReadOnly;
    mBackingArray = backingArray;
    return NOERROR;
}

ECode DoubleArrayBuffer::GetPrimitiveArray(
    /* [out] */ Handle64* arrayHandle)
{
    AutoPtr<ArrayOf<Double> > arrayTmp;
    GetArray((ArrayOf<Double>**)&arrayTmp);
    if (arrayTmp == NULL)
    {
        *arrayHandle = 0;
        return NOERROR;
    }
    Double* primitiveArray = arrayTmp->GetPayload();
    *arrayHandle = reinterpret_cast<Handle64>(primitiveArray);
    return NOERROR;
}

ECode DoubleArrayBuffer::Get(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);
    if (mPosition == mLimit) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }
    *value = (*mBackingArray)[mArrayOffset + mPosition++];
    return NOERROR;
}

ECode DoubleArrayBuffer::Get(
    /* [in] */ Int32 index,
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);
    FAIL_RETURN(CheckIndex(index));
    *value = (*mBackingArray)[mArrayOffset + index];
    return NOERROR;
}

ECode DoubleArrayBuffer::Get(
    /* [out] */ ArrayOf<Double>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 doubleCount)
{
    VALIDATE_NOT_NULL(dst);
    Int32 remaining = 0;
    GetRemaining(&remaining);
    if (doubleCount > remaining) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }
    dst->Copy(dstOffset, mBackingArray, mArrayOffset + mPosition, doubleCount);
    mPosition += doubleCount;
    return NOERROR;
}

ECode DoubleArrayBuffer::IsDirect(
    /* [out] */ Boolean* isDirect)
{
    VALIDATE_NOT_NULL(isDirect);
    *isDirect = FALSE;
    return NOERROR;
}

ECode DoubleArrayBuffer::GetOrder(
    /* [out] */ ByteOrder* byteOrder)
{
    VALIDATE_NOT_NULL(byteOrder)

    *byteOrder = CByteOrderHelper::_GetNativeOrder();
    return NOERROR;
}

ECode DoubleArrayBuffer::AsReadOnlyBuffer(
    /* [out] */ IDoubleBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    *buffer = NULL;

    AutoPtr<DoubleArrayBuffer> dab;
    FAIL_RETURN(Copy(this, mMark, TRUE, (DoubleArrayBuffer**)&dab))
    *buffer = IDoubleBuffer::Probe(dab);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode DoubleArrayBuffer::Compact()
{
    if (mIsReadOnly) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    // System.arraycopy(backingArray, mPosition + mArrayOffset, backingArray, mArrayOffset, remaining());
    Int32 reaminvalue = 0;
    GetRemaining(&reaminvalue);
    mBackingArray->Copy(mArrayOffset, mBackingArray, mPosition + mArrayOffset, reaminvalue);
    mPosition = mLimit - mPosition;
    mLimit = mCapacity;
    mMark = UNSET_MARK;
    return NOERROR;
}

ECode DoubleArrayBuffer::Duplicate(
    /* [out] */ IDoubleBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    *buffer = NULL;

    AutoPtr<DoubleArrayBuffer> dab;
    FAIL_RETURN(Copy(this, mMark, mIsReadOnly, (DoubleArrayBuffer**)&dab))
    *buffer = IDoubleBuffer::Probe(dab);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode DoubleArrayBuffer::ProtectedArray(
    /* [out, callee] */ ArrayOf<Double>** array)
{
    VALIDATE_NOT_NULL(array)
    *array = NULL;

    if (mIsReadOnly) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    *array = mBackingArray;
    REFCOUNT_ADD(*array)

    return NOERROR;
}

ECode DoubleArrayBuffer::ProtectedArrayOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset)
    *offset = 0;

    if (mIsReadOnly) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    *offset = mArrayOffset;
    return NOERROR;
}

ECode DoubleArrayBuffer::ProtectedHasArray(
    /* [out] */ Boolean* hasArray)
{
    VALIDATE_NOT_NULL(hasArray)
    *hasArray = TRUE;

    if (mIsReadOnly) {
        *hasArray = FALSE;
    }
    return NOERROR;
}

ECode DoubleArrayBuffer::Put(
    /* [in] */ Double d)
{
    if (mIsReadOnly) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    if (mPosition == mLimit) {
        // throw new BufferOverflowException();
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }
    (*mBackingArray)[mArrayOffset + mPosition++] = d;
    return NOERROR;
}

ECode DoubleArrayBuffer::Put(
    /* [in] */ Int32 index,
    /* [in] */ Double d)
{
    if (mIsReadOnly) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index));
    (*mBackingArray)[mArrayOffset + index] = d;
    return NOERROR;
}

ECode DoubleArrayBuffer::Put(
    /* [in] */ const ArrayOf<Double>& src,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 doubleCount)
{
    if (mIsReadOnly) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    Int32 remainvalue = 0;
    GetRemaining(&remainvalue);
    if (doubleCount > remainvalue) {
        // throw new BufferOverflowException();
        return E_BUFFER_OVERFLOW_EXCEPTION;
    }
    // System.arraycopy(src, srcOffset, backingArray, mArrayOffset + mPosition, doubleCount);
    mBackingArray->Copy(mArrayOffset + mPosition, &src, srcOffset, doubleCount);
    mPosition += doubleCount;
    return NOERROR;
}

ECode DoubleArrayBuffer::Slice(
    /* [out] */ IDoubleBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)

    Int32 remainvalue = 0;
    GetRemaining(&remainvalue);
    AutoPtr<DoubleArrayBuffer> dab = new DoubleArrayBuffer();
    FAIL_RETURN(dab->constructor(remainvalue, mBackingArray, mArrayOffset + mPosition, mIsReadOnly))
    *buffer = IDoubleBuffer::Probe(dab);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode DoubleArrayBuffer::IsReadOnly(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mIsReadOnly;
    return NOERROR;
}

ECode DoubleArrayBuffer::Copy(
    /* [in] */ DoubleArrayBuffer* other,
    /* [in] */ Int32 mMarkOfOther,
    /* [in] */ Boolean mIsReadOnly,
    /* [out] */ DoubleArrayBuffer** buffer)
{
    Int32 capvalue = 0;
    other->GetCapacity(&capvalue);
    AutoPtr<DoubleArrayBuffer> dab = new DoubleArrayBuffer();
    FAIL_RETURN(dab->constructor(capvalue, other->mBackingArray, other->mArrayOffset + other->mPosition, other->mIsReadOnly))
    dab->mLimit = other->mLimit;
    other->GetPosition(&dab->mPosition);
    dab->mMark = mMarkOfOther;
    *buffer = dab;
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
