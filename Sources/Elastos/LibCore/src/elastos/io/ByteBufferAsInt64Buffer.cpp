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
#include "ByteBufferAsInt64Buffer.h"
#include "DirectByteBuffer.h"
#include "ByteArrayBuffer.h"

using Libcore::IO::ISizeOf;

namespace Elastos {
namespace IO {

ByteBufferAsInt64Buffer::ByteBufferAsInt64Buffer()
    : mCap(0)
{}

ECode ByteBufferAsInt64Buffer::constructor(
    /* [in] */ ByteBuffer* byteBuffer)
{
    byteBuffer->GetCapacity(&mCap);
    FAIL_RETURN(Int64Buffer::constructor(mCap / ISizeOf::INT64, byteBuffer->mEffectiveDirectAddress))
    mByteBuffer = byteBuffer;
    IBuffer::Probe(mByteBuffer)->Clear();
    return NOERROR;
}

ECode ByteBufferAsInt64Buffer::GetPrimitiveArray(
    /* [out] */ Handle64* arrayHandle)
{
    AutoPtr<ArrayOf<Int64> > arrayTmp;
    GetArray((ArrayOf<Int64>**)&arrayTmp);
    if (arrayTmp == NULL)
    {
        *arrayHandle = 0;
        return NOERROR;
    }
    Int64* primitiveArray = arrayTmp->GetPayload();
    *arrayHandle = reinterpret_cast<Handle64>(primitiveArray);
    return NOERROR;
}

AutoPtr<IInt64Buffer> ByteBufferAsInt64Buffer::AsInt64Buffer(
    /* [in] */ IByteBuffer* byteBuffer)
{
    AutoPtr<IByteBuffer> slice;
    byteBuffer->Slice((IByteBuffer**)&slice);

    ByteOrder outorder;
    byteBuffer->GetOrder(&outorder);
    slice->SetOrder(outorder);

    AutoPtr<ByteBufferAsInt64Buffer> bbaib = new ByteBufferAsInt64Buffer();
    bbaib->constructor((ByteBuffer*)slice.Get());
    return IInt64Buffer::Probe(bbaib);
}

ECode ByteBufferAsInt64Buffer::AsReadOnlyBuffer(
    /* [out] */ IInt64Buffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)

    AutoPtr<IByteBuffer> res;
    mByteBuffer->AsReadOnlyBuffer((IByteBuffer**)&res);

    AutoPtr<ByteBufferAsInt64Buffer> buf = new ByteBufferAsInt64Buffer();
    buf->constructor((ByteBuffer*)res.Get());

    buf->mLimit = mLimit;
    buf->mPosition = mPosition;
    buf->mMark = mMark;
    buf->mByteBuffer->SetOrder(((ByteBuffer*)mByteBuffer.Get())->mOrder);
    *buffer = IInt64Buffer::Probe(buf);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode ByteBufferAsInt64Buffer::Compact()
{
    Boolean isflag = FALSE;
    if (IBuffer::Probe(mByteBuffer)->IsReadOnly(&isflag), isflag) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    IBuffer::Probe(mByteBuffer)->SetLimit(mLimit * ISizeOf::INT64);
    IBuffer::Probe(mByteBuffer)->SetPosition(mPosition * ISizeOf::INT64);
    mByteBuffer->Compact();
    IBuffer::Probe(mByteBuffer)->Clear();
    mPosition = mLimit - mPosition;
    mLimit = mCapacity;
    mMark = UNSET_MARK;
    return NOERROR;
}

ECode ByteBufferAsInt64Buffer::Duplicate(
    /* [out] */ IInt64Buffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)

    ByteOrder outorder;
    mByteBuffer->GetOrder(&outorder);
    AutoPtr<IByteBuffer> bb;
    mByteBuffer->Duplicate((IByteBuffer**)&bb);
    bb->SetOrder(outorder);
    AutoPtr<ByteBufferAsInt64Buffer> buf = new ByteBufferAsInt64Buffer();
    FAIL_RETURN(buf->constructor((ByteBuffer*)bb.Get()))
    buf->mLimit = mLimit;
    buf->mPosition = mPosition;
    buf->mMark = mMark;
    *buffer = IInt64Buffer::Probe(buf);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode ByteBufferAsInt64Buffer::Get(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)

