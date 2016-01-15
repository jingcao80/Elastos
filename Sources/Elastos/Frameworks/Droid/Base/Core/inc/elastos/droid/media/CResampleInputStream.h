
#ifndef __ELASTOS_DROID_MEDIA_CRESAMPLEINPUTSTREAM_H__
#define __ELASTOS_DROID_MEDIA_CRESAMPLEINPUTSTREAM_H__

#include "_Elastos_Droid_Media_CResampleInputStream.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/io/InputStream.h>

using Elastos::IO::InputStream;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CResampleInputStream)
    , public InputStream
{
public:
    /**
     * Create a new ResampleInputStream, which converts the sample rate
     * @param inputStream InputStream containing 16 bit PCM.
     * @param rateIn the input sample rate.
     * @param rateOut the output sample rate.
     * This only handles rateIn == rateOut / 2 for the moment.
     */
    CResampleInputStream();

    ~CResampleInputStream();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Available(
        /* [out] */ Int32 * pNumber);

    CARAPI Close();

    CARAPI Mark(
        /* [in] */ Int32 readLimit);

    CARAPI IsMarkSupported(
        /* [out] */ Boolean * supported);

    CARAPI Read(
        /* [out] */ Int32 * value);

    CARAPI ReadBytes(
        /* [out] */ ArrayOf<Byte> * buffer,
        /* [out] */ Int32 * number);

    CARAPI ReadBytes(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32 * number);

    CARAPI Reset();

    CARAPI Skip(
        /* [in] */ Int64 count,
        /* [out] */ Int64 * pNumber);

    CARAPI constructor(
        /* [in] */ IInputStream * inputStream,
        /* [in] */ Int32 rateIn,
        /* [in] */ Int32 rateOut);

private:
    //
    // fir filter code JNI interface
    //
    static CARAPI Fir21(
        /* [in] */ ArrayOf<Byte>* in,
        /* [in] */ Int32 inOffset,
        /* [in] */ ArrayOf<Byte>* out,
        /* [in] */ Int32 outOffset,
        /* [in] */ Int32 npoints);

    static String TAG;// = "ResampleInputStream";

    // pcm input stream
    AutoPtr<IInputStream> mInputStream;

    // sample rates, assumed to be normalized
    Int32 mRateIn;
    Int32 mRateOut;

    // input pcm data
    AutoPtr<ArrayOf<Byte> > mBuf;
    Int32 mBufCount;

    // helper for bytewise read()
    AutoPtr<ArrayOf<Byte> > mOneByte;

    // length of 2:1 fir
    static const Int32 mFirLength;// = 29;
    static const Int32 nFir21;// = sizeof(fir21) / sizeof(fir21[0]);
    static const Int32 BUF_SIZE;// = 2048;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CRESAMPLEINPUTSTREAM_H__
