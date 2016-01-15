#ifndef __ELASTOS_DROID_MEDIA_CAUDIOGAINCONFIG_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOGAINCONFIG_H__

#include "_Elastos_Droid_Media_CAudioGainConfig.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * The AudioGainConfig is used by APIs setting or getting values on a given gain
 * controller. It contains a valid configuration (value, channels...) for a gain controller
 * exposed by an audio port.
 * @see AudioGain
 * @see AudioPort
 * @hide
 */
CarClass(CAudioGainConfig)
    , public Object
    , public IAudioGainConfig
{
public:
    CAudioGainConfig();

    virtual ~CAudioGainConfig();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Int32 index,
        /* [in] */ IAudioGain* gain,
        /* [in] */ Int32 mode,
        /* [in] */ Int32 channelMask,
        /* [in] */ ArrayOf<Int32>* values,
        /* [in] */ Int32 rampDurationMs);

    /**
     * get the index of the parent gain.
     * frameworks use only.
     */
    CARAPI Index(
        /* [out] */ Int32* result);

    /**
     * Bit field indicating requested modes of operation. See {@link AudioGain#MODE_JOINT},
     * {@link AudioGain#MODE_CHANNELS}, {@link AudioGain#MODE_RAMP}
     */
    CARAPI Mode(
        /* [out] */ Int32* result);

    /**
     * Indicates for which channels the gain is set.
     * See {@link AudioFormat#CHANNEL_OUT_STEREO}, {@link AudioFormat#CHANNEL_OUT_MONO} ...
     */
    CARAPI ChannelMask(
        /* [out] */ Int32* result);

    /**
     * Gain values for each channel in the order of bits set in
     * channelMask() from LSB to MSB
     */
    CARAPI Values(
        /* [out, callee] */ ArrayOf<Int32>** result);

    /**
     * Ramp duration in milliseconds. N/A if mode() does not
     * specify MODE_RAMP.
     */
    CARAPI RampDurationMs(
        /* [out] */ Int32* result);

public:
    AutoPtr<IAudioGain> mGain;

private:
    Int32 mIndex;
    Int32 mMode;
    Int32 mChannelMask;
    AutoPtr<ArrayOf<Int32> > mValues;
    Int32 mRampDurationMs;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CAUDIOGAINCONFIG_H__
