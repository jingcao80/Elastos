#ifndef __ELASTOS_DROID_MEDIA_AUDIOPORT_H__
#define __ELASTOS_DROID_MEDIA_AUDIOPORT_H__

#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * An audio port is a node of the audio framework or hardware that can be connected to or
 * disconnect from another audio node to create a specific audio routing configuration.
 * Examples of audio ports are an output device (speaker) or an output mix (see AudioMixPort).
 * All attributes that are relevant for applications to make routing selection are decribed
 * in an AudioPort,  in particular:
 * - possible channel mask configurations.
 * - audio format (PCM 16bit, PCM 24bit...)
 * - gain: a port can be associated with one or more gain controllers (see AudioGain).
 *
 * This object is always created by the framework and read only by applications.
 * A list of all audio port descriptors currently available for applications to control
 * is obtained by AudioManager.listAudioPorts().
 * An application can obtain an AudioPortConfig for a valid configuration of this port
 * by calling AudioPort.buildConfig() and use this configuration
 * to create a connection between audio sinks and sources with AudioManager.connectAudioPatch()
 *
 * @hide
 */
class AudioPort
    : public Object
    , public IAudioPort
{
public:
    AudioPort();

    virtual ~AudioPort();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IAudioHandle* handle,
        /* [in] */ Int32 role,
        /* [in] */ ArrayOf<Int32>* samplingRates,
        /* [in] */ ArrayOf<Int32>* channelMasks,
        /* [in] */ ArrayOf<Int32>* formats,
        /* [in] */ ArrayOf<IAudioGain*>* gains);

    /**
     * Get the audio port role
     */
    CARAPI Role(
        /* [out]*/ Int32* result);

    /**
     * Get the list of supported sampling rates
     * Empty array if sampling rate is not relevant for this audio port
     */
    CARAPI SamplingRates(
        /* [out, callee] */ ArrayOf<Int32>** result);

    /**
     * Get the list of supported channel mask configurations
     * (e.g AudioFormat.CHANNEL_OUT_STEREO)
     * Empty array if channel mask is not relevant for this audio port
     */
    CARAPI ChannelMasks(
        /* [out, callee] */ ArrayOf<Int32>** result);

    /**
     * Get the list of supported audio format configurations
     * (e.g AudioFormat.ENCODING_PCM_16BIT)
     * Empty array if format is not relevant for this audio port
     */
    CARAPI Formats(
        /* [out, callee] */ ArrayOf<Int32>** result);

    /**
     * Get the list of gain descriptors
     * Empty array if this port does not have gain control
     */
    CARAPI Gains(
        /* [out, callee] */ ArrayOf<IAudioGain*>** result);

    /**
     * Build a specific configuration of this audio port for use by methods
     * like AudioManager.connectAudioPatch().
     * @param channelMask The desired channel mask. AudioFormat.CHANNEL_OUT_DEFAULT if no change
     * from active configuration requested.
     * @param format The desired audio format. AudioFormat.ENCODING_DEFAULT if no change
     * from active configuration requested.
     * @param gain The desired gain. null if no gain changed requested.
     */
    CARAPI BuildConfig(
        /* [in] */ Int32 samplingRate,
        /* [in] */ Int32 channelMask,
        /* [in] */ Int32 format,
        /* [in] */ IAudioGainConfig* gain,
        /* [out] */ IAudioPortConfig** result);

    /**
     * Get currently active configuration of this audio port.
     */
    CARAPI ActiveConfig(
        /* [out] */ IAudioPortConfig** result);

    CARAPI Handle(
        /* [out]*/ IAudioHandle** result);

    /**
     * Get the gain descriptor at a given index
     */
    CARAPI Gain(
        /* [in] */ Int32 index,
        /* [out]*/ IAudioGain** result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    AutoPtr<IAudioHandle> mHandle;
    Int32 mRole;
    AutoPtr<ArrayOf<Int32> > mSamplingRates;
    AutoPtr<ArrayOf<Int32> > mChannelMasks;
    AutoPtr<ArrayOf<Int32> > mFormats;
    AutoPtr<ArrayOf<IAudioGain*> > mGains;
    AutoPtr<IAudioPortConfig> mActiveConfig;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOPORT_H__
