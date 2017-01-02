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

#include "CharArrayReader.h"
#include "Character.h"
#include "AutoLock.h"

using Elastos::Core::Character;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(CharArrayReader, Reader, ICharArrayReader)

CharArrayReader::CharArrayReader()
    : mPos(0)
    , mMarkedPos(-1)
    , mCount(0)
{
}

CharArrayReader::~CharArrayReader()
{
}

ECode CharArrayReader::constructor(
    /* [in] */ ArrayOf<Char32>* buf)
{
    VALIDATE_NOT_NULL(buf);
    FAIL_RETURN(Reader::constructor());

    mBuf = buf;
    mCount = mBuf->GetLength();

    return NOERROR;
}

ECode CharArrayReader::constructor(
    /* [in] */ ArrayOf<Char32>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    VALIDATE_NOT_NULL(buf);

    /*
     * The spec of this constructor is broken. In defining the legal values
     * of offset and length, it doesn't consider buffer's length. And to be
     * compatible with the broken spec, we must also test whether
     * (offset + length) overflows.
     */
    if (offset < 0 || offset > buf->GetLength() || length < 0 || offset + length < 0) {
//      throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(Reader::constructor());

    mBuf = buf;
    mPos = offset;
    mMarkedPos = offset;

    /* This is according to spec */
    Int32 bufferLength = mBuf->GetLength();

    //??? android code
    //this.count = offset + length < bufferLength ? length : bufferLength;

    mCount = offset + length < bufferLength ? offset + length : bufferLength;

    return NOERROR;
}

ECode CharArrayReader::Close()
{
    AutoLock lock(mLock);

    if (IsOpen()) {
        mBuf = NULL;
    }
    return NOERROR;
}

Boolean CharArrayReader::IsOpen()
{
    return mBuf != NULL;
}

Boolean CharArrayReader::IsClosed()
{
    return mBuf == NULL;
}

ECode CharArrayReader::Mark(
    /* [in] */ Int32 readLimit)
{
    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());
    mMarkedPos = mPos;
    return NOERROR;
}

ECode CharArrayReader::CheckNotClosed()
{
    if (IsClosed()) {
//      throw new IOException("CharArrayReader is closed");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode CharArrayReader::IsMarkSupported(
    /*[out]*/ Boolean* IsMarkSupported)
{
    VALIDATE_NOT_NULL(IsMarkSupported)

    *IsMarkSupported = TRUE;
    return NOERROR;
}

ECode CharArrayReader::Read(
    /*[out]*/ Int32* character)
{
    VALIDATE_NOT_NULL(character)
    *character = -1;

    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());

    if (mPos != mCount) {
        *character = (*mBuf)[mPos++];
    }

    return NOERROR;
}

ECode CharArrayReader::Read(
    /* [in] */ ArrayOf<Char32>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;
    VALIDATE_NOT_NULL(buffer)

    // BEGIN android-note
    // changed array notation to be consistent with the rest of harmony
    // END android-note
    if (offset < 0 || offset > buffer->GetLength()) {
//      throw new ArrayIndexOutOfBoundsException("Offset out of bounds: " + offset);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (count < 0 || count > buffer->GetLength() - offset) {
//      throw new ArrayIndexOutOfBoundsException("Length out of bounds: " + len);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());
    if (mPos < mCount) {
        Int32 bytesRead = mPos + count > mCount ? mCount - mPos : count;
        buffer->Copy(offset, mBuf, mPos, bytesRead);
        mPos += bytesRead;
        *number = bytesRead;

        return NOERROR;
    }
    *number = -1;
    return NOERROR;
}

ECode CharArrayReader::IsReady(
    /*[out]*/ Boolean* isReady)
{
    VALIDATE_NOT_NULL(isReady)
    *isReady = FALSE;

    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());
    *isReady = mPos != mCount;

    return NOERROR;
}

ECode CharArrayReader::Reset()
{
    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());
    mPos = mMarkedPos != -1 ? mMarkedPos : 0;

    return NOERROR;
}

ECode CharArrayReader::Skip(
    /* [in] */Int64 n,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;

    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());
    if (n <= 0) {
        return NOERROR;
    }

    if (n < mCount - mPos) {
        mPos = mPos + (Int32)n;
        *number = n;
    }
    else {
        *number = mCount - mPos;
        mPos = mCount;
    }

    return NOERROR;
}

} // namespace IO
} // namespace Elastos
