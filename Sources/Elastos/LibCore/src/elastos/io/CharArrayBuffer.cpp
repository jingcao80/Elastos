
#include "CharArrayBuffer.h"
#include "CByteOrderHelper.h"
#include "StringBuilder.h"

using Elastos::Core::EIID_ICharSequence;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace IO {

CharArrayBuffer::CharArrayBuffer()
    : mArrayOffset(0)
    , mIsReadOnly(FALSE)
{
}

ECode CharArrayBuffer::constructor(
    /* [in] */ ArrayOf<Char32>* array)
{
    FAIL_RETURN(CharBuffer::constructor(array->GetLength(), 0))
    mBackingArray = array;
    return NOERROR;
}

ECode CharArrayBuffer::constructor(
    /* [in] */ Int32 capacity,
    /* [in] */ ArrayOf<Char32>* backingArray,
    /* [in] */ Int32 offset,
    /* [in] */ Boolean isReadOnly)
{
    FAIL_RETURN(CharBuffer::constructor(capacity, 0))
    mBackingArray = backingArray;
    mArrayOffset = offset;
    mIsReadOnly = isReadOnly;
    return NOERROR;
}

ECode CharArrayBuffer::Get(
    /* [out] */ Char32* value)
{
    VALIDATE_NOT_NULL(value)
    if (mPosition == mLimit) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }
    *value = (*mBackingArray)[mArrayOffset + mPosition++];
    return NOERROR;
}

ECode CharArrayBuffer::Get(
    /* [in] */ Int32 index,
    /* [out] */ Char32* value)
{
    VALIDATE_NOT_NULL(value)
    FAIL_RETURN(CheckIndex(index))
    *value = (*mBackingArray)[mArrayOffset + index];
    return NOERROR;
}

ECode CharArrayBuffer::Get(
    /* [out] */ ArrayOf<Char32>* dst)
{
    return CharBuffer::Get(dst);
}

ECode CharArrayBuffer::Get(
    /* [out] */ ArrayOf<Char32>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 charCount)
{
    Int32 remaining;
    GetRemaining(&remaining);
    if (charCount > remaining) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }
    dst->Copy(dstOffset, mBackingArray, mArrayOffset + mPosition, charCount);
    mPosition += charCount;
    return NOERROR;
}

ECode CharArrayBuffer::IsDirect(
    /* [out] */ Boolean* isDirect)
{
    VALIDATE_NOT_NULL(isDirect)
    *isDirect = FALSE;
    return NOERROR;
}

ECode CharArrayBuffer::GetOrder(
    /* [out] */ ByteOrder* byteOrder)
{
    VALIDATE_NOT_NULL(byteOrder)
    AutoPtr<IByteOrderHelper> helper;
    ASSERT_SUCCEEDED(CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&helper))
    return helper->GetNativeOrder(byteOrder);
}

ECode CharArrayBuffer::SubSequence(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq)
    *csq = NULL;
    FAIL_RETURN(CheckStartEndRemaining(start, end))
    AutoPtr<ICharBuffer> result;
    Duplicate((ICharBuffer**)&result);
    IBuffer::Probe(result)->SetLimit(mPosition + end);
    IBuffer::Probe(result)->SetPosition(mPosition + start);
    *csq = ICharSequence::Probe(result);
    REFCOUNT_ADD(*csq);
    return NOERROR;
}

ECode CharArrayBuffer::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    // return String.copyValueOf(mBackingArray, offset + mPosition, remaining());
    Int32 remaining = 0;
    Int32 offset = mArrayOffset + mPosition;
    StringBuilder sb;
    GetRemaining(&remaining);
    for (Int32 i = 0; i < remaining; ++i) {
        sb += (*mBackingArray)[offset + i];
    }

    return sb.ToString(str);
}

ECode CharArrayBuffer::IsReadOnly(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mIsReadOnly;
    return NOERROR;
}

ECode CharArrayBuffer::AsReadOnlyBuffer(
    /* [out] */ ICharBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    *buffer = NULL;
    AutoPtr<CharArrayBuffer> res;
    FAIL_RETURN(Copy(this, mMark, TRUE, (CharArrayBuffer**)&res))
    *buffer = res;
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode CharArrayBuffer::Compact()
{
    if (mIsReadOnly) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    // System.arraycopy(mBackingArray, mPosition + mArrayOffset, mBackingArray, mArrayOffset, remaining());
    Int32 lastnum = 0;
    GetRemaining(&lastnum);
    mBackingArray->Copy(mArrayOffset, mBackingArray, mPosition + mArrayOffset, lastnum);
    mPosition = mLimit - mPosition;
    mLimit = mCapacity;
    mMark = UNSET_MARK;
    return NOERROR;
}

ECode CharArrayBuffer::Duplicate(
    /* [out] */ ICharBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    *buffer = NULL;
    AutoPtr<CharArrayBuffer> res;
    FAIL_RETURN(Copy(this, mMark, mIsReadOnly, (CharArrayBuffer**)&res))
    *buffer = res;
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode CharArrayBuffer::Put(
    /* [in] */ Char32 c)
{
    if (mIsReadOnly) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    if (mPosition == mLimit) {
        // throw new BufferOverflowException();
        return E_BUFFER_OVERFLOW_EXCEPTION;
    }
    (*mBackingArray)[mArrayOffset + mPosition++] = c;
    return NOERROR;
}

ECode CharArrayBuffer::Put(
    /* [in] */ Int32 index,
    /* [in] */ Char32 c)
{
    if (mIsReadOnly) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }
    FAIL_RETURN(CheckIndex(index));
    (*mBackingArray)[mArrayOffset + index] = c;
    return NOERROR;
}

ECode CharArrayBuffer::Slice(
    /* [out] */ ICharBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    *buffer = NULL;

    Int32 remvalue = 0;
    GetRemaining(&remvalue);
    AutoPtr<CharArrayBuffer> cab = new CharArrayBuffer();
    FAIL_RETURN(cab->constructor(remvalue, mBackingArray, mArrayOffset + mPosition, mIsReadOnly))
    *buffer = ICharBuffer::Probe(cab);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode CharArrayBuffer::ProtectedArray(
    /* [out, callee] */ ArrayOf<Char32>** array)
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

ECode CharArrayBuffer::ProtectedArrayOffset(
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

ECode CharArrayBuffer::ProtectedHasArray(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
        *result = TRUE;

    if (mIsReadOnly) {
        *result = FALSE;
    }

    return NOERROR;
}

ECode CharArrayBuffer::Copy(
    /* [in] */ CharArrayBuffer* other,
    /* [in] */ Int32 markOfOther,
    /* [in] */ Boolean mIsReadOnly,
    /* [out] */ CharArrayBuffer** cab)
{
    VALIDATE_NOT_NULL(cab)
    *cab = NULL;

    Int32 remvalue = 0;
    other->GetRemaining(&remvalue);
    AutoPtr<CharArrayBuffer> buf = new CharArrayBuffer();
    FAIL_RETURN(buf->constructor(remvalue, other->mBackingArray, other->mArrayOffset, mIsReadOnly))
    buf->mLimit = other->mLimit;
    Int32 posvalue = 0;
    other->GetPosition(&posvalue);
    buf->mPosition = posvalue;
    buf->mMark = markOfOther;
    *cab = buf;
    REFCOUNT_ADD(*cab)
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
