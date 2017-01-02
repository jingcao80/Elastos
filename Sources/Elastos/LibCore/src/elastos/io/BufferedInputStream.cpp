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

#include "BufferedInputStream.h"
#include <elastos/core/AutoLock.h>
#include <cutils/log.h>

using Elastos::Core::AutoLock;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(BufferedInputStream, FilterInputStream, IBufferedInputStream)

BufferedInputStream::BufferedInputStream()
    : mCount(0)
    , mMarklimit(0)
    , mMarkpos(-1)
    , mPos(0)
{}

BufferedInputStream::~BufferedInputStream()
{}

ECode BufferedInputStream::constructor(
    /* [in] */ IInputStream* in)
{
    return constructor(in, IBufferedInputStream::DEFAULT_BUFFER_SIZE);
}

ECode BufferedInputStream::constructor(
    /* [in] */ IInputStream* in,
    /* [in] */ Int32 size)
{
    FAIL_RETURN(FilterInputStream::constructor(in));
    if (size <= 0) {
        ALOGE("BufferedInputStream::constructor: IllegalArgumentException, size <= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mBuf = ArrayOf<Byte>::Alloc(size);
    if (NULL == mBuf) return E_OUT_OF_MEMORY_ERROR;
    return NOERROR;
}

ECode BufferedInputStream::Available(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;

    {    AutoLock syncLock(this);
        AutoPtr<IInputStream> localIn = mIn; // 'in' could be invalidated by close()
        if (NULL == mBuf || NULL == localIn) {
            return StreamClosed();
        }
        Int32 count;
        FAIL_RETURN(localIn->Available(&count));
        *number = mCount - mPos + count;
    }
    return NOERROR;
}

ECode BufferedInputStream::StreamClosed()
{
    ALOGE("BufferedInputStream::StreamClosed: IOException, BufferedInputStream is closed");
    return E_IO_EXCEPTION;
}

ECode BufferedInputStream::Close()
{
    if (mBuf != NULL) {
        mBuf = NULL;
    }
    AutoPtr<IInputStream> localIn = mIn;
    mIn = NULL;
    if (localIn != NULL) {
        return ICloseable::Probe(localIn)->Close();
    }
    return NOERROR;
}

ECode BufferedInputStream::Fillbuf(
    /* [in] */ IInputStream* localIn,
    /* [in, out] */ ArrayOf<Byte>** localBuf,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;
    VALIDATE_NOT_NULL(localBuf)

    if (mMarkpos == -1 || (mPos - mMarkpos >= mMarklimit)) {
        /* Mark position not set or exceeded readlimit */
        Int32 result;
        FAIL_RETURN(localIn->Read(*localBuf, &result));
        if (result > 0) {
            mMarkpos = -1;
            mPos = 0;
            mCount = -1 == result ? 0 : result;
        }
        *number = result;
        return NOERROR;
    }

    Int32 length = (*localBuf)->GetLength();
    if (0 == mMarkpos && mMarklimit > length) {
        /* Increase buffer size to accommodate the readlimit */
        Int32 newLength = length * 2;
        if (newLength > mMarklimit) {
            newLength = mMarklimit;
        }
        AutoPtr<ArrayOf<Byte> > newbuf = ArrayOf<Byte>::Alloc(newLength);
        newbuf->Copy(*localBuf, length);

        // Reassign buf, which will invalidate any local references
        // FIXME: what if buf was null?
        mBuf = newbuf;
        REFCOUNT_RELEASE(*localBuf)
        *localBuf = newbuf;
        REFCOUNT_ADD(*localBuf)
    }
    else if (mMarkpos > 0) {
        memmove((*localBuf)->GetPayload(),
            (*localBuf)->GetPayload() + mMarkpos, length - mMarkpos);
    }

    /* Set the new position and mark position */
    mPos -= mMarkpos;
    mCount = mMarkpos = 0;
    Int32 bytesread;
    FAIL_RETURN(localIn->Read(
        *localBuf, mPos, (*localBuf)->GetLength() - mPos, &bytesread));
    mCount = bytesread <= 0 ? mPos : mPos + bytesread;
    *number = bytesread;
    return NOERROR;
}

ECode BufferedInputStream::Mark(
    /* [in] */ Int32 readLimit)
{
    {    AutoLock syncLock(this);
        mMarklimit = readLimit;
        mMarkpos = mPos;
    }
    return NOERROR;
}

ECode BufferedInputStream::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported)
    *supported = TRUE;
    return NOERROR;
}

ECode BufferedInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = -1;

    {    AutoLock syncLock(this);

        // Use local refs since buf and in may be invalidated by an
        // unsynchronized close()
        AutoPtr<ArrayOf<Byte> > localBuf = mBuf;
        AutoPtr<IInputStream> localIn = mIn;
        if (NULL == localBuf || NULL == localIn) {
            return StreamClosed();
        }

        /* Are there buffered bytes available? */
        if (mPos >= mCount) {
            Int32 number;
            FAIL_RETURN(Fillbuf(localIn, (ArrayOf<Byte>**)&localBuf, &number));
            if (-1 == number) {
                *value = -1;
                return NOERROR; /* no, fill buffer */
            }
        }

        // localBuf may have been invalidated by fillbuf
        if (localBuf != mBuf) {
            localBuf = mBuf;
            if (NULL == localBuf) {
                return StreamClosed();
            }
        }

        /* Did filling the buffer fail with -1 (EOF)? */
        if (mCount - mPos > 0) {
            *value = (*localBuf)[mPos++] & 0xFF;
            return NOERROR;
        }
        *value = -1;
    }
    return NOERROR;
}

