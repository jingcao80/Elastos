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

#include "ByteArrayInputStream.h"
#include "AutoLock.h"
#include "Arrays.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(ByteArrayInputStream, InputStream, IByteArrayInputStream)

ByteArrayInputStream::ByteArrayInputStream()
    : mPos(0)
    , mMark(0)
    , mCount(0)
{
}

ByteArrayInputStream::~ByteArrayInputStream()
{
}

ECode ByteArrayInputStream::constructor(
    /* [in] */ ArrayOf<Byte>* buffer)
{
    VALIDATE_NOT_NULL(buffer);

    mMark = 0;
    mBuf = buffer;
    mCount = mBuf->GetLength();

    return NOERROR;
}

ECode ByteArrayInputStream::constructor(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    VALIDATE_NOT_NULL(buffer);

    mBuf = buffer;
    mPos = offset;
    mMark = offset;
    mCount = offset + length > mBuf->GetLength() ?
            mBuf->GetLength() : offset + length;
    return NOERROR;
}

ECode ByteArrayInputStream::Available(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)

    AutoLock lock(this);

    *number = mCount - mPos;
    return NOERROR;
}

ECode ByteArrayInputStream::Close()
{
    return NOERROR;
}

CARAPI ByteArrayInputStream::Mark(
    /* [in] */ Int32 readLimit)
{
    AutoLock lock(this);

    mMark = mPos;
    return NOERROR;
}

CARAPI ByteArrayInputStream::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported)

    *supported = TRUE;
    return NOERROR;
}

CARAPI ByteArrayInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    AutoLock lock(this);

    *value = mPos < mCount ? (*mBuf)[mPos++] & 0xFF : -1;
    return NOERROR;
}

CARAPI ByteArrayInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(buffer)
    VALIDATE_NOT_NULL(number)

    {    AutoLock syncLock(this);
        FAIL_RETURN(Arrays::CheckOffsetAndCount(buffer->GetLength(), byteOffset, byteCount));

        // Are there any bytes available?
        if (mPos >= mCount) {
            *number = -1;
            return NOERROR;
        }
        if (0 == byteCount) {
            *number = 0;
            return NOERROR;
        }

        Int32 copylen = mCount - mPos < byteCount ? mCount - mPos : byteCount;
        buffer->Copy(byteOffset, mBuf, mPos, copylen);
        mPos += copylen;
        *number = copylen;
    }
    return NOERROR;
}

CARAPI ByteArrayInputStream::Reset()
{
    AutoLock lock(this);

    mPos = mMark;
    return NOERROR;
}

CARAPI ByteArrayInputStream::Skip(
    /* [in] */ Int64 count,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;

    if (count <= 0) {
        return NOERROR;
    }

    AutoLock lock(this);

    Int32 temp = mPos;
    mPos = mCount - mPos < count ? mCount : (Int32)(mPos + count);
    *number = mPos - temp;
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
