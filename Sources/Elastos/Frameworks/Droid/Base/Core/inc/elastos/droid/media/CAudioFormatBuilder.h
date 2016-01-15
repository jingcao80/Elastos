#ifndef __ELASTOS_DROID_MEDIA_CAUDIOFORMATBUILDER_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOFORMATBUILDER_H__

#include "_Elastos_Droid_Media_CAudioFormatBuilder.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Builder class for {@link AudioFormat} objects.
 * Use this class to configure and create an AudioFormat instance. By setting format
 * characteristics such as audio encoding, channel mask or sample rate, you indicate which
 * of those are to vary from the default behavior on this device wherever this audio format
 * is used.
 * <p>{@link AudioFormat} is for instance used in
 * {@link AudioTrack#AudioTrack(AudioAttributes, AudioFormat, int, int, int)}. In this
 * constructor, every format characteristic set on the <code>Builder</code> (e.g. with
 * {@link #setSampleRate(int)}) will alter the default values used by an
 * <code>AudioTrack</code>. In this case for audio playback with <code>AudioTrack</code>, the
 * sample rate set in the <code>Builder</code> would override the platform output sample rate
 * which would otherwise be selected by default.
 */
CarClass(CAudioFormatBuilder)
    , public Object
    , public IAudioFormatBuilder
{
public:
    CAudioFormatBuilder();

    virtual ~CAudioFormatBuilder();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Constructs a new Builder with none of the format characteristics set.
     */
    CARAPI constructor();

    /**
     * Constructs a new Builder from a given {@link AudioFormat}.
     * @param af the {@link AudioFormat} object whose data will be reused in the new Builder.
     */
    CARAPI constructor(
        /* [in] */ IAudioFormat* af);

    /**
     * Combines all of the format characteristics that have been set and return a new
     * {@link AudioFormat} object.
     * @return a new {@link AudioFormat} object
     */
    CARAPI Build(
        /* [out] */ IAudioFormat** result);

    /**
     * Sets the data encoding format.
     * @param encoding one of {@link AudioFormat#ENCODING_DEFAULT},
     *     {@link AudioFormat#ENCODING_PCM_8BIT},
     *     {@link AudioFormat#ENCODING_PCM_16BIT},
     *     {@link AudioFormat#ENCODING_PCM_FLOAT},
     *     {@link AudioFormat#ENCODING_AC3},
     *     {@link AudioFormat#ENCODING_E_AC3}.
     * @return the same Builder instance.
     * @throws java.lang.IllegalArgumentException
     */
    CARAPI SetEncoding(
        /* [in] */ Int32 encoding);

    /**
     * Sets the channel mask.
     * @param channelMask describes the configuration of the audio channels.
     *    <p>For output, the mask should be a combination of
     *    {@link AudioFormat#CHANNEL_OUT_FRONT_LEFT},
     *    {@link AudioFormat#CHANNEL_OUT_FRONT_CENTER},
     *    {@link AudioFormat#CHANNEL_OUT_FRONT_RIGHT},
     *    {@link AudioFormat#CHANNEL_OUT_SIDE_LEFT},
     *    {@link AudioFormat#CHANNEL_OUT_SIDE_RIGHT},
     *    {@link AudioFormat#CHANNEL_OUT_BACK_LEFT},
     *    {@link AudioFormat#CHANNEL_OUT_BACK_RIGHT}.
     *    <p>for input, the mask should be {@link AudioFormat#CHANNEL_IN_MONO} or
     *    {@link AudioFormat#CHANNEL_IN_STEREO}.  {@link AudioFormat#CHANNEL_IN_MONO} is
     *    guaranteed to work on all devices.
     * @return the same Builder instance.
     */
    CARAPI SetChannelMask(
        /* [in] */ Int32 channelMask);

    /**
     * Sets the sample rate.
     * @param sampleRate the sample rate expressed in Hz
     * @return the same Builder instance.
     * @throws java.lang.IllegalArgumentException
     */
    CARAPI SetSampleRate(
        /* [in] */ Int32 sampleRate);

private:
    Int32 mEncoding;
    Int32 mSampleRate;
    Int32 mChannelMask;
    Int32 mPropertySetMask;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CAUDIOFORMATBUILDER_H__
