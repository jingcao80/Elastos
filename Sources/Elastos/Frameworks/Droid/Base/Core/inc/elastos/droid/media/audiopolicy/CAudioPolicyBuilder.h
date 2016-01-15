#ifndef __ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOPOLICYBUILDER_H__
#define __ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOPOLICYBUILDER_H__

#include "_Elastos_Droid_Media_AudioPolicy_CAudioPolicyBuilder.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IArrayList;
using Elastos::Droid::Media::AudioPolicy::IAudioMix;
using Elastos::Droid::Media::AudioPolicy::IAudioPolicy;

namespace Elastos {
namespace Droid {
namespace Media {
namespace AudioPolicy {

/**
 * Builder class for {@link AudioPolicy} objects
 *
 */
CarClass(CAudioPolicyBuilder)
    , public Object
    , public IAudioPolicyBuilder
{
public:
    CAudioPolicyBuilder();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CAudioPolicyBuilder();

    /**
     * Constructs a new Builder with no audio mixes.
     */
    CARAPI constructor();

    /**
     * Add an {@link AudioMix} to be part of the audio policy being built.
     * @param mix a non-null {@link AudioMix} to be part of the audio policy.
     * @return the same Builder instance.
     * @throws IllegalArgumentException
     */
    CARAPI AddMix(
        /* [in] */ IAudioMix * mix,
        /* [out] */ IAudioPolicyBuilder ** builder);

    CARAPI Build(
        /* [out] */ IAudioPolicy** result);

private:
    AutoPtr<IArrayList> mMixes;

};

} // namespace AudioPolicy
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOPOLICYBUILDER_H__
