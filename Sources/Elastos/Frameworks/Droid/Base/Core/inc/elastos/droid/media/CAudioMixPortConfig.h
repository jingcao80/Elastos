
#ifndef __ELASTOS_DROID_MEDIA_CAUDIOMIXPORTCONFIG_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOMIXPORTCONFIG_H__

#include "_Elastos_Droid_Media_CAudioMixPortConfig.h"
#include "elastos/droid/media/AudioPortConfig.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * An AudioMixPortConfig describes a possible configuration of an output or input mixer.
 * It is used to specify a sink or source when creating a connection with
 * AudioManager.connectAudioPatch().
 * An AudioMixPortConfig is obtained from AudioMixPort.buildConfig().
 * @hide
 */
CarClass(CAudioMixPortConfig)
    , public AudioPortConfig
    , public IAudioMixPortConfig
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAudioMixPortConfig();

    virtual ~CAudioMixPortConfig();

    CARAPI constructor(
        /* [in] */ IAudioMixPort* mixPort,
        /* [in] */ Int32 samplingRate,
        /* [in] */ Int32 channelMask,
        /* [in] */ Int32 format,
        /* [in] */ IAudioGainConfig* gain);

    /**
     * Returns the audio mix port this AudioMixPortConfig is issued from.
     */
    CARAPI Port(
        /* [out] */ IAudioMixPort** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAUDIOMIXPORTCONFIG_H__
