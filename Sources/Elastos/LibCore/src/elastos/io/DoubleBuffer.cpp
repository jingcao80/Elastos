
#include "DoubleBuffer.h"
#include "CoreUtils.h"
#include "CArrayOf.h"
#include "DoubleArrayBuffer.h"
#include "Arrays.h"

using Elastos::Core::CArrayOf;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IDouble;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL_2(DoubleBuffer, Object, IDoubleBuffer, IBuffer)

DoubleBuffer::DoubleBuffer()
{}

ECode DoubleBuffer::constructor(
    /* [in] */ Int32 capacity,
    /* [in] */ Int64 effectiveDirectAddress)
{
    return Buffer::constructor(3, capacity, effectiveDirectAddress);
}

ECode DoubleBuffer::Allocate(
    /* [in] */ Int32 capacity,
    /* [out] */ IDoubleBuffer** buf)
{
    VALIDATE_NOT_NULL(buf);

    if (capacity < 0) {
        // throw new IllegalArgumentException("capacity < 0: " + capacity);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr< ArrayOf<Double> > res = ArrayOf<Double>::Alloc(capacity);
    AutoPtr<DoubleArrayBuffer> dab = new DoubleArrayBuffer();
    FAIL_RETURN(dab->constructor(res))
    *buf = IDoubleBuffer::Probe(dab);
    REFCOUNT_ADD(*buf)
    return NOERROR;
}

ECode DoubleBuffer::Wrap(
    /* [in] */ ArrayOf<Double>* array,
    /* [out] */ IDoubleBuffer** buf)
{
    return Wrap(array, 0, array->GetLength(), buf);
}

ECode DoubleBuffer::Wrap(
    /* [in] */ ArrayOf<Double>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 doubleCount,
    /* [out] */ IDoubleBuffer** buf)
{
    VALIDATE_NOT_NULL(buf);

    FAIL_RETURN(Arrays::CheckOffsetAndCount(array->GetLength(), start, doubleCount));
    AutoPtr<DoubleArrayBuffer> dab = new DoubleArrayBuffer();
    FAIL_RETURN(dab->constructor(array))
    dab->mPosition = start;
    dab->mLimit = start + doubleCount;
    *buf = IDoubleBuffer::Probe(dab);
    REFCOUNT_ADD(*buf)
    return NOERROR;
}

ECode DoubleBuffer::GetArray(
    /* [out, callee] */ ArrayOf<Double>** array)
{
    return ProtectedArray(array);
}

ECode DoubleBuffer::GetArrayOffset(
    /* [out] */ Int32* offset)
{
    return ProtectedArrayOffset(offset);
}

ECode DoubleBuffer::CompareTo(
    /* [in] */  IDoubleBuffer* otherBuffer,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(otherBuffer);
    VALIDATE_NOT_NULL(result);

    Int32 remaining = 0;
    Int32 otherRemaining = 0;
    GetRemaining(&remaining);
    IBuffer::Probe(otherBuffer)->GetRemaining(&otherRemaining);
    Int32 compareRemaining = (remaining < otherRemaining) ? remaining : otherRemaining;

    Int32 thisPos = mPosition;
    Int32 otherPos = 0;
    IBuffer::Probe(otherBuffer)->GetPosition(&otherPos);
    Double thisDouble, otherDouble;
    while (compareRemaining > 0) {
        Get(thisPos, &thisDouble);
        otherBuffer->Get(otherPos, &otherDouble);
        if ((thisDouble != otherDouble)
                && ((thisDouble == thisDouble) || (otherDouble == otherDouble))) {
            *result = thisDouble < otherDouble ? -1 : 1;
            return NOERROR;
        }
        thisPos++;
        otherPos++;
        compareRemaining--;
    }

    *result = remaining - otherRemaining;
    return NOERROR;
}

ECode DoubleBuffer::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst);
    *rst = FALSE;
    VALIDATE_NOT_NULL(other);

    AutoPtr<IDoubleBuffer> otherObj = (IDoubleBuffer*)(other->Probe(EIID_IDoubleBuffer));
    if (otherObj == NULL) return NOERROR;

    Int32 remaining = 0;
    Int32 otherRemaining = 0;
    GetRemaining(&remaining);
    IBuffer::Probe(otherObj)->GetRemaining(&otherRemaining);
    if (remaining != otherRemaining) {
        *rst = FALSE;
        return NOERROR;
    }

    Int32 otherPosition = 0;
    Int32 myPosition = mPosition;
    IBuffer::Probe(otherObj)->GetPosition(&otherPosition);
    Boolean equalSoFar = TRUE;
    Double thisValue = 0.0;
    Double otherValue = 0.0;
    while (equalSoFar && (mPosition < mLimit)) {
        this->Get(myPosition++, &thisValue);
        otherObj->Get(otherPosition++, &otherValue);
        equalSoFar = (thisValue == otherValue) ||
                     (thisValue != thisValue && otherValue != otherValue);
    }

    *rst = equalSoFar;
    return NOERROR;
}

