
#ifndef __ELASTOS_DROID_MEDIA_CAUDIODEVICEPORTCONFIG_H__
#define __ELASTOS_DROID_MEDIA_CAUDIODEVICEPORTCONFIG_H__

#include "_Elastos_Droid_Media_CAudioDevicePortConfig.h"
#include "elastos/droid/media/AudioPortConfig.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * An AudioDevicePortConfig describes a possible configuration of an output or input device
 * (speaker, headphone, microphone ...).
 * It is used to specify a sink or source when creating a connection with
 * AudioManager.connectAudioPatch().
 * An AudioDevicePortConfig is obtained from AudioDevicePort.buildConfig().
 * @hide
 */
CarClass(CAudioDevicePortConfig)
    , public AudioPortConfig
    , public IAudioDevicePortConfig
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAudioDevicePortConfig();

    virtual ~CAudioDevicePortConfig();

    CARAPI constructor(
        /* [in] */ IAudioDevicePort* devicePort,
        /* [in] */ Int32 samplingRate,
        /* [in] */ Int32 channelMask,
        /* [in] */ Int32 format,
        /* [in] */ IAudioGainConfig* gain);

    CARAPI constructor(
        /* [in] */ IAudioDevicePortConfig* config);

    /**
     * Returns the audio device port this AudioDevicePortConfig is issued from.
     */
    CARAPI Port(
        /* [out] */ IAudioDevicePort** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAUDIODEVICEPORTCONFIG_H__
