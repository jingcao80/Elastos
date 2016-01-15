
#include "Int64Buffer.h"
#include "CArrayOf.h"
#include "CoreUtils.h"
#include "Int64ArrayBuffer.h"
#include "Arrays.h"

using Elastos::Core::CArrayOf;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IInteger64;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL_2(Int64Buffer, Object, IInt64Buffer, IBuffer)

Int64Buffer::Int64Buffer()
{}

ECode Int64Buffer::constructor(
    /* [in] */ Int32 capacity,
    /* [in] */ Int64 effectiveDirectAddress)
{
    return Buffer::constructor(3, capacity, effectiveDirectAddress);
}

ECode Int64Buffer::Allocate(
    /* [in] */ Int32 capacity,
    /* [out] */ IInt64Buffer** buf)
{
    VALIDATE_NOT_NULL(buf);

    if (capacity < 0) {
        // throw new IllegalArgumentException("capacity < 0: " + capacity);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr< ArrayOf<Int64> > res = ArrayOf<Int64>::Alloc(capacity);
    AutoPtr<Int64ArrayBuffer> iab = new Int64ArrayBuffer();
    FAIL_RETURN(iab->constructor(res))
    *buf = IInt64Buffer::Probe(iab);
    REFCOUNT_ADD(*buf)
    return NOERROR;
}

ECode Int64Buffer::Wrap(
    /* [in] */ ArrayOf<Int64>* array,
    /* [out] */ IInt64Buffer** buf)
{
    return Wrap(array, 0, array->GetLength(), buf);
}

ECode Int64Buffer::Wrap(
    /* [in] */ ArrayOf<Int64>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 int64Count,
    /* [out] */ IInt64Buffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);
    *buffer = NULL;

    FAIL_RETURN(Arrays::CheckOffsetAndCount(array->GetLength(), start, int64Count));
    AutoPtr<Int64ArrayBuffer> iab = new Int64ArrayBuffer();
    FAIL_RETURN(iab->constructor(array))
    iab->mPosition = start;
    iab->mLimit = start + int64Count;
    *buffer = IInt64Buffer::Probe(iab);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode Int64Buffer::GetArray(
    /* [out] */ IArrayOf** array)
{
    VALIDATE_NOT_NULL(array)
    *array = NULL;

    AutoPtr< ArrayOf<Int64> > res;
    GetArray((ArrayOf<Int64>**)&res);
    AutoPtr<IArrayOf> iarr;
    CArrayOf::New(EIID_IInteger64, res->GetLength(), (IArrayOf**)&iarr);
    for (int i = 0; i < res->GetLength(); ++i) {
        iarr->Set(i, CoreUtils::Convert((*res)[i]));
    }
    *array = iarr;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode Int64Buffer::GetArray(
    /* [out, callee] */ ArrayOf<Int64>** array)
{
    return ProtectedArray(array);
}

ECode Int64Buffer::GetArrayOffset(
    /* [out] */ Int32* offset)
{
    return ProtectedArrayOffset(offset);
}

ECode Int64Buffer::CompareTo(
    /* [in] */ IInt64Buffer* otherBuffer,
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
    Int64 thisInt64 = 0;
    Int64 otherInt64 = 0;
    while (compareRemaining > 0) {
        Get(thisPos, &thisInt64);
        otherBuffer->Get(otherPos, &otherInt64);
        // checks for Int64 and NaN inequality
        if (thisInt64 != otherInt64) {
            *result = thisInt64 < otherInt64 ? -1 : 1;
            return NOERROR;
        }
        thisPos++;
        otherPos++;
        compareRemaining--;
    }
    *result = remaining - otherRemaining;
    return NOERROR;
}

ECode Int64Buffer::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)
    *rst = FALSE;

    if (IInt64Buffer::Probe(other) == NULL) {
        return NOERROR;
    }

    AutoPtr<IInt64Buffer> otherBuffer = (IInt64Buffer*) other;
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
    Int64 thisValue = 0;
    Int64 otherValue = 0;
    while (equalSoFar && (myPosition < mLimit)) {
        FAIL_RETURN(Get(myPosition++, &thisValue))
        FAIL_RETURN(otherBuffer->Get(otherPosition++, &otherValue))
        equalSoFar = thisValue == otherValue;
    }
    *rst = equalSoFar;
    return NOERROR;
}

ECode Int64Buffer::Get(
    /* [out] */ ArrayOf<Int64>* dst)
{
    return Get(dst, 0, dst->GetLength());
}

ECode Int64Buffer::Get(
    /* [out] */ ArrayOf<Int64>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 int64Count)
{
    VALIDATE_NOT_NULL(dst);
    Int32 arrayLength = dst->GetLength();
    if ((dstOffset | int64Count) < 0 || dstOffset > arrayLength || arrayLength - dstOffset < int64Count) {
        // throw new ArrayIndexOutOfBoundsException(arrayLength, offset,
        //         count);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 remaining = 0;
    GetRemaining(&remaining);
    if (int64Count > remaining) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }

    for (Int32 i = dstOffset; i < dstOffset + int64Count; ++i) {
        Get(&(*dst)[i]);
    }
    return NOERROR;
}

ECode Int64Buffer::HasArray(
    /* [out] */ Boolean* hasArray)
{
    return ProtectedHasArray(hasArray);
}

ECode Int64Buffer::Put(
    /* [in] */ const ArrayOf<Int64>& src)
{
    return Put(src, 0, src.GetLength());
}

ECode Int64Buffer::Put(
    /* [in] */ const ArrayOf<Int64>& src,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 int64Count)
{
    Int32 arrayLength = src.GetLength();
    if ((srcOffset | int64Count) < 0 || srcOffset > arrayLength || arrayLength - srcOffset < int64Count) {
        // throw new ArrayIndexOutOfBoundsException(arrayLength, offset,
        //         count);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 remaining = 0;
    GetRemaining(&remaining);
    if (int64Count > remaining) {
        // throw new BufferOverflowException();
        return E_BUFFER_OVER_FLOW_EXCEPTION;
    }

    for (Int32 i = srcOffset; i < srcOffset + int64Count; ++i) {
        Put(src[i]);
    }
    return NOERROR;
}

ECode Int64Buffer::Put(
    /* [in] */ IInt64Buffer* src)
{
    if (src == (IInt64Buffer*)this->Probe(EIID_IInt64Buffer)) {
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

    AutoPtr< ArrayOf<Int64> > contents = ArrayOf<Int64>::Alloc(srcRemaining);
    FAIL_RETURN(src->Get(contents))
    return Put(*contents);
}

} // namespace IO
} // namespace Elastos
