
#include "StringBufferInputStream.h"
#include "AutoLock.h"
#include "Arrays.h"

using Elastos::Utility::Arrays;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(StringBufferInputStream, InputStream, IStringBufferInputStream)

StringBufferInputStream::StringBufferInputStream()
    : mCount(0)
    , mPos(0)
{}

StringBufferInputStream::~StringBufferInputStream()
{}

ECode StringBufferInputStream::constructor(
    /* [in] */ const String& str)
{
    if (str.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mBuffer = str;
    mCount = mBuffer.GetLength();
    return NOERROR;
}

ECode StringBufferInputStream::Available(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)

    AutoLock lock(this);

    *number = mCount - mPos;

    return NOERROR;
}

ECode StringBufferInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    AutoLock lock(this);

    *value = mPos < mCount ? mBuffer.GetChar(mPos++) & 0xFF : -1;
    return NOERROR;
}

ECode StringBufferInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;
    VALIDATE_NOT_NULL(buffer)

    AutoLock lock(this);

    if (NULL == buffer) {
//      throw new NullPointerException("buffer == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    FAIL_RETURN(Arrays::CheckOffsetAndCount(buffer->GetLength(), byteOffset, byteCount));
    if (0 == byteCount) {
        *number = 0;
        return NOERROR;
    }

    Int32 copylen = mCount - mPos < byteCount ? mCount - mPos : byteCount;
    for (Int32 i = 0; i < copylen; ++i) {
        (*buffer)[byteOffset + i] = (Byte)mBuffer.GetChar(mPos + i);
    }
    mPos += copylen;
    *number = copylen;
    return NOERROR;
}

ECode StringBufferInputStream::Reset()
{
    AutoLock lock(this);
    mPos = 0;
    return NOERROR;
}

ECode StringBufferInputStream::Skip(
    /* [in] */ Int64 count,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;

    AutoLock lock(this);

    if (count <= 0) {
        return NOERROR;
    }

    Int32 numskipped;
    if (mCount - mPos < count) {
        numskipped = mCount - mPos;
        mPos = mCount;
    }
    else {
        numskipped = (Int32)count;
        mPos += count;
    }
    *number = numskipped;

    return NOERROR;
}

} // namespace IO
} // namespace Elastos
