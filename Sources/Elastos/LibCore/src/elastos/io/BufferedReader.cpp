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

#include "BufferedReader.h"
#include "StringBuilder.h"
#include "AutoLock.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(BufferedReader, Reader, IBufferedReader)

BufferedReader::BufferedReader()
    : mPos(0)
    , mEnd(0)
    , mMark(-1)
    , mMarkLimit(-1)
    , mLastWasCR(FALSE)
    , mMarkedLastWasCR(FALSE)
{
}

BufferedReader::~BufferedReader()
{
}

ECode BufferedReader::constructor(
    /* [in] */ IReader* rin)
{
    return constructor(rin, 8192);
}

ECode BufferedReader::constructor(
    /* [in] */ IReader* rin,
    /* [in] */ Int32 size)
{
    VALIDATE_NOT_NULL(rin);

    FAIL_RETURN(Reader::constructor(ISynchronize::Probe(rin)))

    if (size <= 0) {
//      throw new IllegalArgumentException("size <= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mIn = rin;
    mBuf = ArrayOf<Char32>::Alloc(size);
    return NOERROR;
}

ECode BufferedReader::Close()
{
    AutoLock lock(mLock);

    if (!IsClosed()) {
        FAIL_RETURN(ICloseable::Probe(mIn)->Close());
        mBuf = NULL;
    }

    return NOERROR;
}

ECode BufferedReader::FillBuf(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;

    // assert(pos == end);

    if (mMark == -1 || (mPos - mMark >= mMarkLimit)) {
        /* mark isn't set or has exceeded its limit. use the whole buffer */
        Int32 result;
        FAIL_RETURN(mIn->Read(mBuf, 0, mBuf->GetLength(), &result));
        if (result > 0) {
            mMark = -1;
            mPos = 0;
            mEnd = result;
        }
        *number = result;
        return NOERROR;
    }

    if (mMark == 0 && mMarkLimit > mBuf->GetLength()) {
        /* the only way to make room when mark=0 is by growing the buffer */
        Int32 newLength = mBuf->GetLength() * 2;
        if (newLength > mMarkLimit) {
            newLength = mMarkLimit;
        }
        AutoPtr< ArrayOf<Char32> > newbuf = ArrayOf<Char32>::Alloc(newLength);
        newbuf->Copy(mBuf);
        mBuf = newbuf;
    }
    else if (mMark > 0) {
        /* make room by shifting the buffered data to left mark positions */
        mBuf->Copy(mBuf, mMark, mBuf->GetLength() - mMark);
        mPos -= mMark;
        mEnd -= mMark;
        mMark = 0;
    }

    /* Set the new position and mark position */
    Int32 count;
    FAIL_RETURN(mIn->Read(mBuf, mPos, mBuf->GetLength() - mPos, &count));
    if (count != -1) {
        mEnd += count;
    }
    *number = count;
    return NOERROR;
}

Boolean BufferedReader::IsClosed()
{
    return mBuf == NULL;
}

ECode BufferedReader::Mark(
    /* [in] */ Int32 markLimit)
{
    if (markLimit < 0) {
//      throw new IllegalArgumentException("markLimit < 0:" + markLimit);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());
    mMarkLimit = markLimit;
    mMark = mPos;
    mMarkedLastWasCR = mLastWasCR;

    return NOERROR;
}

ECode BufferedReader::CheckNotClosed()
{
    if (IsClosed()) {
//      throw new IOException("BufferedReader is closed");
        return E_IO_EXCEPTION;
    }

    return NOERROR;
}

ECode BufferedReader::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported)

    *supported = TRUE;
    return NOERROR;
}

ECode BufferedReader::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());
    ReadChar(value);
    if(mLastWasCR && '\n' == *value) {
        ReadChar(value);
    }
    mLastWasCR = FALSE;
    return NOERROR;
}

ECode BufferedReader::ReadChar(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    Int32 number = 0;

    if (mPos < mEnd || (FillBuf(&number), -1 != number)) {
        *value = (*mBuf)[mPos++];
        return NOERROR;
    }

    *value = -1;
    return NOERROR;
}

