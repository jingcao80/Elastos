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

#include "Int16ArrayBuffer.h"
#include "CByteOrderHelper.h"

namespace Elastos {
namespace IO {

Int16ArrayBuffer::Int16ArrayBuffer()
    : mArrayOffset(0)
    , mIsReadOnly(FALSE)
{
}

ECode Int16ArrayBuffer::constructor(
    /* [in] */ ArrayOf<Int16>* array)
{
    FAIL_RETURN(Int16Buffer::constructor(array->GetLength(), 0))
    mBackingArray = array;
    return NOERROR;
}

ECode Int16ArrayBuffer::constructor(
    /* [in] */ Int32 capacity,
    /* [in] */ ArrayOf<Int16>* backingArray,
    /* [in] */ Int32 offset,
    /* [in] */ Boolean isReadOnly)
{
    FAIL_RETURN(Int16Buffer::constructor(capacity, 0))
    mBackingArray = backingArray;
    mArrayOffset = offset;
    mIsReadOnly = isReadOnly;
    return NOERROR;
}

ECode Int16ArrayBuffer::GetPrimitiveArray(
    /* [out] */ Handle64* arrayHandle)
{
    AutoPtr<ArrayOf<Int16> > arrayTmp;
    GetArray((ArrayOf<Int16>**)&arrayTmp);
    if (arrayTmp == NULL)
    {
        *arrayHandle = 0;
        return NOERROR;
    }
    Int16* primitiveArray = arrayTmp->GetPayload();
    *arrayHandle = reinterpret_cast<Handle64>(primitiveArray);
    return NOERROR;
}

ECode Int16ArrayBuffer::Get(
    /* [out] */ Int16* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    if (mPosition == mLimit) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }
    *value = (*mBackingArray)[mArrayOffset + mPosition++];
    return NOERROR;
}

ECode Int16ArrayBuffer::Get(
    /* [in] */ Int32 index,
    /* [out] */ Int16* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    FAIL_RETURN(CheckIndex(index))
    *value = (*mBackingArray)[mArrayOffset + index];
    return NOERROR;
}

ECode Int16ArrayBuffer::Get(
    /* [out] */ ArrayOf<Int16>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 int16Count)
{
    VALIDATE_NOT_NULL(dst);
    Int32 remaining = 0;
    GetRemaining(&remaining);
    if (int16Count > remaining) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }

    dst->Copy(dstOffset, mBackingArray, mArrayOffset + mPosition, int16Count);
    mPosition += int16Count;
    return NOERROR;
}

ECode Int16ArrayBuffer::IsDirect(
    /* [out] */ Boolean* isDirect)
{
    VALIDATE_NOT_NULL(isDirect);
    *isDirect = FALSE;
    return NOERROR;
}

ECode Int16ArrayBuffer::GetOrder(
   /* [out] */ ByteOrder* byteOrder)
{
    VALIDATE_NOT_NULL(byteOrder)
    AutoPtr<IByteOrderHelper> helper;
    ASSERT_SUCCEEDED(CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&helper))
    return helper->GetNativeOrder(byteOrder);
}

ECode Int16ArrayBuffer::AsReadOnlyBuffer(
    /* [out] */ IInt16Buffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    *buffer = NULL;

    AutoPtr<Int16ArrayBuffer> iab;
    FAIL_RETURN(Copy(this, mMark, TRUE, (Int16ArrayBuffer**)&iab))
    *buffer = IInt16Buffer::Probe(iab);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode Int16ArrayBuffer::Compact()
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

ECode Int16ArrayBuffer::Duplicate(
    /* [out] */ IInt16Buffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    *buffer = NULL;

    AutoPtr<Int16ArrayBuffer> iab;
    FAIL_RETURN(Copy(this, mMark, mIsReadOnly, (Int16ArrayBuffer**)&iab))
    *buffer = IInt16Buffer::Probe(iab);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode Int16ArrayBuffer::ProtectedArray(
    /* [out, callee] */ ArrayOf<Int16>** array)
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

ECode Int16ArrayBuffer::ProtectedArrayOffset(
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

ECode Int16ArrayBuffer::ProtectedHasArray(
    /* [out] */ Boolean* hasArray)
{
    VALIDATE_NOT_NULL(hasArray)
    *hasArray = TRUE;

    if (mIsReadOnly) {
        *hasArray = FALSE;
    }

    return NOERROR;
}

ECode Int16ArrayBuffer::Put(
    /* [in] */ Int16 d)
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

ECode Int16ArrayBuffer::Put(
    /* [in] */ Int32 index,
    /* [in] */ Int16 d)
{
    if (mIsReadOnly) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index));
    (*mBackingArray)[mArrayOffset + index] = d;
    return NOERROR;
}

ECode Int16ArrayBuffer::Put(
    /* [in] */ const ArrayOf<Int16>& src,
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

ECode Int16ArrayBuffer::Slice(
    /* [out] */ IInt16Buffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    *buffer = NULL;

    Int32 remainvalue = 0;
    GetRemaining(&remainvalue);
    AutoPtr<Int16ArrayBuffer> iab = new Int16ArrayBuffer();
    FAIL_RETURN(iab->constructor(remainvalue, mBackingArray, mArrayOffset + mPosition, mIsReadOnly))
    *buffer = IInt16Buffer::Probe(iab);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode Int16ArrayBuffer::IsReadOnly(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mIsReadOnly;
    return NOERROR;
}

ECode Int16ArrayBuffer::Copy(
    /* [in] */ Int16ArrayBuffer* other,
    /* [in] */ Int32 mMarkOfOther,
    /* [in] */ Boolean mIsReadOnly,
    /* [out] */ Int16ArrayBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    *buffer = NULL;

    Int32 capvalue = 0;
    other->GetCapacity(&capvalue);
    AutoPtr<Int16ArrayBuffer> iab = new Int16ArrayBuffer();
    FAIL_RETURN(iab->constructor(capvalue, other->mBackingArray, other->mArrayOffset, mIsReadOnly))
    iab->mLimit = other->mLimit;
    other->GetPosition(&iab->mPosition);
    iab->mMark = mMarkOfOther;
    *buffer = iab;
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
