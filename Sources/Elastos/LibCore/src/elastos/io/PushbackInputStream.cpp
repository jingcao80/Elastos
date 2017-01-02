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

#include "PushbackInputStream.h"
#include "Character.h"

namespace Elastos{
namespace IO{

CAR_INTERFACE_IMPL(PushbackInputStream, FilterInputStream, IPushbackInputStream)

PushbackInputStream::PushbackInputStream()
    : mPos(0)
{
}

PushbackInputStream::~PushbackInputStream()
{
}

ECode PushbackInputStream::constructor(
    /* [in] */ IInputStream* in)
{
    FAIL_RETURN(FilterInputStream::constructor(in));
    if(in != NULL) {
        mBuf = ArrayOf<Byte>::Alloc(1);
        if (mBuf == NULL)
            return E_OUT_OF_MEMORY_ERROR;
    }
    mPos = 1;

    return NOERROR;
}

ECode PushbackInputStream::constructor(
    /* [in] */ IInputStream* in,
    /* [in] */ Int32 size)
{
    FAIL_RETURN(FilterInputStream::constructor(in));
    if (size <= 0) {
//      throw new IllegalArgumentException("size <= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if(in != NULL) {
        mBuf = ArrayOf<Byte>::Alloc(size);
        if (mBuf == NULL)
            return E_OUT_OF_MEMORY_ERROR;
    }

    mPos = size;

    return NOERROR;
}

ECode PushbackInputStream::Available(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;

    if (mBuf == NULL) {
//      throw new IOException();
        return E_IO_EXCEPTION;
    }

    Int32 al;
    FAIL_RETURN(mIn->Available(&al));
    *number = mBuf->GetLength() - mPos + al;

    return NOERROR;
}

ECode PushbackInputStream::Close()
{
    if (mIn != NULL) {
        ICloseable* c = ICloseable::Probe(mIn);
        FAIL_RETURN(c->Close());
        mIn = NULL;
        mBuf = NULL;
    }

    return NOERROR;
}

ECode PushbackInputStream::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);

    *supported = FALSE;

    return NOERROR;
}

ECode PushbackInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    if (mBuf == NULL) {
//      throw new IOException();
        return E_IO_EXCEPTION;
    }
    // Is there a pushback byte available?
    if (mPos < mBuf->GetLength()) {
        *value = (*mBuf)[mPos++] & 0xFF;
        return NOERROR;
    }
    // Assume read() in the InputStream will return low-order byte or -1
    // if end of stream.
    return mIn->Read(value);
}

ECode PushbackInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = -1;
    VALIDATE_NOT_NULL(buffer);

    if (mBuf == NULL) {
//      throw streamClosed();
        return E_IO_EXCEPTION;
    }
    // Force buffer null check first!
    if (byteOffset > buffer->GetLength() || byteOffset < 0) {
//        throw new ArrayIndexOutOfBoundsException("Offset out of bounds: " + byteOffset);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (byteCount < 0 || byteCount > buffer->GetLength() - byteOffset) {
//      throw new ArrayIndexOutOfBoundsException("Length out of bounds: " + byteCount);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 copiedBytes = 0, copyLength = 0, newOffset = byteOffset;
    // Are there pushback bytes available?
    if (mPos < mBuf->GetLength()) {
        copyLength = (mBuf->GetLength() - mPos >= byteCount) ? byteCount : mBuf->GetLength()
                - mPos;
        memcpy(buffer->GetPayload() + newOffset, mBuf->GetPayload() + mPos, copyLength);
        newOffset += copyLength;
        copiedBytes += copyLength;
        // Use up the bytes in the local buffer
        mPos += copyLength;
    }
    // Have we copied enough?
    if (copyLength == byteCount) {
        *number = byteCount;
        return NOERROR;
    }
    Int32 inCopied;
    FAIL_RETURN(mIn->Read(buffer, newOffset, byteCount - copiedBytes, &inCopied));
    if (inCopied > 0) {
        *number = inCopied + copiedBytes;
        return NOERROR;
    }
    if (copiedBytes == 0) {
        *number = inCopied;
        return NOERROR;
    }
    *number = copiedBytes;
    return NOERROR;
}

ECode PushbackInputStream::Skip(
    /* [in] */ Int64 count,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;

    if (mIn == NULL) {
//      throw streamClosed();
        return E_IO_EXCEPTION;
    }
    if (count <= 0) {
        *number = 0;
        return NOERROR;
    }
    Int32 numSkipped = 0;
    if (mPos < mBuf->GetLength()) {
        numSkipped += (count < mBuf->GetLength() - mPos) ? count : mBuf->GetLength() - mPos;
        mPos += numSkipped;
    }
    if (numSkipped < count) {
        Int64 num;
        FAIL_RETURN(mIn->Skip(count - numSkipped, &num));
        numSkipped += num;
    }
    *number = numSkipped;
    return NOERROR;
}

ECode PushbackInputStream::Unread(
    /* [in] */ Int32 oneByte)
{
    if (mBuf == NULL) {
//      throw new IOException();
        return E_IO_EXCEPTION;
    }
    if (mPos == 0) {
//      throw new IOException("Pushback buffer full");
        return E_IO_EXCEPTION;
    }

    (*mBuf)[--mPos] = (Byte)oneByte;
    return NOERROR;
}

ECode PushbackInputStream::Unread(
    /* [in] */ ArrayOf<Byte>* buffer)
{
    VALIDATE_NOT_NULL(buffer);
    return Unread(buffer, 0, buffer->GetLength());
}

ECode PushbackInputStream::Unread(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    VALIDATE_NOT_NULL(buffer);
    if (length > mPos) {
//      throw new IOException("Pushback buffer full");
        return E_IO_EXCEPTION;
    }
    if (offset > buffer->GetLength() || offset < 0) {
//      throw new ArrayIndexOutOfBoundsException("Offset out of bounds: " + offset);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (length < 0 || length > buffer->GetLength() - offset) {
//      throw new ArrayIndexOutOfBoundsException("Length out of bounds: " + length);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (mBuf == NULL) {
//      throw streamClosed();
        return E_IO_EXCEPTION;
    }

    memcpy(mBuf->GetPayload() + mPos - length, buffer->GetPayload() + offset, length);
    mPos = mPos - length;

    return NOERROR;
}

ECode PushbackInputStream::Mark(
    /* [in] */ Int32 readLimit)
{
    return NOERROR;
}

ECode PushbackInputStream::Reset()
{
    return E_IO_EXCEPTION;
}

} // namespace IO
} // namespace Elastos
