
#ifndef __ELASTOS_DROID_MEDIA_CMEDIAROUTERROUTEINFO_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAROUTERROUTEINFO_H__

#include "_Elastos_Droid_Media_CMediaRouterRouteInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/MediaRouterRouteInfo.h"

using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Information about a media route.
 */
CarClass(CMediaRouterRouteInfo)
    , public MediaRouterRouteInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIAROUTERROUTEINFO_H__
