#include "elastos/droid/utility/Base64OutputStream.h"

using Elastos::IO::ICloseable;
using Elastos::IO::IFlushable;

namespace Elastos {
namespace Droid {
namespace Utility {

AutoPtr<ArrayOf<Byte> > Base64OutputStream::EMPTY = ArrayOf<Byte>::Alloc(0);

CAR_INTERFACE_IMPL(Base64OutputStream, FilterOutputStream, IBase64OutputStream)

Base64OutputStream::Base64OutputStream()
    : mFlags(0)
    , mBpos(0)
{
}

ECode Base64OutputStream::constructor(
    /* [in] */ IOutputStream* out,
    /* [in] */ Int32 flags)
{
    return constructor(out, flags, TRUE);
}

ECode Base64OutputStream::constructor(
    /* [in] */ IOutputStream* out,
    /* [in] */ Int32 flags,
    /* [in] */ Boolean encode)
{
    FAIL_RETURN(FilterOutputStream::constructor(out))
    mFlags = flags;
    if (encode) {
        mCoder = new CBase64::Encoder(flags, NULL);
    }
    else {
        mCoder = new CBase64::Decoder(flags, NULL);
    }
    return NOERROR;
}

ECode Base64OutputStream::Write(
    /* [in] */ Int32 b)
{
    // To avoid invoking the encoder/decoder routines for single
    // bytes, we buffer up calls to write(int) in an internal
    // Byte array to transform them into writes of decently-sized
    // arrays.

    if (mBuffer == NULL) {
        mBuffer = ArrayOf<Byte>::Alloc(1024);
    }
    if (mBpos >= mBuffer->GetLength()) {
        // internal buffer full; write it out.
        InternalWrite(mBuffer, 0, mBpos, FALSE);
        mBpos = 0;
    }
    (*mBuffer)[mBpos++] = (Byte) b;
    return NOERROR;
}

ECode Base64OutputStream::FlushBuffer()
{
    if (mBpos > 0) {
        InternalWrite(mBuffer, 0, mBpos, FALSE);
        mBpos = 0;
    }
    return NOERROR;
}

ECode Base64OutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    if (len <= 0) return NOERROR;
    FlushBuffer();
    InternalWrite(buffer, off, len, FALSE);
    return NOERROR;
}

ECode Base64OutputStream::Close()
{
    ECode ec = NOERROR;
    // try {
        ec = FlushBuffer();
        ec = InternalWrite(EMPTY, 0, 0, TRUE);
    // } catch (IOException e) {
    //     thrown = e;
    // }

    // try {
    if ((mFlags & CBase64::NO_CLOSE) == 0) {
        ec = ICloseable::Probe(mOut)->Close();
    }
    else {
        ec = IFlushable::Probe(mOut)->Flush();
    }
    // } catch (IOException e) {
    //     if (thrown != NULL) {
    //         thrown = e;
    //     }
    // }

    return ec;
}

ECode Base64OutputStream::InternalWrite(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len,
    /* [in] */ Boolean finish)
{
    mCoder->mOutput = Embiggen(mCoder->mOutput, mCoder->MaxOutputSize(len));
    if (!mCoder->Process(b, off, len, finish)) {
        //throw new Base64DataException("bad base-64");
        return E_BASE64_DATA_EXCEPTION;
    }
    return mOut->Write(mCoder->mOutput, 0, mCoder->mOp);
}

AutoPtr<ArrayOf<Byte> > Base64OutputStream::Embiggen(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 len)
{
    AutoPtr<ArrayOf<Byte> > bytes = b;
    if (b == NULL || b->GetLength() < len) {
        bytes = ArrayOf<Byte>::Alloc(len);
    }

    return bytes;
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
