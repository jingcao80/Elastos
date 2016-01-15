
#include "InflaterInputStream.h"
#include "CStreams.h"
#include "CZipFile.h"
#include "CInflater.h"
#include "Arrays.h"

using Elastos::IO::EIID_IInputStream;
using Libcore::IO::IStreams;
using Libcore::IO::CStreams;

namespace Elastos {
namespace Utility {
namespace Zip {

const Int32 InflaterInputStream::BUF_SIZE;

CAR_INTERFACE_IMPL(InflaterInputStream, FilterInputStream, IInflaterInputStream)

InflaterInputStream::InflaterInputStream()
    : mClosed(FALSE)
    , mEof(FALSE)
    , mNativeEndBufSize(0)
    , mLen(0)
{}

InflaterInputStream::~InflaterInputStream()
{
}

ECode InflaterInputStream::constructor(
    /* [in] */ IInputStream* is)
{
    AutoPtr<IInflater> inflater;
    CInflater::New((IInflater**)&inflater);
    return constructor(is, inflater, BUF_SIZE);
}

ECode InflaterInputStream::constructor(
    /* [in] */ IInputStream* is,
    /* [in] */ IInflater* inflater)
{
    return constructor(is, inflater, BUF_SIZE);
}

ECode InflaterInputStream::constructor(
    /* [in] */ IInputStream* is,
    /* [in] */ IInflater* inflater,
    /* [in] */ Int32 bsize)
{
    FAIL_RETURN(FilterInputStream::constructor(is))

    if (is == NULL || inflater == NULL) {
//        throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    if (bsize <= 0) {
//        throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mInf = inflater;

    if (is->Probe(EIID_IZipFileRAFStream)) {
        mNativeEndBufSize = bsize;
    }
    else {
        mBuf = ArrayOf<Byte>::Alloc(bsize);
    }

    return NOERROR;
}

ECode InflaterInputStream::Read(
    /* [out] */ Int32* value)
{
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    return streams->ReadSingleByte(THIS_PROBE(IInputStream), value);
}

ECode InflaterInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = -1;
    VALIDATE_NOT_NULL(buffer);

    FAIL_RETURN(CheckClosed());

    FAIL_RETURN(Arrays::CheckOffsetAndCount(buffer->GetLength(), byteOffset, byteCount))

    if (byteCount == 0) {
        *number = 0;
        return NOERROR;
    }

    if (mEof) {
        *number = -1;
        return NOERROR;
    }

    Boolean need;
    Int32 result;
    ECode ec = NOERROR;
    do {
        mInf->NeedsInput(&need);
        if (need) {
            FAIL_RETURN(Fill());
        }

        // Invariant: if reading returns -1 or throws, eof must be true.
        // It may also be true if the next read() should return -1.
        ec = mInf->Inflate(buffer, byteOffset, byteCount, &result);
        if (ec == (ECode)E_DATA_FORMAT_EXCEPTION) goto _EXIT_;

        ec = mInf->Finished(&mEof);
        if (ec == (ECode)E_DATA_FORMAT_EXCEPTION) goto _EXIT_;

        if (result > 0) {
            *number = result;
            return NOERROR;
        }
        else if (mEof) {
            *number = -1;
            return NOERROR;
        }
        else if (mInf->NeedsDictionary(&need), need) {
            mEof = TRUE;
            *number = -1;
            return NOERROR;
        }
        else if (mLen == -1) {
            mEof = TRUE;
//            throw new EOFException();
            return E_EOF_EXCEPTION;
            // If result == 0, fill() and try again
        }
    }
    while (TRUE);

    return NOERROR;
_EXIT_:
    mEof = TRUE;
    if (mLen == -1) {
//      throw new EOFException();
        return E_EOF_EXCEPTION;
    }
//      throw (IOException) (new IOException().initCause(e));
    return E_IO_EXCEPTION;
}

ECode InflaterInputStream::Fill()
{
    FAIL_RETURN(CheckClosed());

    if (mNativeEndBufSize > 0) {
        IZipFileRAFStream* is = IZipFileRAFStream::Probe(mIn);
        is->Fill(mInf, mNativeEndBufSize, &mLen);
    }
    else {
        mIn->Read(mBuf, &mLen);
        if (mLen > 0) {
            mInf->SetInput(mBuf, 0, mLen);
        }
    }
    return NOERROR;
}

ECode InflaterInputStream::Skip(
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number);
    *number = -1;

    if (byteCount < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("byteCount < 0");
    }

    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    IInputStream* is = THIS_PROBE(IInputStream);
    return streams->SkipByReading(is, byteCount, number);
}

ECode InflaterInputStream::Available(
    /* [out] */ Int32* number)
{
    FAIL_RETURN(CheckClosed());
    if (mEof) {
        *number = 0;
        return NOERROR;
    }
    *number = 1;
    return NOERROR;
}

ECode InflaterInputStream::Close()
{
    if (!mClosed) {
        mInf->End();
        mClosed = TRUE;
        mEof = TRUE;
        ICloseable::Probe(mIn)->Close();
    }
    return NOERROR;
}

ECode InflaterInputStream::Mark(
    /* [in] */ Int32 readlimit)
{
    // do nothing
    return NOERROR;
}

ECode InflaterInputStream::Reset()
{
//    throw new IOException();
    return E_IO_EXCEPTION;
}

ECode InflaterInputStream::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported)
    *supported = FALSE;
    return NOERROR;
}

ECode InflaterInputStream::CheckClosed()
{
    if (mClosed) {
//        throw new IOException("Stream is closed");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode InflaterInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;
    VALIDATE_NOT_NULL(buffer)
    return Read(buffer,0, buffer->GetLength(), number);
}

} // namespace Zip
} // namespace Utility
} // namespace Elastos
