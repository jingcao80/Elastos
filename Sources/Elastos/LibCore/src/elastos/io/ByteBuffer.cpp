
#include "ByteBuffer.h"
#include "ByteArrayBuffer.h"
#include "DirectByteBuffer.h"
#include "NioUtils.h"
#include "CArrayOf.h"
#include "CoreUtils.h"
#include "Arrays.h"
#include "Memory.h"

using Elastos::Core::IComparable;
using Elastos::Core::CArrayOf;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IByte;
using Elastos::Utility::Arrays;
using Elastos::IO::Channels::FileChannelMapMode_NONE;
using Libcore::IO::Memory;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL_2(ByteBuffer, Buffer, IByteBuffer, IComparable)

ECode ByteBuffer::Allocate(
    /* [in] */ Int32 capacity,
    /* [out] */ IByteBuffer** buf)
{
    VALIDATE_NOT_NULL(buf);
    *buf = NULL;

    if (capacity < 0) {
        // throw new IllegalArgumentException("capacity < 0: " + capacity);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(capacity);
    AutoPtr<ByteArrayBuffer> bb = new ByteArrayBuffer();
    FAIL_RETURN(bb->constructor(bytes))
    *buf = IByteBuffer::Probe(bb);
    REFCOUNT_ADD(*buf);
    return NOERROR;
}

ECode ByteBuffer::AllocateDirect(
    /* [in] */ Int32 capacity,
    /* [out] */ IByteBuffer** buf)
{
    VALIDATE_NOT_NULL(buf);
    *buf = NULL;

    if (capacity < 0) {
        // throw new IllegalArgumentException("capacity < 0: " + capacity);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // Ensure alignment by 8.
    AutoPtr<MemoryBlock> memoryBlock = MemoryBlock::Allocate(capacity + 7);
    Int64 address = memoryBlock->ToInt64();
    Int64 alignedAddress = (address + 7) & ~(Int64)7;
    AutoPtr<DirectByteBuffer> bufObj = new DirectByteBuffer();
    FAIL_RETURN(bufObj->constructor(memoryBlock, capacity, (Int32)(alignedAddress - address), FALSE, FileChannelMapMode_NONE))
    *buf = IByteBuffer::Probe(bufObj);
    REFCOUNT_ADD(*buf);
    return NOERROR;
}

ECode ByteBuffer::Wrap(
    /* [in] */ ArrayOf<Byte>* array,
    /* [out] */ IByteBuffer** buf)
{
    VALIDATE_NOT_NULL(buf);
    *buf = NULL;

    AutoPtr<ByteArrayBuffer> bb = new ByteArrayBuffer();
    FAIL_RETURN(bb->constructor(array))
    *buf = IByteBuffer::Probe(bb);
    REFCOUNT_ADD(*buf);
    return NOERROR;
}

ECode ByteBuffer::Wrap(
    /* [in] */ ArrayOf<Byte>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 byteCount,
    /* [out] */ IByteBuffer** buf)
{
    VALIDATE_NOT_NULL(buf)
    *buf = NULL;
    VALIDATE_NOT_NULL(array)

    FAIL_RETURN(Arrays::CheckOffsetAndCount(array->GetLength(), start, byteCount))
    AutoPtr<ByteArrayBuffer> bb = new ByteArrayBuffer();
    FAIL_RETURN(bb->constructor(array))
    bb->mPosition = start;
    bb->mLimit = start + byteCount;
    *buf = IByteBuffer::Probe(bb);;
    REFCOUNT_ADD(*buf);
    return NOERROR;
}

ByteBuffer::ByteBuffer()
    : mOrder(ByteOrder_BIG_ENDIAN)
{}

ECode ByteBuffer::constructor(
    /* [in] */ Int32 capacity,
    /* [in] */ Int64 effectiveDirectAddress)
{
    return Buffer::constructor(0, capacity, effectiveDirectAddress);
}

ECode ByteBuffer::GetArray(
    /* [out] */ IArrayOf** array)
{
    VALIDATE_NOT_NULL(array)

    AutoPtr< ArrayOf<Byte> > res;
    GetArray((ArrayOf<Byte>**)&res);
    AutoPtr<IArrayOf> iarr;
    CArrayOf::New(EIID_IByte, res->GetLength(), (IArrayOf**)&iarr);
    for (int i = 0; i < res->GetLength(); ++i) {
        iarr->Set(i, CoreUtils::ConvertByte((*res)[i]));
    }
    *array = iarr;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode ByteBuffer::GetArray(
    /* [out, callee] */ ArrayOf<Byte>** array)
{
    return ProtectedArray(array);
}

ECode ByteBuffer::GetArrayOffset(
    /* [out] */ Int32* offset)
{
    return ProtectedArrayOffset(offset);
}

ECode ByteBuffer::CompareTo(
    /* [in] */ IByteBuffer* other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;
    VALIDATE_NOT_NULL(other)

    Int32 thisRemaining = 0;
    Int32 otherRemaining = 0;
    GetRemaining(&thisRemaining);
    IBuffer::Probe(other)->GetRemaining(&otherRemaining);
    Int32 compareRemaining = thisRemaining < otherRemaining ?
                       thisRemaining : otherRemaining;
    Int32 thisPos = mPosition;
    Int32 otherPos = 0;
    IBuffer::Probe(other)->GetPosition(&otherPos);
    Byte thisByte, otherByte;
    while (compareRemaining > 0) {
        this->Get(thisPos, &thisByte);
        other->Get(otherPos, &otherByte);
        if (thisByte != otherByte) {
            *result = thisByte < otherByte ? -1 : 1;
            return NOERROR;
        }
        ++thisPos;
        ++otherPos;
        --compareRemaining;
    }
    GetRemaining(&thisRemaining);
    IBuffer::Probe(other)->GetRemaining(&otherRemaining);
    *result = thisRemaining - otherRemaining;
    return NOERROR;
}

ECode ByteBuffer::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Int32* result)
{
    return CompareTo(IByteBuffer::Probe(other), result);
}

ECode ByteBuffer::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* isEquals)
{
    IByteBuffer* otherObj = IByteBuffer::Probe(other);
    if (NULL == otherObj) {
        *isEquals = FALSE;
        return NOERROR;
    }

    Int32 remaining = 0;
    Int32 otherRemaining = 0;
    GetRemaining(&remaining);
    IBuffer::Probe(other)->GetRemaining(&otherRemaining);
    if (remaining != otherRemaining) {
        *isEquals = FALSE;
        return NOERROR;
    }

    Int32 myPosition = mPosition;
    Int32 otherPosition;
    IBuffer::Probe(other)->GetPosition(&otherPosition);
    Boolean equalSoFar = TRUE;
    Byte value, otherValue;
    while (equalSoFar && (myPosition < mLimit)) {
        Get(myPosition++, &value);
        otherObj->Get(otherPosition++, &otherValue);
        equalSoFar = (value == otherValue);
    }

    *isEquals = equalSoFar;
    return NOERROR;
}

ECode ByteBuffer::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    return NOERROR;
}

