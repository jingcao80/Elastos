
#include "CDeflater.h"
#include "Arrays.h"
#include "CCloseGuardHelper.h"
#include "AutoLock.h"

using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;

namespace Elastos {
namespace Utility {
namespace Zip {

const Int32 CDeflater::FINISH;

CAR_INTERFACE_IMPL(CDeflater, Object, IDeflater)

CAR_OBJECT_IMPL(CDeflater)

CDeflater::CDeflater()
    : mFlushParm(IDeflater::NO_FLUSH)
    , mFinished(FALSE)
    , mCompressLevel(IDeflater::DEFAULT_COMPRESSION)
    , mStrategy(IDeflater::DEFAULT_STRATEGY)
    , mStreamHandle(NULL)
    , mInRead(0)
    , mInLength(0)
{}

CDeflater::~CDeflater()
{
//    try {
    if (mGuard != NULL) {
        mGuard->WarnIfOpen();
    }

    AutoLock locK(this);
    {
        End(); // to allow overriding classes to clean up
        EndImplLocked(); // in case those classes don't call super.end()
    }
//    } finally {
//        try {
//            super.finalize();
//        } catch (Throwable t) {
//            throw new AssertionError(t);
//        }
//    }
}

ECode CDeflater::constructor()
{
    return constructor(IDeflater::DEFAULT_COMPRESSION, FALSE);
}

ECode CDeflater::constructor(
    /* [in] */ Int32 level)
{
    return constructor(level, FALSE);
}

ECode CDeflater::constructor(
    /* [in] */ Int32 level,
    /* [in] */ Boolean noHeader)
{
    if (level < IDeflater::DEFAULT_COMPRESSION
        || level > IDeflater::BEST_COMPRESSION) {
//        throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mGuard);

    mCompressLevel = level;
    CreateStream(mCompressLevel, mStrategy, noHeader, (NativeZipStream**)&mStreamHandle);
    mGuard->Open(String("end"));
    return NOERROR;
}

ECode CDeflater::Deflate(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = -1;
    VALIDATE_NOT_NULL(buf);

    return Deflate(buf, 0, buf->GetLength(), number);
}

ECode CDeflater::Deflate(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = -1;
    VALIDATE_NOT_NULL(buf);

    AutoLock locK(this);

    return DeflateImplLocked(offset, byteCount, mFlushParm, buf, number);
}

ECode CDeflater::Deflate(
    /* [out] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int32 flush,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = -1;
    VALIDATE_NOT_NULL(buf);

    AutoLock locK(this);

    if (flush != NO_FLUSH && flush != SYNC_FLUSH && flush != FULL_FLUSH) {
//        throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return DeflateImplLocked(offset, byteCount, flush, buf, number);
}

ECode CDeflater::DeflateImplLocked(
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int32 flush,
    /* [in] */ ArrayOf<Byte>* buf,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;
    FAIL_RETURN(CheckOpen());

    FAIL_RETURN(Arrays::CheckOffsetAndCount(buf->GetLength(), offset, byteCount))

    if (mInputBuffer == NULL) {
        AutoPtr<ArrayOf<Byte> > emptyByteArray = ArrayOf<Byte>::Alloc(0);
        SetInput(emptyByteArray);
    }
    *number = DeflateImplLocked(offset, byteCount, flush, mStreamHandle, buf);
    return NOERROR;
}

Int32 CDeflater::DeflateImplLocked(
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int32 flushParm,
    /* [in] */ NativeZipStream* stream,
    /* [in] */ ArrayOf<Byte>* buf)
{
    // /* We need to get the number of bytes already read */
    // Int32 inBytes = mInRead;

    // stream->mStream.avail_out = byteCount;
    // Int32 sin = stream->mStream.total_in;
    // Int32 sout = stream->mStream.total_out;
    if (buf == NULL) {
        return -1;
    }
    stream->mStream.next_out = reinterpret_cast<Bytef*>(buf->GetPayload() + offset);
    stream->mStream.avail_out = byteCount;

    Bytef* initialNextIn = stream->mStream.next_in;
    Bytef* initialNextOut = stream->mStream.next_out;

    Int32 err = deflate(&stream->mStream, flushParm);
    switch (err) {
    case Z_OK:
        break;
    case Z_STREAM_END:
        mFinished = TRUE;
        break;
    case Z_BUF_ERROR:
        // zlib reports this "if no progress is possible (for example avail_in or avail_out was
        // zero) ... Z_BUF_ERROR is not fatal, and deflate() can be called again with more
        // input and more output space to continue compressing".
        break;
    default:
//        throwExceptionForZlibError(env, "java/util/zip/DataFormatException", err);
        return -1;
    }

    Int32 bytesRead = (Int32)stream->mStream.next_in - (Int32)initialNextIn;
    Int32 bytesWritten = (Int32)stream->mStream.next_out - (Int32)initialNextOut;
    mInRead += bytesRead;
    return bytesWritten;
}

void CDeflater::EndImplLocked(
    /* [in] */ NativeZipStream* stream)
{
    deflateEnd(&stream->mStream);
}

ECode CDeflater::End()
{
    AutoLock locK(this);

    mGuard->Close();
    EndImplLocked();
    return NOERROR;
}

void CDeflater::EndImplLocked()
{
    if (mStreamHandle != NULL) {
        EndImplLocked(mStreamHandle);
        mInputBuffer = NULL;
        mStreamHandle = NULL;
    }
}

ECode CDeflater::Finish()
{
    AutoLock locK(this);

    mFlushParm = FINISH;
    return NOERROR;
}

ECode CDeflater::Finished(
    /* [out] */ Boolean* finished)
{
    VALIDATE_NOT_NULL(finished);

    AutoLock locK(this);
    *finished = mFinished;
    return NOERROR;
}

ECode CDeflater::GetAdler(
    /* [out] */ Int32* checksum)
{
    VALIDATE_NOT_NULL(checksum);

    AutoLock locK(this);

    FAIL_RETURN(CheckOpen());

    *checksum = GetAdlerImplLocked(mStreamHandle);
    return NOERROR;
}

Int32 CDeflater::GetAdlerImplLocked(
    /* [in] */ NativeZipStream* stream)
{
    return stream->mStream.adler;
}

ECode CDeflater::GetTotalIn(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);

    AutoLock locK(this);

    FAIL_RETURN(CheckOpen());

    *number = (Int32)GetTotalInImplLocked(mStreamHandle);
    return NOERROR;
}

Int64 CDeflater::GetTotalInImplLocked(
    /* [in] */ NativeZipStream* stream)
{
    return stream->mStream.total_in;
}

ECode CDeflater::GetTotalOut(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);

    AutoLock locK(this);

    FAIL_RETURN(CheckOpen());

    *number = (Int32)GetTotalOutImplLocked(mStreamHandle);
    return NOERROR;
}

Int64 CDeflater::GetTotalOutImplLocked(
    /* [in] */ NativeZipStream* stream)
{
    return stream->mStream.total_out;
}

ECode CDeflater::NeedsInput(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock locK(this);

    if (mInputBuffer == NULL) {
        *result = TRUE;
        return NOERROR;
    }
    *result = mInRead == mInLength;
    return NOERROR;
}

ECode CDeflater::Reset()
{
    AutoLock locK(this);

    FAIL_RETURN(CheckOpen());

    mFlushParm = NO_FLUSH;
    mFinished = FALSE;
    mInputBuffer = NULL;
    return ResetImplLocked(mStreamHandle);
}

ECode CDeflater::ResetImplLocked(
    /* [in] */ NativeZipStream* stream)
{
    Int32 err = deflateReset(&stream->mStream);
    if (err != Z_OK) {
//        throwExceptionForZlibError(env, "java/lang/IllegalArgumentException", err);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CDeflater::SetDictionary(
    /* [in] */ ArrayOf<Byte>* buf)
{
    VALIDATE_NOT_NULL(buf)
    return SetDictionary(buf, 0, buf->GetLength());
}

ECode CDeflater::SetDictionary(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount)
{
    VALIDATE_NOT_NULL(buf)
    AutoLock locK(this);

    FAIL_RETURN(CheckOpen());
    FAIL_RETURN(Arrays::CheckOffsetAndCount(buf->GetLength(), offset, byteCount))

    SetDictionaryImplLocked(buf, offset, byteCount, mStreamHandle);
    return NOERROR;
}

void CDeflater::SetDictionaryImplLocked(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount,
    /* [in] */ NativeZipStream* stream)
{
    stream->SetDictionary(buf, offset, byteCount, FALSE);
}

ECode CDeflater::SetInput(
    /* [in] */ ArrayOf<Byte>* buf)
{
    VALIDATE_NOT_NULL(buf)
    return SetInput(buf, 0, buf->GetLength());
}

ECode CDeflater::SetInput(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount)
{
    VALIDATE_NOT_NULL(buf)
    AutoLock locK(this);

    FAIL_RETURN(CheckOpen())
    FAIL_RETURN(Arrays::CheckOffsetAndCount(buf->GetLength(), offset, byteCount))

    mInLength = byteCount;
    mInRead = 0;
    if (mInputBuffer == NULL) {
        FAIL_RETURN(SetLevelsImplLocked(mCompressLevel, mStrategy, mStreamHandle));
    }
    mInputBuffer = buf->Clone();
    SetInputImplLocked(buf, offset, byteCount, mStreamHandle);
    return NOERROR;
}

ECode CDeflater::SetLevelsImplLocked(
    /* [in] */ Int32 level,
    /* [in] */ Int32 strategy,
    /* [in] */ NativeZipStream* stream)
{
    // The deflateParams documentation says that avail_out must never be 0 because it may be
    // necessary to flush, but the Java API ensures that we only get here if there's nothing
    // to flush. To be on the safe side, make sure that we're not pointing to a no longer valid
    // buffer.
    stream->mStream.next_out = reinterpret_cast<Bytef*>(NULL);
    stream->mStream.avail_out = 0;
    Int32 err = deflateParams(&stream->mStream, level, strategy);
    if (err != Z_OK) {
//        throwExceptionForZlibError(env, "java/lang/IllegalStateException", err);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return NOERROR;
}

void CDeflater::SetInputImplLocked(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount,
    /* [in] */ NativeZipStream* stream)
{
    stream->SetInput(buf, offset, byteCount);
}

ECode CDeflater::SetLevel(
    /* [in] */ Int32 level)
{
    AutoLock locK(this);

    if (level < DEFAULT_COMPRESSION ||
            level > BEST_COMPRESSION) {
//        throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mInputBuffer != NULL) {
//        throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mCompressLevel = level;
    return NOERROR;
}

ECode CDeflater::SetStrategy(
    /* [in] */ Int32 strategy)
{
    AutoLock locK(this);

    if (strategy < DEFAULT_STRATEGY ||
        strategy > HUFFMAN_ONLY) {
//        throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mInputBuffer != NULL) {
//        throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mStrategy = strategy;
    return NOERROR;
}

ECode CDeflater::GetBytesRead(
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number);

    AutoLock locK(this);

    FAIL_RETURN(CheckOpen());
    *number = GetTotalInImplLocked(mStreamHandle);
    return NOERROR;
}

ECode CDeflater::GetBytesWritten(
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number);

    AutoLock locK(this);

    FAIL_RETURN(CheckOpen());
    *number = GetTotalOutImplLocked(mStreamHandle);
    return NOERROR;
}

ECode CDeflater::CreateStream(
    /* [in] */ Int32 level,
    /* [in] */ Int32 strategy,
    /* [in] */ Boolean noHeader,
    /* [out] */ NativeZipStream** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<NativeZipStream> stream = new NativeZipStream();
    if (stream == NULL) {
        return E_OUT_OF_MEMORY_ERROR;
    }
    /*
     * See zlib.h for documentation of the deflateInit2 windowBits and memLevel parameters.
     *
     * zconf.h says the "requirements for deflate are (in bytes):
     *         (1 << (windowBits+2)) +  (1 << (memLevel+9))
     * that is: 128K for windowBits=15  +  128K for memLevel = 8  (default values)
     * plus a few kilobytes for small objects."
     */
    Int32 windowBits = noHeader ? -DEF_WBITS : DEF_WBITS;
    Int32 memLevel = DEF_MEM_LEVEL;

    Int32 err = deflateInit2(&stream->mStream, level, Z_DEFLATED, windowBits,
            memLevel, strategy);
    if (err != Z_OK) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throwExceptionForZlibError(env, "java/lang/IllegalArgumentException", err);
    }

    *result = stream;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDeflater::CheckOpen()
{
    if (mStreamHandle == NULL) {
        return E_ILLEGAL_STATE_EXCEPTION;
//        throw new IllegalStateException("attempt to use Deflater after calling end");
    }
    return NOERROR;
}

} // namespace Zip
} // namespace Utility
} // namespace Elastos
