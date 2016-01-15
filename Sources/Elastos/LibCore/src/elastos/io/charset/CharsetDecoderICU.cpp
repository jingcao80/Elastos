
#include "CCoderResult.h"
#include "CharsetDecoderICU.h"
#include "NativeConverter.h"
#include <unicode/utypes.h>

using Libcore::ICU::NativeConverter;

namespace Elastos {
namespace IO {
namespace Charset {

const Int32 CharsetDecoderICU::MAX_CHARS_PER_BYTE;
const Int32 CharsetDecoderICU::INPUT_OFFSET;
const Int32 CharsetDecoderICU::OUTPUT_OFFSET;
const Int32 CharsetDecoderICU::INVALID_BYTES;

CharsetDecoderICU::CharsetDecoderICU()
    : mConverterHandle(0)
    , mInEnd(0)
    , mOutEnd(0)
{
    mData = ArrayOf<Int32>::Alloc(3);
}

CharsetDecoderICU::~CharsetDecoderICU()
{
    NativeConverter::CloseConverter(mConverterHandle);
    mConverterHandle = 0;
}

ECode CharsetDecoderICU::Init(
    /* [in] */ ICharset* cs,
    /* [in] */ Float averageCharsPerByte,
    /* [in] */ Int64 address)
{
    mConverterHandle = address;
    return CharsetDecoder::Init(cs, averageCharsPerByte, MAX_CHARS_PER_BYTE);
}

ECode CharsetDecoderICU::NewInstance(
    /* [in] */ ICharset* cs,
    /* [in] */ const String& icuCanonicalName,
    /* [out] */ CharsetDecoderICU** decoderICU)
{
    VALIDATE_NOT_NULL(decoderICU)
    // This complexity is necessary to ensure that even if the constructor, superclass
    // constructor, or call to updateCallback throw, we still free the native peer.
    Int64 address = 0;
    ECode ecode = NOERROR;
    {
        ecode = NativeConverter::OpenConverter(icuCanonicalName, &address);
        if (FAILED(ecode))
        {
            goto EXIT;
        }

        Float averageCharsPerByte = 0.0f;
        NativeConverter::GetAveCharsPerByte(address, &averageCharsPerByte);
        AutoPtr<CharsetDecoderICU> result = new CharsetDecoderICU();
        ASSERT_SUCCEEDED(result->Init(cs, averageCharsPerByte, address));

        address = 0; // CharsetDecoderICU has taken ownership; its finalizer will do the free.
        result->UpdateCallback();

        *decoderICU = result;
        REFCOUNT_ADD(*decoderICU)
    }
EXIT:
    if (address != 0) {
        NativeConverter::CloseConverter(address);
    }
    return ecode;
}

ECode CharsetDecoderICU::ImplReplaceWith(
    /* [in] */ const String& newReplacement)
{
    return UpdateCallback();
}

ECode CharsetDecoderICU::ImplOnMalformedInput(
    /* [in] */ ICodingErrorAction* newAction)
{
    return UpdateCallback();
}

ECode CharsetDecoderICU::ImplOnUnmappableCharacter(
    /* [in] */ ICodingErrorAction* newAction)
{
    return UpdateCallback();
}

ECode CharsetDecoderICU::ImplReset()
{
    NativeConverter::ResetByteToChar(mConverterHandle);
    (*mData)[INPUT_OFFSET] = 0;
    (*mData)[OUTPUT_OFFSET] = 0;
    (*mData)[INVALID_BYTES] = 0;
    mOutput = NULL;
    mInput = NULL;
    mAllocatedInput = NULL;
    mAllocatedOutput = NULL;
    mInEnd = 0;
    mOutEnd = 0;
    return NOERROR;
}

ECode CharsetDecoderICU::ImplFlush(
    /* [in] */ ICharBuffer* outInput,
    /* [out] */ ICoderResult** result)
{
    VALIDATE_NOT_NULL(result)
    // ICU needs to see an empty input.
    mInput = ArrayOf<Byte>::Alloc(0);// EmptyArray.BYTE;
    mInEnd = 0;
    (*mData)[INPUT_OFFSET] = 0;
    Int32 ret = 0;
    GetArray(outInput, &ret);
    (*mData)[OUTPUT_OFFSET] = ret;
    (*mData)[INVALID_BYTES] = 0; // Make sure we don't see earlier errors.

    Int32 error;
    NativeConverter::Decode(mConverterHandle, mInput, mInEnd, mOutput, mOutEnd, mData, TRUE, &error);
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
    SetPosition(outInput);
    ImplReset();
    return NOERROR;
}

ECode CharsetDecoderICU::DecodeLoop(
    /* [in] */ IByteBuffer* byteBuffer,
    /* [in] */ ICharBuffer* charBuffer,
    /* [out] */ ICoderResult** result)
{
    ECode ec = NOERROR;
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    VALIDATE_NOT_NULL(byteBuffer)

    Boolean ret = FALSE;
    if (!(IBuffer::Probe(byteBuffer)->HasRemaining(&ret), ret)) {
        CCoderResult::GetUNDERFLOW(result);
        return ec;
    }

    Int32 num = 0;
    GetArray(byteBuffer, &num);
    (*mData)[INPUT_OFFSET] = num;
    GetArray(charBuffer, &num);
    (*mData)[OUTPUT_OFFSET] = num;
    //try {
    Int32 error;
    NativeConverter::Decode(mConverterHandle, mInput, mInEnd, mOutput, mOutEnd, mData, FALSE, &error);
    if (U_FAILURE(UErrorCode(error))) {
        if (error == U_BUFFER_OVERFLOW_ERROR) {
            CCoderResult::GetOVERFLOW(result);
            goto EXIT;
        } else if (error == U_INVALID_CHAR_FOUND) {
            CCoderResult::UnmappableForLength((*mData)[INVALID_BYTES], result);
            goto EXIT;
        } else if (error == U_ILLEGAL_CHAR_FOUND) {
            CCoderResult::MalformedForLength((*mData)[INVALID_BYTES], result);
            goto EXIT;
        } else {
            ec = E_ASSERTION_ERROR;
            goto EXIT;
        }
    }
    // Decoding succeeded: give us more data.
    CCoderResult::GetUNDERFLOW(result);
    // } finally {
    //     setPosition(in);
    //     setPosition(out);
    // }
EXIT:
    SetPosition(byteBuffer);
    SetPosition(charBuffer);
    return ec;
}

ECode CharsetDecoderICU::UpdateCallback()
{
    return NativeConverter::SetCallbackDecode(mConverterHandle, this);
}

ECode CharsetDecoderICU::GetArray(
    /* [in] */ ICharBuffer* outBuffer,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    VALIDATE_NOT_NULL(outBuffer)

    Boolean has = FALSE;
    if ((IBuffer::Probe(outBuffer)->HasArray(&has), has)) {
        mOutput = NULL;
        outBuffer->GetArray((ArrayOf<Char32>**)&mOutput);
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
            mAllocatedOutput = ArrayOf<Char32>::Alloc(mOutEnd);
        }
        // The array's start position is 0.
        mOutput = mAllocatedOutput;
        *result = 0;
        return NOERROR;
    }
}

ECode CharsetDecoderICU::GetArray(
    /* [in] */ IByteBuffer* inBuffer,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    VALIDATE_NOT_NULL(inBuffer)

    Boolean has = FALSE;
    if ((IBuffer::Probe(inBuffer)->HasArray(&has), has)) {
        mInput = NULL;
        inBuffer->GetArray((ArrayOf<Byte>**)&mInput);
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
            mAllocatedInput = ArrayOf<Byte>::Alloc(mInEnd);
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

ECode CharsetDecoderICU::SetPosition(
    /* [in] */ ICharBuffer* outBuffer)
{
    VALIDATE_NOT_NULL(outBuffer)
    Boolean has = FALSE;
    if ((IBuffer::Probe(outBuffer)->HasArray(&has), has)) {
        Int32 pos = 0;
        IBuffer::Probe(outBuffer)->GetPosition(&pos);
        Int32 offset = 0;
        IBuffer::Probe(outBuffer)->GetArrayOffset(&offset);
        IBuffer::Probe(outBuffer)->SetPosition(pos + (*mData)[OUTPUT_OFFSET] - offset);
    }
    else {
        outBuffer->Put(mOutput, 0, (*mData)[OUTPUT_OFFSET]);
    }
    // release reference to output array, which may not be ours
    mOutput = NULL;
    return NOERROR;
}

ECode CharsetDecoderICU::SetPosition(
    /* [in] */ IByteBuffer* inBuffer)
{
    VALIDATE_NOT_NULL(inBuffer)
    Int32 pos = 0;
    IBuffer::Probe(inBuffer)->GetPosition(&pos);
    IBuffer::Probe(inBuffer)->SetPosition(pos + (*mData)[INPUT_OFFSET]);
    // release reference to input array, which may not be ours
    mInput = NULL;
    return NOERROR;
}

} // namespace Charset
} // namespace IO
} // namespace Elastos
