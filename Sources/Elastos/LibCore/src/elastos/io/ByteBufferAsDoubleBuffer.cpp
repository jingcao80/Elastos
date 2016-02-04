
#include "Elastos.CoreLibrary.Libcore.h"
#include "ByteBufferAsDoubleBuffer.h"
#include "DirectByteBuffer.h"
#include "ByteArrayBuffer.h"

using Libcore::IO::ISizeOf;

namespace Elastos {
namespace IO {

ByteBufferAsDoubleBuffer::ByteBufferAsDoubleBuffer()
    : mCap(0)
{}

ECode ByteBufferAsDoubleBuffer::constructor(
    /* [in] */ ByteBuffer* byteBuffer)
{
    byteBuffer->GetCapacity(&mCap);
    FAIL_RETURN(DoubleBuffer::constructor(mCap / ISizeOf::DOUBLE, byteBuffer->mEffectiveDirectAddress))
    mByteBuffer = byteBuffer;
    mByteBuffer->Clear();
    return NOERROR;
}

ECode ByteBufferAsDoubleBuffer::GetPrimitiveArray(
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

AutoPtr<IDoubleBuffer> ByteBufferAsDoubleBuffer::AsDoubleBuffer(
    /* [in] */ ByteBuffer* mByteBuffer)
{
    AutoPtr<IByteBuffer> slice;
    mByteBuffer->Slice((IByteBuffer**)&slice);
    ByteOrder midorder;
    mByteBuffer->GetOrder(&midorder);
    slice->SetOrder(midorder);
    AutoPtr<ByteBufferAsDoubleBuffer> bbadb = new ByteBufferAsDoubleBuffer();
    bbadb->constructor((ByteBuffer*)slice.Get());
    AutoPtr<IDoubleBuffer> res = IDoubleBuffer::Probe(bbadb);
    return res;
}

ECode ByteBufferAsDoubleBuffer::AsReadOnlyBuffer(
    /* [out] */ IDoubleBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)

    AutoPtr<IByteBuffer> resbb;
    mByteBuffer->AsReadOnlyBuffer((IByteBuffer**)&resbb);
    AutoPtr<ByteBufferAsDoubleBuffer> buf = new ByteBufferAsDoubleBuffer();
    buf->constructor((ByteBuffer*)resbb.Get());
    buf->mLimit = mLimit;
    buf->mPosition = mPosition;
    buf->mMark = mMark;
    buf->mByteBuffer->mOrder = mByteBuffer->mOrder;
    *buffer = IDoubleBuffer::Probe(buf);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode ByteBufferAsDoubleBuffer::Compact()
{
    Boolean isflag = FALSE;
    if (mByteBuffer->IsReadOnly(&isflag), isflag) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    mByteBuffer->SetLimit(mLimit * ISizeOf::DOUBLE);
    mByteBuffer->SetPosition(mPosition * ISizeOf::DOUBLE);
    mByteBuffer->Compact();
    mByteBuffer->Clear();
    mPosition = mLimit - mPosition;
    mLimit = mCapacity;
    mMark = UNSET_MARK;
    return NOERROR;
}

ECode ByteBufferAsDoubleBuffer::Duplicate(
    /* [out] */ IDoubleBuffer** buffer)
{
    AutoPtr<IByteBuffer> bb;
    mByteBuffer->Duplicate((IByteBuffer**)&bb);
    ByteOrder midorder;
    mByteBuffer->GetOrder(&midorder);
    bb->SetOrder(midorder);

    AutoPtr<ByteBufferAsDoubleBuffer> buf = new ByteBufferAsDoubleBuffer();
    buf->constructor((ByteBuffer*)bb.Get());
    buf->mLimit = mLimit;
    buf->mPosition = mPosition;
    buf->mMark = mMark;
    *buffer = IDoubleBuffer::Probe(buf);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode ByteBufferAsDoubleBuffer::Get(
    /* [out] */ Double* value)
{
    if (mPosition == mLimit) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    return mByteBuffer->GetDouble(mPosition++ * ISizeOf::DOUBLE, value);
}

ECode ByteBufferAsDoubleBuffer::Get(
    /* [in] */ Int32 index,
    /* [out] */ Double* value)
{
    FAIL_RETURN(CheckIndex(index));
    return mByteBuffer->GetDouble(index * ISizeOf::DOUBLE, value);
}

ECode ByteBufferAsDoubleBuffer::Get(
    /* [out] */ ArrayOf<Double>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 doubleCount)
{
    mByteBuffer->SetLimit(mLimit * ISizeOf::DOUBLE);
    mByteBuffer->SetPosition(mPosition * ISizeOf::DOUBLE);
    AutoPtr<DirectByteBuffer> res = static_cast<DirectByteBuffer*>(mByteBuffer.Get());
    if (res) {
        res->GetDoubles(dst, dstOffset, doubleCount);
    } else {
        ((ByteArrayBuffer*) mByteBuffer.Get())->GetDoubles(dst, dstOffset, doubleCount);
    }
    mPosition += doubleCount;
    return NOERROR;
}

ECode ByteBufferAsDoubleBuffer::IsDirect(
    /* [out] */ Boolean* rst)
{
    return mByteBuffer->IsDirect(rst);
}

ECode ByteBufferAsDoubleBuffer::IsReadOnly(
    /* [out] */ Boolean* rst)
{
    return mByteBuffer->IsReadOnly(rst);
}

ECode ByteBufferAsDoubleBuffer::GetOrder(
    /* [out] */ ByteOrder* byteOrder)
{
    return mByteBuffer->GetOrder(byteOrder);
}

ECode ByteBufferAsDoubleBuffer::Put(
    /* [in] */ Double c)
{
    if (mPosition == mLimit) {
        // throw new BufferOverflowException();
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    return mByteBuffer->PutDouble(mPosition++ * ISizeOf::DOUBLE, c);
}

ECode ByteBufferAsDoubleBuffer::Put(
    /* [in] */ Int32 index,
    /* [in] */ Double c)
{
    FAIL_RETURN(CheckIndex(index));
    return mByteBuffer->PutDouble(index * ISizeOf::DOUBLE, c);
}

ECode ByteBufferAsDoubleBuffer::Put(
    /* [in] */ ArrayOf<Double>* src,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 doubleCount)
{
    mByteBuffer->SetLimit(mLimit * ISizeOf::DOUBLE);
    mByteBuffer->SetPosition(mPosition * ISizeOf::DOUBLE);
    AutoPtr<DirectByteBuffer> res = static_cast<DirectByteBuffer*>(mByteBuffer.Get());
    if (res) {
        res->PutDoubles(src, srcOffset, doubleCount);
    } else {
        ((ByteArrayBuffer*) mByteBuffer.Get())->PutDoubles(src, srcOffset, doubleCount);
    }
    mPosition += doubleCount;
    return NOERROR;
}

ECode ByteBufferAsDoubleBuffer::Slice(
    /* [out] */ IDoubleBuffer** buffer)
{
    mByteBuffer->SetLimit(mLimit * ISizeOf::DOUBLE);
    mByteBuffer->SetPosition(mPosition * ISizeOf::DOUBLE);
    AutoPtr<IByteBuffer> bb;
    mByteBuffer->Slice((IByteBuffer**)&bb);
    ByteOrder midorder;
    mByteBuffer->GetOrder(&midorder);
    bb->SetOrder(midorder);

    AutoPtr<ByteBufferAsDoubleBuffer> result = new ByteBufferAsDoubleBuffer();
    result->constructor((ByteBuffer*)bb.Get());
    mByteBuffer->Clear();
    *buffer = IDoubleBuffer::Probe(result);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode ByteBufferAsDoubleBuffer::ProtectedArray(
    /* [out, callee] */ ArrayOf<Double>** array)
{
    assert(0);
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ByteBufferAsDoubleBuffer::ProtectedArrayOffset(
    /* [out] */ Int32* offset)
{
    assert(0);
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ByteBufferAsDoubleBuffer::ProtectedHasArray(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
