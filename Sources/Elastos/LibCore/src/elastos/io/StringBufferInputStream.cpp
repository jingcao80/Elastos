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