ECode ByteBuffer::Get(
    /* [in] */ ArrayOf<Byte>* dst)
{
    return Get(dst, 0, dst->GetLength());
}

ECode ByteBuffer::Get(
    /* [in] */ ArrayOf<Byte>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 byteCount)
{
    VALIDATE_NOT_NULL(dst)

    FAIL_RETURN(Arrays::CheckOffsetAndCount(dst->GetLength(), dstOffset, byteCount))
    Int32 srcRemaining = 0;
    GetRemaining(&srcRemaining);
    if (byteCount > srcRemaining) {
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }

    for (Int32 i = dstOffset; i < dstOffset + byteCount; ++i) {
        Byte value;
        Get(&value);
        (*dst)[i] = value;
    }
    return NOERROR;
}

ECode ByteBuffer::HasArray(
    /* [out] */ Boolean* hasArray)
{
    return ProtectedHasArray(hasArray);
}

ECode ByteBuffer::GetOrder(
    /* [out] */ ByteOrder* byteOrder)
{
    VALIDATE_NOT_NULL(byteOrder);
    *byteOrder = mOrder;
    return NOERROR;
}

ECode ByteBuffer::SetOrder(
    /* [in] */ ByteOrder byteOrder)
{
    return SetOrderImpl(byteOrder);
}

