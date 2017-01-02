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

#include "CCoderResult.h"
#include "CCodingErrorAction.h"
#include "CharsetDecoder.h"
#include "CharBuffer.h"

using Elastos::IO::Charset::CCoderResult;

namespace Elastos {
namespace IO {
namespace Charset {

const String CharsetDecoder::RESET("RESET");
const String CharsetDecoder::ONGOING("ONGOING");
const String CharsetDecoder::END_OF_INPUT("END_OF_INPUT");
const String CharsetDecoder::FLUSHED("FLUSHED");

CharsetDecoder::CharsetDecoder()
    : mAverageCharsPerByte(0.0f)
    , mMaxCharsPerByte(0.0f)
    , mReplacementChars("\ufffd")
    , mState(RESET)
{
}

CharsetDecoder::~CharsetDecoder()
{}

ECode CharsetDecoder::constructor(
    /* [in] */ ICharset* charset,
    /* [in] */ Float averageCharsPerByte,
    /* [in] */ Float maxCharsPerByte)
{
    if (averageCharsPerByte <= 0 || maxCharsPerByte <= 0) {
        // throw new IllegalArgumentException("averageCharsPerByte and maxCharsPerByte must be positive");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (averageCharsPerByte > maxCharsPerByte) {
        // throw new IllegalArgumentException("averageCharsPerByte is greater than maxCharsPerByte");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mAverageCharsPerByte = averageCharsPerByte;
    mMaxCharsPerByte = maxCharsPerByte;
    mCharset = charset;

    CCodingErrorAction::GetREPORT((ICodingErrorAction**)&mMalformedInputAction);
    CCodingErrorAction::GetREPORT((ICodingErrorAction**)&mUnmappableCharacterAction);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CharsetDecoder, Object, ICharsetDecoder)

ECode CharsetDecoder::AverageCharsPerByte(
    /* [out] */ Float* average)
{
    VALIDATE_NOT_NULL(average)
    *average = mAverageCharsPerByte;
    return NOERROR;
}

ECode CharsetDecoder::Charset(
    /* [out] */ ICharset** charset)
{
    VALIDATE_NOT_NULL(charset)
    *charset = mCharset;
    REFCOUNT_ADD(*charset)
    return NOERROR;
}

ECode CharsetDecoder::Decode(
    /* [in] */ IByteBuffer* byteBuffer,
    /* [out] */ ICharBuffer** charBuffer)
{
    VALIDATE_NOT_NULL(charBuffer)
    *charBuffer = NULL;
    assert(byteBuffer != NULL);

    Int32 remaining = 0;
    FAIL_RETURN(IBuffer::Probe(byteBuffer)->GetRemaining(&remaining))
    Int32 length = (Int32) (remaining * mAverageCharsPerByte);

    AutoPtr<ICharBuffer> out;
    FAIL_RETURN(CharBuffer::Allocate(length, (ICharBuffer**)&out))

    Reset();

    AutoPtr<ICoderResult> OVERFLOW;
    CCoderResult::GetOVERFLOW((ICoderResult**)&OVERFLOW);
    while (mState != FLUSHED) {
        AutoPtr<ICoderResult> result;
        FAIL_RETURN(Decode(byteBuffer, out, TRUE, (ICoderResult**)&result))

        if (result.Get() == OVERFLOW.Get()) {
            AutoPtr<ICharBuffer> tempOut;
            FAIL_RETURN(AllocateMore(out, (ICharBuffer**)&tempOut))
            out = tempOut;
            continue; // No point trying to flush to an already-full buffer.
        }
        else {
            FAIL_RETURN(CheckCoderResult(result))
        }

        result = NULL;
        Flush(out, (ICoderResult**)&result);
        if (result.Get() == OVERFLOW.Get()) {
            AutoPtr<ICharBuffer> tempOut;
            FAIL_RETURN(AllocateMore(out, (ICharBuffer**)&tempOut))
            out = tempOut;
        }
        else {
            FAIL_RETURN(CheckCoderResult(result))
        }
    }

    IBuffer::Probe(out)->Flip();

    *charBuffer = out;
    REFCOUNT_ADD(*charBuffer)
    return NOERROR;
}

ECode CharsetDecoder::Decode(
    /* [in] */ IByteBuffer* byteBuffer,
    /* [in] */ ICharBuffer* charBuffer,
    /* [in] */ Boolean endOfInput,
    /* [out] */ ICoderResult** codeResult)
{
    VALIDATE_NOT_NULL(codeResult)
    *codeResult = NULL;
    assert(byteBuffer != NULL);

    if (mState != RESET && mState != ONGOING && !(endOfInput && mState == END_OF_INPUT)) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<ICoderResult> OVERFLOW, UNDERFLOW;
    CCoderResult::GetOVERFLOW((ICoderResult**)&OVERFLOW);
    CCoderResult::GetUNDERFLOW((ICoderResult**)&UNDERFLOW);

    AutoPtr<ICodingErrorAction> REPORT, REPLACE;
    CCodingErrorAction::GetREPORT((ICodingErrorAction**)&REPORT);
    CCodingErrorAction::GetREPLACE((ICodingErrorAction**)&REPLACE);

    IBuffer* bb = IBuffer::Probe(byteBuffer);
    IBuffer* bc = IBuffer::Probe(charBuffer);
    mState = endOfInput ? END_OF_INPUT : ONGOING;
    ECode ec = NOERROR;
    Boolean hasRemaining;
    Int32 position, length;
    while (TRUE) {
        AutoPtr<ICoderResult> result;
        // try {
        ec = DecodeLoop(byteBuffer, charBuffer, (ICoderResult**)&result);
        if (ec == (ECode)E_BUFFER_OVER_FLOW_EXCEPTION
            || ec == (ECode)E_BUFFER_UNDER_FLOW_EXCEPTION) {
            return E_CODER_MALFUNCTION_ERROR;
        }
        else if (FAILED(ec)) {
            return ec;
        }

        if (result.Get() == UNDERFLOW.Get()) {
            if (endOfInput && (bb->HasRemaining(&hasRemaining), hasRemaining)) {
                Int32 remaining = 0;
                FAIL_RETURN(bb->GetRemaining(&remaining))
                result = NULL;
                CCoderResult::MalformedForLength(remaining, (ICoderResult**)&result);
            }
            else {
                *codeResult = result;
                REFCOUNT_ADD(*codeResult)
                return NOERROR;
            }
        }
        else if (result.Get() == OVERFLOW.Get()) {
            *codeResult = result;
            REFCOUNT_ADD(*codeResult)
            return NOERROR;
        }

        // We have a real error, so do what the appropriate action tells us what to do...
        Boolean bval;
        result->IsUnmappable(&bval);
        AutoPtr<ICodingErrorAction> action =
            bval ? mUnmappableCharacterAction : mMalformedInputAction;
        if (action.Get() == REPORT.Get()) {
            *codeResult = result;
            REFCOUNT_ADD(*codeResult)
            return NOERROR;
        }
        else if (action.Get() == REPLACE.Get()) {
            Int32 remaining = 0;
            FAIL_RETURN(bc->GetRemaining(&remaining))
            if (remaining < mReplacementChars.GetLength()) {
                *codeResult = OVERFLOW;
                REFCOUNT_ADD(*codeResult)
                return NOERROR;
            }

            charBuffer->Put(mReplacementChars);
        }

        bb->GetPosition(&position);
        result->GetLength(&length);
        bb->SetPosition(position + length);
    }
    return NOERROR;
}

ECode CharsetDecoder::DetectedCharset(
    /* [out] */ ICharset** charset)
{
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CharsetDecoder::Flush(
    /* [in] */ ICharBuffer* charBuffer,
    /* [out] */ ICoderResult** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (mState != FLUSHED && mState != END_OF_INPUT) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    FAIL_RETURN(ImplFlush(charBuffer, result))
    AutoPtr<ICoderResult> UNDERFLOW;
    CCoderResult::GetUNDERFLOW((ICoderResult**)&UNDERFLOW);
    if (*result == UNDERFLOW.Get()) {
        mState = FLUSHED;
    }
    return NOERROR;
}

ECode CharsetDecoder::IsAutoDetecting(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode CharsetDecoder::IsCharsetDetected(
    /* [out] */ Boolean* result)
{
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CharsetDecoder::MalformedInputAction(
    /* [out] */ ICodingErrorAction** codingErrorAction)
{
    VALIDATE_NOT_NULL(codingErrorAction)
    *codingErrorAction = mMalformedInputAction;
    REFCOUNT_ADD(*codingErrorAction)
    return NOERROR;
}

ECode CharsetDecoder::MaxCharsPerByte(
    /* [out] */ Float* maxNumber)
{
    VALIDATE_NOT_NULL(maxNumber)
    *maxNumber = mMaxCharsPerByte;
    return NOERROR;
}

ECode CharsetDecoder::OnMalformedInput(
    /* [in] */ ICodingErrorAction* newAction)
{
    VALIDATE_NOT_NULL(newAction)
    mMalformedInputAction = newAction;
    ImplOnMalformedInput(newAction);
    return NOERROR;
}

ECode CharsetDecoder::OnUnmappableCharacter(
    /* [in] */ ICodingErrorAction* newAction)
{
    VALIDATE_NOT_NULL(newAction)
    mUnmappableCharacterAction = newAction;
    ImplOnUnmappableCharacter(newAction);
    return NOERROR;
}

ECode CharsetDecoder::Replacement(
    /* [out] */ String* replacement)
{
    VALIDATE_NOT_NULL(replacement)
    *replacement = mReplacementChars;
    return NOERROR;
}

ECode CharsetDecoder::ReplaceWith(
    /* [in] */ const String& newReplacement)
{
    if (newReplacement.IsNullOrEmpty()) {
        // throw new IllegalArgumentException("replacement == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (newReplacement.GetByteLength() > mMaxCharsPerByte) {
        // throw new IllegalArgumentException("replacement length > maxCharsPerByte: " +
        //         newReplacement.length() + " > " + maxCharsPerByte());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mReplacementChars = newReplacement;
    ImplReplaceWith(newReplacement);
    return NOERROR;
}

ECode CharsetDecoder::Reset()
{
    mState = RESET;
    ImplReset();
    return NOERROR;
}

ECode CharsetDecoder::UnmappableCharacterAction(
    /* [out] */ ICodingErrorAction** codingErrorAction)
{
    VALIDATE_NOT_NULL(codingErrorAction)
    *codingErrorAction = mUnmappableCharacterAction;
    REFCOUNT_ADD(*codingErrorAction)
    return NOERROR;
}

ECode CharsetDecoder::ImplFlush(
    /* [in] */ ICharBuffer* outInput,
    /* [out] */ ICoderResult** result)
{
    return CCoderResult::GetUNDERFLOW(result);
}

ECode CharsetDecoder::ImplOnMalformedInput(
    /* [in] */ ICodingErrorAction* newAction)
{
    // default implementation is empty
    return NOERROR;
}

ECode CharsetDecoder::ImplOnUnmappableCharacter(
    /* [in] */ ICodingErrorAction* newAction)
{
    // default implementation is empty
    return NOERROR;
}

ECode CharsetDecoder::ImplReplaceWith(
    /* [in] */ const String& newReplacement)
{
    // // default implementation is empty
    return NOERROR;
}

ECode CharsetDecoder::ImplReset()
{
    // default implementation is empty
    return NOERROR;
}

ECode CharsetDecoder::CheckCoderResult(
    /* [in] */ ICoderResult* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean ret = FALSE;
    AutoPtr<ICodingErrorAction> REPORT;
    CCodingErrorAction::GetREPORT((ICodingErrorAction**)&REPORT);
    if ((result->IsMalformed(&ret), ret) && (mMalformedInputAction == REPORT)) {
        // throw new MalformedInputException(result.length());
        return E_MALFORMED_INPUT_EXCEPTION;
    }
    else if ((result->IsUnmappable(&ret), ret) && (mUnmappableCharacterAction == REPORT)) {
        // throw new UnmappableCharacterException(result.length());
        return E_UNMAPPABLE_CHARACTER_EXCEPTION;
    }
    return NOERROR;
}

ECode CharsetDecoder::AllocateMore(
    /* [in] */ ICharBuffer* output,
    /* [out] */ ICharBuffer** newOutput)
{
    VALIDATE_NOT_NULL(newOutput)
    *newOutput = NULL;

    Int32 capacity = 0;
    IBuffer::Probe(output)->GetCapacity(&capacity);
    if (capacity == 0) {
        return CharBuffer::Allocate(1, newOutput);
    }
    FAIL_RETURN(CharBuffer::Allocate(capacity * 2, newOutput))
    IBuffer::Probe(output)->Flip();
    (*newOutput)->Put(output);
    return NOERROR;
}

} // namespace Charset
} // namespace IO
} // namespace Elastos
