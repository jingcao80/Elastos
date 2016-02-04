
#include "Elastos.CoreLibrary.Libcore.h"
#include "ByteBufferAsInt16Buffer.h"
#include "DirectByteBuffer.h"
#include "ByteArrayBuffer.h"

using Libcore::IO::ISizeOf;

namespace Elastos {
namespace IO {

ByteBufferAsInt16Buffer::ByteBufferAsInt16Buffer()
    : mCap(0)
{}

ECode ByteBufferAsInt16Buffer::constructor(
    /* [in] */ ByteBuffer* byteBuffer)
{
    byteBuffer->GetCapacity(&mCap);
    FAIL_RETURN(Int16Buffer::constructor(mCap / ISizeOf::INT16, byteBuffer->mEffectiveDirectAddress))
    mByteBuffer = byteBuffer;
    mByteBuffer->Clear();
    return NOERROR;
}

ECode ByteBufferAsInt16Buffer::GetPrimitiveArray(
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

AutoPtr<IInt16Buffer> ByteBufferAsInt16Buffer::AsInt16Buffer(
    /* [in] */ ByteBuffer* mByteBuffer)
{
    AutoPtr<IByteBuffer> slice;
    mByteBuffer->Slice((IByteBuffer**)&slice);
    ByteOrder midorder;
    mByteBuffer->GetOrder(&midorder);
    slice->SetOrder(midorder);
    AutoPtr<ByteBufferAsInt16Buffer> bbaib = new ByteBufferAsInt16Buffer();
    bbaib->constructor((ByteBuffer*)slice.Get());
    return IInt16Buffer::Probe(bbaib);
}

ECode ByteBufferAsInt16Buffer::AsReadOnlyBuffer(
    /* [out] */ IInt16Buffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)

    AutoPtr<IByteBuffer> resbb;
    mByteBuffer->AsReadOnlyBuffer((IByteBuffer**)&resbb);

    AutoPtr<ByteBufferAsInt16Buffer> buf = new ByteBufferAsInt16Buffer();
    buf->constructor((ByteBuffer*)resbb.Get());

    buf->mLimit = mLimit;
    buf->mPosition = mPosition;
    buf->mMark = mMark;
    buf->mByteBuffer->mOrder = mByteBuffer->mOrder;
    *buffer = IInt16Buffer::Probe(buf);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode ByteBufferAsInt16Buffer::Compact()
{
    Boolean isflag = FALSE;
    if (mByteBuffer->IsReadOnly(&isflag), isflag) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    mByteBuffer->SetLimit(mLimit * ISizeOf::INT16);
    mByteBuffer->SetPosition(mPosition * ISizeOf::INT16);
    mByteBuffer->Compact();
    mByteBuffer->Clear();
    mPosition = mLimit - mPosition;
    mLimit = mCapacity;
    mMark = UNSET_MARK;
    return NOERROR;
}

ECode ByteBufferAsInt16Buffer::Duplicate(
    /* [out] */ IInt16Buffer** buffer)
{
    AutoPtr<IByteBuffer> bb;
    mByteBuffer->Duplicate((IByteBuffer**)&bb);
    ByteOrder midorder;
    mByteBuffer->GetOrder(&midorder);
    bb->SetOrder(midorder);
    AutoPtr<ByteBufferAsInt16Buffer> buf = new ByteBufferAsInt16Buffer();
    FAIL_RETURN(buf->constructor((ByteBuffer*)bb.Get()))
    buf->mLimit = mLimit;
    buf->mPosition = mPosition;
    buf->mMark = mMark;
    *buffer = IInt16Buffer::Probe(buf);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode ByteBufferAsInt16Buffer::Get(
    /* [out] */ Int16* value)
{
    if (mPosition == mLimit) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    return mByteBuffer->GetInt16(mPosition++ * ISizeOf::INT16, value);
}

ECode ByteBufferAsInt16Buffer::Get(
    /* [in] */ Int32 index,
    /* [out] */ Int16* value)
{
    FAIL_RETURN(CheckIndex(index));
    return mByteBuffer->GetInt16(index * ISizeOf::INT16, value);
}

ECode ByteBufferAsInt16Buffer::Get(
    /* [out] */ ArrayOf<Int16>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 charCount)
{
    mByteBuffer->SetLimit(mLimit * ISizeOf::INT16);
    mByteBuffer->SetPosition(mPosition * ISizeOf::INT16);
    AutoPtr<DirectByteBuffer> res = static_cast<DirectByteBuffer*>(mByteBuffer.Get());
    if (res) {
        res->GetInt16s(dst, dstOffset, charCount);
    }
    else {
        ((ByteArrayBuffer*) mByteBuffer.Get())->GetInt16s(dst, dstOffset, charCount);
    }
    mPosition += charCount;
    return NOERROR;
}

ECode ByteBufferAsInt16Buffer::IsDirect(
    /* [out] */ Boolean* rst)
{
    return mByteBuffer->IsDirect(rst);
}

ECode ByteBufferAsInt16Buffer::IsReadOnly(
    /* [out] */ Boolean* rst)
{
    return mByteBuffer->IsReadOnly(rst);
}

ECode ByteBufferAsInt16Buffer::GetOrder(
    /* [out] */ ByteOrder* byteOrder)
{
    return mByteBuffer->GetOrder(byteOrder);
}

ECode ByteBufferAsInt16Buffer::Put(
    /* [in] */ Int16 c)
{
    if (mPosition == mLimit) {
        // throw new BufferOverflowException();
        return E_BUFFER_UNDERFLOW_EXCEPTION;
    }
    return mByteBuffer->PutInt16(mPosition++ * ISizeOf::INT16, c);
}

ECode ByteBufferAsInt16Buffer::Put(
    /* [in] */ Int32 index,
    /* [in] */ Int16 c)
{
    FAIL_RETURN(CheckIndex(index));
    return mByteBuffer->PutInt16(index * ISizeOf::INT16, c);
}

ECode ByteBufferAsInt16Buffer::Put(
    /* [in] */ ArrayOf<Int16>* src,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 charCount)
{
    mByteBuffer->SetLimit(mLimit * ISizeOf::INT16);
    mByteBuffer->SetPosition(mPosition * ISizeOf::INT16);
    AutoPtr<DirectByteBuffer> res = static_cast<DirectByteBuffer*>(mByteBuffer.Get());
    if (res) {
        res->PutInt16s(src, srcOffset, charCount);
    } else {
        ((ByteArrayBuffer*) mByteBuffer.Get())->PutInt16s(src, srcOffset, charCount);
    }
    mPosition += charCount;
    return NOERROR;
}

ECode ByteBufferAsInt16Buffer::Slice(
    /* [out] */ IInt16Buffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)

    mByteBuffer->SetLimit(mLimit * ISizeOf::INT16);
    mByteBuffer->SetPosition(mPosition * ISizeOf::INT16);
    AutoPtr<IByteBuffer> bb;
    mByteBuffer->Slice((IByteBuffer**)&bb);
    ByteOrder midorder;
    mByteBuffer->GetOrder(&midorder);
    bb->SetOrder(midorder);
    AutoPtr<ByteBufferAsInt16Buffer> result = new ByteBufferAsInt16Buffer();
    FAIL_RETURN(result->constructor((ByteBuffer*)bb.Get()))
    mByteBuffer->Clear();
    *buffer = IInt16Buffer::Probe(result);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode ByteBufferAsInt16Buffer::ProtectedArray(
    /* [out, callee] */ ArrayOf<Int16>** array)
{
    assert(0);
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ByteBufferAsInt16Buffer::ProtectedArrayOffset(
    /* [out] */ Int32* offset)
{
    assert(0);
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ByteBufferAsInt16Buffer::ProtectedHasArray(
    /* [out] */ Boolean* result)
{
    *result = FALSE;
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
