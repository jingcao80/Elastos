
#include "Buffer.h"
#include "StringBuilder.h"
#include "logging/Logger.h"

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(Buffer, Object, IBuffer)

Buffer::Buffer()
    : mCapacity(0)
    , mLimit(0)
    , mMark(IBuffer::UNSET_MARK)
    , mPosition(0)
    , mElementSizeShift(0)
    , mEffectiveDirectAddress(0)
{}

Buffer::~Buffer()
{
}

ECode Buffer::constructor(
    /* [in] */ Int32 elementSizeShift,
    /* [in] */ Int32 capacity,
    /* [in] */ Int64 effectiveDirectAddress)
{
    if (capacity < 0) {
        Logger::E("Buffer", "IllegalArgumentException: capacity < 0: %d", capacity);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mCapacity = capacity;
    mLimit = capacity;
    mEffectiveDirectAddress = effectiveDirectAddress;
    mElementSizeShift = elementSizeShift;

    return NOERROR;
}

ECode Buffer::GetCapacity(
    /* [out] */ Int32* cap)
{
    VALIDATE_NOT_NULL(cap)
    *cap = mCapacity;
    return NOERROR;
}

ECode Buffer::CheckIndex(
    /* [in] */ Int32 index)
{
    if (index < 0 || index >= mLimit) {
        // throw new IndexOutOfBoundsException("index=" + index + ", limit=" + limit);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

ECode Buffer::CheckIndex(
    /* [in] */ Int32 index,
    /* [in] */ Int32 sizeOfType)
{
    if (index < 0 || index > mLimit - sizeOfType) {
        // throw new IndexOutOfBoundsException("index=" + index + ", limit=" + limit +
        //         ", size of type=" + sizeOfType);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

ECode Buffer::CheckGetBounds(
    /* [in] */  Int32 bytesPerElement,
    /* [in] */  Int32 length,
    /* [in] */  Int32 offset,
    /* [in] */  Int32 count,
    /* [out] */ Int32* bounds)
{
    VALIDATE_NOT_NULL(bounds)
    Int32 byteCount = bytesPerElement * count;
    if ((offset | count) < 0 || offset > length || length - offset < count) {
        // throw new IndexOutOfBoundsException("offset=" + offset +
        //         ", count=" + count + ", length=" + length);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    Int32 remaining;
    GetRemaining(&remaining);
    if (byteCount > remaining) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }
    *bounds = byteCount;
    return NOERROR;
}

ECode Buffer::CheckPutBounds(
    /* [in] */  Int32 bytesPerElement,
    /* [in] */  Int32 length,
    /* [in] */  Int32 offset,
    /* [in] */  Int32 count,
    /* [out] */ Int32* bounds)
{
    VALIDATE_NOT_NULL(bounds)
    Int32 byteCount = bytesPerElement * count;
    if ((offset | count) < 0 || offset > length || length - offset < count) {
        // throw new IndexOutOfBoundsException("offset=" + offset +
        //         ", count=" + count + ", length=" + length);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    Int32 remaining;
    GetRemaining(&remaining);
    if (byteCount > remaining) {
        // throw new BufferOverflowException();
        return E_BUFFER_OVER_FLOW_EXCEPTION;
    }
    Boolean result;
    IsReadOnly(&result);
    if (result) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    *bounds = byteCount;
    return NOERROR;
}

ECode Buffer::CheckStartEndRemaining(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    Int32 remaining;
    GetRemaining(&remaining);
    if (end < start || start < 0 || end > remaining) {
        // throw new IndexOutOfBoundsException("start=" + start + ", end=" + end +
        //         ", remaining()=" + remaining());
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

ECode Buffer::Clear()
{
    mPosition = 0;
    mMark = IBuffer::UNSET_MARK;
    mLimit = mCapacity;
    return NOERROR;
}

ECode Buffer::Flip()
{
    mLimit = mPosition;
    mPosition = 0;
    mMark = IBuffer::UNSET_MARK;
    return NOERROR;
}

ECode Buffer::HasRemaining(
    /* [out] */ Boolean* hasRemaining)
{
    VALIDATE_NOT_NULL(hasRemaining)
    *hasRemaining = mPosition < mLimit;
    return NOERROR;
}


ECode Buffer::CheckWritable()
{
    Boolean isReadOnly = FALSE;
    this->IsReadOnly(&isReadOnly);
    if (isReadOnly) {
        // throw new IllegalArgumentException("Read-only buffer");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode Buffer::GetLimit(
    /* [out] */ Int32* limit)
{
    VALIDATE_NOT_NULL(limit)
    *limit = mLimit;
    return NOERROR;
}

ECode Buffer::SetLimit(
    /* [in] */ Int32 newLimit)
{
    return SetLimitImpl(newLimit);
}

ECode Buffer::SetLimitImpl(
    /* [in] */ Int32 newLimit)
{
    if (newLimit < 0 || newLimit > mCapacity) {
        // throw new IllegalArgumentException("Bad limit (capacity " + capacity + "): " + newLimit);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mLimit = newLimit;
    if (mPosition > newLimit) {
        mPosition = newLimit;
    }
    if ((mMark != IBuffer::UNSET_MARK) && (mMark > newLimit)) {
        mMark = IBuffer::UNSET_MARK;
    }
    return NOERROR;
}

ECode Buffer::Mark()
{
    mMark = mPosition;
    return NOERROR;
}

ECode Buffer::GetPosition(
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos)
    *pos = mPosition;
    return NOERROR;
}

ECode Buffer::SetPosition(
    /* [in] */ Int32 newPosition)
{
    return SetPositionImpl(newPosition);
}

ECode Buffer::SetPositionImpl(
    /* [in] */ Int32 newPosition)
{
    if ((newPosition < 0) || (newPosition > mLimit)) {
        // throw new IllegalArgumentException("Bad position (limit " + limit + "): " + newPosition);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mPosition = newPosition;
    if ((mMark != IBuffer::UNSET_MARK) && (mMark > mPosition)) {
        mMark = IBuffer::UNSET_MARK;
    }
    return NOERROR;
}

ECode Buffer::GetRemaining(
   /* [out] */ Int32* remaining)
{
    VALIDATE_NOT_NULL(remaining)
    *remaining = mLimit - mPosition;
    return NOERROR;
}

ECode Buffer::Reset()
{
    if (IBuffer::UNSET_MARK == mMark) {
        // throw new InvalidMarkException("Mark not set");
        return E_INVALID_MARK_EXCEPTION;
    }
    mPosition = mMark;
    return NOERROR;
}

ECode Buffer::Rewind()
{
    mPosition = 0;
    mMark = IBuffer::UNSET_MARK;
    return NOERROR;
}

ECode Buffer::GetElementSizeShift(
    /* [out] */ Int32* elementSizeShift)
{
    VALIDATE_NOT_NULL(elementSizeShift)
    *elementSizeShift = mElementSizeShift;
    return NOERROR;
}

ECode Buffer::GetEffectiveDirectAddress(
    /* [out] */ Handle64* effectiveDirectAddress)
{
    VALIDATE_NOT_NULL(effectiveDirectAddress)
    *effectiveDirectAddress = mEffectiveDirectAddress;
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
