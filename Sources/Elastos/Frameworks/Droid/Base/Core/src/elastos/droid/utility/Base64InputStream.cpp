#include "elastos/droid/utility/Base64InputStream.h"
#include <elastos/core/Math.h>

using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Utility {

AutoPtr<ArrayOf<Byte> > Base64InputStream::EMPTY = ArrayOf<Byte>::Alloc(0);

const Int32 Base64InputStream::BUFFER_SIZE = 2048;

CAR_INTERFACE_IMPL(Base64InputStream, FilterInputStream, IBase64InputStream)

Base64InputStream::Base64InputStream()
    : mEof(FALSE)
    , mOutputStart(0)
    , mOutputEnd(0)
{
}

ECode Base64InputStream::constructor(
    /* [in] */ IInputStream* inputStream,
    /* [in] */ Int32 flags)
{
    return constructor(inputStream, flags, FALSE);
}

ECode Base64InputStream::constructor(
    /* [in] */ IInputStream* in,
    /* [in] */ Int32 flags,
    /* [in] */ Boolean encode)
{
    FAIL_RETURN(FilterInputStream::constructor(in))

    mEof = FALSE;
    mInputBuffer = ArrayOf<Byte>::Alloc(BUFFER_SIZE);
    if (encode) {
        mCoder = new CBase64::Encoder(flags, NULL);
    }
    else {
        mCoder = new CBase64::Decoder(flags, NULL);
    }
    mCoder->mOutput = ArrayOf<Byte>::Alloc(mCoder->MaxOutputSize(BUFFER_SIZE));
    mOutputStart = 0;
    mOutputEnd = 0;
    return NOERROR;
}

ECode Base64InputStream::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported)
    *supported = FALSE;
    return NOERROR;
}

ECode Base64InputStream::Mark(
    /* [in] */ Int32 readLimit)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Base64InputStream::Reset()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Base64InputStream::Close()
{
    ICloseable::Probe(mIn)->Close();
    mInputBuffer = NULL;
    return NOERROR;
}

ECode Base64InputStream::Available(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = mOutputEnd - mOutputStart;
    return NOERROR;
}

ECode Base64InputStream::Skip(
    /* [in] */ Int64 n,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;

    if (mOutputStart >= mOutputEnd) {
        Refill();
    }
    if (mOutputStart >= mOutputEnd) {
        return NOERROR;
    }

    Int64 bytes = Elastos::Core::Math::Min(n, (Int64)mOutputEnd - mOutputStart);
    mOutputStart += bytes;
    *number = bytes;
    return NOERROR;
}

ECode Base64InputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = -1;

    if (mOutputStart >= mOutputEnd) {
        Refill();
    }
    if (mOutputStart >= mOutputEnd) {
        return NOERROR;
    }

    *value = (*mCoder->mOutput)[mOutputStart++] & 0xff;
    return NOERROR;
}

ECode Base64InputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;

    if (mOutputStart >= mOutputEnd) {
        Refill();
    }
    if (mOutputStart >= mOutputEnd) {
        return NOERROR;
    }

    Int32 bytes = Elastos::Core::Math::Min(len, mOutputEnd - mOutputStart);
    buffer->Copy(off, mCoder->mOutput + mOutputStart, bytes);

    mOutputStart += bytes;
    *number = bytes;
    return NOERROR;
}

ECode Base64InputStream::Refill()
{
    if (mEof) return NOERROR;

    Int32 bytesRead;
    mIn->Read(mInputBuffer, &bytesRead);

    Boolean success;
    if (bytesRead == -1) {
        mEof = TRUE;
        success = mCoder->Process(EMPTY, 0, 0, TRUE);
    }
    else {
        success = mCoder->Process(mInputBuffer, 0, bytesRead, FALSE);
    }

    if (!success) {
        //throw new Base64DataException("bad base-64");
        return E_BASE64_DATA_EXCEPTION;
    }

    mOutputEnd = mCoder->mOp;
    mOutputStart = 0;
    return NOERROR;
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos