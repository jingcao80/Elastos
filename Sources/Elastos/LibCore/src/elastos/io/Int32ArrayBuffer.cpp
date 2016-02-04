
#include "Int32ArrayBuffer.h"
#include "CByteOrderHelper.h"

namespace Elastos {
namespace IO {

Int32ArrayBuffer::Int32ArrayBuffer()
    : mArrayOffset(0)
    , mIsReadOnly(FALSE)
{
}

ECode Int32ArrayBuffer::constructor(
    /* [in] */ ArrayOf<Int32>* array)
{
    FAIL_RETURN(Int32Buffer::constructor(array->GetLength(), 0))
    mBackingArray = array;
    return NOERROR;
}

ECode Int32ArrayBuffer::constructor(
    /* [in] */ Int32 capacity,
    /* [in] */ ArrayOf<Int32>* backingArray,
    /* [in] */ Int32 offset,
    /* [in] */ Boolean isReadOnly)
{
    FAIL_RETURN(Int32Buffer::constructor(capacity, 0))
    mArrayOffset = offset;
    mIsReadOnly = isReadOnly;
    mBackingArray = backingArray;
    return NOERROR;
}

ECode Int32ArrayBuffer::GetPrimitiveArray(
    /* [out] */ Handle64* arrayHandle)
{
    AutoPtr<ArrayOf<Int32> > arrayTmp;
    GetArray((ArrayOf<Int32>**)&arrayTmp);
    if (arrayTmp == NULL)
    {
        *arrayHandle = 0;
        return NOERROR;
    }
    Int32* primitiveArray = arrayTmp->GetPayload();
    *arrayHandle = reinterpret_cast<Handle64>(primitiveArray);
    return NOERROR;
}

ECode Int32ArrayBuffer::Get(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    if (mPosition == mLimit) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }
    *value = (*mBackingArray)[mArrayOffset + mPosition++];
    return NOERROR;
}

ECode Int32ArrayBuffer::Get(
    /* [in] */ Int32 index,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;
    FAIL_RETURN(CheckIndex(index))
    *value = (*mBackingArray)[mArrayOffset + index];
    return NOERROR;
}

ECode Int32ArrayBuffer::Get(
    /* [out] */ ArrayOf<Int32>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 int32Count)
{
    VALIDATE_NOT_NULL(dst);
    Int32 remaining = 0;
    GetRemaining(&remaining);
    if (int32Count > remaining) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }

    dst->Copy(dstOffset, mBackingArray, mArrayOffset + mPosition, int32Count);
    mPosition += int32Count;
    return NOERROR;
}

ECode Int32ArrayBuffer::IsDirect(
    /* [out] */ Boolean* isDirect)
{
    VALIDATE_NOT_NULL(isDirect);
    *isDirect = FALSE;
    return NOERROR;
}

ECode Int32ArrayBuffer::GetOrder(
   /* [out] */ ByteOrder* byteOrder)
{
    VALIDATE_NOT_NULL(byteOrder)
    AutoPtr<IByteOrderHelper> helper;
    ASSERT_SUCCEEDED(CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&helper))
    return helper->GetNativeOrder(byteOrder);
}


ECode Int32ArrayBuffer::AsReadOnlyBuffer(
    /* [out] */ IInt32Buffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    *buffer = 0;

    AutoPtr<Int32ArrayBuffer> iab;
    FAIL_RETURN(Copy(this, mMark, TRUE, (Int32ArrayBuffer**)&iab))
    *buffer = IInt32Buffer::Probe(iab);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode Int32ArrayBuffer::Compact()
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

ECode Int32ArrayBuffer::Duplicate(
    /* [out] */ IInt32Buffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    *buffer = 0;

    AutoPtr<Int32ArrayBuffer> iab;
    FAIL_RETURN(Copy(this, mMark, mIsReadOnly, (Int32ArrayBuffer**)&iab))
    *buffer = IInt32Buffer::Probe(iab);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode Int32ArrayBuffer::ProtectedArray(
    /* [out, callee] */ ArrayOf<Int32>** array)
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

ECode Int32ArrayBuffer::ProtectedArrayOffset(
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

ECode Int32ArrayBuffer::ProtectedHasArray(
    /* [out] */ Boolean* hasArray)
{
    VALIDATE_NOT_NULL(hasArray)
    *hasArray = TRUE;
    if (mIsReadOnly) {
        *hasArray = FALSE;
    }

    return NOERROR;
}

ECode Int32ArrayBuffer::Put(
    /* [in] */ Int32 d)
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

ECode Int32ArrayBuffer::Put(
    /* [in] */ Int32 index,
    /* [in] */ Int32 d)
{
    if (mIsReadOnly) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index));
    (*mBackingArray)[mArrayOffset + index] = d;
    return NOERROR;
}

ECode Int32ArrayBuffer::Put(
    /* [in] */ const ArrayOf<Int32>& src,
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

ECode Int32ArrayBuffer::Slice(
    /* [out] */ IInt32Buffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    *buffer = 0;

    Int32 remainvalue = 0;
    GetRemaining(&remainvalue);
    AutoPtr<Int32ArrayBuffer> iab = new Int32ArrayBuffer();
    FAIL_RETURN(iab->constructor(remainvalue, mBackingArray, mArrayOffset + mPosition, mIsReadOnly))
    *buffer = IInt32Buffer::Probe(iab);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode Int32ArrayBuffer::IsReadOnly(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mIsReadOnly;
    return NOERROR;
}

ECode Int32ArrayBuffer::Copy(
    /* [in] */ Int32ArrayBuffer* other,
    /* [in] */ Int32 mMarkOfOther,
    /* [in] */ Boolean mIsReadOnly,
    /* [out] */ Int32ArrayBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    *buffer = NULL;

    Int32 capvalue = 0;
    other->GetCapacity(&capvalue);
    AutoPtr<Int32ArrayBuffer> iab = new Int32ArrayBuffer();
    FAIL_RETURN(iab->constructor(capvalue, other->mBackingArray, other->mArrayOffset, mIsReadOnly))

    iab->mLimit = other->mLimit;
    other->GetPosition(&iab->mPosition);
    iab->mMark = mMarkOfOther;
    *buffer = iab;
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