ECode ByteBuffer::SetOrderImpl(
    /* [in] */ ByteOrder byteOrder)
{
    if (byteOrder != ByteOrder_BIG_ENDIAN && byteOrder != ByteOrder_LITTLE_ENDIAN) {
        byteOrder = ByteOrder_LITTLE_ENDIAN;
    }
    mOrder = byteOrder;
    return NOERROR;
}

ECode ByteBuffer::Put(
    /* [in] */ ArrayOf<Byte>* src)
{
    VALIDATE_NOT_NULL(src)

    return Put(src, 0, src->GetLength());
}

ECode ByteBuffer::Put(
    /* [in] */ ArrayOf<Byte>* src,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 byteCount)
{
    VALIDATE_NOT_NULL(src)

    FAIL_RETURN(Arrays::CheckOffsetAndCount(src->GetLength(), srcOffset, byteCount))
    Int32 remaining = 0;
    GetRemaining(&remaining);
    if (byteCount > remaining) {
        return E_BUFFER_OVER_FLOW_EXCEPTION;
    }
    for (Int32 i = srcOffset; i < srcOffset + byteCount; ++i) {
        Put((*src)[i]);
    }
    return NOERROR;
}

ECode ByteBuffer::Put(
    /* [in] */ IByteBuffer* src)
{
    VALIDATE_NOT_NULL(src)

    Boolean isflag = FALSE;
    if (IsAccessible(&isflag), !isflag) {
        // throw new IllegalStateException("buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (IsReadOnly(&isflag), isflag) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }

    if (src == THIS_PROBE(IByteBuffer)) {
        // throw new IllegalArgumentException("src == this");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (src->IsAccessible(&isflag), !isflag) {
        // throw new IllegalStateException("src buffer is inaccessible");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Int32 thisByteCount = 0;
    Int32 srcByteCount = 0;
    GetRemaining(&thisByteCount);
    IBuffer::Probe(src)->GetRemaining(&srcByteCount);
    if (srcByteCount > thisByteCount) {
        return E_BUFFER_OVER_FLOW_EXCEPTION;
    }

    AutoPtr< ArrayOf<Byte> > srcObject;
    Boolean isDirect = FALSE;
    Int32 srcOffset = 0;
    IBuffer::Probe(src)->GetPosition(&srcOffset);
    IBuffer::Probe(src)->IsDirect(&isDirect);
    if (isDirect) {
        FAIL_RETURN(src->GetArray((ArrayOf<Byte>**)&srcObject))
    }
    else {
        srcObject = NioUtils::GetUnsafeArray(src);
        srcOffset += NioUtils::GetUnsafeArrayOffset(src);
    }

    AutoPtr< ArrayOf<Byte> > dstObject;
    Int32 dstOffset = 0;
    GetPosition(&dstOffset);
    IsDirect(&isDirect);
    if (isDirect) {
        FAIL_RETURN(GetArray((ArrayOf<Byte>**)&dstObject))
    }
    else {
        dstObject = NioUtils::GetUnsafeArray(this);
        dstOffset += NioUtils::GetUnsafeArrayOffset(this);
    }

    FAIL_RETURN(Memory::Memmove(dstObject, dstOffset, srcObject, srcOffset, srcByteCount))
    Int32 limit = 0;
    IBuffer::Probe(src)->GetLimit(&limit);
    IBuffer::Probe(src)->SetPosition(limit);
    SetPosition(dstOffset + srcByteCount);
    return NOERROR;
}

ECode ByteBuffer::IsAccessible(
    /* [out] */ Boolean* hasArray)
{
    VALIDATE_NOT_NULL(hasArray)

    *hasArray = TRUE;
    return NOERROR;
}

ECode ByteBuffer::SetAccessible(
    /* [in] */ Boolean accessible)
{
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

} // namespace IO
} // namespace Elastos
