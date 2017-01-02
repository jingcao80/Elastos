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
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::Character;
using Elastos::IO::Charset::CCoderResult;
using Elastos::IO::Charset::CCodingErrorAction;
using Elastos::IO::Charset::Charset;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::ICoderResult;
using Elastos::IO::Charset::ICodingErrorAction;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

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
    CCodingErrorAction::GetREPLACE((ICodingErrorAction**)&replace);

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
    CCodingErrorAction::GetREPLACE((ICodingErrorAction**)&replace);

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
        Logger::E("InputStreamReader", "InputStreamReader is closed. E_IO_EXCEPTION");
        return E_IO_EXCEPTION;
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
    *number = -1;
    VALIDATE_NOT_NULL(buffer);

    AutoLock lock(mLock);

    if (!IsOpen()) {
        Logger::E("InputStreamReader", "InputStreamReader is closed. E_IO_EXCEPTION");
        return E_IO_EXCEPTION;
    }
    FAIL_RETURN(Arrays::CheckOffsetAndCount(buffer->GetLength(), offset, count));
    if (count == 0) {
        *number = 0;
        return NOERROR;
    }

    AutoPtr<ICharBuffer> out;
    FAIL_RETURN(CharBuffer::Wrap(buffer, offset, count, (ICharBuffer**)&out));
    IBuffer* bout = IBuffer::Probe(out);
    AutoPtr<ICoderResult> UNDERFLOW, result;
    CCoderResult::GetUNDERFLOW((ICoderResult**)&UNDERFLOW);
    result = UNDERFLOW;

    // bytes.remaining() indicates number of bytes in buffer
    // when 1-st time entered, it'll be equal to zero
    Boolean hasRemaining, needInput;
    IBuffer* bBytes = IBuffer::Probe(mBytes);
    bBytes->HasRemaining(&hasRemaining);
    needInput = !hasRemaining;

    Int32 num = 0, position = 0;
    while (bout->HasRemaining(&hasRemaining), hasRemaining) {
        // fill the buffer if needed
        if (needInput) {
            mIn->Available(&num);
            bout->GetPosition(&position);
            if (num == 0 && position > offset) {
                // we could return the result without blocking read
                break;
            }

            Int32 capacity, limit, arrayOffset;
            bBytes->GetCapacity(&capacity);
            bBytes->GetLimit(&limit);
            bBytes->GetArrayOffset(&arrayOffset);
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
            bBytes->SetLimit(limit + actualByteCount);
            needInput = FALSE;
        }

        // decode bytes
        result = NULL;
        mDecoder->Decode(mBytes, out, FALSE, (ICoderResult**)&result);
        Boolean isUnderflow;
        result->IsUnderflow(&isUnderflow);
        if (isUnderflow) {
            // compact the buffer if no space left
            Int32 capacity, limit;
            bBytes->GetLimit(&limit);
            bBytes->GetCapacity(&capacity);

            // compact the buffer if no space left
            if (limit == capacity) {
                mBytes->Compact();
                bBytes->GetPosition(&position);
                bBytes->SetLimit(position);
                bBytes->SetPosition(0);
            }
            needInput = TRUE;
        }
        else {
            break;
        }
    }

    if (result.Get() == UNDERFLOW.Get() && mEndOfInput) {
        result = NULL;
        mDecoder->Decode(mBytes, out, TRUE, (ICoderResult**)&result);
        if (result.Get() == UNDERFLOW.Get()) {
            AutoPtr<ICoderResult> resultFlush;
            mDecoder->Flush(out, (ICoderResult**)&resultFlush);
            result = resultFlush;
        }

        mDecoder->Reset();
    }

    Boolean isMalformed, isUnmappable;
    result->IsMalformed(&isMalformed);
    result->IsUnmappable(&isUnmappable);
    if (isMalformed || isUnmappable) {
        return result->ThrowException();
    }

    bout->GetPosition(&position);
    *number = position - offset == 0 ? -1 : position - offset;
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
