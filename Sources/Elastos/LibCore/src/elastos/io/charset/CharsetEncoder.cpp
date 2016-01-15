
#include "CCoderResult.h"
#include "CCodingErrorAction.h"
#include "CharsetEncoder.h"
#include "CharBuffer.h"
#include "ByteBuffer.h"

namespace Elastos {
namespace IO {
namespace Charset {

const Int32 CharsetEncoder::READY;
const Int32 CharsetEncoder::ONGOING;
const Int32 CharsetEncoder::END;
const Int32 CharsetEncoder::FLUSH;
const Int32 CharsetEncoder::INIT;

CharsetEncoder::CharsetEncoder()
    : mAverageBytesPerChar(0.0f)
    , mMaxBytesPerChar(0.0f)
    , mStatus(0)
    , mFinished(FALSE)
{}

CharsetEncoder::~CharsetEncoder()
{}

CAR_INTERFACE_IMPL(CharsetEncoder, Object, ICharsetEncoder)

ECode CharsetEncoder::Init(
    /* [in] */ ICharset* cs,
    /* [in] */ Float averageBytesPerChar,
    /* [in] */ Float maxBytesPerChar)
{
    AutoPtr<ArrayOf<Byte> > replacement = ArrayOf<Byte>::Alloc(1);
    (*replacement)[0] = (Byte)'?';
    return Init(cs, averageBytesPerChar, maxBytesPerChar, replacement);
}

ECode CharsetEncoder::Init(
    /* [in] */ ICharset* cs,
    /* [in] */ Float averageBytesPerChar,
    /* [in] */ Float maxBytesPerChar,
    /* [in] */ ArrayOf<Byte>* replacement)
{
    return Init(cs, averageBytesPerChar, maxBytesPerChar, replacement, FALSE);
}

ECode CharsetEncoder::Init(
    /* [in] */ ICharset* cs,
    /* [in] */ Float averageBytesPerChar,
    /* [in] */ Float maxBytesPerChar,
    /* [in] */ ArrayOf<Byte>* replacement,
    /* [in] */ Boolean trusted)
{
    if (replacement == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (averageBytesPerChar <= 0 || maxBytesPerChar <= 0) {
        // throw new IllegalArgumentException("averageBytesPerChar and maxBytesPerChar must both be positive");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (averageBytesPerChar > maxBytesPerChar) {
        // throw new IllegalArgumentException("averageBytesPerChar is greater than maxBytesPerChar");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mCs = cs;
    mAverageBytesPerChar = averageBytesPerChar;
    mMaxBytesPerChar = maxBytesPerChar;
    mStatus = INIT;

    AutoPtr<ICodingErrorAction> report;
    CCodingErrorAction::GetREPORT((ICodingErrorAction **)&report);
    mMalformedInputAction = report;
    mUnmappableCharacterAction = report;

    if (trusted) {
        // The RI enforces unnecessary restrictions on the replacement bytes. We trust ICU to
        // know what it's doing. Doing so lets us support ICU's EUC-JP, SCSU, and Shift_JIS.
        mReplacementBytes = replacement;
    }
    else {
        FAIL_RETURN(ReplaceWith(replacement))
    }

    return NOERROR;
}

ECode CharsetEncoder::AverageBytesPerChar(
    /* [out] */ Float* averageNumber)
{
    VALIDATE_NOT_NULL(averageNumber);
    *averageNumber = mAverageBytesPerChar;
    return NOERROR;
}

ECode CharsetEncoder::CanEncode(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Char32> > chars = ArrayOf<Char32>::Alloc(1);
    (*chars)[0] = c;
    AutoPtr<ICharBuffer> charBuffer;
    CharBuffer::Wrap(chars, (ICharBuffer**)&charBuffer);
    return ImplCanEncode(charBuffer.Get(), result);
}

ECode CharsetEncoder::CanEncode(
    /* [in] */ ICharSequence* sequence,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(sequence);

    AutoPtr<ICharBuffer> cb;
    AutoPtr<ICharBuffer> charBuffer = (ICharBuffer*)sequence->Probe(EIID_ICharBuffer);
    if (charBuffer != NULL) {
        ((ICharBuffer*) sequence)->Duplicate((ICharBuffer**)&cb);
    }
    else {
        CharBuffer::Wrap(sequence, (ICharBuffer**)&cb);
    }

    return ImplCanEncode(cb.Get(), result);
}

ECode CharsetEncoder::GetCharset(
    /* [out] */ ICharset** charset)
{
    VALIDATE_NOT_NULL(charset);
    *charset = mCs;
    REFCOUNT_ADD(*charset);
    return NOERROR;
}

ECode CharsetEncoder::Encode(
    /* [in] */ ICharBuffer* charBuffer,
    /* [out] */ IByteBuffer** byteBuffer)
{
    VALIDATE_NOT_NULL(byteBuffer);
    *byteBuffer = NULL;
    VALIDATE_NOT_NULL(charBuffer);

    Int32 remaining = 0;
    FAIL_RETURN(IBuffer::Probe(charBuffer)->GetRemaining(&remaining));
    if (remaining == 0) {
        return ByteBuffer::Allocate(0, byteBuffer);
    }

    FAIL_RETURN(Reset());

    FAIL_RETURN(IBuffer::Probe(charBuffer)->GetRemaining(&remaining));
    Int32 length = (Int32) (remaining * mAverageBytesPerChar);
    AutoPtr<IByteBuffer> output, temp;
    FAIL_RETURN(ByteBuffer::Allocate(length, (IByteBuffer**)&output));
    AutoPtr<ICoderResult> result;
    AutoPtr<ICoderResult> UNDERFLOW;
    AutoPtr<ICoderResult> OVERFLOW;
    CCoderResult::GetUNDERFLOW((ICoderResult**)&UNDERFLOW);
    CCoderResult::GetOVERFLOW((ICoderResult**)&OVERFLOW);

    while (TRUE) {
        result = NULL;
        FAIL_RETURN(Encode(charBuffer, output.Get(), FALSE, (ICoderResult**)&result));
        if (result.Get() == UNDERFLOW) {
            temp = NULL;
            FAIL_RETURN(AllocateMore(output.Get(), (IByteBuffer**)&temp));
            output = temp;
            continue;   // No point trying to flush to an already-full buffer.
        }
        FAIL_RETURN(CheckCoderResult(result.Get()));
    }

    result = NULL;
    FAIL_RETURN(Encode(charBuffer, output.Get(), TRUE, (ICoderResult**)&result));
    FAIL_RETURN(CheckCoderResult(result.Get()));

    while (TRUE) {
        result = NULL;
        FAIL_RETURN(Flush(output.Get(), (ICoderResult**)&result));
        if ((result == OVERFLOW)) {
            temp = NULL;
            FAIL_RETURN(AllocateMore(output.Get(), (IByteBuffer**)&temp));
            output = temp;
            continue;
        }
        FAIL_RETURN(CheckCoderResult(result.Get()));
        FAIL_RETURN(IBuffer::Probe(output)->Flip());
        Boolean res;
        FAIL_RETURN(result->IsMalformed(&res));
        if (res) {
            // throw new MalformedInputException(result.length());
            return E_MALFORMED_INPUT_EXCEPTION;
        }
        FAIL_RETURN(result->IsUnmappable(&res));
        if (res) {
            // throw new UnmappableCharacterException(result.length());
            return E_UNMAPPABLE_CHARACTER_EXCEPTION;
        }
        break;
    }
    mStatus = READY;
    mFinished = TRUE;
    *byteBuffer = output;
    REFCOUNT_ADD(*byteBuffer)
    return NOERROR;
}

ECode CharsetEncoder::Encode(
    /* [in] */ ICharBuffer* charBuffer,
    /* [in, out] */ IByteBuffer* byteBuffer,
    /* [in] */ Boolean endOfInput,
    /* [out] */ ICoderResult** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(charBuffer);
    VALIDATE_NOT_NULL(byteBuffer);

    // If the previous step is encode(CharBuffer), then no more input is needed
    // thus endOfInput should not be false
    if (mStatus == READY && mFinished && !endOfInput) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if ((mStatus == FLUSH) || (!endOfInput && mStatus == END)) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<ICoderResult> UNDERFLOW;
    AutoPtr<ICoderResult> OVERFLOW;
    CCoderResult::GetUNDERFLOW((ICoderResult**)&UNDERFLOW);
    CCoderResult::GetOVERFLOW((ICoderResult**)&OVERFLOW);

    AutoPtr<ICoderResult> res;
    while (TRUE) {
        res = NULL;
        FAIL_RETURN(EncodeLoop(charBuffer, byteBuffer, (ICoderResult**)&res));
        if ((res == UNDERFLOW)) {
            mStatus = endOfInput ? END : ONGOING;
            if (endOfInput) {
                Int32 remaining = 0;
                IBuffer::Probe(charBuffer)->GetRemaining(&remaining);
                if (remaining > 0) {
                    res = NULL;
                    FAIL_RETURN(CCoderResult::MalformedForLength(remaining, (ICoderResult**)&res));
                } else {
                    *result = res;
                    REFCOUNT_ADD(*result)
                    return NOERROR;
                }
            } else {
                *result = res;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        } else if ((res == OVERFLOW)) {
            mStatus = endOfInput ? END : ONGOING;
            *result = res;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        AutoPtr<ICodingErrorAction> action = mMalformedInputAction;
        Boolean enable = FALSE;
        FAIL_RETURN(res->IsUnmappable(&enable));
        if (enable) {
            action = mUnmappableCharacterAction;
        }
        // If the action is IGNORE or REPLACE, we should continue
        // encoding.

        AutoPtr<ICodingErrorAction> REPLACE;
        AutoPtr<ICodingErrorAction> IGNORE;
        CCodingErrorAction::GetREPLACE((ICodingErrorAction **)&REPLACE);
        CCodingErrorAction::GetIGNORE((ICodingErrorAction **)&IGNORE);

        if ((action == REPLACE)) {
            Int32 remaining = 0;
            FAIL_RETURN(IBuffer::Probe(byteBuffer)->GetRemaining(&remaining));
            if (remaining < mReplacementBytes->GetLength()) {
                *result = OVERFLOW;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
            byteBuffer->Put(mReplacementBytes);
        } else {
            if (action != IGNORE) {
                *result = res;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        }
        Int32 pos = 0;
        FAIL_RETURN(IBuffer::Probe(charBuffer)->GetPosition(&pos));
        Int32 len = 0;
        res->GetLength(&len);
        FAIL_RETURN(IBuffer::Probe(charBuffer)->SetPosition(pos + len));
    }

    return NOERROR;
}

ECode CharsetEncoder::Flush(
    /* [in, out] */ IByteBuffer* byteBuffer,
    /* [out] */ ICoderResult** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(byteBuffer);

    if (mStatus != END && mStatus != READY) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<ICoderResult> res;
    FAIL_RETURN(ImplFlush(byteBuffer, (ICoderResult**)&res));

    AutoPtr<ICoderResult> UNDERFLOW;
    CCoderResult::GetUNDERFLOW((ICoderResult**)&UNDERFLOW);

    if ((res == UNDERFLOW)) {
        mStatus = FLUSH;
    }

    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CharsetEncoder::IsLegalReplacement(
    /* [in] */ ArrayOf<Byte>* replacement,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(replacement);

    if (mDecoder == NULL) {
        FAIL_RETURN(mCs->NewDecoder((ICharsetDecoder**)&mDecoder));

        AutoPtr<ICodingErrorAction> REPORT;
        CCodingErrorAction::GetREPORT((ICodingErrorAction **)&REPORT);

        FAIL_RETURN(mDecoder->OnMalformedInput(REPORT.Get()))
        FAIL_RETURN(mDecoder->OnUnmappableCharacter(REPORT.Get()))
    }
    AutoPtr<IByteBuffer> in;
    FAIL_RETURN(ByteBuffer::Wrap(replacement, (IByteBuffer**)&in));
    AutoPtr<ICharBuffer> out;
    Float maxNum = 0.0f;
    mDecoder->MaxCharsPerByte(&maxNum);
    FAIL_RETURN(CharBuffer::Allocate((Int32) (replacement->GetLength() * maxNum), (ICharBuffer**)&out));
    AutoPtr<ICoderResult> res;
    FAIL_RETURN(mDecoder->Decode(in.Get(), out, TRUE, (ICoderResult**)&res));

    Boolean err = FALSE;
    res->IsError(&err);
    *result = !err;

    return NOERROR;
}

ECode CharsetEncoder::MalformedInputAction(
    /* [out] */ ICodingErrorAction** errorAction)
{
    VALIDATE_NOT_NULL(errorAction);
    *errorAction = mMalformedInputAction;
    REFCOUNT_ADD(*errorAction)
    return NOERROR;
}

ECode CharsetEncoder::MaxBytesPerChar(
    /* [out] */ Float* maxNumber)
{
    VALIDATE_NOT_NULL(maxNumber);
    *maxNumber = mMaxBytesPerChar;
    return NOERROR;
}

ECode CharsetEncoder::OnMalformedInput(
    /* [in] */ ICodingErrorAction* newAction)
{
    VALIDATE_NOT_NULL(newAction);

    mMalformedInputAction = newAction;
    return ImplOnMalformedInput(newAction);
}

ECode CharsetEncoder::OnUnmappableCharacter(
    /* [in] */ ICodingErrorAction* newAction)
{
    VALIDATE_NOT_NULL(newAction);

    mUnmappableCharacterAction = newAction;
    return ImplOnUnmappableCharacter(newAction);
}

ECode CharsetEncoder::Replacement(
    /* [out, callee] */ ArrayOf<Byte>** replace)
{
    VALIDATE_NOT_NULL(replace);
    *replace = mReplacementBytes;
    REFCOUNT_ADD(*replace);
    return NOERROR;
}

ECode CharsetEncoder::ReplaceWith(
    /* [in] */ ArrayOf<Byte>* replacement)
{
    VALIDATE_NOT_NULL(replacement);

    if (replacement->GetLength() == 0) {
        // throw new IllegalArgumentException("replacement.length == 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (replacement->GetLength() > mMaxBytesPerChar) {
        // throw new IllegalArgumentException("replacement.length > maxBytesPerChar: " +
        //        replacement.length + " > " + maxBytesPerChar());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean result = FALSE;
    FAIL_RETURN(IsLegalReplacement(replacement, &result));
    if (!result) {
        // throw new IllegalArgumentException("bad replacement: " + Arrays.toString(replacement));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // It seems like a bug, but the RI doesn't clone, and we have tests that check we don't.
    mReplacementBytes = replacement;
    FAIL_RETURN(ImplReplaceWith(mReplacementBytes));
    return NOERROR;
}

ECode CharsetEncoder::Reset()
{
    mStatus = INIT;
    FAIL_RETURN(ImplReset());
    return NOERROR;
}

ECode CharsetEncoder::UnmappableCharacterAction(
    /* [out] */ ICodingErrorAction** errorAction)
{
    VALIDATE_NOT_NULL(errorAction);
    *errorAction = mUnmappableCharacterAction;
    REFCOUNT_ADD(*errorAction)
    return NOERROR;
}

ECode CharsetEncoder::ImplFlush(
    /* [in] */ IByteBuffer* outBuffer,
    /* [out] */  ICoderResult** result)
{
    CCoderResult::GetUNDERFLOW(result);
    return NOERROR;
}

ECode CharsetEncoder::ImplOnMalformedInput(
    /* [in] */ ICodingErrorAction* newAction)
{
    // default implementation is empty
    return NOERROR;
}
ECode CharsetEncoder::ImplOnUnmappableCharacter(
    /* [in] */ ICodingErrorAction* newAction)
{
    // default implementation is empty
    return NOERROR;
}

ECode CharsetEncoder::ImplReplaceWith(
    /* [in] */ ArrayOf<Byte>* newReplacement)
{
    // default implementation is empty
    return NOERROR;
}

ECode CharsetEncoder::ImplReset()
{
    // default implementation is empty
    return NOERROR;
}

ECode CharsetEncoder::ImplCanEncode(
    /* [in] */ ICharBuffer* cb,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    VALIDATE_NOT_NULL(cb)

    if (mStatus == FLUSH || mStatus == INIT) {
        mStatus = READY;
    }
    if (mStatus != READY) {
        // throw new IllegalStateException("encoding already in progress");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<ICodingErrorAction> malformBak = mMalformedInputAction;
    AutoPtr<ICodingErrorAction> unmapBak = mUnmappableCharacterAction;
    AutoPtr<ICodingErrorAction> REPORT;
    CCodingErrorAction::GetREPORT((ICodingErrorAction**)&REPORT);

    FAIL_RETURN(OnMalformedInput(REPORT.Get()))
    FAIL_RETURN(OnUnmappableCharacter(REPORT.Get()))
    *result = TRUE;

    AutoPtr<IByteBuffer> bb;
    if(FAILED(Encode(cb, (IByteBuffer**)&bb))) {
        *result = FALSE;
    }

    FAIL_RETURN(OnMalformedInput(malformBak))
    FAIL_RETURN(OnUnmappableCharacter(unmapBak))
    FAIL_RETURN(Reset())
    return NOERROR;
}

ECode CharsetEncoder::CheckCoderResult(
    /* [in] */ ICoderResult* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICodingErrorAction> REPORT;
    CCodingErrorAction::GetREPORT((ICodingErrorAction **)&REPORT);
    Boolean isEnable = FALSE;

    if ((mMalformedInputAction == REPORT) && (result->IsMalformed(&isEnable), isEnable)) {
        // throw new MalformedInputException(result.length());
        return E_MALFORMED_INPUT_EXCEPTION;
    }
    else if ((mUnmappableCharacterAction == REPORT) && (result->IsUnmappable(&isEnable), isEnable)) {
        // throw new UnmappableCharacterException(result.length());
        return E_UNMAPPABLE_CHARACTER_EXCEPTION;
    }
    return NOERROR;
}

ECode CharsetEncoder::AllocateMore(
    /* [in, out] */ IByteBuffer* output,
    /* [out] */ IByteBuffer** byteBuffer)
{
    VALIDATE_NOT_NULL(byteBuffer);
    *byteBuffer = NULL;
    VALIDATE_NOT_NULL(output);

    Int32 cap = 0;
    FAIL_RETURN(IBuffer::Probe(output)->GetCapacity(&cap));
    if (cap == 0) {
        FAIL_RETURN(ByteBuffer::Allocate(1, byteBuffer));
        return NOERROR;
    }
    FAIL_RETURN(ByteBuffer::Allocate(cap * 2, byteBuffer));
    FAIL_RETURN(IBuffer::Probe(output)->Flip());
    FAIL_RETURN((*byteBuffer)->Put(output));
    return NOERROR;
}

ECode CharsetEncoder::GetMalformedInputAction(
    /* [out] */ ICodingErrorAction** errorAction)
{
    VALIDATE_NOT_NULL(errorAction);
    *errorAction = mMalformedInputAction;
    REFCOUNT_ADD(*errorAction);
    return NOERROR;
}

ECode CharsetEncoder::GetMaxBytesPerChar(
    /* [out] */ Float* maxNumber)
{
    VALIDATE_NOT_NULL(maxNumber);
    *maxNumber = mMaxBytesPerChar;
    return NOERROR;
}

ECode CharsetEncoder::GetReplacement(
    /* [out] */ ArrayOf<Byte>** replace)
{
    VALIDATE_NOT_NULL(replace);
    *replace = mReplacementBytes;
    REFCOUNT_ADD(*replace);
    return NOERROR;
}

ECode CharsetEncoder::GetUnmappableCharacterAction(
    /* [out] */ ICodingErrorAction** errorAction)
{
    VALIDATE_NOT_NULL(errorAction);
    *errorAction = mUnmappableCharacterAction;
    REFCOUNT_ADD(*errorAction);
    return NOERROR;
}

} // namespace Charset
} // namespace IO
} // namespace Elastos