ECode DoubleBuffer::Get(
    /* [out] */ ArrayOf<Double>* dst)
{
    return Get(dst, 0, dst->GetLength());
}

ECode DoubleBuffer::Get(
    /* [out] */ ArrayOf<Double>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 doubleCount)
{
    VALIDATE_NOT_NULL(dst);
    Int32 arrayLength = dst->GetLength();
    if ((dstOffset | doubleCount) < 0 || dstOffset > arrayLength || arrayLength - dstOffset < doubleCount) {
        // throw new ArrayIndexOutOfBoundsException(arrayLength, offset,
        //         count);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 remaining = 0;
    GetRemaining(&remaining);
    if (doubleCount > remaining) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }
    for (Int32 i = dstOffset; i < dstOffset + doubleCount; i++) {
        Get(&(*dst)[i]);
    }

    return NOERROR;
}

ECode DoubleBuffer::HasArray(
    /* [out] */ Boolean* hasArray)
{
    return ProtectedHasArray(hasArray);
}

ECode DoubleBuffer::Put(
    /* [in] */ const ArrayOf<Double>& src)
{
    return Put(src, 0, src.GetLength());
}

ECode DoubleBuffer::Put(
    /* [in] */ const ArrayOf<Double>& src,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 doubleCount)
{
    Int32 arrayLength = src.GetLength();
    if ((srcOffset | doubleCount) < 0 || srcOffset > arrayLength || arrayLength - srcOffset < doubleCount) {
        // throw new ArrayIndexOutOfBoundsException(arrayLength, offset,
        //         count);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 remaining = 0;
    GetRemaining(&remaining);
    if (doubleCount > remaining) {
        // throw new BufferOverflowException();
        return E_BUFFER_OVER_FLOW_EXCEPTION;
    }
    for (Int32 i = srcOffset; i < srcOffset + doubleCount; i++) {
        Put(src[i]);
    }
    return NOERROR;
}

ECode DoubleBuffer::Put(
    /* [in] */ IDoubleBuffer* src)
{
    if (src == this) {
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

    AutoPtr< ArrayOf<Double> > doubles = ArrayOf<Double>::Alloc(srcRemaining);
    FAIL_RETURN(src->Get(doubles))
    return Put(*doubles);
}

ECode DoubleBuffer::GetArray(
    /* [out] */ IArrayOf** array)
{
    VALIDATE_NOT_NULL(array)

    AutoPtr< ArrayOf<Double> > res;
    GetArray((ArrayOf<Double>**)&res);
    AutoPtr<IArrayOf> iarr;
    CArrayOf::New(EIID_IDouble, res->GetLength(), (IArrayOf**)&iarr);
    for (Int32 i = 0; i < res->GetLength(); ++i) {
        iarr->Set(i, CoreUtils::Convert((*res)[i]));
    }
    *array = iarr;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
