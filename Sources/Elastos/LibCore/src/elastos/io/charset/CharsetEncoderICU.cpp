
#include "CCoderResult.h"
#include "CharsetEncoderICU.h"
#include "NativeConverter.h"
#include "CharBuffer.h"
#include <unicode/utypes.h>

using Libcore::ICU::NativeConverter;

namespace Elastos {
namespace IO {
namespace Charset {

AutoPtr< HashMap<String, AutoPtr<ArrayOf<Byte> > > > CharsetEncoderICU::DEFAULT_REPLACEMENTS = new HashMap<String, AutoPtr<ArrayOf<Byte> > >();

Boolean CharsetEncoderICU::InitStatic()
{
    // ICU has different default replacements to the RI in some cases. There are many
    // additional cases, but this covers all the charsets that Java guarantees will be
    // available, which is where compatibility seems most important. (The RI even uses
    // the byte corresponding to '?' in ASCII as the replacement byte for charsets where that
    // byte corresponds to an entirely different character.)
    // It's odd that UTF-8 doesn't use U+FFFD, given that (unlike ISO-8859-1 and US-ASCII) it
    // can represent it, but this is what the RI does...
    AutoPtr< ArrayOf<Byte> > questionMark = ArrayOf<Byte>::Alloc(1);
    (*questionMark)[0] = (Byte) '?';
    (*CharsetEncoderICU::DEFAULT_REPLACEMENTS)[String("UTF-8")] = questionMark;
    (*CharsetEncoderICU::DEFAULT_REPLACEMENTS)[String("ISO-8859-1")] = questionMark;
    (*CharsetEncoderICU::DEFAULT_REPLACEMENTS)[String("US-ASCII")] = questionMark;
    return TRUE;
}

const Int32 CharsetEncoderICU::INPUT_OFFSET;
const Int32 CharsetEncoderICU::OUTPUT_OFFSET;
const Int32 CharsetEncoderICU::INVALID_CHARS;

Boolean CharsetEncoderICU::sInitBlock = InitStatic();

CharsetEncoderICU::CharsetEncoderICU()
    : mConverterHandle(0)
    , mInEnd(0)
    , mOutEnd(0)
{
    mData = ArrayOf<Int32>::Alloc(3);
}

CharsetEncoderICU::~CharsetEncoderICU()
{
    NativeConverter::CloseConverter(mConverterHandle);
    mConverterHandle=0;
}

ECode CharsetEncoderICU::Init(
    /* [in] */ ICharset* cs,
    /* [in] */ Float averageBytesPerChar,
    /* [in] */ Float maxBytesPerChar,
    /* [in] */ ArrayOf<Byte>* replacement,
    /* [in] */ Int64 address)
{
    ASSERT_SUCCEEDED(CharsetEncoder::Init(cs, averageBytesPerChar, maxBytesPerChar, replacement, TRUE));
    // Our native peer needs to know what just happened...
    mConverterHandle = address;
    return UpdateCallback();
}

ECode CharsetEncoderICU::NewInstance(
    /* [in] */ ICharset* cs,
    /* [in] */ const String& icuCanonicalName,
    /* [out] */ CharsetEncoderICU** encoderICU)
{
    VALIDATE_NOT_NULL(encoderICU)
    *encoderICU = NULL;

    // This complexity is necessary to ensure that even if the constructor, superclass
    // constructor, or call to updateCallback throw, we still free the native peer.
    Int64 address = 0;
    ECode ecode = NOERROR;
    //try {
    {
        ecode = NativeConverter::OpenConverter(icuCanonicalName, &address);
        if(FAILED(ecode)) goto EXIT;

        Float averageBytesPerChar = 0;
        NativeConverter::GetAveBytesPerChar(address, &averageBytesPerChar);
        Int32 maxBytesPerChar = 0;
        NativeConverter::GetMaxBytesPerChar(address, &maxBytesPerChar);
        AutoPtr< ArrayOf<Byte> > replacement;
        MakeReplacement(icuCanonicalName, address, (ArrayOf<Byte>**)&replacement);
        AutoPtr<CharsetEncoderICU> result = new CharsetEncoderICU();
        result->Init(cs, averageBytesPerChar, maxBytesPerChar, replacement, address);

        *encoderICU = result;
        REFCOUNT_ADD(*encoderICU);

        address = 0; // CharsetEncoderICU has taken ownership; its finalizer will do the free.
        // } finally {
        if (address != 0) {
            NativeConverter::CloseConverter(address);
        }
        // }
    }
EXIT:
    if (address != 0) {
        NativeConverter::CloseConverter(address);
    }

    return ecode;
}

ECode CharsetEncoderICU::ImplReplaceWith(
    /* [in] */ ArrayOf<Byte>* newReplacement)
{
    return UpdateCallback();
}

ECode CharsetEncoderICU::ImplOnMalformedInput(
    /* [in] */ ICodingErrorAction* newAction)
{
    return UpdateCallback();
}

ECode CharsetEncoderICU::ImplOnUnmappableCharacter(
    /* [in] */ ICodingErrorAction* newAction)
{
    return UpdateCallback();
}

ECode CharsetEncoderICU::ImplReset()
{
    NativeConverter::ResetCharToByte(mConverterHandle);
    (*mData)[INPUT_OFFSET] = 0;
    (*mData)[OUTPUT_OFFSET] = 0;
    (*mData)[INVALID_CHARS] = 0;
    mOutput = NULL;
    mInput = NULL;
    mAllocatedInput = NULL;
    mAllocatedOutput = NULL;
    mInEnd = 0;
    mOutEnd = 0;
    return NOERROR;
}

ECode CharsetEncoderICU::ImplFlush(
    /* [in] */ IByteBuffer* outBuffer,
    /* [out] */  ICoderResult** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    // try {
    // ICU needs to see an empty input.
    mInput = ArrayOf<Char32>::Alloc(0);// EmptyArray.CHAR;
    mInEnd = 0;
    (*mData)[INPUT_OFFSET] = 0;
    Int32 ret = 0;
    GetArray(outBuffer, &ret);
    (*mData)[OUTPUT_OFFSET] = ret;
    (*mData)[INVALID_CHARS] = 0; // Make sure we don't see earlier errors.

    Int32 error;
    NativeConverter::Encode(mConverterHandle, mInput, mInEnd, mOutput, mOutEnd, mData, TRUE, &error);
    if (U_FAILURE(UErrorCode(error))) {
        if (error == U_BUFFER_OVERFLOW_ERROR) {
            CCoderResult::GetOVERFLOW(result);
            goto EXIT;
        } else if (error == U_TRUNCATED_CHAR_FOUND) {
            if ((*mData)[INPUT_OFFSET] > 0) {
                CCoderResult::MalformedForLength((*mData)[INPUT_OFFSET], result);
                goto EXIT;
            }
        }
    }
    CCoderResult::GetUNDERFLOW(result);
EXIT:
    SetPosition(outBuffer);
    ImplReset();
    return NOERROR;
}

ECode CharsetEncoderICU::EncodeLoop(
    /* [in] */ ICharBuffer* charBuffer,
    /* [in, out] */ IByteBuffer* byteBuffer,
    /* [out] */ ICoderResult** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    VALIDATE_NOT_NULL(charBuffer)
    Boolean ret = FALSE;

    if (!(IBuffer::Probe(charBuffer)->HasRemaining(&ret), ret)) {
        CCoderResult::GetUNDERFLOW(result);
        return NOERROR;
    }

    Int32 num = 0;
    GetArray(charBuffer, &num);
    (*mData)[INPUT_OFFSET] = num;
    GetArray(byteBuffer, &num);
    (*mData)[OUTPUT_OFFSET]= num;
    (*mData)[INVALID_CHARS] = 0; // Make sure we don't see earlier errors.

    //try {
    Int32 error;
    ECode ecode = NOERROR;
    NativeConverter::Encode(mConverterHandle, mInput, mInEnd, mOutput, mOutEnd, mData, FALSE, &error);
    if (U_FAILURE(UErrorCode(error))) {
        if (error == U_BUFFER_OVERFLOW_ERROR) {
            CCoderResult::GetOVERFLOW(result);
            goto EXIT;
        } else if (error == U_INVALID_CHAR_FOUND) {
            CCoderResult::UnmappableForLength((*mData)[INVALID_CHARS], result);
            goto EXIT;
        } else if (error == U_ILLEGAL_CHAR_FOUND) {
            CCoderResult::MalformedForLength((*mData)[INVALID_CHARS], result);
            goto EXIT;
        } else {
            //throw new AssertionError(error);
            ecode = E_ASSERTION_ERROR;
            goto EXIT;
        }
    }
    // Decoding succeeded: give us more data.
    CCoderResult::GetUNDERFLOW(result);
    // } finally {
        // SetPosition(charBuffer);
        // SetPosition(byteBuffer);
    // }
EXIT:
    SetPosition(charBuffer);
    SetPosition(byteBuffer);

    return ecode;
}

ECode CharsetEncoderICU::MakeReplacement(
    /* [in] */ const String& icuCanonicalName,
    /* [in] */ Int64 address,
    /* [out, callee] */ ArrayOf<Byte>** resultArray)
{
    VALIDATE_NOT_NULL(resultArray)
    *resultArray = NULL;
    // We have our own map of RI-compatible default replacements (where ICU disagrees)...
    AutoPtr< ArrayOf<Byte> > replacement;
    HashMap<String, AutoPtr<ArrayOf<Byte> > >::Iterator iter = DEFAULT_REPLACEMENTS->Find(icuCanonicalName);
    if (iter != DEFAULT_REPLACEMENTS->End()) {
        replacement = iter->mSecond;
    }
    if (replacement != NULL) {
        *resultArray = replacement->Clone();
        REFCOUNT_ADD(*resultArray)
        return NOERROR;
    }
    // ...but fall back to asking ICU.
    return NativeConverter::GetSubstitutionBytes(address, resultArray);
}

ECode CharsetEncoderICU::UpdateCallback()
{
    NativeConverter::SetCallbackEncode(mConverterHandle, this);
    return NOERROR;
}

ECode CharsetEncoderICU::GetArray(
    /* [in] */ IByteBuffer* outBuffer,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    VALIDATE_NOT_NULL(outBuffer)
    Boolean has = FALSE;
    if ((IBuffer::Probe(outBuffer)->HasArray(&has), has)) {
        mOutput = NULL;
        outBuffer->GetArray((ArrayOf<Byte>**)&mOutput);
        Int32 offset = 0;
        Int32 limit = 0;
        IBuffer::Probe(outBuffer)->GetArrayOffset(&offset);
        IBuffer::Probe(outBuffer)->GetLimit(&limit);
        mOutEnd = offset + limit;
        Int32 pos = 0;
        IBuffer::Probe(outBuffer)->GetPosition(&pos);
        *result = offset + pos;
        return NOERROR;
    }
    else {
        IBuffer::Probe(outBuffer)->GetRemaining(&mOutEnd);
        if (mAllocatedOutput == NULL || mOutEnd > mAllocatedOutput->GetLength()) {
            mAllocatedOutput = ArrayOf<Byte>::Alloc(mOutEnd);
        }
        // The array's start position is 0.
        mOutput = mAllocatedOutput;
        *result = 0;
        return NOERROR;
    }
}

ECode CharsetEncoderICU::GetArray(
    /* [in] */ ICharBuffer* inBuffer,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    VALIDATE_NOT_NULL(inBuffer)
    Boolean has = FALSE;
    if ((IBuffer::Probe(inBuffer)->HasArray(&has), has)) {
        mInput = NULL;
        inBuffer->GetArray((ArrayOf<Char32>**)&mInput);
        Int32 offset = 0;
        Int32 limit = 0;
        IBuffer::Probe(inBuffer)->GetArrayOffset(&offset);
        IBuffer::Probe(inBuffer)->GetLimit(&limit);
        mInEnd = offset + limit;
        Int32 pos = 0;
        IBuffer::Probe(inBuffer)->GetPosition(&pos);
        *result = offset + pos;
        return NOERROR;
    }
    else {
        IBuffer::Probe(inBuffer)->GetRemaining(&mInEnd);
        if (mAllocatedInput == NULL || mInEnd > mAllocatedInput->GetLength()) {
            mAllocatedInput = ArrayOf<Char32>::Alloc(mInEnd);
        }
        // Copy the input buffer into the allocated array.
        Int32 pos = 0;
        IBuffer::Probe(inBuffer)->GetPosition(&pos);
        inBuffer->Get(mAllocatedInput, 0, mInEnd);
        IBuffer::Probe(inBuffer)->SetPosition(pos);
        // The array's start position is 0.
        mInput = mAllocatedInput;
        *result = 0;
        return NOERROR;
    }
}

ECode CharsetEncoderICU::SetPosition(
    /* [in] */ IByteBuffer* outBuffer)
{
    VALIDATE_NOT_NULL(outBuffer)
    Boolean has = FALSE;
    if ((IBuffer::Probe(outBuffer)->HasArray(&has), has)) {
        Int32 offset = 0;
        IBuffer::Probe(outBuffer)->GetArrayOffset(&offset);
        IBuffer::Probe(outBuffer)->SetPosition((*mData)[OUTPUT_OFFSET] - offset);
    }
    else {
        outBuffer->Put(mOutput, 0, (*mData)[OUTPUT_OFFSET]);
    }
    // release reference to output array, which may not be ours
    mOutput = NULL;
    return NOERROR;
}

ECode CharsetEncoderICU::SetPosition(
    /* [in] */ ICharBuffer* inBuffer)
{
    VALIDATE_NOT_NULL(inBuffer)
    Int32 pos = 0;
    IBuffer::Probe(inBuffer)->GetPosition(&pos);
    IBuffer::Probe(inBuffer)->SetPosition(pos + (*mData)[INPUT_OFFSET] - (*mData)[INVALID_CHARS]);
    // release reference to input array, which may not be ours
    mInput = NULL;
    return NOERROR;
}

} // namespace Charset
} // namespace IO
} // namespace Elastos
