#ifndef __ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOMIX_H__
#define __ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOMIX_H__

#include "_Elastos_Droid_Media_AudioPolicy_CAudioMix.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace AudioPolicy {

/**
 * @hide CANDIDATE FOR PUBLIC API
 */
CarClass(CAudioMix)
    , public Object
    , public IAudioMix
{
public:
    CAudioMix();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CAudioMix();

    /**
     * All parameters are guaranteed valid through the Builder.
     */
    CARAPI constructor(
        /* [in] */ IAudioMixingRule * rule,
        /* [in] */ IAudioFormat * format,
        /* [in] */ Int32 routeFlags);

private:
    Int32 GetRouteFlags();

    CARAPI_(AutoPtr<IAudioFormat>) GetFormat();

    CARAPI_(AutoPtr<IAudioMixingRule>) GetRule();

private:
    AutoPtr<IAudioMixingRule> mRule;
    AutoPtr<IAudioFormat> mFormat;
    Int32 mRouteFlags;

};

} // namespace AudioPolicy
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOMIX_H__
