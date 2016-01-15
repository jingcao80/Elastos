#ifndef __ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAudioMixBuilder_H__
#define __ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAudioMixBuilder_H__

#include "_Elastos_Droid_Media_AudioPolicy_CAudioMixBuilder.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Media::IAudioFormat;
using Elastos::Droid::Media::AudioPolicy::IAudioMix;
using Elastos::Droid::Media::AudioPolicy::IAudioMixingRule;

namespace Elastos {
namespace Droid {
namespace Media {
namespace AudioPolicy {

/**
 * Builder class for {@link AudioMix} objects
 *
 */
CarClass(CAudioMixBuilder)
    , public Object
    , public IAudioMixBuilder
{
public:
    CAudioMixBuilder();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CAudioMixBuilder();

    CARAPI constructor();

    /**
     * Construct an instance for the given {@link AudioMixingRule}.
     * @param rule a non-null {@link AudioMixingRule} instance.
     * @throws IllegalArgumentException
     */
    CARAPI constructor(
        /* [in] */ IAudioMixingRule * rule);

    /**
     * @hide
     * Only used by AudioPolicyConfig, not a public API.
     * @param rule
     * @return the same Builder instance.
     * @throws IllegalArgumentException
     */
    CARAPI SetMixingRule(
        /* [in] */ IAudioMixingRule * rule,
        /* [out] */ IAudioMixBuilder** builder);

    /**
     * Sets the {@link AudioFormat} for the mix.
     * @param format a non-null {@link AudioFormat} instance.
     * @return the same Builder instance.
     * @throws IllegalArgumentException
     */
    CARAPI SetFormat(
        /* [in] */ IAudioFormat * format,
        /* [out] */ IAudioMixBuilder** builder);

    /**
     * Sets the routing behavior for the mix.
     * @param routeFlags one of {@link AudioMix#ROUTE_FLAG_LOOP_BACK},
     *     {@link AudioMix#ROUTE_FLAG_RENDER}
     * @return the same Builder instance.
     * @throws IllegalArgumentException
     */
    CARAPI SetRouteFlags(
        /* [in] */ Int32 routeFlags,
        /* [out] */ IAudioMixBuilder** builder);

    /**
     * Combines all of the settings and return a new {@link AudioMix} object.
     * @return a new {@link AudioMix} object
     * @throws IllegalArgumentException if no {@link AudioMixingRule} has been set.
     */
    CARAPI Build(
        /* [out] */ IAudioMix** result);

private:
    AutoPtr<IAudioMixingRule> mRule;
    AutoPtr<IAudioFormat> mFormat;
    Int32 mRouteFlags;

};

} // namespace AudioPolicy
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAudioMixBuilder_H__
