
#include "StringWriter.h"
#include "Character.h"

using Elastos::Core::Character;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(StringWriter, Writer, IStringWriter)

StringWriter::StringWriter()
{
}

StringWriter::~StringWriter()
{
}

ECode StringWriter::constructor()
{
    mBuf = new StringBuffer(16);
    if (mBuf == NULL)
        return E_OUT_OF_MEMORY_ERROR;

    mLock = mBuf;

    return NOERROR;
}

ECode StringWriter::constructor(
    /* [in] */ Int32 initialSize)
{
    if (initialSize < 0) {
        // throw new IllegalArgumentException("initialSize < 0: " + initialSize);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mBuf = new StringBuffer(initialSize);
    if (mBuf == NULL)
        return E_OUT_OF_MEMORY_ERROR;

    mLock = ISynchronize::Probe(mBuf);

    return NOERROR;
}

ECode StringWriter::Close()
{
    return NOERROR;
}

ECode StringWriter::Flush()
{
    return NOERROR;
}

ECode StringWriter::GetBuffer(
   /* [out] */ IStringBuffer** buf)
{
    VALIDATE_NOT_NULL(buf)

    *buf = mBuf;
    REFCOUNT_ADD(*buf)

    return NOERROR;
}

ECode StringWriter::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    mBuf->ToString(str);
    return NOERROR;
}

ECode StringWriter::Write(
    /* [in] */ Int32 oneChar8)
{
    mBuf->AppendChar(oneChar8);

    return NOERROR;
}

ECode StringWriter::Write(
    /* [in] */ ArrayOf<Char32>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    VALIDATE_NOT_NULL(buffer)

    // avoid int overflow
    // BEGIN android-changed
    // Exception priorities (in case of multiple errors) differ from
    // RI, but are spec-compliant.
    // removed redundant check, added null check, used (offset | count) < 0
    // instead of (offset < 0) || (count < 0) to safe one operation
    if ((offset | count) < 0 || count > buffer->GetLength() - offset) {
//      throw new IndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    // END android-changed
    if (count == 0) {
        return NOERROR;
    }

    AutoPtr<ArrayOf<Byte> > dst;
    Int32 dstOffset = 0;
    FAIL_RETURN(Character::ToChars(*buffer, offset, count, (ArrayOf<Byte>**)&dst, &dstOffset));

    mBuf->Append(String((const char*)dst->GetPayload(), dst->GetLength()));

    return NOERROR;
}

ECode StringWriter::Write(
    /* [in] */ const String& str)
{
    return mBuf->Append(str);
}

ECode StringWriter::Write(
    /* [in] */ const String& str,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    return mBuf->Append(str.Substring(offset, offset + count));
}

ECode StringWriter::AppendChar(
    /* [in] */ Char32 c)
{
    return Write(c);
}

ECode StringWriter::Append(
    /* [in] */ ICharSequence* csq)
{
    String str("NULL");
    if (csq != NULL) {
        csq->ToString(&str);
    }

    return Write(str);
}

ECode StringWriter::Append(
    /* [in] */ ICharSequence* csq,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    String str("NULL");
    if (csq != NULL) {
        csq->ToString(&str);
    }

    return Write(str, start, end - start);
}

} // namespace IO
} // namespace Elastos
