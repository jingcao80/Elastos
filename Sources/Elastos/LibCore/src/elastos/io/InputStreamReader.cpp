
#include "InputStreamReader.h"
#include "Charset.h"
#include "CCoderResult.h"
#include "CharBuffer.h"
#include "ByteBuffer.h"
#include "CCodingErrorAction.h"
#include "Math.h"
#include "Character.h"
#include "AutoLock.h"
#include "Arrays.h"

using Elastos::Core::Character;
using Elastos::IO::Charset::CCoderResult;
using Elastos::IO::Charset::CCodingErrorAction;
using Elastos::IO::Charset::Charset;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::ICoderResult;
using Elastos::IO::Charset::ICodingErrorAction;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(InputStreamReader, Reader, IInputStreamReader)

InputStreamReader::InputStreamReader()
    : mEndOfInput(FALSE)
{
    ASSERT_SUCCEEDED(ByteBuffer::Allocate(8192, (IByteBuffer**)&mBytes));
}

InputStreamReader::~InputStreamReader()
{}

ECode InputStreamReader::constructor(
    /* [in] */ IInputStream *in)
{
    AutoPtr<ICharset> ch;
    FAIL_RETURN(Charset::Charset::DefaultCharset((ICharset**)&ch));

    return constructor(in, (ICharset*)ch);
}

ECode InputStreamReader::constructor(
    /* [in] */ IInputStream *in,
    /* [in] */ const String &enc)
{
    FAIL_RETURN(Reader::constructor(ISynchronize::Probe(in)))

    if (enc.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
//        throw new NullPointerException();
    }

    mIn = in;
    AutoPtr<ICharset> charset;
    FAIL_RETURN(Charset::Charset::ForName(enc, (ICharset**)&charset));
    FAIL_RETURN(charset->NewDecoder((ICharsetDecoder**)&mDecoder));

    AutoPtr<ICodingErrorAction> replace;
    CCodingErrorAction::GetREPLACE((ICodingErrorAction **)&replace);

    mDecoder->OnMalformedInput(replace);
    mDecoder->OnUnmappableCharacter(replace);
    return IBuffer::Probe(mBytes)->SetLimit(0);
}

ECode InputStreamReader::constructor(
    /* [in] */ IInputStream* in,
    /* [in] */ ICharsetDecoder* dec)
{
    VALIDATE_NOT_NULL(dec)
    FAIL_RETURN(Reader::constructor(ISynchronize::Probe(in)))
    Float fval;
    dec->AverageCharsPerByte(&fval);
    mIn = in;
    mDecoder = dec;
    IBuffer::Probe(mBytes)->SetLimit(0);
    return NOERROR;
}

ECode InputStreamReader::constructor(
    /* [in] */ IInputStream* in,
    /* [in] */ ICharset* charset)
{
    FAIL_RETURN(Reader::constructor(ISynchronize::Probe(in)))

    mIn = in;
    charset->NewDecoder((ICharsetDecoder**)&mDecoder);
    AutoPtr<ICodingErrorAction> replace;
    CCodingErrorAction::GetREPLACE((ICodingErrorAction **)&replace);

    mDecoder->OnMalformedInput(replace);
    mDecoder->OnUnmappableCharacter(replace);
    return IBuffer::Probe(mBytes)->SetLimit(0);
}

ECode InputStreamReader::Close()
{
    AutoLock lock(mLock);
    if (mDecoder != NULL) {
        mDecoder->Reset();
    }
    mDecoder = NULL;
    if (mIn != NULL) {
        FAIL_RETURN(ICloseable::Probe(mIn)->Close());
        mIn = NULL;
    }
    return NOERROR;
}

ECode InputStreamReader::GetEncoding(
    /* [out] */ String* encoding)
{
    VALIDATE_NOT_NULL(encoding);
    if (!IsOpen()) {
        *encoding = String(NULL);
        return NOERROR;
    }

    AutoPtr<ICharset> c = NULL;
    mDecoder->Charset((ICharset**)&c);
    c->GetName(encoding);

    return NOERROR;
}

ECode InputStreamReader::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    AutoLock lock(mLock);
    if (!IsOpen()) {
        return E_IO_EXCEPTION;
//        throw new IOException("InputStreamReader is closed");
    }
    AutoPtr< ArrayOf<Char32> > buf = ArrayOf<Char32>::Alloc(1);
    Int32 number;
    FAIL_RETURN(Read(buf, 0, 1, &number));
    *value = number != -1 ? (*buf)[0] : -1;

    return NOERROR;
}

