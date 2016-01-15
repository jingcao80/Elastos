
#include "CharArrayWriter.h"
#include "Math.h"
#include "Character.h"
#include "CString.h"
#include "AutoLock.h"

using Elastos::Core::Character;
using Elastos::Core::CString;
using Elastos::Core::Math;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(CharArrayWriter, Writer, ICharArrayWriter)

CharArrayWriter::CharArrayWriter()
    : mCount(0)
{
    mBufLock = new Object();
}

CharArrayWriter::~CharArrayWriter()
{
}

ECode CharArrayWriter::constructor()
{
    mBuf = ArrayOf<Char32>::Alloc(32);
    if (mBuf == NULL)
        return E_OUT_OF_MEMORY_ERROR;

    mLock = mBufLock;
    return NOERROR;
}

ECode CharArrayWriter::constructor(
    /* [in] */ Int32 initialSize)
{
    if (initialSize < 0) {
//      throw new IllegalArgumentException("size < 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mBuf = ArrayOf<Char32>::Alloc(initialSize);
    if (mBuf == NULL)
        return E_OUT_OF_MEMORY_ERROR;

    mLock = ISynchronize::Probe(mBufLock);
    return NOERROR;
}

ECode CharArrayWriter::Close()
{
    return NOERROR;
}

ECode CharArrayWriter::Expand(
    /* [in] */ Int32 i)
{
    Int32 size = mBuf->GetLength();
    /* Can the buffer handle @i more chars, if not expand it */
    if (mCount + i <= size) {
        return NOERROR;
    }

    Int32 newLen = Elastos::Core::Math::Max(2 * size, mCount + i);
    AutoPtr<ArrayOf<Char32> > newbuf = ArrayOf<Char32>::Alloc(newLen);
    newbuf->Copy(mBuf);
    mBuf = newbuf;

    return NOERROR;
}

ECode CharArrayWriter::Flush()
{
    return NOERROR;
}

ECode CharArrayWriter::Reset()
{
    AutoLock lock(mLock);

    mCount = 0;
    return NOERROR;
}

ECode CharArrayWriter::GetSize(
    /* [out] */Int32* size)
{
    VALIDATE_NOT_NULL(size)

    AutoLock lock(mLock);

    *size = mCount;
    return NOERROR;
}

ECode CharArrayWriter::ToCharArray(
    /* [out] */ ArrayOf<Char32>** str)
{
    VALIDATE_NOT_NULL(str)
    *str = NULL;

    AutoLock lock(mLock);

    AutoPtr<ArrayOf<Char32> > arr = mBuf->Clone();
    if (arr == NULL)
        return E_OUT_OF_MEMORY_ERROR;

    *str = arr;
    REFCOUNT_ADD(*str);
    return NOERROR;
}

ECode CharArrayWriter::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    AutoLock lock(mLock);

    AutoPtr<ArrayOf<Byte> > dst;
    Int32 dstOffset = 0;
    FAIL_RETURN(Character::ToChars(*mBuf, 0, mCount, (ArrayOf<Byte>**)&dst, &dstOffset));

    *result = String((char*)dst->GetPayload(), dst->GetLength());

    if (*result == NULL)
        return E_OUT_OF_MEMORY_ERROR;
    else
        return NOERROR;
}

ECode CharArrayWriter::Write(
    /* [in] */ Int32 oneChar32)
{
    AutoLock lock(mLock);

    Expand(1);
    (*mBuf)[mCount++] = oneChar32;

    return NOERROR;
}

ECode CharArrayWriter::Write(
    /* [in] */ ArrayOf<Char32>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    VALIDATE_NOT_NULL(buffer)
    // avoid int overflow
    // BEGIN android-changed
    // Exception priorities (in case of multiple errors) differ from
    // RI, but are spec-compliant.
    // made implicit null check explicit,
    // removed redundant check,
    // added null check, used (offset | len) < 0 instead of
    // (offset < 0) || (len < 0) to safe one operation

    if ((offset | count) < 0 || count > buffer->GetLength() - offset) {
//      throw new IndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    // END android-changed
    AutoLock lock(mLock);

    Expand(count);
    mBuf->Copy(mCount, buffer, offset, count);
    mCount += count;

    return NOERROR;
}

ECode CharArrayWriter::Write(
    /* [in] */ const String& str,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    // avoid int overflow
    // BEGIN android-changed
    // Exception priorities (in case of multiple errors) differ from
    // RI, but are spec-compliant.
    // removed redundant check, used (offset | len) < 0
    // instead of (offset < 0) || (len < 0) to safe one operation

    if ((offset | count) < 0 || offset > str.GetLength() - count) {
//      throw new StringIndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    // END android-changed
    AutoLock lock(mLock);

    AutoPtr<ArrayOf<Char32> > charArray = str.GetChars(offset, offset + count);
    count = charArray->GetLength();
    if (count > 0) {
        Expand(count);
        mBuf->Copy(mCount, charArray, count);
        mCount += count;
    }

    return NOERROR;
}

ECode CharArrayWriter::WriteTo(
    /* [in] */ IWriter* out)
{
    AutoLock lock(mLock);

    return out->Write(mBuf, 0, mCount);
}

ECode CharArrayWriter::Append(
    /* [in] */ Char32 c)
{
    return Write(c);
}

ECode CharArrayWriter::Append(
    /* [in] */ ICharSequence* csq)
{
    Int32 number = 0;
    if (csq) {
        csq->GetLength(&number);
    }
    return Append(csq, 0, number);
}

ECode CharArrayWriter::Append(
    /* [in] */ ICharSequence* csq,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    String str("NULL");
    if (NULL != csq) {
        csq->ToString(&str);
    }

    String subStr = str.Substring(start, end);
    return Write(subStr);
}

} // namespace IO
} // namespace Elastos
