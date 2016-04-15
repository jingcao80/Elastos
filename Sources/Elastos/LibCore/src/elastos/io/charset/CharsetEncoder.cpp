
#include "CCoderResult.h"
#include "CCodingErrorAction.h"
#include "CharsetEncoder.h"
#include "CharBuffer.h"
#include "ByteBuffer.h"

namespace Elastos {
namespace IO {
namespace Charset {

const String CharsetEncoder::RESET("RESET");
const String CharsetEncoder::CharsetEncoder::ONGOING("ONGOING");
const String CharsetEncoder::END_OF_INPUT("END_OF_INPUT");
const String CharsetEncoder::FLUSHED("FLUSHED");

CAR_INTERFACE_IMPL(CharsetEncoder, Object, ICharsetEncoder)

CharsetEncoder::CharsetEncoder()
    : mAverageBytesPerChar(0.0f)
    , mMaxBytesPerChar(0.0f)
    , mState(RESET)
{}

CharsetEncoder::~CharsetEncoder()
{}

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

    mCharset = cs;
    mAverageBytesPerChar = averageBytesPerChar;
    mMaxBytesPerChar = maxBytesPerChar;

    AutoPtr<ICodingErrorAction> report;
    CCodingErrorAction::GetREPORT((ICodingErrorAction**)&report);
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
    return CanEncode(ICharSequence::Probe(charBuffer), result);
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

    if (mState == FLUSHED) {
        Reset();
    }
    if (mState != RESET) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<ICodingErrorAction> originalMalformedInputAction = mMalformedInputAction;
    AutoPtr<ICodingErrorAction> originalUnmappableCharacterAction = mUnmappableCharacterAction;

    AutoPtr<ICodingErrorAction> report;
    CCodingErrorAction::GetREPORT((ICodingErrorAction**)&report);
    FAIL_RETURN(OnMalformedInput(report))
    FAIL_RETURN(OnUnmappableCharacter(report))

    AutoPtr<IByteBuffer> bb;
    ECode ec = Encode(cb, (IByteBuffer**)&bb);

    FAIL_RETURN(OnMalformedInput(originalMalformedInputAction))
    FAIL_RETURN(OnUnmappableCharacter(originalUnmappableCharacterAction))
    Reset();

    *result = FAILED(ec) ? FALSE : TRUE;
    return NOERROR;
}

ECode CharsetEncoder::GetCharset(
    /* [out] */ ICharset** charset)
{
    VALIDATE_NOT_NULL(charset);
    *charset = mCharset;
    REFCOUNT_ADD(*charset);
    return NOERROR;
}

ECode CharsetEncoder::Encode(
    /* [in] */ ICharBuffer* in,
    /* [out] */ IByteBuffer** byteBuffer)
{
    VALIDATE_NOT_NULL(byteBuffer);
    *byteBuffer = NULL;
    VALIDATE_NOT_NULL(in);

    Int32 remaining;
    IBuffer* cb = IBuffer::Probe(in);
    FAIL_RETURN(cb->GetRemaining(&remaining));
    Int32 length = (Int32) (remaining * mAverageBytesPerChar);

    AutoPtr<IByteBuffer> out;
    ByteBuffer::Allocate(length, (IByteBuffer**)&out);

    FAIL_RETURN(Reset())

    AutoPtr<ICoderResult> OVERFLOW;
    CCoderResult::GetOVERFLOW((ICoderResult**)&OVERFLOW);

    AutoPtr<IByteBuffer> temp;
    while (mState != FLUSHED) {
        AutoPtr<ICoderResult> result;
        FAIL_RETURN(Encode(in, out, TRUE, (ICoderResult**)&result));
        if (result.Get() == OVERFLOW.Get()) {
            temp = out;
            out = NULL;
            FAIL_RETURN(AllocateMore(temp, (IByteBuffer**)&out))
            continue; // No point trying to flush to an already-full buffer.
        }
        else {
            FAIL_RETURN(CheckCoderResult(result))
        }

        result = NULL;
        FAIL_RETURN(Flush(out, (ICoderResult**)&result));
        if (result.Get() == OVERFLOW.Get()) {
            temp = out;
            out = NULL;
            FAIL_RETURN(AllocateMore(temp, (IByteBuffer**)&out))
        }
        else {
            FAIL_RETURN(CheckCoderResult(result))
        }
    }

    FAIL_RETURN(IBuffer::Probe(out)->Flip())
    *byteBuffer = out;
    REFCOUNT_ADD(*byteBuffer)
    return NOERROR;
}

ECode CharsetEncoder::CheckCoderResult(
    /* [in] */ ICoderResult* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICodingErrorAction> REPORT;
    CCodingErrorAction::GetREPORT((ICodingErrorAction**)&REPORT);
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

    if (mState != RESET && mState != ONGOING && !(endOfInput && mState == END_OF_INPUT)) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mState = endOfInput ? END_OF_INPUT : ONGOING;

    AutoPtr<ICoderResult> UNDERFLOW, OVERFLOW;
    CCoderResult::GetUNDERFLOW((ICoderResult**)&UNDERFLOW);
    CCoderResult::GetOVERFLOW((ICoderResult**)&OVERFLOW);

    AutoPtr<ICodingErrorAction> REPLACE, REPORT;
    CCodingErrorAction::GetREPLACE((ICodingErrorAction**)&REPLACE);
    CCodingErrorAction::GetREPORT((ICodingErrorAction**)&REPORT);

    IBuffer* cb = IBuffer::Probe(charBuffer);
    IBuffer* bb = IBuffer::Probe(byteBuffer);

    AutoPtr<ICoderResult> res;
    Int32 remaining = 0;
    Boolean hasRemaining = FALSE;
    while (TRUE) {
        res = NULL;
        FAIL_RETURN(EncodeLoop(charBuffer, byteBuffer, (ICoderResult**)&res));
        if (res == UNDERFLOW) {
            if (endOfInput && (cb->HasRemaining(&hasRemaining), hasRemaining)) {
                cb->GetRemaining(&remaining);
                res = NULL;
                FAIL_RETURN(CCoderResult::MalformedForLength(remaining, (ICoderResult**)&res));
            }
            break;
        }
        else if (res.Get() == OVERFLOW) {
            break;
        }

    // We have a real error, so do what the appropriate action tells us what to do...
        AutoPtr<ICodingErrorAction> action = mMalformedInputAction;
        Boolean enable = FALSE;
        FAIL_RETURN(res->IsUnmappable(&enable));
        if (enable) {
            action = mUnmappableCharacterAction;
        }

        if (action.Get() == REPORT) {
            break;
        }
        else if (action.Get() == REPLACE) {
            FAIL_RETURN(bb->GetRemaining(&remaining));
            if (remaining < mReplacementBytes->GetLength()) {
                res = OVERFLOW;
                break;
            }
            byteBuffer->Put(mReplacementBytes);
        }

        Int32 pos = 0, len = 0;
        FAIL_RETURN(cb->GetPosition(&pos));
        res->GetLength(&len);
        FAIL_RETURN(cb->SetPosition(pos + len));
    }

    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CharsetEncoder::Flush(
    /* [in, out] */ IByteBuffer* byteBuffer,
    /* [out] */ ICoderResult** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(byteBuffer);

    if (mState != FLUSHED && mState != END_OF_INPUT) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<ICoderResult> res;
    FAIL_RETURN(ImplFlush(byteBuffer, (ICoderResult**)&res));

    AutoPtr<ICoderResult> UNDERFLOW;
    CCoderResult::GetUNDERFLOW((ICoderResult**)&UNDERFLOW);

    if (res.Get() == UNDERFLOW) {
        mState = FLUSHED;
    }

    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CharsetEncoder::ImplFlush(
    /* [in] */ IByteBuffer* outBuffer,
    /* [out] */  ICoderResult** result)
{
    return CCoderResult::GetUNDERFLOW(result);
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

ECode CharsetEncoder::IsLegalReplacement(
    /* [in] */ ArrayOf<Byte>* replacement,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(replacement);

    if (mDecoder == NULL) {
        FAIL_RETURN(mCharset->NewDecoder((ICharsetDecoder**)&mDecoder));

        AutoPtr<ICodingErrorAction> REPORT;
        CCodingErrorAction::GetREPORT((ICodingErrorAction**)&REPORT);

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
    mState = RESET;
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
