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

#include "PushbackReader.h"
#include "Character.h"
#include "AutoLock.h"

namespace Elastos{
namespace IO{

CAR_INTERFACE_IMPL(PushbackReader, FilterReader, IPushbackReader)

PushbackReader::PushbackReader()
    : mPos(0)
{
}

PushbackReader::~PushbackReader()
{
}

ECode PushbackReader::constructor(
    /* [in] */ IReader* in)
{
    FAIL_RETURN(FilterReader::constructor(in));
    mBuf = ArrayOf<Char32>::Alloc(1);
    if (mBuf == NULL)
        return E_OUT_OF_MEMORY_ERROR;
    mPos = 1;

    return NOERROR;
}

ECode PushbackReader::constructor(
    /* [in] */ IReader* in,
    /* [in] */ Int32 size)
{
    FAIL_RETURN(FilterReader::constructor(in));
    if (size <= 0) {
//      throw new IllegalArgumentException("size <= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mBuf = ArrayOf<Char32>::Alloc(size);
    mPos = size;

    return NOERROR;
}

ECode PushbackReader::Close()
{
    assert(mLock != NULL);
    AutoLock lock(mLock);

    mBuf = NULL;
    return ICloseable::Probe(mIn)->Close();
}

ECode PushbackReader::Mark(
    /* [in] */ Int32 readLimit)
{
    return E_IO_EXCEPTION;
}

ECode PushbackReader::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported)

    *supported = FALSE;

    return NOERROR;
}

ECode PushbackReader::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    assert(mLock != NULL);
    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());
    /* Is there a pushback character available? */
    if (mPos < mBuf->GetLength()) {
        *value = (*mBuf)[mPos++];
        return NOERROR;
    }
    /**
     * Assume read() in the InputStream will return 2 lowest-order bytes
     * or -1 if end of stream.
     */
    return mIn->Read(value);
}

ECode PushbackReader::CheckNotClosed()
{
    if(mBuf == NULL)
        return E_IO_EXCEPTION;
    return NOERROR;
}

ECode PushbackReader::Read(
    /* [out] */ ArrayOf<Char32>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;
    VALIDATE_NOT_NULL(buffer)
    assert(mLock != NULL);
    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());
    // avoid int overflow
    // BEGIN android-changed
    // Exception priorities (in case of multiple errors) differ from
    // RI, but are spec-compliant.
    // made implicit null check explicit, used (offset | count) < 0
    // instead of (offset < 0) || (count < 0) to safe one operation
    if (buffer == NULL) {
//      throw new NullPointerException("buffer == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if ((offset | count) < 0 || offset > buffer->GetLength() - count) {
//      throw new IndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    // END android-changed

    Int32 copiedChars = 0;
    Int32 copyLength = 0;
    Int32 newOffset = offset;
    /* Are there pushback chars available? */
    if (mPos < mBuf->GetLength()) {
        copyLength = (mBuf->GetLength() - mPos >= count) ? count : mBuf->GetLength()
                - mPos;
        buffer->Copy(newOffset, mBuf, mPos, copyLength);

        newOffset += copyLength;
        copiedChars += copyLength;
        /* Use up the chars in the local buffer */
        mPos += copyLength;
    }
    /* Have we copied enough? */
    if (copyLength == count) {
        *number = count;
        return NOERROR;
    }
    Int32 inCopied;
    FAIL_RETURN(mIn->Read(buffer, newOffset, count - copiedChars, &inCopied));
    if (inCopied > 0) {
        *number = inCopied + copiedChars;
        return NOERROR;
    }
    if (copiedChars == 0) {
        *number = inCopied;
        return NOERROR;
    }
    *number = copiedChars;

    return copiedChars;
}

ECode PushbackReader::IsReady(
    /* [out] */ Boolean* ready)
{
    VALIDATE_NOT_NULL(ready)
    *ready = FALSE;
    assert(mLock != NULL);
    AutoLock lock(mLock);

    if (mBuf == NULL) {
//      throw new IOException("Reader is closed");
        return E_IO_EXCEPTION;
    }
    FAIL_RETURN(mIn->IsReady(ready));
    *ready = (mBuf->GetLength() - mPos > 0 || *ready);

    return NOERROR;
}

ECode PushbackReader::Reset()
{
    return E_IO_EXCEPTION;
}

ECode PushbackReader::Unread(
   /* [in] */ Int32 oneChar)
{
    assert(mLock != NULL);
    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());
    if (mPos == 0) {
//      throw new IOException("Pushback buffer full");
        return E_IO_EXCEPTION;
    }
    (*mBuf)[--mPos] = oneChar;

    return NOERROR;
}

ECode PushbackReader::Unread(
    /* [in] */ ArrayOf<Char32>* buffer)
{
    return Unread(buffer, 0, buffer->GetLength());
}

ECode PushbackReader::Unread(
    /* [in] */ ArrayOf<Char32>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    VALIDATE_NOT_NULL(buffer)
    assert(mLock != NULL);
    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());
    if (length > mPos) {
//      throw new IOException("Pushback buffer full");
        return E_IO_EXCEPTION;
    }
    // Force buffer null check first!
    if (offset > buffer->GetLength() - length || offset < 0) {
//      throw new ArrayIndexOutOfBoundsException("Offset out of bounds: " + offset);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (length < 0) {
//      throw new ArrayIndexOutOfBoundsException("Length out of bounds: " + length);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    for (Int32 i = offset + length - 1; i >= offset; i--) {
        FAIL_RETURN(Unread((*buffer)[i]));
    }

    return NOERROR;
}

ECode PushbackReader::Skip(
    /* [in] */ Int64 count,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;

    if (count < 0) {
//      throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());
    if (count == 0) {
        *number = 0;
        return NOERROR;
    }
    Int64 inSkipped;
    Int32 availableFromBuffer = mBuf->GetLength() - mPos;
    if (availableFromBuffer > 0) {
        Int64 requiredFromIr = count - availableFromBuffer;
        if (requiredFromIr <= 0) {
            mPos += count;
            *number = count;
            return NOERROR;
        }
        mPos += availableFromBuffer;
        FAIL_RETURN(mIn->Skip(requiredFromIr, &inSkipped));
    } else {
        FAIL_RETURN(mIn->Skip(count, &inSkipped));
    }
    *number = inSkipped + availableFromBuffer;

    return NOERROR;
}

} // namespace IO
} // namespace Elastos
