
#ifndef __ELASTOS_DROID_MEDIA_CMEDIAROUTERSIMPLECALLBACK_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAROUTERSIMPLECALLBACK_H__

#include "_Elastos_Droid_Media_CMediaRouterSimpleCallback.h"
#include "elastos/droid/media/MediaRouterSimpleCallback.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Stub implementation of {@link MediaRouter.Callback}.
 * Each abstract method is defined as a no-op. Override just the ones
 * you need.
 */
CarClass(CMediaRouterSimpleCallback)
    , public MediaRouterSimpleCallback
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIAROUTERSIMPLECALLBACK_H__
