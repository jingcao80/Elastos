
#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODECINFOAUDIOCAPABILITIES_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODECINFOAUDIOCAPABILITIES_H__

#include "Elastos.Droid.Utility.h"
#include "_Elastos_Droid_Media_CMediaCodecInfoAudioCapabilities.h"
#include "elastos/droid/ext/frameworkext.h"
// #include "elastos/droid/utility/Range.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IRange;
// using Elastos::Droid::Utility::Range;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodecInfoAudioCapabilities)
    , public Object
    , public IMediaCodecInfoAudioCapabilities
{
public:
    CMediaCodecInfoAudioCapabilities();

    virtual ~CMediaCodecInfoAudioCapabilities();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Returns the range of supported bitrates in bits/second.
     */
    CARAPI GetBitrateRange(
        /* [out] */ IRange** result);

    /**
     * Returns the array of supported sample rates if the codec
     * supports only discrete values.  Otherwise, it returns
     * {@code null}.  The array is sorted in ascending order.
     */
    CARAPI GetSupportedSampleRates(
        /* [out, callee] */ ArrayOf<Int32>** result);

    /**
     * Returns the array of supported sample rate ranges.  The
     * array is sorted in ascending order, and the ranges are
     * distinct.
     */
    CARAPI GetSupportedSampleRateRanges(
        /* [out, callee] */ ArrayOf<IRange*>** result);

    /**
     * Returns the maximum number of input channels supported.  The codec
     * supports any number of channels between 1 and this maximum value.
     */
    CARAPI GetMaxInputChannelCount(
        /* [out] */ Int32* result);

    /** @hide */
    CARAPI Init(
        /* [in] */ IMediaFormat* info,
        /* [in] */ IMediaCodecInfoCodecCapabilities* parent);

    /**
     * Query whether the sample rate is supported by the codec.
     */
    CARAPI IsSampleRateSupported(
        /* [in] */ Int32 sampleRate,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI SetDefaultFormat(
        /* [in] */ IMediaFormat* format);

    /** @hide */
    CARAPI SupportsFormat(
        /* [in] */ IMediaFormat* format,
        /* [out] */ Boolean* result);

    /** @hide */
    static CARAPI Create(
        /* [in] */ IMediaFormat* info,
        /* [in] */ IMediaCodecInfoCodecCapabilities* parent,
        /* [out] */ IMediaCodecInfoAudioCapabilities** result);

private:
    CARAPI_(void) InitWithPlatformLimits();

    CARAPI_(Boolean) Supports(
        /* [in] */ IInteger32* sampleRate,
        /* [in] */ IInteger32* inputChannels);

    /** modifies rates */
    CARAPI_(void) LimitSampleRates(
        /* [in] */ ArrayOf<Int32>* rates);

    CARAPI_(void) CreateDiscreteSampleRates();

    /** modifies rateRanges */
    CARAPI_(void) LimitSampleRates(
        /* [in] */ ArrayOf<IRange*>* rateRanges);
         // [in]  ArrayOf<Range<IInteger32*> >* rateRanges);

    CARAPI_(void) ApplyLevelLimits();

    CARAPI_(void) ApplyLimits(
        /* [in] */ Int32 maxInputChannels,
        /* [in] */ IRange* bitRates);
         // [in]  Range<IInteger32*>* bitRates);

    CARAPI_(void) ParseFromInfo(
        /* [in] */ IMediaFormat* info);

private:
    static const String TAG;

    static const Int32 MAX_INPUT_CHANNEL_COUNT;

    AutoPtr<IMediaCodecInfoCodecCapabilities> mParent;
    AutoPtr<IRange> mBitrateRange;
    // AutoPtr<Range<IInteger32*> > mBitrateRange;

    AutoPtr<ArrayOf<Int32> > mSampleRates;
    AutoPtr<ArrayOf<IRange*> > mSampleRateRanges;
    // AutoPtr<ArrayOf<Range<IInteger32*> > > mSampleRateRanges;
    Int32 mMaxInputChannelCount;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CMEDIACODECINFOAUDIOCAPABILITIES_H__
