
#include "CCoderResult.h"
#include "CCodingErrorAction.h"
#include "CharsetDecoder.h"
#include "CharBuffer.h"

using Elastos::IO::Charset::CCoderResult;

namespace Elastos {
namespace IO {
namespace Charset {

const Int32 CharsetDecoder::INIT;
const Int32 CharsetDecoder::ONGOING;
const Int32 CharsetDecoder::END;
const Int32 CharsetDecoder::FLUSH;

CharsetDecoder::CharsetDecoder()
    : mAverageCharsPerByte(0.0f)
    , mMaxCharsPerByte(0.0f)
    , mStatus(0)
{}

CharsetDecoder::~CharsetDecoder()
{}

ECode CharsetDecoder::Init(
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
    mCs = charset;
    mStatus = INIT;

    AutoPtr<ICodingErrorAction> report;
    CCodingErrorAction::GetREPORT((ICodingErrorAction**)&report);
    mMalformedInputAction = report;
    mUnmappableCharacterAction = report;
    mReplacementChars = "\ufffd";
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
    *charset = mCs;
    REFCOUNT_ADD(*charset)
    return NOERROR;
}

ECode CharsetDecoder::Decode(
    /* [in] */ IByteBuffer* byteBuffer,
    /* [out] */ ICharBuffer** charBuffer)
{
    VALIDATE_NOT_NULL(charBuffer)
    *charBuffer = NULL;

    Reset();
    Int32 remaining = 0;
    assert(byteBuffer != NULL);
    FAIL_RETURN(IBuffer::Probe(byteBuffer)->GetRemaining(&remaining))
    Int32 length = (Int32) (remaining * mAverageCharsPerByte);
    AutoPtr<ICharBuffer> output;
    FAIL_RETURN(CharBuffer::Allocate(length, (ICharBuffer**)&output))
    AutoPtr<ICoderResult> result;

    while (TRUE) {
        result = NULL;
        FAIL_RETURN(Decode(byteBuffer, output, FALSE, (ICoderResult**)&result))
        FAIL_RETURN(CheckCoderResult(result))
        Boolean ret = FALSE;
        if ((result->IsUnderflow(&ret), ret)) {
            break;
        }
        else if ((result->IsOverflow(&ret), ret)) {
            AutoPtr<ICharBuffer> tmpOutput;
            FAIL_RETURN(AllocateMore(output, (ICharBuffer**)&tmpOutput))
            output = tmpOutput;
        }
    }

    result = NULL;
    FAIL_RETURN(Decode(byteBuffer, output, TRUE, (ICoderResult**)&result))
    FAIL_RETURN(CheckCoderResult(result))

    while (TRUE) {
        result = NULL;
        Flush(output, (ICoderResult**)&result);
        FAIL_RETURN(CheckCoderResult(result))
        Boolean ret = FALSE;
        if ((result->IsOverflow(&ret), ret)) {
            AutoPtr<ICharBuffer> tmpOutput;
            FAIL_RETURN(AllocateMore(output, (ICharBuffer**)&tmpOutput))
            output = tmpOutput;
        }
        else {
            break;
        }
    }

    IBuffer::Probe(output)->Flip();
    mStatus = FLUSH;
    *charBuffer = output;
    REFCOUNT_ADD(*charBuffer)
    return NOERROR;
}

ECode CharsetDecoder::Decode(
    /* [in] */ IByteBuffer* byteBuffer,
    /* [in] */ ICharBuffer* charBuffer,
    /* [in] */ Boolean endOfInput,
    /* [out] */ ICoderResult** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    /*
     * status check
     */
    if ((mStatus == FLUSH) || (!endOfInput && mStatus == END)) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // begin to decode
    while (TRUE) {
        AutoPtr<ICodingErrorAction> action;
        ECode ec = DecodeLoop(byteBuffer, charBuffer, result);
        if (ec == (ECode)E_BUFFER_OVER_FLOW_EXCEPTION || ec == (ECode)E_BUFFER_UNDER_FLOW_EXCEPTION) {
            // unexpected exception
            // throw new CoderMalfunctionError(ex);
            return E_CODER_MALFUNCTION_ERROR;
        }
        /*
         * result handling
         */
        Boolean ret = FALSE;
        if (((*result)->IsUnderflow(&ret), ret)) {
            Int32 remaining = 0;
            assert(byteBuffer != NULL);
            IBuffer::Probe(byteBuffer)->GetRemaining(&remaining);
            mStatus = endOfInput ? END : ONGOING;
            if (endOfInput && remaining > 0) {
                *result = NULL;
                CCoderResult::MalformedForLength(remaining, result);
            }
            else {
                return NOERROR;
            }
        }
        ret = FALSE;
        if (((*result)->IsOverflow(&ret), ret)) {
            return NOERROR;
        }
        // set coding error handle action
        action = mMalformedInputAction;
        ret = FALSE;
        if (((*result)->IsUnmappable(&ret), ret)) {
            action = mUnmappableCharacterAction;
        }
        // If the action is IGNORE or REPLACE, we should continue decoding.
        AutoPtr<ICodingErrorAction> REPLACE;
        CCodingErrorAction::GetREPLACE((ICodingErrorAction**)&REPLACE);
        if ((action == REPLACE)) {
            Int32 remaining = 0;
            assert(charBuffer != NULL);
            IBuffer::Probe(charBuffer)->GetRemaining(&remaining);
            if (remaining < mReplacementChars.GetLength()) {
                *result = NULL;
                return CCoderResult::GetOVERFLOW(result);
            }
            charBuffer->Put(mReplacementChars);
        }
        else {
            AutoPtr<ICodingErrorAction> IGNORE;
            CCodingErrorAction::GetIGNORE((ICodingErrorAction**)&IGNORE);
            if (!(action == IGNORE)){
                return NOERROR;
            }
        }
        Int32 tmpPosition = 0;
        IBuffer::Probe(byteBuffer)->GetPosition(&tmpPosition);
        Int32 tmpLen = 0;
        (*result)->GetLength(&tmpLen);
        IBuffer::Probe(byteBuffer)->SetPosition(tmpPosition + tmpLen);
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

    if (mStatus != END && mStatus != INIT) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    FAIL_RETURN(ImplFlush(charBuffer, result))
    AutoPtr<ICoderResult> UNDERFLOW;
    CCoderResult::GetUNDERFLOW((ICoderResult**)&UNDERFLOW);
    if (*result == UNDERFLOW) {
        mStatus = FLUSH;
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
    mStatus = INIT;
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
