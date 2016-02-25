
#ifndef __ELASTOS_DROID_MEDIA_CMEDIAROUTERUSERROUTEINFO_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAROUTERUSERROUTEINFO_H__

#include "_Elastos_Droid_Media_CMediaRouterUserRouteInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/MediaRouterUserRouteInfo.h"

using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Information about a route that the application may define and modify.
 * A user route defaults to {@link RouteInfo#PLAYBACK_TYPE_REMOTE} and
 * {@link RouteInfo#PLAYBACK_VOLUME_FIXED}.
 *
 * @see MediaRouter.RouteInfo
 */
CarClass(CMediaRouterUserRouteInfo)
    , public MediaRouterUserRouteInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIAROUTERUSERROUTEINFO_H__
