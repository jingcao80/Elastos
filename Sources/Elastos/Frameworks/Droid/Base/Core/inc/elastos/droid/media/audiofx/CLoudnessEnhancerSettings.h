
#ifndef __ELASTOS_DROID_MEDIA_AUDIOFX_CLOUDNESSENHANCERSETTINGS_H__
#define __ELASTOS_DROID_MEDIA_AUDIOFX_CLOUDNESSENHANCERSETTINGS_H__

#include "_Elastos_Droid_Media_Audiofx_CLoudnessEnhancerSettings.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

/**
 * LoudnessEnhancer is an audio effect for increasing audio loudness.
 * The processing is parametrized by a target gain value, which determines the maximum amount
 * by which an audio signal will be amplified; signals amplified outside of the sample
 * range supported by the platform are compressed.
 * An application creates a LoudnessEnhancer object to instantiate and control a
 * this audio effect in the audio framework.
 * To attach the LoudnessEnhancer to a particular AudioTrack or MediaPlayer,
 * specify the audio session ID of this AudioTrack or MediaPlayer when constructing the effect
 * (see {@link AudioTrack#getAudioSessionId()} and {@link MediaPlayer#getAudioSessionId()}).
 */
CarClass(CLoudnessEnhancerSettings)
    , public Object
    , public ILoudnessEnhancerSettings
{
public:
    CLoudnessEnhancerSettings();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Settings class constructor from a key=value; pairs formatted string. The string is
     * typically returned by Settings.toString() method.
     * @throws IllegalArgumentException if the string is not correctly formatted.
     */
    CARAPI constructor(
        /* [in] */ const String& settings);

    CARAPI ToString(
        /* [out] */ String* result);

public:
    Int32 mTargetGainmB;
};

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOFX_CLOUDNESSENHANCERSETTINGS_H__
