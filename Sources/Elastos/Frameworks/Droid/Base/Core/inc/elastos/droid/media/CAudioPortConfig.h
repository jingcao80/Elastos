
#ifndef __ELASTOS_DROID_MEDIA_CAUDIOPORTCONFIG_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOPORTCONFIG_H__

#include "_Elastos_Droid_Media_CAudioPortConfig.h"
#include "elastos/droid/media/AudioPortConfig.h"

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Stub implementation of {@link MediaRouter.Callback}.
 * Each abstract method is defined as a no-op. Override just the ones
 * you need.
 */
CarClass(CAudioPortConfig)
    , public AudioPortConfig
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAUDIOPORTCONFIG_H__
