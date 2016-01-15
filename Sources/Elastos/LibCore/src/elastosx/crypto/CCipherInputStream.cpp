#include "CCipherInputStream.h"
#include "CStreams.h"
#include "Math.h"

using Elastos::IO::IBufferedInputStream;
using Libcore::IO::IStreams;
using Libcore::IO::CStreams;

namespace Elastosx {
namespace Crypto {

CAR_INTERFACE_IMPL(CCipherInputStream, FilterInputStream, ICipherInputStream)

CCipherInputStream::CCipherInputStream()
    : mOutputIndex(0)
    , mOutputLength(0)
    , mFinished(FALSE)
{
}

ECode CCipherInputStream::constructor(
    /* [in] */ IInputStream* is,
    /* [in] */ ICipher* c)
{
    FAIL_RETURN(FilterInputStream::constructor(is));
    mCipher = c;
    Int32 size;
    c->GetBlockSize(&size);
    Int32 blockSize = Elastos::Core::Math::Max(size, 1);
    Int32 bufferSize = Elastos::Core::Math::Max(blockSize,
            IBufferedInputStream::DEFAULT_BUFFER_SIZE / blockSize * blockSize);
    mInputBuffer = ArrayOf<Byte>::Alloc(bufferSize);
    mOutputBuffer = ArrayOf<Byte>::Alloc(bufferSize + ((blockSize > 1) ? 2 * blockSize : 0));
    return NOERROR;
}

ECode CCipherInputStream::constructor(
    /* [in] */ IInputStream* is)
{
//TODO: Need INullCipher
    // AutoPtr<INullCipher> c;
    // CNullCipher::New((INullCipher**)&c);
    // return constructor(is, c);
    return NOERROR;
}

ECode CCipherInputStream::Read(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (mIn == NULL) {
        // throw new NullPointerException("in == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (mOutputIndex == mOutputLength && !FillBuffer()) {
        *result = -1;
        return NOERROR;
    }
    *result = (*mOutputBuffer)[mOutputIndex++] & 0xFF;
    return NOERROR;
}

ECode CCipherInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (mIn == NULL) {
        // throw new NullPointerException("in == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (mOutputIndex == mOutputLength && !FillBuffer()) {
        *result = -1;
        return NOERROR;
    }
    Int32 available = mOutputLength - mOutputIndex;
    if (available < len) {
        len = available;
    }
    if (buf != NULL) {
        // Elastos::Droid::System::Arraycopy(mOutputBuffer, mOutputIndex, buf, off, len);
        buf->Copy(off, mOutputBuffer, mOutputIndex, len);
    }
    mOutputIndex += len;
    *result = len;
    return NOERROR;
}

ECode CCipherInputStream::Skip(
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* result)
{
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    return streams->SkipByReading(this, byteCount, result);
}

ECode CCipherInputStream::Available(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOutputLength - mOutputIndex;
    return NOERROR;
}

ECode CCipherInputStream::Close()
{
    mIn->Close();
    // try {
    AutoPtr<ArrayOf<Byte> > array;
    return mCipher->DoFinal((ArrayOf<Byte>**)&array);
    // } catch (GeneralSecurityException ignore) {
    //     //do like RI does
    // }
}

ECode CCipherInputStream::MarkSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

Boolean CCipherInputStream::FillBuffer()
{
    if (mFinished) {
        return FALSE;
    }
    mOutputIndex = 0;
    mOutputLength = 0;
    while (mOutputLength == 0) {
        // check output size on each iteration since pending state
        // in the cipher can cause this to vary from call to call
        Int32 outputSize;
        mCipher->GetOutputSize(mInputBuffer->GetLength(), &outputSize);
        if ((mOutputBuffer == NULL) || (mOutputBuffer->GetLength() < outputSize)) {
            this->mOutputBuffer = ArrayOf<Byte>::Alloc(outputSize);
        }
        Int32 byteCount;
        mIn->Read(mInputBuffer, &byteCount);
        if (byteCount == -1) {
            // try {
                mCipher->DoFinal(mOutputBuffer, 0, &mOutputLength);
            // } catch (Exception e) {
            //     throw new IOException("Error while finalizing mCipher", e);
            // }
            mFinished = TRUE;
            return mOutputLength != 0;
        }
        // try {
            mCipher->Update(mInputBuffer, 0, byteCount, mOutputBuffer, 0, &mOutputLength);
        // } catch (ShortBufferException e) {
        //     throw new AssertionError(e);  // should not happen since we sized with getOutputSize
        // }
    }
    return TRUE;
}


} // Crypto
} // Elastosx