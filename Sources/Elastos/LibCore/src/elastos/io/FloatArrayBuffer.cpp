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

#include "FloatArrayBuffer.h"
#include "CByteOrderHelper.h"

namespace Elastos {
namespace IO {

FloatArrayBuffer::FloatArrayBuffer()
    : mArrayOffset(0)
    , mIsReadOnly(FALSE)
{
}

ECode FloatArrayBuffer::constructor(
    /* [in] */ ArrayOf<Float>* array)
{
    FAIL_RETURN(FloatBuffer::constructor(array->GetLength(), 0))
    mBackingArray = array;
    return NOERROR;
}

ECode FloatArrayBuffer::constructor(
    /* [in] */ Int32 capacity,
    /* [in] */ ArrayOf<Float>* backingArray,
    /* [in] */ Int32 offset,
    /* [in] */ Boolean isReadOnly)
{
    FAIL_RETURN(FloatBuffer::constructor(capacity, 0))
    mArrayOffset = offset;
    mIsReadOnly = isReadOnly;
    mBackingArray = backingArray;
    return NOERROR;
}

ECode FloatArrayBuffer::GetPrimitiveArray(
    /* [out] */ HANDLE* arrayHandle)
{
    AutoPtr<ArrayOf<Float> > arrayTmp;
    GetArray((ArrayOf<Float>**)&arrayTmp);
    if (arrayTmp == NULL)
    {
        *arrayHandle = 0;
        return NOERROR;
    }
    Float* primitiveArray = arrayTmp->GetPayload();
    *arrayHandle = reinterpret_cast<HANDLE>(primitiveArray);
    return NOERROR;
}

ECode FloatArrayBuffer::Get(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    if (mPosition == mLimit) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }
    *value = (*mBackingArray)[mArrayOffset + mPosition++];
    return NOERROR;
}

ECode FloatArrayBuffer::Get(
    /* [in] */ Int32 index,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    FAIL_RETURN(CheckIndex(index))
    *value = (*mBackingArray)[mArrayOffset + index];
    return NOERROR;
}

ECode FloatArrayBuffer::Get(
    /* [out] */ ArrayOf<Float>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 floatCount)
{
    VALIDATE_NOT_NULL(dst);
    Int32 remaining = 0;
    GetRemaining(&remaining);
    if (floatCount > remaining) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }

    dst->Copy(dstOffset, mBackingArray, mArrayOffset + mPosition, floatCount);
    mPosition += floatCount;
    return NOERROR;
}

ECode FloatArrayBuffer::IsDirect(
    /* [out] */ Boolean* isDirect)
{
    VALIDATE_NOT_NULL(isDirect);
    *isDirect = FALSE;
    return NOERROR;
}

ECode FloatArrayBuffer::GetOrder(
   /* [out] */ ByteOrder* byteOrder)
{
    AutoPtr<IByteOrderHelper> helper;
    ASSERT_SUCCEEDED(CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&helper))
    return helper->GetNativeOrder(byteOrder);
}

ECode FloatArrayBuffer::AsReadOnlyBuffer(
    /* [out] */ IFloatBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)

    AutoPtr<FloatArrayBuffer> fab;
    FAIL_RETURN(Copy(this, mMark, TRUE, (FloatArrayBuffer**)&fab))
    *buffer = IFloatBuffer::Probe(fab);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode FloatArrayBuffer::Compact()
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

ECode FloatArrayBuffer::Duplicate(
    /* [out] */ IFloatBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)

    AutoPtr<FloatArrayBuffer> fab;
    FAIL_RETURN(Copy(this, mMark, mIsReadOnly, (FloatArrayBuffer**)&fab))
    *buffer = IFloatBuffer::Probe(fab);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode FloatArrayBuffer::ProtectedArray(
    /* [out, callee] */ ArrayOf<Float>** array)
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

ECode FloatArrayBuffer::ProtectedArrayOffset(
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

ECode FloatArrayBuffer::ProtectedHasArray(
    /* [out] */ Boolean* hasArray)
{
    VALIDATE_NOT_NULL(hasArray)
    *hasArray = TRUE;

    if (mIsReadOnly) {
        *hasArray = FALSE;
    }

    return NOERROR;
}

ECode FloatArrayBuffer::Put(
    /* [in] */ Float d)
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

ECode FloatArrayBuffer::Put(
    /* [in] */ Int32 index,
    /* [in] */ Float d)
{
    if (mIsReadOnly) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index));
    (*mBackingArray)[mArrayOffset + index] = d;
    return NOERROR;
}

ECode FloatArrayBuffer::Put(
    /* [in] */ const ArrayOf<Float>& src,
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

ECode FloatArrayBuffer::Slice(
    /* [out] */ IFloatBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)

    Int32 remainvalue = 0;
    GetRemaining(&remainvalue);
    AutoPtr<FloatArrayBuffer> fab = new FloatArrayBuffer();
    FAIL_RETURN(fab->constructor(remainvalue, mBackingArray, mArrayOffset + mPosition, mIsReadOnly))
    *buffer = IFloatBuffer::Probe(fab);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode FloatArrayBuffer::IsReadOnly(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mIsReadOnly;
    return NOERROR;
}

ECode FloatArrayBuffer::Copy(
    /* [in] */ FloatArrayBuffer* other,
    /* [in] */ Int32 mMarkOfOther,
    /* [in] */ Boolean mIsReadOnly,
    /* [out] */ FloatArrayBuffer** fab)
{
    VALIDATE_NOT_NULL(fab)
    *fab = NULL;

    Int32 capvalue = 0;
    other->GetCapacity(&capvalue);
    AutoPtr<FloatArrayBuffer> buf = new FloatArrayBuffer();
    FAIL_RETURN(buf->constructor(capvalue, other->mBackingArray, other->mArrayOffset, mIsReadOnly))
    buf->mLimit = other->mLimit;
    other->GetPosition(&buf->mPosition);
    buf->mMark = mMarkOfOther;
    *fab = buf;
    REFCOUNT_ADD(*fab)
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
