#ifndef __ELASTOS_DROID_MEDIA_AUDIOPORTCONFIG_H__
#define __ELASTOS_DROID_MEDIA_AUDIOPORTCONFIG_H__

#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * An AudioPortConfig contains a possible configuration of an audio port chosen
 * among all possible attributes described by an AudioPort.
 * An AudioPortConfig is created by AudioPort.buildConfiguration().
 * AudioPorts are used to specify the sources and sinks of a patch created
 * with AudioManager.connectAudioPatch().
 * Several specialized versions of AudioPortConfig exist to handle different categories of
 * audio ports and their specific attributes:
 * - AudioDevicePortConfig for input (e.g micropohone) and output devices (e.g speaker)
 * - AudioMixPortConfig for input or output streams of the audio framework.
 * @hide
 */
class AudioPortConfig
    : public Object
    , public IAudioPortConfig
{
public:
    AudioPortConfig();

    virtual ~AudioPortConfig();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IAudioPort* port,
        /* [in] */ Int32 samplingRate,
        /* [in] */ Int32 channelMask,
        /* [in] */ Int32 format,
        /* [in] */ IAudioGainConfig* gain);

    /**
     * Returns the audio port this AudioPortConfig is issued from.
     */
    CARAPI Port(
        /* [out] */ IAudioPort** result);

    /**
     * Sampling rate configured for this AudioPortConfig.
     */
    CARAPI SamplingRate(
        /* [out] */ Int32* result);

    /**
     * Channel mask configuration (e.g AudioFormat.CHANNEL_CONFIGURATION_STEREO).
     */
    CARAPI ChannelMask(
        /* [out] */ Int32* result);

    /**
     * Audio format configuration (e.g AudioFormat.ENCODING_PCM_16BIT).
     */
    CARAPI Format(
        /* [out] */ Int32* result);

    /**
     * The gain configuration if this port supports gain control, null otherwise
     * @see AudioGainConfig.
     */
    CARAPI Gain(
        /* [out] */ IAudioGainConfig** result);

    CARAPI ToString(
        /* [out] */ String* info);

public:
    AutoPtr<IAudioPort> mPort;
    // mConfigMask indicates which fields in this configuration should be
    // taken into account. Used with AudioSystem.setAudioPortConfig()
    // framework use only.
    static Int32 SAMPLE_RATE;
    static Int32 CHANNEL_MASK;
    static Int32 FORMAT;
    static Int32 GAIN;
    Int32 mConfigMask;

private:
    Int32 mSamplingRate;
    Int32 mChannelMask;
    Int32 mFormat;
    AutoPtr<IAudioGainConfig> mGain;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOPORTCONFIG_H__
