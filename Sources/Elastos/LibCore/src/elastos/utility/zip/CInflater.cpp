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

#include "Elastos.CoreLibrary.IO.h"
#include "CInflater.h"
#include "Math.h"
#include "Arrays.h"
#include "CCloseGuardHelper.h"
#include "AutoLock.h"
#include <unistd.h>
#include <errno.h>

using Elastos::Core::Math;
using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;

namespace Elastos {
namespace Utility {
namespace Zip {

CAR_INTERFACE_IMPL(CInflater, Object, IInflater)

CAR_OBJECT_IMPL(CInflater)

CInflater::CInflater()
    : mInLength(0)
    , mInRead(0)
    , mFinished(FALSE)
    , mNeedsDictionary(FALSE)
    , mStreamHandle(NULL)
{
    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mGuard);
}

CInflater::~CInflater()
{
//    try {
    if (mGuard != NULL) {
        mGuard->WarnIfOpen();
    }
    End();
//    } finally {
//        try {
//            super.finalize();
//        } catch (Throwable t) {
//            throw new AssertionError(t);
//        }
//    }
}

ECode CInflater::CreateStream(
    /* [in] */ Boolean noHeader)
{
    AutoPtr<NativeZipStream> stream = new NativeZipStream;
    if (stream == NULL) {
//        jniThrowOutOfMemoryError(env, NULL);
        mStreamHandle = NULL;
        return E_OUT_OF_MEMORY_ERROR;
    }
    stream->mStream.adler = 1;

    /*
     * See zlib.h for documentation of the inflateInit2 windowBits parameter.
     *
     * zconf.h says the "requirements for inflate are (in bytes) 1 << windowBits
     * that is, 32K for windowBits=15 (default value) plus a few kilobytes
     * for small objects." This means that we can happily use the default
     * here without worrying about memory consumption.
     */
    Int32 err = inflateInit2(&stream->mStream, noHeader ? -DEF_WBITS : DEF_WBITS);
    if (err != Z_OK) {
//        throwExceptionForZlibError(env, "java/lang/IllegalArgumentException", err);
        mStreamHandle = NULL;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mStreamHandle = stream;
    return NOERROR;
}

ECode CInflater::End()
{
    AutoLock locK(this);

    mGuard->Close();
    if (mStreamHandle != NULL) {
        EndImpl(mStreamHandle);
        mInRead = 0;
        mInLength = 0;
        mStreamHandle = NULL;
    }

    return NOERROR;
}

void CInflater::EndImpl(
    /* [in] */ NativeZipStream* stream)
{
    inflateEnd(&stream->mStream);
}

ECode CInflater::Finished(
    /* [out] */ Boolean* finished)
{
    VALIDATE_NOT_NULL(finished);

    AutoLock locK(this);

    *finished = mFinished;
    return NOERROR;
}

ECode CInflater::GetAdler(
    /* [out] */ Int32* checksum)
{
    VALIDATE_NOT_NULL(checksum);

    AutoLock locK(this);

    FAIL_RETURN(CheckOpen());
    *checksum = GetAdlerImpl(mStreamHandle);

    return NOERROR;
}

Int32 CInflater::GetAdlerImpl(
    /* [in] */ NativeZipStream* stream)
{
    return stream->mStream.adler;
}

ECode CInflater::GetBytesRead(
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number);

    AutoLock locK(this);

    FAIL_RETURN(CheckOpen());
    *number = GetTotalInImpl(mStreamHandle);

    return NOERROR;
}

ECode CInflater::GetBytesWritten(
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number);

    AutoLock locK(this);

    FAIL_RETURN(CheckOpen());
    *number = GetTotalOutImpl(mStreamHandle);
    return NOERROR;
}

ECode CInflater::GetRemaining(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);

    AutoLock locK(this);

    *number = mInLength - mInRead;

    return NOERROR;
}

ECode CInflater::GetCurrentOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);

    AutoLock locK(this);
    *offset = mInRead;
    return NOERROR;
}

ECode CInflater::GetTotalIn(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);

    AutoLock locK(this);

    FAIL_RETURN(CheckOpen());
    *number = (Int32)Elastos::Core::Math::Min(
            GetTotalInImpl(mStreamHandle), (Int64)Elastos::Core::Math::INT32_MAX_VALUE);
    return NOERROR;
}

Int64 CInflater::GetTotalInImpl(
    /* [in] */ NativeZipStream* stream)
{
    return stream->mStream.total_in;
}

ECode CInflater::GetTotalOut(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);

    AutoLock locK(this);

    FAIL_RETURN(CheckOpen());
    *number = (Int32)Elastos::Core::Math::Min(
            GetTotalOutImpl(mStreamHandle), (Int64)Elastos::Core::Math::INT32_MAX_VALUE);
    return NOERROR;
}

Int64 CInflater::GetTotalOutImpl(
    /* [in] */ NativeZipStream* stream)
{
    return stream->mStream.total_out;
}

ECode CInflater::Inflate(
    /* [out] */ ArrayOf<Byte>* buf,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(buf);
    VALIDATE_NOT_NULL(number);

    return Inflate(buf, 0, buf->GetLength(), number);
}

ECode CInflater::Inflate(
    /* [out] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = -1;
    VALIDATE_NOT_NULL(buf);

    AutoLock locK(this);

    FAIL_RETURN(Arrays::CheckOffsetAndCount(buf->GetLength(), offset, byteCount))

    FAIL_RETURN(CheckOpen());

    Boolean need;
    NeedsInput(&need);
    if (need) {
        *number = 0;
        return NOERROR;
    }

    Boolean neededDict = mNeedsDictionary;
    mNeedsDictionary = FALSE;
    Int32 result;
    FAIL_RETURN(InflateImpl(offset, byteCount, buf, mStreamHandle, &result));
    if (mNeedsDictionary && neededDict) {
//        throw new DataFormatException("Needs dictionary");
        return E_DATA_FORMAT_EXCEPTION;
    }

    *number = result;
    return NOERROR;
}

ECode CInflater::InflateImpl(
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount,
    /* [out] */ ArrayOf<Byte>* buf,
    /* [in] */ NativeZipStream* stream,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;
    if (buf == NULL) return NOERROR;

    stream->mStream.next_out = reinterpret_cast<Bytef*>(buf->GetPayload() + offset);
    stream->mStream.avail_out = byteCount;

    Bytef* initialNextIn = stream->mStream.next_in;
    Bytef* initialNextOut = stream->mStream.next_out;

    Int32 err = inflate(&stream->mStream, Z_SYNC_FLUSH);
    if (err != Z_OK) {
        if (err == Z_STREAM_ERROR) {
            *result =0;
            return NOERROR;
        }
        if (err == Z_STREAM_END) {
            mFinished = TRUE;
        }
        else if (err == Z_NEED_DICT) {
            mNeedsDictionary = TRUE;
        }
        else {
            // PFL_EX(" DataFormatException: err: %d", err);
//            throwExceptionForZlibError(env, "java/util/zip/DataFormatException", err);
            *result = -1;
            return E_DATA_FORMAT_EXCEPTION;
        }
    }

    Int32 bytesRead = stream->mStream.next_in - initialNextIn;
    Int32 bytesWritten = stream->mStream.next_out - initialNextOut;

    mInRead += bytesRead;
    *result = bytesWritten;
    return NOERROR;
}

ECode CInflater::NeedsDictionary(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock locK(this);

    *result = mNeedsDictionary;
    return NOERROR;
}

ECode CInflater::NeedsInput(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock locK(this);

    *result = mInRead == mInLength;
    return NOERROR;
}

ECode CInflater::Reset()
{
    AutoLock locK(this);

    FAIL_RETURN(CheckOpen());
    mFinished = FALSE;
    mNeedsDictionary = FALSE;
    mInLength = mInRead = 0;
    return ResetImpl(mStreamHandle);
}

ECode CInflater::ResetImpl(
    /* [in] */ NativeZipStream* stream)
{
    Int32 err = inflateReset(&stream->mStream);
    if (err != Z_OK) {
//        throwExceptionForZlibError(env, "java/lang/IllegalArgumentException", err);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CInflater::SetDictionary(
    /* [in] */ ArrayOf<Byte>* buf)
{
    VALIDATE_NOT_NULL(buf)
    return SetDictionary(buf, 0, buf->GetLength());
}

ECode CInflater::SetDictionary(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount)
{
    VALIDATE_NOT_NULL(buf)
    AutoLock locK(this);

    FAIL_RETURN(CheckOpen());
    FAIL_RETURN(Arrays::CheckOffsetAndCount(buf->GetLength(), offset, byteCount))
    SetDictionaryImpl(buf, offset, byteCount, mStreamHandle);
    return NOERROR;
}

void CInflater::SetDictionaryImpl(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount,
    /* [in] */ NativeZipStream* stream)
{
    stream->SetDictionary(buf, offset, byteCount, TRUE);
}

ECode CInflater::SetInput(
    /* [in] */ ArrayOf<Byte>* buf)
{
    VALIDATE_NOT_NULL(buf)
    return SetInput(buf, 0, buf->GetLength());
}

ECode CInflater::SetInput(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount)
{
    VALIDATE_NOT_NULL(buf)
    AutoLock locK(this);

    FAIL_RETURN(CheckOpen());

    FAIL_RETURN(Arrays::CheckOffsetAndCount(buf->GetLength(), offset, byteCount))

    mInRead = 0;
    mInLength = byteCount;
    SetInputImpl(buf, offset, byteCount, mStreamHandle);
    return NOERROR;
}

Int32 CInflater::SetFileInput(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int32 byteCount)
{
    AutoLock locK(this);

    FAIL_RETURN(CheckOpen());
    mInRead = 0;
    mInLength = SetFileInputImpl(fd, offset, byteCount, mStreamHandle);
    return mInLength;
}

void CInflater::SetInputImpl(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount,
    /* [in] */ NativeZipStream* stream)
{
    stream->SetInput(buf, offset, byteCount);
}

Int32 CInflater::SetFileInputImpl(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int32 byteCount,
    /* [in] */ NativeZipStream* stream)
{
    // We reuse the existing native buffer if it's large enough.
    // TODO: benchmark.
    if (stream->mInCap < byteCount) {
        stream->SetInput(NULL, 0, byteCount);
    }
    else {
        stream->mStream.next_in = reinterpret_cast<Bytef*>(stream->mInput->GetPayload());
        stream->mStream.avail_in = byteCount;
    }

    // As an Android-specific optimization, we read directly onto the native heap.
    // The original code used Java to read onto the Java heap and then called setInput(byte[]).
    // TODO: benchmark.
    Int32 _fd;
    fd->GetDescriptor(&_fd);
    Int32 rc = TEMP_FAILURE_RETRY(lseek(_fd, offset, SEEK_SET));
    if (rc == -1) {
//        jniThrowIOException(env, errno);
        return 0;
    }
    Int32 totalByteCount = 0;
    Bytef* dst = reinterpret_cast<Bytef*>(stream->mInput->GetPayload());
    ssize_t len;
    while ((len = TEMP_FAILURE_RETRY(read(_fd, dst, byteCount))) > 0) {
        dst += len;
        byteCount -= len;
        totalByteCount += len;
    }
    if (len == -1) {
//        jniThrowIOException(env, errno);
        return 0;
    }
    return totalByteCount;
}

/**
 * This constructor creates an inflater that expects a header from the input
 * stream. Use {@code Inflater(boolean)} if the input comes without a ZLIB
 * header.
 */
ECode CInflater::constructor()
{
    return constructor(FALSE);
}

/**
 * This constructor allows to create an inflater that expects no header from
 * the input stream.
 *
 * @param noHeader
 *            {@code true} indicates that no ZLIB header comes with the
 *            input.
 */
ECode CInflater::constructor(
    /* [in] */ Boolean onHeader)
{
    return CreateStream(onHeader);
}

ECode CInflater::CheckOpen()
{
    if (mStreamHandle == NULL) {
        return E_ILLEGAL_STATE_EXCEPTION;
//        throw new IllegalStateException("attempt to use Inflater after calling end");
    }
    return NOERROR;
}


} // namespace Zip
} // namespace Utility
} // namespace Elastos