    if (mPosition == mLimit) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    return mByteBuffer->GetInt64(mPosition++ * ISizeOf::INT64, value);
}

ECode ByteBufferAsInt64Buffer::Get(
    /* [in] */ Int32 index,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)

    FAIL_RETURN(CheckIndex(index));
    return mByteBuffer->GetInt64(index * ISizeOf::INT64, value);
}

ECode ByteBufferAsInt64Buffer::Get(
    /* [out] */ ArrayOf<Int64>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 charCount)
{
    VALIDATE_NOT_NULL(dst)

    IBuffer::Probe(mByteBuffer)->SetLimit(mLimit * ISizeOf::INT64);
    IBuffer::Probe(mByteBuffer)->SetPosition(mPosition * ISizeOf::INT64);
    AutoPtr<DirectByteBuffer> res = static_cast<DirectByteBuffer*>(mByteBuffer.Get());
    if (res) {
        res->GetInt64s(dst, dstOffset, charCount);
    }
    else {
        ((ByteArrayBuffer*) mByteBuffer.Get())->GetInt64s(dst, dstOffset, charCount);
    }
    mPosition += charCount;
    return NOERROR;
}

ECode ByteBufferAsInt64Buffer::IsDirect(
    /* [out] */ Boolean* rst)
{
    return mByteBuffer->IsDirect(rst);
}

ECode ByteBufferAsInt64Buffer::IsReadOnly(
    /* [out] */ Boolean* rst)
{
    return mByteBuffer->IsReadOnly(rst);
}

ECode ByteBufferAsInt64Buffer::GetOrder(
    /* [out] */ ByteOrder* byteOrder)
{
    return mByteBuffer->GetOrder(byteOrder);
}

ECode ByteBufferAsInt64Buffer::Put(
    /* [in] */ Int64 c)
{
    if (mPosition == mLimit) {
        // throw new BufferOverflowException();
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    return mByteBuffer->PutInt64(mPosition++ * ISizeOf::INT64, c);
}

ECode ByteBufferAsInt64Buffer::Put(
    /* [in] */ Int32 index,
    /* [in] */ Int64 c)
{
    FAIL_RETURN(CheckIndex(index));
    return mByteBuffer->PutInt64(index * ISizeOf::INT64, c);
}

ECode ByteBufferAsInt64Buffer::Put(
    /* [in] */ ArrayOf<Int64>* src,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 charCount)
{
    mByteBuffer->SetLimit(mLimit * ISizeOf::INT64);
    mByteBuffer->SetPosition(mPosition * ISizeOf::INT64);
    AutoPtr<DirectByteBuffer> res = static_cast<DirectByteBuffer*>(mByteBuffer.Get());
    if (res) {
        res->PutInt64s(src, srcOffset, charCount);
    } else {
        ((ByteArrayBuffer*) mByteBuffer.Get())->PutInt64s(src, srcOffset, charCount);
    }
    mPosition += charCount;
    return NOERROR;
}

ECode ByteBufferAsInt64Buffer::Slice(
    /* [out] */ IInt64Buffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)

    mByteBuffer->SetLimit(mLimit * ISizeOf::INT64);
    mByteBuffer->SetPosition(mPosition * ISizeOf::INT64);
    AutoPtr<IByteBuffer> bb;
    mByteBuffer->Slice((IByteBuffer**)&bb);
    ByteOrder midorder;
    mByteBuffer->GetOrder(&midorder);
    bb->SetOrder(midorder);
    AutoPtr<ByteBufferAsInt64Buffer> result = new ByteBufferAsInt64Buffer();
    FAIL_RETURN(result->constructor((ByteBuffer*)bb.Get()))
    mByteBuffer->Clear();
    *buffer = IInt64Buffer::Probe(result);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode ByteBufferAsInt64Buffer::ProtectedArray(
    /* [out, callee] */ ArrayOf<Int64>** array)
{
    assert(0);
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ByteBufferAsInt64Buffer::ProtectedArrayOffset(
    /* [out] */ Int32* offset)
{
    assert(0);
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ByteBufferAsInt64Buffer::ProtectedHasArray(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
