#ifndef __ELASTOS_DROID_MEDIA_CAUDIOGAIN_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOGAIN_H__

#include "_Elastos_Droid_Media_CAudioGain.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * The AudioGain describes a gain controller. Gain controllers are exposed by
 * audio ports when the gain is configurable at this port's input or output.
 * Gain values are expressed in millibels.
 * A gain controller has the following attributes:
 * - mode: defines modes of operation or features
 *    MODE_JOINT: all channel gains are controlled simultaneously
 *    MODE_CHANNELS: each channel gain is controlled individually
 *    MODE_RAMP: ramps can be applied when gain changes
 * - channel mask: indicates for which channels the gain can be controlled
 * - min value: minimum gain value in millibel
 * - max value: maximum gain value in millibel
 * - default value: gain value after reset in millibel
 * - step value: granularity of gain control in millibel
 * - min ramp duration: minimum ramp duration in milliseconds
 * - max ramp duration: maximum ramp duration in milliseconds
 *
 * This object is always created by the framework and read only by applications.
 * Applications get a list of AudioGainDescriptors from AudioPortDescriptor.gains() and can build a
 * valid gain configuration from AudioGain.buildConfig()
 * @hide
 */
CarClass(CAudioGain)
    , public Object
    , public IAudioGain
{
public:
    CAudioGain();

    virtual ~CAudioGain();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    // The channel mask passed to the constructor is as specified in AudioFormat
    // (e.g. AudioFormat.CHANNEL_OUT_STEREO)
    CARAPI constructor(
        /* [in] */ Int32 index,
        /* [in] */ Int32 mode,
        /* [in] */ Int32 channelMask,
        /* [in] */ Int32 minValue,
        /* [in] */ Int32 maxValue,
        /* [in] */ Int32 defaultValue,
        /* [in] */ Int32 stepValue,
        /* [in] */ Int32 rampDurationMinMs,
        /* [in] */ Int32 rampDurationMaxMs);

    /**
     * Bit field indicating supported modes of operation
     */
    CARAPI Mode(
        /* [out] */ Int32* result);

    /**
     * Indicates for which channels the gain can be controlled
     * (e.g. AudioFormat.CHANNEL_OUT_STEREO)
     */
    CARAPI ChannelMask(
        /* [out] */ Int32* result);

    /**
     * Minimum gain value in millibel
     */
    CARAPI MinValue(
        /* [out] */ Int32* result);

    /**
     * Maximum gain value in millibel
     */
    CARAPI MaxValue(
        /* [out] */ Int32* result);

    /**
     * Default gain value in millibel
     */
    CARAPI DefaultValue(
        /* [out] */ Int32* result);

    /**
     * Granularity of gain control in millibel
     */
    CARAPI StepValue(
        /* [out] */ Int32* result);

    /**
     * Minimum ramp duration in milliseconds
     * 0 if MODE_RAMP not set
     */
    CARAPI RampDurationMinMs(
        /* [out] */ Int32* result);

    /**
     * Maximum ramp duration in milliseconds
     * 0 if MODE_RAMP not set
     */
    CARAPI RampDurationMaxMs(
        /* [out] */ Int32* result);

    /**
     * Build a valid gain configuration for this gain controller for use by
     * AudioPortDescriptor.setGain()
     * @param mode: desired mode of operation
     * @param channelMask: channels of which the gain should be modified.
     * @param values: gain values for each channels.
     * @param rampDurationMs: ramp duration if mode MODE_RAMP is set.
     * ignored if MODE_JOINT.
     */
    CARAPI BuildConfig(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 channelMask,
        /* [in] */ ArrayOf<Int32>* values,
        /* [in] */ Int32 rampDurationMs,
        /* [out] */ IAudioGainConfig** result);

private:
    Int32 mIndex;
    Int32 mMode;
    Int32 mChannelMask;
    Int32 mMinValue;
    Int32 mMaxValue;
    Int32 mDefaultValue;
    Int32 mStepValue;
    Int32 mRampDurationMinMs;
    Int32 mRampDurationMaxMs;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CAUDIOGAIN_H__
