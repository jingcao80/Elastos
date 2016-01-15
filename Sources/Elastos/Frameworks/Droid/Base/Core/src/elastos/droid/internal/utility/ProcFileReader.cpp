
#include "elastos/droid/internal/utility/ProcFileReader.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(ProcFileReader, Object, IProcFileReader)

ProcFileReader::ProcFileReader()
    : mTail(0)
    , mLineFinished(FALSE)
{}

ECode ProcFileReader::constructor(
    /* [in] */ IInputStream* stream)
{
    return constructor(stream, 4096);
}

ECode ProcFileReader::constructor(
    /* [in] */ IInputStream* stream,
    /* [in] */ Int32 bufferSize)
{
    mStream = stream;
    mBuffer = ArrayOf<Byte>::Alloc(bufferSize);

    // read enough to answer hasMoreData
    Int32 num;
    return FillBuf(&num);
}

ECode ProcFileReader::FillBuf(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    Int32 length = mBuffer->GetLength() - mTail;
    if (length == 0) {
        Logger::E("ProcFileReader", "attempting to fill already-full buffer");
        return E_IO_EXCEPTION;
    }

    Int32 read;
    FAIL_RETURN(mStream->Read(mBuffer, mTail, length, &read))
    if (read != -1) {
        mTail += read;
    }
    *num = read;
    return NOERROR;
}

ECode ProcFileReader::ConsumeBuf(
    /* [in] */ Int32 count)
{
    // TODO: consider moving to read pointer, but for now traceview says
    // these copies aren't a bottleneck.
    mBuffer->Copy(mBuffer, count, mTail - count);
    mTail -= count;
    if (mTail == 0) {
        Int32 num;
        FAIL_RETURN(FillBuf(&num))
    }
    return NOERROR;
}

ECode ProcFileReader::NextTokenIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    if (mLineFinished) {
        *index = -1;
        return NOERROR;
    }

    Int32 i = 0, num;
    do {
        // scan forward for token boundary
        for (; i < mTail; i++) {
            Byte b = (*mBuffer)[i];
            if (b == '\n') {
                mLineFinished = TRUE;
                *index = i;
                return NOERROR;
            }
            if (b == ' ') {
                *index = i;
                return NOERROR;
            }
        }
        FAIL_RETURN(FillBuf(&num))
    } while (num > 0);

    Logger::E("ProcFileReader", "End of stream while looking for token boundary");
    return E_PROTOCOL_EXCEPTION;
}


ECode ProcFileReader::HasMoreData(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTail > 0;
    return NOERROR;
}

ECode ProcFileReader::FinishLine()
{
    // last token already finished line; reset silently
    if (mLineFinished) {
        mLineFinished = FALSE;
        return NOERROR;;
    }

    Int32 i = 0;
    Int32 num;
    do {
        // scan forward for line boundary and consume
        for (; i < mTail; i++) {
            if ((*mBuffer)[i] == '\n') {
                return ConsumeBuf(i + 1);
            }
        }
        FAIL_RETURN(FillBuf(&num))
    } while (num > 0);

    Logger::E("ProcFileReader", "End of stream while looking for line boundary");
    return E_PROTOCOL_EXCEPTION;
}

ECode ProcFileReader::NextString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    Int32 tokenIndex;
    FAIL_RETURN(NextTokenIndex(&tokenIndex))
    if (tokenIndex == -1) {
        Logger::E("ProcFileReader", "Missing required string");
        return E_PROTOCOL_EXCEPTION;
    }
    else {
        return ParseAndConsumeString(tokenIndex, str);
    }
}

ECode ProcFileReader::NextInt64(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 tokenIndex;
    FAIL_RETURN(NextTokenIndex(&tokenIndex))
    if (tokenIndex == -1) {
        Logger::E("ProcFileReader", "Missing required Int64");
        return E_PROTOCOL_EXCEPTION;
    }
    else {
        return ParseAndConsumeInt64(tokenIndex, result);
    }
}

ECode ProcFileReader::NextOptionalInt64(
    /* [in] */ Int64 def,
    /* [out] */ Int64* result)
{
    Int32 tokenIndex;
    FAIL_RETURN(NextTokenIndex(&tokenIndex))
    if (tokenIndex == -1) {
        *result = def;
        return NOERROR;
    }
    else {
        return ParseAndConsumeInt64(tokenIndex, result);
    }
}

ECode ProcFileReader::ParseAndConsumeString(
    /* [in] */ Int32 tokenIndex,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String s(*mBuffer, 0, tokenIndex);//StandardCharsets.US_ASCII
    FAIL_RETURN(ConsumeBuf(tokenIndex + 1))
    *result = s;
    return NOERROR;
}

ECode ProcFileReader::ParseAndConsumeInt64(
    /* [in] */ Int32 tokenIndex,
    /* [out] */ Int64* res)
{
    VALIDATE_NOT_NULL(res)
    Boolean negative = (*mBuffer)[0] == '-';

    // TODO: refactor into something like IntegralToString
    Int64 result = 0;
    for (Int32 i = negative ? 1 : 0; i < tokenIndex; i++) {
        Int32 digit = (*mBuffer)[i] - '0';
        if (digit < 0 || digit > 9) {
            return InvalidInt64(tokenIndex);
        }

        // always parse as negative number and apply sign later; this
        // correctly handles MIN_VALUE which is "larger" than MAX_VALUE.
        Int64 next = result * 10 - digit;
        if (next > result) {
            return InvalidInt64(tokenIndex);
        }
        result = next;
    }

    FAIL_RETURN(ConsumeBuf(tokenIndex + 1))
    *res = negative ? result : -result;
    return NOERROR;
}

ECode ProcFileReader::InvalidInt64(
    /* [in] */ Int32 tokenIndex)
{
    Logger::E("ProcFileReader", "invalid Int64 %s", String(*mBuffer, 0, tokenIndex).string());
    return E_NUMBER_FORMAT_EXCEPTION;
}

ECode ProcFileReader::NextInt32(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 value;
    FAIL_RETURN(NextInt64(&value))
    if (value > Elastos::Core::Math::INT32_MAX_VALUE || value < Elastos::Core::Math::INT32_MIN_VALUE) {
        Logger::E("ProcFileReader", "parsed value larger than integer");
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    *result = (Int32)value;
    return NOERROR;
}

ECode ProcFileReader::Close()
{
    return ICloseable::Probe(mStream)->Close();
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
