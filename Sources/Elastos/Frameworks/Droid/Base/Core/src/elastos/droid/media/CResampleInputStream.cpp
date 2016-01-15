
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/CResampleInputStream.h"

using Elastos::IO::EIID_ICloseable;
using Elastos::IO::EIID_IInputStream;

namespace Elastos {
namespace Droid {
namespace Media {

#define FIR_COEF(coef) (Int16)(0x10000 * coef)
static const Int16 fir21[] = {
    FIR_COEF(-0.006965742326),
    FIR_COEF(-0.008428945737),
    FIR_COEF( 0.004241280174),
    FIR_COEF( 0.022141096893),
    FIR_COEF( 0.018765669437),
    FIR_COEF(-0.009871891152),
    FIR_COEF(-0.024842433247),
    FIR_COEF( 0.006121772058),
    FIR_COEF( 0.045890841611),
    FIR_COEF( 0.021573503509),
    FIR_COEF(-0.059681984668),
    FIR_COEF(-0.076036275138),
    FIR_COEF( 0.072405390275),
    FIR_COEF( 0.308255674582),
    FIR_COEF( 0.424321210495),
    FIR_COEF( 0.308255674582),
    FIR_COEF( 0.072405390275),
    FIR_COEF(-0.076036275138),
    FIR_COEF(-0.059681984668),
    FIR_COEF( 0.021573503509),
    FIR_COEF( 0.045890841611),
    FIR_COEF( 0.006121772058),
    FIR_COEF(-0.024842433247),
    FIR_COEF(-0.009871891152),
    FIR_COEF( 0.018765669437),
    FIR_COEF( 0.022141096893),
    FIR_COEF( 0.004241280174),
    FIR_COEF(-0.008428945737),
    FIR_COEF(-0.006965742326)
};

String CResampleInputStream::TAG("ResampleInputStream");

// length of 2:1 fir
const Int32 CResampleInputStream::mFirLength = 29;

const Int32 CResampleInputStream::nFir21 = sizeof(fir21) / sizeof(fir21[0]);
const Int32 CResampleInputStream::BUF_SIZE = 2048;

/**
 * Create a new ResampleInputStream, which converts the sample rate
 * @param inputStream InputStream containing 16 bit PCM.
 * @param rateIn the input sample rate.
 * @param rateOut the output sample rate.
 * This only handles rateIn == rateOut / 2 for the moment.
 */
CResampleInputStream::CResampleInputStream()
    : mRateIn(0)
    , mRateOut(0)
    , mBufCount(0)
{
    mOneByte = ArrayOf<Byte>::Alloc(1);
}

CResampleInputStream::~CResampleInputStream()
{
    if (mInputStream != NULL) {
        Close();
        //throw new IllegalStateException("someone forgot to close ResampleInputStream");
    }

    mBuf = NULL;
}

CAR_OBJECT_IMPL(CResampleInputStream)

CAR_INTERFACE_IMPL_2(CResampleInputStream, Object, IInputStream, ICloseable)

ECode CResampleInputStream::Available(
    /* [out] */ Int32 * number)
{
    return InputStream::Available(number);
}

ECode CResampleInputStream::Close()
{
    //try {
    if (mInputStream != NULL) {
        mInputStream->Close();
        mInputStream = NULL;
    }
    //} finally {
        // mInputStream = NULL;
    //}

    return InputStream::Close();
}

ECode CResampleInputStream::Mark(
    /* [in] */ Int32 readLimit)
{
    return InputStream::Mark(readLimit);
}

ECode CResampleInputStream::IsMarkSupported(
    /* [out] */ Boolean * isSupported)
{
    return InputStream::IsMarkSupported(isSupported);
}

ECode CResampleInputStream::Read(
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value);
    Int32 rtn;
    FAIL_RETURN(ReadBytes(mOneByte, 0, 1, &rtn));
    *value = rtn == 1 ? (0xff & (*mOneByte)[0]) : -1;
    return NOERROR;
}

ECode CResampleInputStream::ReadBytes(
    /* [out] */ ArrayOf<Byte> * buffer,
    /* [out] */ Int32 * number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;
    VALIDATE_NOT_NULL(buffer);

    return ReadBytes(buffer, 0, buffer->GetLength(), number);
}

ECode CResampleInputStream::ReadBytes(
    /* [out] */ ArrayOf<Byte> * buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32 * number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;
    VALIDATE_NOT_NULL(buffer);

    if (mInputStream == NULL) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // ensure that mBuf is big enough to cover requested 'length'
    Int32 nIn = ((length / 2) * mRateIn / mRateOut + mFirLength) * 2;
    if (mBuf == NULL) {
        mBuf = ArrayOf<Byte>::Alloc(nIn);
    }
    else if (nIn > mBuf->GetLength()) {
        AutoPtr<ArrayOf<Byte> > bf = ArrayOf<Byte>::Alloc(nIn);
        bf->Copy(mBuf, mBufCount);
        mBuf = bf;
    }

    // read until we have enough data for at least one output sample
    while (TRUE) {
        Int32 len = ((mBufCount / 2 - mFirLength) * mRateOut / mRateIn) * 2;
        if (len > 0) {
            length = len < length ? len : (length / 2) * 2;
            break;
        }
        // TODO: should mBuf.length below be nIn instead?
        Int32 n;
        mInputStream->Read(mBuf, mBuf->GetLength() - mBufCount, mBufCount, &n);
        if (n == -1) {
            *number = -1;
            return NOERROR;
        }

        mBufCount += n;
    }

    // resample input data
    FAIL_RETURN(Fir21(mBuf, 0, buffer, offset, length / 2));

    // move any unused bytes to front of mBuf
    Int32 nFwd = length * mRateIn / mRateOut;
    mBufCount -= nFwd;
    if (mBufCount > 0) {
        mBuf->Copy(mBuf, nFwd, mBufCount);
    }

    *number = length;
    return NOERROR;
}

ECode CResampleInputStream::Reset()
{
    return InputStream::Reset();
}

ECode CResampleInputStream::Skip(
    /* [in] */ Int64 count,
    /* [out] */ Int64 * pNumber)
{
    return InputStream::Skip(count, pNumber);
}

ECode CResampleInputStream::constructor(
    /* [in] */ IInputStream * inputStream,
    /* [in] */ Int32 rateIn,
    /* [in] */ Int32 rateOut)
{
    // only support 2:1 at the moment
    if (rateIn != 2 * rateOut) {
        return E_INVALID_ARGUMENT;//throw new IllegalArgumentException("only support 2:1 at the moment");
    }
    rateIn = 2;
    rateOut = 1;

    mInputStream = inputStream;
    mRateIn = rateIn;
    mRateOut = rateOut;

    return NOERROR;
}

ECode CResampleInputStream::Fir21(
   /* [in] */ ArrayOf<Byte>* inArray,
   /* [in] */ Int32 inOffset,
   /* [in] */ ArrayOf<Byte>* outArray,
   /* [in] */ Int32 outOffset,
   /* [in] */ Int32 npoints)
{
    // safety first!
    if (nFir21 + npoints * 2 > BUF_SIZE) {
        /*throwException(env, "java/lang/IllegalArgumentException",
                "FIR+data too long %d", nFir21 + jNpoints);*/
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // get input data
    Int16 in[BUF_SIZE];
    memcpy(in, inArray->GetPayload() + inOffset, (npoints * 2 + nFir21 - 1) * 2);

    // compute filter
    Int16 out[BUF_SIZE];
    Int64 sum = 0;
    for (Int32 i = 0; i < npoints; i++) {
        sum = 0;
        const Int16* firp = &fir21[0];
        const Int16* inp = &in[i * 2];
        for (Int32 n = nFir21; --n >= 0; ) {
            sum += ((Int64)*firp++) * ((Int64)*inp++);
        }
        out[i] = (Int16)(sum >> 16);
    }

    // save new values
    memcpy(outArray->GetPayload() + outOffset, out, npoints * 2);
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
