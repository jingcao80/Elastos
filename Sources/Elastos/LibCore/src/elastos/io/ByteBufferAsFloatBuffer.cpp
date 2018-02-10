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

#include "Elastos.CoreLibrary.Libcore.h"
#include "ByteBufferAsFloatBuffer.h"
#include "DirectByteBuffer.h"
#include "ByteArrayBuffer.h"

using Libcore::IO::ISizeOf;

namespace Elastos {
namespace IO {

ByteBufferAsFloatBuffer::ByteBufferAsFloatBuffer()
    : mCap(0)
{}

ECode ByteBufferAsFloatBuffer::constructor(
    /* [in] */ ByteBuffer* byteBuffer)
{
    byteBuffer->GetCapacity(&mCap);
    FAIL_RETURN(FloatBuffer::constructor(mCap / ISizeOf::FLOAT, byteBuffer->mEffectiveDirectAddress))
    mByteBuffer = byteBuffer;
    mByteBuffer->Clear();
    return NOERROR;
}

ECode ByteBufferAsFloatBuffer::GetPrimitiveArray(
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

AutoPtr<IFloatBuffer> ByteBufferAsFloatBuffer::AsFloatBuffer(
    /* [in] */ ByteBuffer* mByteBuffer)
{
    AutoPtr<IByteBuffer> slice;
    mByteBuffer->Slice((IByteBuffer**)&slice);
    ByteOrder midorder;
    mByteBuffer->GetOrder(&midorder);
    slice->SetOrder(midorder);
    AutoPtr<ByteBufferAsFloatBuffer> res = new ByteBufferAsFloatBuffer();
    res->constructor((ByteBuffer*)slice.Get());
    return IFloatBuffer::Probe(res);
}

ECode ByteBufferAsFloatBuffer::AsReadOnlyBuffer(
    /* [out] */ IFloatBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)

    AutoPtr<IByteBuffer> resbb;
    mByteBuffer->AsReadOnlyBuffer((IByteBuffer**)&resbb);

    AutoPtr<ByteBufferAsFloatBuffer> buf = new ByteBufferAsFloatBuffer();
    buf->constructor((ByteBuffer*)resbb.Get());

    buf->mLimit = mLimit;
    buf->mPosition = mPosition;
    buf->mMark = mMark;
    buf->mByteBuffer->mOrder = mByteBuffer->mOrder;
    *buffer = IFloatBuffer::Probe(buf);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode ByteBufferAsFloatBuffer::Compact()
{
    Boolean isflag = FALSE;
    if (mByteBuffer->IsReadOnly(&isflag), isflag) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    mByteBuffer->SetLimit(mLimit * ISizeOf::FLOAT);
    mByteBuffer->SetPosition(mPosition * ISizeOf::FLOAT);
    mByteBuffer->Compact();
    mByteBuffer->Clear();
    mPosition = mLimit - mPosition;
    mLimit = mCapacity;
    mMark = UNSET_MARK;
    return NOERROR;
}

ECode ByteBufferAsFloatBuffer::Duplicate(
    /* [out] */ IFloatBuffer** buffer)
{
    AutoPtr<IByteBuffer> bb;
    mByteBuffer->Duplicate((IByteBuffer**)&bb);
    ByteOrder midorder;
    mByteBuffer->GetOrder(&midorder);
    bb->SetOrder(midorder);

    AutoPtr<ByteBufferAsFloatBuffer> buf = new ByteBufferAsFloatBuffer();
    buf->constructor((ByteBuffer*)bb.Get());

    buf->mLimit = mLimit;
    buf->mPosition = mPosition;
    buf->mMark = mMark;
    *buffer = IFloatBuffer::Probe(buf);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode ByteBufferAsFloatBuffer::Get(
    /* [out] */ Float* value)
{
    if (mPosition == mLimit) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    return mByteBuffer->GetFloat(mPosition++ * ISizeOf::FLOAT, value);
}

ECode ByteBufferAsFloatBuffer::Get(
    /* [in] */ Int32 index,
    /* [out] */ Float* value)
{
    FAIL_RETURN(CheckIndex(index));
    return mByteBuffer->GetFloat(index * ISizeOf::FLOAT, value);
}

ECode ByteBufferAsFloatBuffer::Get(
    /* [out] */ ArrayOf<Float>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 floatCount)
{
    mByteBuffer->SetLimit(mLimit * ISizeOf::FLOAT);
    mByteBuffer->SetPosition(mPosition * ISizeOf::FLOAT);
    AutoPtr<DirectByteBuffer> res = static_cast<DirectByteBuffer*>(mByteBuffer.Get());
    if (res) {
        res->GetFloats(dst, dstOffset, floatCount);
    }
    else {
        ((ByteArrayBuffer*) mByteBuffer.Get())->GetFloats(dst, dstOffset, floatCount);
    }
    mPosition += floatCount;
    return NOERROR;
}

ECode ByteBufferAsFloatBuffer::IsDirect(
    /* [out] */ Boolean* rst)
{
    return mByteBuffer->IsDirect(rst);
}

ECode ByteBufferAsFloatBuffer::IsReadOnly(
    /* [out] */ Boolean* rst)
{
    return mByteBuffer->IsReadOnly(rst);
}

ECode ByteBufferAsFloatBuffer::GetOrder(
    /* [out] */ ByteOrder* byteOrder)
{
    return mByteBuffer->GetOrder(byteOrder);
}

ECode ByteBufferAsFloatBuffer::Put(
    /* [in] */ Float c)
{
    if (mPosition == mLimit) {
        // throw new BufferOverflowException();
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    return mByteBuffer->PutFloat(mPosition++ * ISizeOf::FLOAT, c);
}

ECode ByteBufferAsFloatBuffer::Put(
    /* [in] */ Int32 index,
    /* [in] */ Float c)
{
    FAIL_RETURN(CheckIndex(index));
    return mByteBuffer->PutFloat(index * ISizeOf::FLOAT, c);
}

ECode ByteBufferAsFloatBuffer::Put(
    /* [in] */ ArrayOf<Float>* src,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 floatCount)
{
    mByteBuffer->SetLimit(mLimit * ISizeOf::FLOAT);
    mByteBuffer->SetPosition(mPosition * ISizeOf::FLOAT);
    AutoPtr<DirectByteBuffer> res = static_cast<DirectByteBuffer*>(mByteBuffer.Get());
    if (res) {
        res->PutFloats(src, srcOffset, floatCount);
    } else {
        ((ByteArrayBuffer*) mByteBuffer.Get())->PutFloats(src, srcOffset, floatCount);
    }
    mPosition += floatCount;
    return NOERROR;
}

ECode ByteBufferAsFloatBuffer::Slice(
    /* [out] */ IFloatBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)

    mByteBuffer->SetLimit(mLimit * ISizeOf::FLOAT);
    mByteBuffer->SetPosition(mPosition * ISizeOf::FLOAT);
    AutoPtr<IByteBuffer> bb;
    mByteBuffer->Slice((IByteBuffer**)&bb);
    ByteOrder midorder;
    mByteBuffer->GetOrder(&midorder);
    bb->SetOrder(midorder);

    AutoPtr<ByteBufferAsFloatBuffer> result = new ByteBufferAsFloatBuffer();
    result->constructor((ByteBuffer*)bb.Get());

    mByteBuffer->Clear();
    *buffer = IFloatBuffer::Probe(result);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode ByteBufferAsFloatBuffer::ProtectedArray(
    /* [out, callee] */ ArrayOf<Float>** array)
{
    assert(0);
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ByteBufferAsFloatBuffer::ProtectedArrayOffset(
    /* [out] */ Int32* offset)
{
    assert(0);
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ByteBufferAsFloatBuffer::ProtectedHasArray(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
