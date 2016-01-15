
#include "elastos/droid/content/pm/CLimitedLengthInputStream.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL(CLimitedLengthInputStream, FilterInputStream, ILimitedLengthInputStream)

CAR_OBJECT_IMPL(CLimitedLengthInputStream)

CLimitedLengthInputStream::CLimitedLengthInputStream()
    : mEnd(0)
    , mOffset(0)
{}

CLimitedLengthInputStream::~CLimitedLengthInputStream()
{}

ECode CLimitedLengthInputStream::constructor(
    /* [in] */ IInputStream* inStream,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length)
{
    FAIL_RETURN(FilterInputStream::constructor(inStream))

    if (inStream == NULL) {
        Logger::E(String("CLimitedLengthInputStream"), String("in == null"));
        return E_IO_EXCEPTION;
    }

    if (offset < 0) {
        Logger::E(String("CLimitedLengthInputStream"), String("offset < 0"));
        return E_IO_EXCEPTION;
    }

    if (length < 0) {
        Logger::E(String("CLimitedLengthInputStream"), String("length < 0"));
        return E_IO_EXCEPTION;
    }

    if (length > MAX_VALUE - offset) {
        Logger::E(String("CLimitedLengthInputStream"), String("offset + length > Long.MAX_VALUE"));
        return E_IO_EXCEPTION;
    }

    mEnd = offset + length;

    Int64 outSkip;
    Skip(offset, &outSkip);
    mOffset = offset;

    return NOERROR;
}

ECode CLimitedLengthInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    if (mOffset >= mEnd) {
        *value = -1;
        return NOERROR;
    }

    mOffset++;
    return FilterInputStream::Read(value);
}

ECode CLimitedLengthInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;
    VALIDATE_NOT_NULL(buffer)

    return Read(buffer, 0, buffer->GetLength(), number);
}

ECode CLimitedLengthInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)

    if (mOffset >= mEnd) {
        *number = -1;
        return NOERROR;
    }

    const Int32 arrayLength = buffer->GetLength();
    // Arrays.checkOffsetAndCount(arrayLength, offset, byteCount);
    if ((offset | byteCount) < 0 || offset > arrayLength || arrayLength - offset < byteCount) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (mOffset > MAX_VALUE - byteCount) {
        return E_IO_EXCEPTION;
    }

    if (mOffset + byteCount > mEnd) {
        byteCount = (Int32) (mEnd - mOffset);
    }

    Int32 numRead;
    FilterInputStream::Read(buffer, offset, byteCount, &numRead);
    mOffset += numRead;

    *number = numRead;
    return NOERROR;
}

}
}
}
}

