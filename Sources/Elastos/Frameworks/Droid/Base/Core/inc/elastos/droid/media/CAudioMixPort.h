
#ifndef __ELASTOS_DROID_MEDIA_CAUDIOMIXPORT_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOMIXPORT_H__

#include "_Elastos_Droid_Media_CAudioMixPort.h"
#include "elastos/droid/media/AudioPort.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * The AudioMixPort is a specialized type of AudioPort
 * describing an audio mix or stream at an input or output stream of the audio
 * framework.
 * @see AudioPort
 * @hide
 */
CarClass(CAudioMixPort)
    , public AudioPort
    , public IAudioMixPort
{
public:
    CAR_INTERFACE_DECL()

    virtual ~CAudioMixPort();

    CAR_OBJECT_DECL()

    CAudioMixPort();

    CARAPI constructor(
        /* [in] */ IAudioHandle* handle,
        /* [in] */ Int32 role,
        /* [in] */ ArrayOf<Int32>* samplingRates,
        /* [in] */ ArrayOf<Int32>* channelMasks,
        /* [in] */ ArrayOf<Int32>* formats,
        /* [in] */ ArrayOf<IAudioGain*>* gains);

    /**
     * Build a specific configuration of this audio mix port for use by methods
     * like AudioManager.connectAudioPatch().
     */
    CARAPI BuildConfig(
        /* [in] */ Int32 samplingRate,
        /* [in] */ Int32 channelMask,
        /* [in] */ Int32 format,
        /* [in] */ IAudioGainConfig* gain,
        /* [out] */ IAudioMixPortConfig** result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAUDIOMIXPORT_H__
