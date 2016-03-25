
#include "LineNumberInputStream.h"
#include "Character.h"
#include "CStreams.h"

using Elastos::Core::Character;
using Libcore::IO::IStreams;
using Libcore::IO::CStreams;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(LineNumberInputStream, FilterInputStream, ILineNumberInputStream)

LineNumberInputStream::LineNumberInputStream()
    : mLineNumber(0)
    , mMarkedLineNumber(-1)
    , mLastChar(-1)
    , mMarkedLastChar(0)
{
}

LineNumberInputStream::~LineNumberInputStream()
{
}

ECode LineNumberInputStream::constructor(
    /* [in] */ IInputStream* in)
{
    return FilterInputStream::constructor(in);
}

ECode LineNumberInputStream::Available(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)

    FAIL_RETURN(mIn->Available(number));
    *number = *number / 2 + (mLastChar == -1 ? 0 : 1);

    return NOERROR;
}

ECode LineNumberInputStream::GetLineNumber(
    /* [out] */ Int32* lineNumber)
{
    VALIDATE_NOT_NULL(lineNumber)

    *lineNumber = mLineNumber;

    return NOERROR;
}

ECode LineNumberInputStream::Mark(
    /* [in] */ Int32 readLimit)
{
    FAIL_RETURN(mIn->Mark(readLimit));
    mMarkedLineNumber = mLineNumber;
    mMarkedLastChar = mLastChar;

    return NOERROR;
}

ECode LineNumberInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    Int32 currentChar = mLastChar;
    if (currentChar == -1) {
        FAIL_RETURN(mIn->Read(&currentChar));
    }
    else {
        mLastChar = -1;
    }

    switch (currentChar) {
        case '\r':
            currentChar = '\n';
            FAIL_RETURN(mIn->Read(&mLastChar));
            if (mLastChar == '\n') {
                mLastChar = -1;
            }
            // fall through
        case '\n':
            mLineNumber++;
    }
    *value = currentChar;

    return NOERROR;
}

ECode LineNumberInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(buffer)
    *number = 0;
    VALIDATE_NOT_NULL(number)

    if (byteOffset > buffer->GetLength() || byteOffset < 0) {
//      throw new ArrayIndexOutOfBoundsException("Offset out of bounds: " + offset);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (byteCount < 0 || byteCount > buffer->GetLength() - byteOffset) {
//      throw new ArrayIndexOutOfBoundsException("Length out of bounds: " + length);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    for (Int32 i = 0; i < byteCount; i++) {
        Int32 currentChar;
        ECode ec = Read(&currentChar);
        if(ec != NOERROR){
            if(i != 0)
                *number = i;
            return ec;
        }
        if (currentChar == -1) {
            *number = i == 0 ? -1 : i;
            return NOERROR;
        }
        (*buffer)[byteOffset + i] = (Byte)currentChar;
    }
    *number = byteCount;

    return NOERROR;
}

ECode LineNumberInputStream::Reset()
{
    FAIL_RETURN(mIn->Reset());
    mLineNumber = mMarkedLineNumber;
    mLastChar = mMarkedLastChar;

    return NOERROR;
}

ECode LineNumberInputStream::SetLineNumber(
    /* [in] */ Int32 lineNumber)
{
    mLineNumber = lineNumber;

    return NOERROR;
}

ECode LineNumberInputStream::Skip(
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* number)
{
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    return streams->SkipByReading(this, byteCount, number);
}

} // namespace IO
} // namespace Elastos
