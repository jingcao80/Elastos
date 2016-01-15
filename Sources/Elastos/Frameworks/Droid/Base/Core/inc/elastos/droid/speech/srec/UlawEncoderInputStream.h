#ifndef __ELASTOS_DROID_SPEECH_SREC_UlawEncoderInputStream_H__
#define __ELASTOS_DROID_SPEECH_SREC_UlawEncoderInputStream_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"
#include <elastos/io/InputStream.h>
#include "Elastos.Droid.Speech.h"

using Elastos::IO::IInputStream;
using Elastos::IO::InputStream;
using Elastos::Droid::Speech::Srec::IUlawEncoderInputStream;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Srec {

/**
 * InputStream which transforms 16 bit pcm data to ulaw data.
 *
 * Not yet ready to be supported, so
 * @hide
 */
class UlawEncoderInputStream
    : public InputStream
    , public IUlawEncoderInputStream
{
public:
    CAR_INTERFACE_DECL();

    /**
     * Create an InputStream which takes 16 bit pcm data and produces ulaw data.
     * @param in InputStream containing 16 bit pcm data.
     * @param max pcm value corresponding to maximum ulaw value.
     */
    UlawEncoderInputStream();

    virtual ~UlawEncoderInputStream();

    CARAPI constructor();

    /**
     * Create an InputStream which takes 16 bit pcm data and produces ulaw data.
     * @param in InputStream containing 16 bit pcm data.
     * @param max pcm value corresponding to maximum ulaw value.
     */
    CARAPI constructor(
        /* [in]  */ IInputStream* ins,
        /* [in]  */ Int32 max);

    static CARAPI Encode(
        /* [in]  */ ArrayOf<Byte>* pcmBuf,
        /* [in]  */ Int32 pcmOffset,
        /* [in]  */ ArrayOf<Byte>* ulawBuf,
        /* [in]  */ Int32 ulawOffset,
        /* [in]  */ Int32 length,
        /* [in]  */ Int32 max);

    /**
     * Compute the maximum of the absolute value of the pcm samples.
     * The return value can be used to set ulaw encoder scaling.
     * @param pcmBuf array containing 16 bit pcm data.
     * @param offset offset of start of 16 bit pcm data.
     * @param length number of pcm samples (not number of input bytes)
     * @return maximum abs of pcm data values
     */
    static CARAPI MaxAbsPcm(
        /* [in]  */ ArrayOf<Byte>* pcmBuf,
        /* [in]  */ Int32 offset,
        /* [in]  */ Int32 length,
        /* [out] */ Int32* ret);

    //@Override
    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* buf,
        /* [in]  */ Int32 offset,
        /* [in]  */ Int32 length,
        /* [out] */ Int32* number);

    //@Override
    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* buf,
        /* [out] */ Int32* number);

    //@Override
    CARAPI Read(
        /* [out] */ Int32* value);

    CARAPI Close();

    CARAPI Available(
        /* [out] */ Int32* number);

    CARAPI Mark(
        /* [in]  */ Int32 readLimit);

    CARAPI IsMarkSupported(
        /* [out] */ Boolean* supported);

    CARAPI Reset();

    CARAPI Skip(
        /* [in]  */ Int64 byteCount,
        /* [out] */ Int64* number);

    CARAPI GetLock(
        /* [out] */ IInterface** lockobj);

private:
    static const String TAG;        // = "UlawEncoderInputStream";

    static const Int32 MAX_ULAW;    // = 8192;
    static const Int32 SCALE_BITS;  // = 16;

    AutoPtr<IInputStream> mIn;

    Int32 mMax;

    AutoPtr< ArrayOf<Byte> > mBuf;  // = ArrayOf<Byte>::Alloc(1024);
    Int32 mBufCount;                // = 0; // should be 0 or 1

    AutoPtr< ArrayOf<Byte> > mOneByte;  // = ArrayOf<Byte>::Alloc(1);
};

} // namespace Srec
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_SREC_UlawEncoderInputStream_H__
