
#include "OutputStreamWriter.h"
#include "ByteBuffer.h"
#include "CharBuffer.h"
#include "CString.h"
#include "Charset.h"
#include "AutoLock.h"
#include "CCodingErrorAction.h"

using Elastos::Core::CString;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::ICodingErrorAction;
using Elastos::IO::Charset::CCodingErrorAction;
using Elastos::IO::Charset::ICoderResult;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(OutputStreamWriter, Writer, IOutputStreamWriter)

OutputStreamWriter::OutputStreamWriter()
{
    ASSERT_SUCCEEDED(ByteBuffer::Allocate(8192, (IByteBuffer**)&mBytes));
}

OutputStreamWriter::~OutputStreamWriter()
{
}

ECode OutputStreamWriter::constructor(
    /* [in] */ IOutputStream *out)
{
    AutoPtr<ICharset> cs;
    Charset::Charset::DefaultCharset((ICharset**)&cs);
    return constructor(out, cs);
}

ECode OutputStreamWriter::constructor(
    /* [in] */ IOutputStream *out,
    /* [in] */ const String &charsetName)
{
    FAIL_RETURN(Writer::constructor(ISynchronize::Probe(out)));

    if (charsetName.IsNull()) {
        // throw new NullPointerException("charsetName == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mOut = out;
    // try {
    AutoPtr<ICharset> cs;
    Charset::Charset::ForName(charsetName, (ICharset**)&cs);
    ECode ec = cs->NewEncoder((ICharsetEncoder**)&mEncoder);
    if (FAILED(ec)) {
        return E_UNSUPPORTED_ENCODING_EXCEPTION;
    }
    // } catch (Exception e) {
    //     throw new UnsupportedEncodingException(charsetName);
    // }
    AutoPtr<ICodingErrorAction> replace;
    CCodingErrorAction::GetREPLACE((ICodingErrorAction**)&replace);
    mEncoder->OnMalformedInput(replace);
    mEncoder->OnUnmappableCharacter(replace);
    return NOERROR;
}

ECode OutputStreamWriter::constructor(
    /* [in] */ IOutputStream *out,
    /* [in] */ ICharset* charset)
{
    FAIL_RETURN(Writer::constructor(ISynchronize::Probe(out)));
    mOut = out;
    charset->NewEncoder((ICharsetEncoder**)&mEncoder);
    AutoPtr<ICodingErrorAction> replace;
    CCodingErrorAction::GetREPLACE((ICodingErrorAction**)&replace);
    mEncoder->OnMalformedInput(replace);
    mEncoder->OnUnmappableCharacter(replace);
    return NOERROR;
}

ECode OutputStreamWriter::constructor(
    /* [in] */ IOutputStream *out,
    /* [in] */ ICharsetEncoder* charsetEncoder)
{
    FAIL_RETURN(Writer::constructor(ISynchronize::Probe(out)));
    AutoPtr<ICharset> charset;
    charsetEncoder->GetCharset((ICharset**)&charset);
    mOut = out;
    mEncoder = charsetEncoder;
    return NOERROR;
}

ECode OutputStreamWriter::Close()
{
    AutoLock lock(mLock);
    if (mEncoder != NULL) {
        DrainEncoder();
        FlushBytes(FALSE);
        IFlushable::Probe(mOut)->Flush();
        ICloseable::Probe(mOut)->Close();
        mBytes = NULL;
    }

    return NOERROR;
}

ECode OutputStreamWriter::Flush()
{
    return FlushBytes(TRUE);
}

ECode OutputStreamWriter::FlushBytes(
    /* [in] */ Boolean flushUnderlyingStream)
{
    AutoLock lock(mLock);

    FAIL_RETURN(CheckStatus());
    Int32 position;
    IBuffer::Probe(mBytes)->GetPosition(&position);
    if (position > 0) {
        IBuffer::Probe(mBytes)->Flip();
        AutoPtr<ArrayOf<Byte> > buf;
        mBytes->GetArray((ArrayOf<Byte>**)&buf);
        Int32 offset;
        IBuffer::Probe(mBytes)->GetArrayOffset(&offset);
        mOut->Write(buf, offset, position);
        IBuffer::Probe(mBytes)->Clear();
    }

    if (flushUnderlyingStream) {
        FAIL_RETURN(IFlushable::Probe(mOut)->Flush());
    }

    return NOERROR;
}

ECode OutputStreamWriter::Convert(
    /* [in] */ ICharBuffer* chars)
{
    Boolean bval;
    while (TRUE) {
        AutoPtr<ICoderResult> result;
        mEncoder->Encode(chars, mBytes, FALSE, (ICoderResult**)&result);

        Int32 count;
        chars->GetLength(&count);
        for(Int32 i = 0; i < count; ++i) {
            Char32 ch;
            chars->Get(&ch);
            mBytes->Put((Byte)ch);
        }
        if (result->IsOverflow(&bval), bval) {
            // Make room and try again.
            FlushBytes(FALSE);
            continue;
        }
        else if (result->IsError(&bval), bval) {
            return result->ThrowException();
        }
        break;
    }
    return NOERROR;
}

ECode OutputStreamWriter::DrainEncoder()
{
    // Strictly speaking, I think it's part of the CharsetEncoder contract that you call
    // encode with endOfInput true before flushing. Our ICU-based implementations don't
    // actually need this, and you'd hope that any reasonable implementation wouldn't either.
    // CharsetEncoder.encode doesn't actually pass the boolean through to encodeLoop anyway!
    AutoPtr<ICharBuffer> chars;
    CharBuffer::Allocate(0, (ICharBuffer**)&chars);
    Boolean bval;
    while (TRUE) {
        AutoPtr<ICoderResult> result;
        mEncoder->Encode(chars, mBytes, TRUE, (ICoderResult**)&result);
        if (result->IsError(&bval), bval) {
            return result->ThrowException();
        }
        else if (result->IsOverflow(&bval), bval) {
            FlushBytes(FALSE);
            continue;
        }
        break;
    }

    // Some encoders (such as ISO-2022-JP) have stuff to write out after all the
    // characters (such as shifting back into a default state). In our implementation,
    // this is actually the first time ICU is told that we've run out of input.
    AutoPtr<ICoderResult> result;
    mEncoder->Flush(mBytes, (ICoderResult**)&result);
    while (result->IsUnderflow(&bval), !bval) {
        if (result->IsOverflow(&bval), bval) {
            FlushBytes(false);
            result = NULL;
            mEncoder->Flush(mBytes, (ICoderResult**)&result);
        }
        else {
            return result->ThrowException();
        }
    }
    return NOERROR;
}

ECode OutputStreamWriter::CheckStatus()
{
    if (mEncoder == NULL) {
        return E_IO_EXCEPTION;
//        throw new IOException("OutputStreamWriter is closed");
    }
    return NOERROR;
}

ECode OutputStreamWriter::GetEncoding(
    /* [out] */ String *encoding)
{
    VALIDATE_NOT_NULL(encoding);

    if (mEncoder == NULL) {
        *encoding = String(NULL);
        return NOERROR;
    }

    AutoPtr<ICharset> cs;
    mEncoder->GetCharset((ICharset**)&cs);
    return cs->GetName(encoding);
}

ECode OutputStreamWriter::Write(
    /* [in] */ Int32 oneChar32)
{
    AutoLock lock(mLock);

    FAIL_RETURN(CheckStatus());
    AutoPtr<ArrayOf<Char32> > c = ArrayOf<Char32>::Alloc(1);
    (*c)[0] = (Char32)oneChar32;
    AutoPtr<ICharBuffer> chars;
    CharBuffer::Wrap(c, (ICharBuffer**)&chars);
    return Convert(chars);
}

ECode OutputStreamWriter::Write(
    /* [in] */ ArrayOf<Char32>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    VALIDATE_NOT_NULL(buffer)

    AutoLock lock(mLock);

    FAIL_RETURN(CheckStatus());
    Int32 arrayLength = buffer->GetLength();
    if ((offset | count) < 0 || offset > arrayLength || arrayLength - offset < count) {
        // throw new ArrayIndexOutOfBoundsException(arrayLength, offset,
        //         count);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    AutoPtr<ICharBuffer> chars;
    CharBuffer::Wrap(buffer, offset, count, (ICharBuffer**)&chars);
    return Convert(chars);
}

ECode OutputStreamWriter::Write(
    /* [in] */ const String& str,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    AutoLock lock(mLock);

    if (count < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
//        throw new StringIndexOutOfBoundsException();
    }
    if (str.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
//        throw new NullPointerException("str == null");
    }
    if ((offset | count) < 0 || offset > str.GetLength() - count) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
//        throw new StringIndexOutOfBoundsException();
    }
    FAIL_RETURN(CheckStatus());
    AutoPtr<ICharSequence> charSeq;
    CString::New(String(str), (ICharSequence**)&charSeq);
    AutoPtr<ICharBuffer> chars;
    CharBuffer::Wrap(charSeq, offset, count + offset, (ICharBuffer**)&chars);
    return Convert(chars);
}

} // namespace IO
} // namespace Elastos
