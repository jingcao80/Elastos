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

#include "CharSequenceAdapter.h"
#include "CByteOrderHelper.h"

using Elastos::Core::EIID_ICharSequence;

namespace Elastos {
namespace IO {

CharSequenceAdapter::CharSequenceAdapter()
{
}

ECode CharSequenceAdapter::constructor(
    /* [in] */ Int32 capacity,
    /* [in] */ ICharSequence* chseq)
{
    FAIL_RETURN(CharBuffer::constructor(capacity, 0))
    mSequence = chseq;
    return NOERROR;
}

ECode CharSequenceAdapter::GetPrimitiveArray(
    /* [out] */ HANDLE* arrayHandle)
{
    AutoPtr<ArrayOf<Char32> > arrayTmp;
    GetArray((ArrayOf<Char32>**)&arrayTmp);
    if (arrayTmp == NULL)
    {
        *arrayHandle = 0;
        return NOERROR;
    }
    Char32* primitiveArray = arrayTmp->GetPayload();
    *arrayHandle = reinterpret_cast<HANDLE>(primitiveArray);
    return NOERROR;
}

ECode CharSequenceAdapter::Copy(
    /* [in] */ CharSequenceAdapter* other,
    /* [out] */ CharSequenceAdapter** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    *buffer = NULL;

    Int32 len = 0;
    other->mSequence->GetLength(&len);
    AutoPtr<CharSequenceAdapter> buf = new CharSequenceAdapter();
    FAIL_RETURN(buf->constructor(len, other->mSequence))
    buf->mLimit = other->mLimit;
    buf->mPosition = other->mPosition;
    buf->mMark = other->mMark;
    *buffer = buf;
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode CharSequenceAdapter::AsReadOnlyBuffer(
    /* [out] */ ICharBuffer** buffer)
{
    return Duplicate(buffer);
}

ECode CharSequenceAdapter::Compact()
{
    // throw new ReadOnlyBufferException();
    return E_READ_ONLY_BUFFER_EXCEPTION;
}

ECode CharSequenceAdapter::Duplicate(
    /* [out] */ ICharBuffer** buffer)
{
    AutoPtr<CharSequenceAdapter> csa;
    FAIL_RETURN(Copy(this, (CharSequenceAdapter**)&csa))
    *buffer = ICharBuffer::Probe(csa);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode CharSequenceAdapter::Get(
    /* [out] */ Char32* value)
{
    if (mPosition == mLimit) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }

    return mSequence->GetCharAt(mPosition++, value);
}

ECode CharSequenceAdapter::Get(
    /* [in] */ Int32 index,
    /* [out] */ Char32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = '\0';
    FAIL_RETURN(CheckIndex(index))
    return mSequence->GetCharAt(index, value);
}

ECode CharSequenceAdapter::Get(
    /* [out] */ ArrayOf<Char32>* dst,
    /* [in] */ Int32 dstOffset,
    /* [in] */ Int32 charCount)
{
    VALIDATE_NOT_NULL(dst);
    Int32 length = dst->GetLength();
    if ((dstOffset | charCount) < 0 || dstOffset > length || length - dstOffset < charCount) {
        // throw new ArrayIndexOutOfBoundsException(length, offset, count);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 remaining = 0;
    GetRemaining(&remaining);
    if (charCount > remaining) {
        // throw new BufferUnderflowException();
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }

    // sequence.toString().getChars(position, newPosition, dst, dstOffset);
    Int32 newPosition = mPosition + charCount;
    String s;
    mSequence->ToString(&s);

    // Note: last character not copied!
    if (mPosition >= 0 && mPosition <= newPosition && newPosition <= (Int32)s.GetLength()) {
        // System.arraycopy(value, mPosition, dst, dstOffset, newPosition - mPosition);
        AutoPtr<ArrayOf<Char32> > charArray = s.GetChars(mPosition, newPosition);
        dst->Copy(dstOffset, charArray, 0, charArray->GetLength());
    }
    else {
        // We throw StringIndexOutOfBoundsException rather than System.arraycopy's AIOOBE.
        // throw startEndAndLength(start, end);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    mPosition = newPosition;
    return NOERROR;
}

ECode CharSequenceAdapter::IsDirect(
    /* [out] */ Boolean* isDirect)
{
    VALIDATE_NOT_NULL(isDirect);
    *isDirect = FALSE;
    return NOERROR;
}

ECode CharSequenceAdapter::IsReadOnly(
    /* [out] */ Boolean* isReadOnly)
{
    VALIDATE_NOT_NULL(isReadOnly);
    *isReadOnly = TRUE;
    return NOERROR;
}

ECode CharSequenceAdapter::GetOrder(
    /* [out] */ ByteOrder* order)
{
    AutoPtr<IByteOrderHelper> helper;
    ASSERT_SUCCEEDED(CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&helper))
    return helper->GetNativeOrder(order);
}

ECode CharSequenceAdapter::ProtectedArray(
    /* [out, callee] */ ArrayOf<Char32>** array)
{
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CharSequenceAdapter::ProtectedArrayOffset(
    /* [out] */ Int32* offset)
{
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CharSequenceAdapter::ProtectedHasArray(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode CharSequenceAdapter::Put(
    /* [in] */ Char32 c)
{
    // throw new ReadOnlyBufferException();
    return E_READ_ONLY_BUFFER_EXCEPTION;
}

ECode CharSequenceAdapter::Put(
    /* [in] */ Int32 index,
    /* [in] */ Char32 c)
{
    // throw new ReadOnlyBufferException();
    return E_READ_ONLY_BUFFER_EXCEPTION;
}

ECode CharSequenceAdapter::Put(
    /* [in] */ ArrayOf<Char32>* src,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 charCount)
{
    // throw new ReadOnlyBufferException();
    return E_READ_ONLY_BUFFER_EXCEPTION;
}

ECode CharSequenceAdapter::Put(
    /* [in] */ const String& str,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    // throw new ReadOnlyBufferException();
    return E_READ_ONLY_BUFFER_EXCEPTION;
}

ECode CharSequenceAdapter::Slice(
    /* [out] */ ICharBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(mSequence->SubSequence(mPosition, mLimit, (ICharSequence**)&seq))
    Int32 len = 0;
    seq->GetLength(&len);
    AutoPtr<CharSequenceAdapter> csa = new CharSequenceAdapter();
    FAIL_RETURN(csa->constructor(len, seq))
    *buffer = ICharBuffer::Probe(csa);
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode CharSequenceAdapter::GetLength(
    /* [out] */ Int32* number)
{
    return CharBuffer::GetLength(number);
}

ECode CharSequenceAdapter::GetCharAt(
    /* [in] */ Int32 index,
    /* [out] */ Char32* c)
{
    return CharBuffer::GetCharAt(index, c);
}

ECode CharSequenceAdapter::SubSequence(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq);
    FAIL_RETURN(CheckStartEndRemaining(start, end))
    AutoPtr<CharSequenceAdapter> result;
    FAIL_RETURN(Copy(this, (CharSequenceAdapter**)&result))
    result->mPosition = mPosition + start;
    result->mLimit = mPosition + end;
    *csq = ICharSequence::Probe(result);
    REFCOUNT_ADD(*csq)
    return NOERROR;
}

ECode CharSequenceAdapter::ToString(
    /* [out] */ String* str)
{
    return CharBuffer::ToString(str);
}

} // namespace IO
} // namespace Elastos
