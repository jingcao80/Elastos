#ifndef __ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOPOLICYCONFIG_H__
#define __ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOPOLICYCONFIG_H__

#include "_Elastos_Droid_Media_AudioPolicy_CAudioPolicyConfig.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Media {
namespace AudioPolicy {

/**
 * @hide
 * Internal storage class for AudioPolicy configuration.
 */
CarClass(CAudioPolicyConfig)
    , public Object
    , public IAudioPolicyConfig
    , public IParcelable
{
public:
    CAudioPolicyConfig();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CAudioPolicyConfig();

    CARAPI constructor(
        /* [in] */ IArrayList * mixes);

    /**
     * Add an {@link AudioMix} to be part of the audio policy being built.
     * @param mix a non-null {@link AudioMix} to be part of the audio policy.
     * @return the same Builder instance.
     * @throws IllegalArgumentException
     */
    CARAPI AddMix(
        /* [in] */ IAudioMix * mix);

    CARAPI ToString(
        /* [out] */ String * result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    AutoPtr<IArrayList> mMixes;

private:
    static String TAG; // = "AudioPolicyConfig";
};

} // namespace AudioPolicy
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOPOLICYCONFIG_H__
