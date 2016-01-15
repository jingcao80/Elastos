
#include "Int32Buffer.h"
#include "CoreUtils.h"
#include "CArrayOf.h"
#include "Int32ArrayBuffer.h"
#include "Arrays.h"

using Elastos::Core::CArrayOf;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IInteger32;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL_2(Int32Buffer, Object, IInt32Buffer, IBuffer)

Int32Buffer::Int32Buffer()
{}

ECode Int32Buffer::constructor(
    /* [in] */ Int32 capacity,
    /* [in] */ Int64 effectiveDirectAddress)
{
    return Buffer::constructor(2, capacity, effectiveDirectAddress);
}

ECode Int32Buffer::Allocate(
    /* [in] */ Int32 capacity,
    /* [out] */ IInt32Buffer** buf)
{
    VALIDATE_NOT_NULL(buf);
    *buf = NULL;
    if (capacity < 0) {
        // throw new IllegalArgumentException("capacity < 0: " + capacity);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr< ArrayOf<Int32> > res = ArrayOf<Int32>::Alloc(capacity);
    AutoPtr<Int32ArrayBuffer> iab = new Int32ArrayBuffer();
    FAIL_RETURN(iab->constructor(res))
    *buf = IInt32Buffer::Probe(iab);
    REFCOUNT_ADD(*buf)
    return NOERROR;
}

ECode Int32Buffer::Wrap(
    /* [in] */ ArrayOf<Int32>* array,
    /* [out] */ IInt32Buffer** buf)
{
    return Wrap(array, 0, array->GetLength(), buf);
}

ECode Int32Buffer::Wrap(
    /* [in] */ ArrayOf<Int32>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 int32Count,
    /* [out] */ IInt32Buffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);
    *buffer = NULL;
    FAIL_RETURN(Arrays::CheckOffsetAndCount(array->GetLength(), start, int32Count));

    AutoPtr<Int32ArrayBuffer> iab = new Int32ArrayBuffer();
    FAIL_RETURN(iab->constructor(array))
    iab->mPosition = start;
    iab->mLimit = start + int32Count;
    *buffer = IInt32Buffer::Probe(iab);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode Int32Buffer::GetArray(
    /* [out, callee] */ ArrayOf<Int32>** array)
{
    return ProtectedArray(array);
}

ECode Int32Buffer::GetArrayOffset(
    /* [out] */ Int32* offset)
{
    return ProtectedArrayOffset(offset);
}

ECode Int32Buffer::CompareTo(
    /* [in] */ IInt32Buffer* otherBuffer,
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
    Int32 thisInt32, otherInt32;
    while (compareRemaining > 0) {
        Get(thisPos, &thisInt32);
        otherBuffer->Get(otherPos, &otherInt32);
        // checks for Int32 and NaN inequality
        if (thisInt32 != otherInt32) {
            *result = thisInt32 < otherInt32 ? -1 : 1;
            return NOERROR;
        }
        thisPos++;
        otherPos++;
        compareRemaining--;
    }
    *result = remaining - otherRemaining;
    return NOERROR;
}

ECode Int32Buffer::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)
    *rst = FALSE;

    if (IInt32Buffer::Probe(other) == NULL) {
        return NOERROR;
    }

    AutoPtr<IInt32Buffer> otherBuffer = (IInt32Buffer*) other;
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
    Int32 thisValue = 0;
    Int32 otherValue = 0;
    while (equalSoFar && (myPosition < mLimit)) {
        FAIL_RETURN(Get(myPosition++, &thisValue))
        FAIL_RETURN(otherBuffer->Get(otherPosition++, &otherValue))
        equalSoFar = thisValue == otherValue;
    }
    *rst = equalSoFar;
    return NOERROR;
}

ECode Int32Buffer::Get(
    /* [out] */ ArrayOf<Int32>* dst)
{
    return Get(dst, 0, dst->GetLength());
}

ECode Int32Buffer::Get(
    /* [out] */ ArrayOf<Int32>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 int32Count)
{
    VALIDATE_NOT_NULL(dst);
    Int32 arrayLength = dst->GetLength();
    if ((dstOffset | int32Count) < 0 || dstOffset > arrayLength || arrayLength - dstOffset < int32Count) {
        // throw new ArrayIndexOutOfBoundsException(arrayLength, offset,
        //         count);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 remaining = 0;
    GetRemaining(&remaining);
    if (int32Count > remaining) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }
    for (Int32 i = dstOffset; i < dstOffset + int32Count; ++i) {
        Get(&(*dst)[i]);
    }

    return NOERROR;
}

ECode Int32Buffer::HasArray(
    /* [out] */ Boolean* hasArray)
{
    return ProtectedHasArray(hasArray);
}

ECode Int32Buffer::Put(
    /* [in] */ ArrayOf<Int32>* src)
{
    return Put(src, 0, src->GetLength());
}

ECode Int32Buffer::Put(
    /* [in] */ ArrayOf<Int32>* src,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 int32Count)
{
    Int32 arrayLength = src->GetLength();
    if ((srcOffset | int32Count) < 0 || srcOffset > arrayLength || arrayLength - srcOffset < int32Count) {
        // throw new ArrayIndexOutOfBoundsException(arrayLength, offset,
        //         count);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 remaining = 0;
    GetRemaining(&remaining);
    if (int32Count > remaining) {
        // throw new BufferOverflowException();
        return E_BUFFER_OVER_FLOW_EXCEPTION;
    }
    for (Int32 i = srcOffset; i < srcOffset + int32Count; ++i) {
        Put((*src)[i]);
    }
    return NOERROR;
}

ECode Int32Buffer::Put(
    /* [in] */ IInt32Buffer* src)
{
    if (src == (IInt32Buffer*)this->Probe(EIID_IInt32Buffer)) {
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

    AutoPtr< ArrayOf<Int32> > contents = ArrayOf<Int32>::Alloc(srcRemaining);
    FAIL_RETURN(src->Get(contents))
    return Put(contents);
}

ECode Int32Buffer::GetArray(
    /* [out] */ IArrayOf** array)
{
    VALIDATE_NOT_NULL(array)

    AutoPtr< ArrayOf<Int32> > res;
    GetArray((ArrayOf<Int32>**)&res);
    AutoPtr<IArrayOf> iarr;
    CArrayOf::New(EIID_IInteger32, res->GetLength(), (IArrayOf**)&iarr);
    for (Int32 i = 0; i < res->GetLength(); ++i) {
        iarr->Set(i, CoreUtils::Convert((*res)[i]));
    }
    *array = iarr;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