ECode InputStreamReader::Read(
    /* [out] */ ArrayOf<Char32>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;
    VALIDATE_NOT_NULL(buffer);

    AutoLock lock(mLock);

    if (!IsOpen()) {
        return E_IO_EXCEPTION;
        // throw new IOException("InputStreamReader is closed");
    }
    FAIL_RETURN(Arrays::CheckOffsetAndCount(buffer->GetLength(), offset, count));
    if (count == 0) {
        *number = 0;
        return NOERROR;
    }

    AutoPtr<ICharBuffer> out;
    FAIL_RETURN(CharBuffer::Wrap(buffer, offset, count, (ICharBuffer**)&out));
    AutoPtr<ICoderResult> result, resultTmp;
    CCoderResult::GetUNDERFLOW((ICoderResult**)&result);
    resultTmp = result;

    // bytes.remaining() indicates number of bytes in buffer
    // when 1-st time entered, it'll be equal to zero
    Boolean hasRemaining;
    IBuffer::Probe(mBytes)->HasRemaining(&hasRemaining);
    Boolean needInput = !hasRemaining;

    while(IBuffer::Probe(out)->HasRemaining(&hasRemaining), hasRemaining) {
        // fill the buffer if needed
        if (needInput) {
            Int32 num, position;
            FAIL_RETURN(mIn->Available(&num));
            FAIL_RETURN(IBuffer::Probe(out)->GetPosition(&position));
            if (num == 0 && position > offset) {
                // we could return the result without blocking read
                break;
            }

            Int32 capacity, limit, arrayOffset;
            IBuffer::Probe(mBytes)->GetCapacity(&capacity);
            IBuffer::Probe(mBytes)->GetLimit(&limit);
            IBuffer::Probe(mBytes)->GetArrayOffset(&arrayOffset);
            Int32 desiredByteCount = capacity - limit;
            Int32 off = arrayOffset + limit;
            AutoPtr<ArrayOf<Byte> > buf;
            mBytes->GetArray((ArrayOf<Byte>**)&buf);
            Int32 actualByteCount;
            mIn->Read(buf, off, desiredByteCount, &actualByteCount);
            if (actualByteCount == -1) {
                mEndOfInput = TRUE;
                break;
            }
            else if (actualByteCount == 0) {
                break;
            }
            IBuffer::Probe(mBytes)->SetLimit(limit + actualByteCount);
            needInput = FALSE;
        }

        // decode bytes
        result = NULL;
        mDecoder->Decode(mBytes, out, FALSE, (ICoderResult**)&result);
        Boolean isUnderflow;
        if (result->IsUnderflow(&isUnderflow), isUnderflow) {
            // compact the buffer if no space left
            Int32 capacity, limit, pos;
            IBuffer::Probe(mBytes)->GetLimit(&limit);
            IBuffer::Probe(mBytes)->GetCapacity(&capacity);
            // compact the buffer if no space left
            if (limit == capacity) {
                mBytes->Compact();
                IBuffer::Probe(mBytes)->GetPosition(&pos);
                IBuffer::Probe(mBytes)->SetLimit(pos);
                IBuffer::Probe(mBytes)->SetPosition(0);
            }
            needInput = TRUE;
        }
        else {
            break;
        }
    }

    if (result == resultTmp && mEndOfInput) {
        result = NULL;
        mDecoder->Decode(mBytes, out, TRUE, (ICoderResult**)&result);
        Boolean isUnderflow = false;
        if(result->IsUnderflow(&isUnderflow), isUnderflow)
        {
            AutoPtr<ICoderResult> resultFlush;
            mDecoder->Flush(out, (ICoderResult**)&resultFlush);
        }

        mDecoder->Reset();

        Int32 pos, arrayOffset, limit;
        IBuffer::Probe(mBytes)->GetArrayOffset(&arrayOffset);
        IBuffer::Probe(mBytes)->GetPosition(&pos);
        IBuffer::Probe(mBytes)->GetLimit(&limit);
        IBuffer::Probe(mBytes)->SetPosition(Elastos::Core::Math::Min(pos, limit) - arrayOffset);
    }
    Boolean isMalformed, isUnmappable;
    if ((result->IsMalformed(&isMalformed), isMalformed) || (result->IsUnmappable(&isUnmappable), isUnmappable)) {
        return result->ThrowException();
    }

    Int32 charBufPos;
    IBuffer::Probe(out)->GetPosition(&charBufPos);
    *number = charBufPos - offset == 0 ? -1 : charBufPos - offset;
    return NOERROR;
}

Boolean InputStreamReader::IsOpen()
{
    return mIn != NULL;
}

ECode InputStreamReader::IsReady(
    /* [out] */ Boolean* ready)
{
    VALIDATE_NOT_NULL(ready);
    AutoLock lock(mLock);
    if (mIn == NULL) {
        return E_IO_EXCEPTION;
//        throw new IOException("InputStreamReader is closed");
    }

    Boolean hasRemaining;
    IBuffer::Probe(mBytes)->HasRemaining(&hasRemaining);
    Int32 number;
    mIn->Available(&number);
    *ready = hasRemaining || number > 0;

    return NOERROR;
}

} // namespace IO
} // namespace Elastos