ECode BufferedReader::Read(
    /* [in] */ ArrayOf<Char32>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;
    VALIDATE_NOT_NULL(buffer)

    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());
    Int32 arrayLength = buffer->GetLength();
    if ((offset | length) < 0 || offset > arrayLength || arrayLength - offset < length) {
        // throw new ArrayIndexOutOfBoundsException(arrayLength, offset,
        //         count);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (0 == length){
        *number = 0;
        return NOERROR;
    }

    MaybeSwallowLF();

    Int32 outstanding = length;
    while (outstanding > 0) {
        // If there are chars in the buffer, grab those first.
        Int32 available = mEnd - mPos;
        if (available > 0) {
            Int32 count = available >= outstanding ? outstanding : available;
            buffer->Copy(offset, mBuf, mPos, count);
            mPos += count;
            offset += count;
            outstanding -= count;
        }

        /*
         * Before attempting to read from the underlying stream, make
         * sure we really, really want to. We won't bother if we're
         * done, or if we've already got some chars and reading from the
         * underlying stream would block.
         */
        Boolean ready;
        if (outstanding == 0 || (outstanding < length && (mIn->IsReady(&ready), !ready))) {
            break;
        }

        // assert(pos == end);

        /*
         * If we're unmarked and the requested size is greater than our
         * buffer, read the chars directly into the caller's buffer. We
         * don't read into smaller buffers because that could result in
         * a many reads.
         */
        if ((mMark == -1 || (mPos - mMark >= mMarkLimit)) && outstanding >= mBuf->GetLength()) {
            Int32 count;
            FAIL_RETURN(mIn->Read(buffer, offset, outstanding, &count));
            if (count > 0) {
                outstanding -= count;
                mMark = -1;
            }
            break; // assume the source stream gave us all that it could
        }

        Int32 value;
        FAIL_RETURN(FillBuf(&value));
        if (value == -1) {
            break; // source is exhausted
        }
    }

    Int32 count = length - outstanding;
    if (count > 0){
        *number = count;
        return NOERROR;
    }
    *number = -1;
    return NOERROR;
}

ECode BufferedReader::ChompNewline()
{
    Int32 number = 0;
    if ((mPos != mEnd || (FillBuf(&number), number != -1)) && (*mBuf)[mPos] == '\n') {
        ++mPos;
    }
    return NOERROR;
}

ECode BufferedReader::MaybeSwallowLF()
{
    if (mLastWasCR) {
        ChompNewline();
        mLastWasCR = false;
    }
    return NOERROR;
}

ECode BufferedReader::ReadLine(
    /* [out] */ String* contents)
{
    VALIDATE_NOT_NULL(contents)
    *contents = String(NULL);

    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());

    MaybeSwallowLF();

    // Do we have a whole line in the buffer?
    for (Int32 i = mPos; i < mEnd; ++i) {
        Char32 ch = (*mBuf)[i];
        if ('\n' == ch || '\r' == ch) {
            *contents = String(*mBuf, mPos, i - mPos);
            mPos = i + 1;
            mLastWasCR = ('\r' == ch);
            return NOERROR;
        }
    }

    // Accumulate buffers in a StringBuilder until we've read a whole line.
    AutoPtr<StringBuilder> result = new StringBuilder(mEnd - mPos + 80);

    result->Append(*mBuf, mPos, mEnd - mPos);
    while (TRUE) {
        mPos = mEnd;
        Int32 number;
        if (FillBuf(&number), -1 == number) {
            // If there's no more input, return what we've read so far, if anything.
            *contents = result->GetLength() > 0 ? result->ToString() : String(NULL);
            return NOERROR;
        }

        // Do we have a whole line in the buffer now?
        for (Int32 i = mPos; i < mEnd; ++i) {
            Char32 ch = (*mBuf)[i];
            if ('\n' == ch || '\r' == ch) {
                result->Append(*mBuf, mPos, i - mPos);
                mPos = i + 1;
                mLastWasCR = ('\r' == ch);
                *contents = result->ToString();
                return NOERROR;
            }
        }

        // Add this whole buffer to the line-in-progress and try again...
        result->Append(*mBuf, mPos, mEnd - mPos);
    }
    return NOERROR;
}

ECode BufferedReader::IsReady(
    /* [out] */ Boolean* ready)
{
    VALIDATE_NOT_NULL(ready)
    *ready = FALSE;

    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());
    Boolean isReady;
    *ready = ((mEnd - mPos) > 0) || (mIn->IsReady(&isReady), isReady);
    return NOERROR;
}

ECode BufferedReader::Reset()
{
    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());
    if (mMark == -1) {
//      throw new IOException("Invalid mark");
        return E_IO_EXCEPTION;
    }
    mPos = mMark;
    mLastWasCR = mMarkedLastWasCR;
    return NOERROR;
}

ECode BufferedReader::Skip(
    /* [in] */ Int64 charCount,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;

    if (charCount < 0) {
//      throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());

    if (mEnd - mPos >= charCount) {
        mPos += charCount;
        *number = charCount;
        return NOERROR;
    }
    Int64 read = mEnd - mPos;
    mPos = mEnd;
    while (read < charCount) {
        Int32 num;
        if (FillBuf(&num), -1 == num) {
            *number = read;
            return NOERROR;
        }
        if (mEnd - mPos >= charCount - read) {
            mPos += charCount - read;
            *number = charCount;
            return NOERROR;
        }
        // Couldn't get all the characters, skip what we read
        read += (mEnd - mPos);
        mPos = mEnd;
    }
    *number = charCount;
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