ECode BufferedInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;

    {    AutoLock syncLock(this);
        if (NULL == buffer) {
            // throw new NullPointerException("buffer == null");
            return E_NULL_POINTER_EXCEPTION;
        }

        // Use local ref since buf may be invalidated by an unsynchronized
        // close()
        AutoPtr<ArrayOf<Byte> > localBuf = mBuf;
        if (NULL == localBuf) {
            return StreamClosed();
        }

        if ((byteOffset | byteCount) < 0 || byteOffset > buffer->GetLength()
                || buffer->GetLength() - byteOffset < byteCount) {
            // throw new IndexOutOfBoundsException();
            return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }

        if (0 == byteCount) {
            return NOERROR;
        }

        AutoPtr<IInputStream> localIn = mIn;
        if (NULL == localIn) {
            return StreamClosed();
        }

        Int32 required, available;
        if (mPos < mCount) {
            /* There are bytes available in the buffer. */
            Int32 copylength = mCount - mPos >= byteCount ? byteCount : mCount - mPos;
            buffer->Copy(byteOffset, localBuf, mPos, copylength);

            mPos += copylength;
            if (copylength == byteCount) {
                *number = copylength;
                return NOERROR;
            }
            FAIL_RETURN(localIn->Available(&available));
            if (0 == available) {
                *number = copylength;
                return NOERROR;
            }
            byteOffset += copylength;
            required = byteCount - copylength;
        } else {
            required = byteCount;
        }

        while (TRUE) {
            Int32 read;
            /*
             * If we're not marked and the required size is greater than the
             * buffer, simply read the bytes directly bypassing the buffer.
             */
            if (-1 == mMarkpos && required >= localBuf->GetLength()) {
                FAIL_RETURN(localIn->Read(buffer, byteOffset, required, &read));
                if (-1 == read) {
                    *number = required == byteCount ? -1 : byteCount - required;
                    return NOERROR;
                }
            } else {
                FAIL_RETURN(Fillbuf(localIn, (ArrayOf<Byte>**)&localBuf, &read));

                if (-1 == read) {
                    *number = required == byteCount ? -1 : byteCount - required;
                    return NOERROR;
                }
                // localBuf may have been invalidated by fillbuf
                if (localBuf != mBuf) {
                    localBuf = mBuf;
                    if (NULL == localBuf) {
                        return StreamClosed();
                    }
                }

                read = mCount - mPos >= required ? required : mCount - mPos;
                for (Int32 i = 0; i < read; i++) {
                    (*buffer)[byteOffset + i] = (*localBuf)[mPos + i];
                }
                mPos += read;
            }
            required -= read;
            if (0 == required) {
                *number = byteCount;
                return NOERROR;
            }
            FAIL_RETURN(localIn->Available(&available));
            if (0 == available) {
                *number = byteCount - required;
                return NOERROR;
            }
            byteOffset += read;
        }
    }

    return NOERROR;
}

ECode BufferedInputStream::Reset()
{
    {    AutoLock syncLock(this);
        // BEGIN android-changed
        /*
         * These exceptions get thrown in some "normalish" circumstances,
         * so it is preferable to avoid loading up the whole big set of
         * messages just for these cases.
         */
        if (NULL == mBuf) {
    //        throw new IOException("Stream is closed");
            return E_IO_EXCEPTION;
        }
        if (-1 == mMarkpos) {
    //        throw new IOException("Mark has been invalidated.");
            return E_IO_EXCEPTION;
        }
        // END android-changed
        mPos = mMarkpos;
    }
    return NOERROR;
}

ECode BufferedInputStream::Skip(
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;

    {    AutoLock syncLock(this);
        // Use local refs since buf and in may be invalidated by an
        // unsynchronized close()
        AutoPtr<ArrayOf<Byte> > localBuf = mBuf;
        AutoPtr<IInputStream> localIn = mIn;
        if (NULL == localBuf) {
            return StreamClosed();
        }
        if (byteCount < 1) {
            *number = 0;
            return NOERROR;
        }
        if (NULL == localIn) {
            return StreamClosed();
        }

        if (mCount - mPos >= byteCount) {
            mPos += byteCount;
            *number = byteCount;
            return NOERROR;
        }
        Int64 read = mCount - mPos;
        mPos = mCount;

        if (mMarkpos != -1) {
            if (byteCount <= mMarklimit) {
                Int32 fillnum;
                FAIL_RETURN(Fillbuf(localIn, (ArrayOf<Byte>**)&localBuf, &fillnum));
                if (-1 == fillnum) {
                    *number = read;
                    return NOERROR;
                }
                if (mCount - mPos >= byteCount - read) {
                    mPos += byteCount - read;
                    *number = byteCount;
                    return NOERROR;
                }
                // Couldn't get all the bytes, skip what we read
                read += (mCount - mPos);
                mPos = mCount;
                *number = read;
                return NOERROR;
            }
        }
        FAIL_RETURN(localIn->Skip(byteCount - read, number));
        *number = read + *number;
    }
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
