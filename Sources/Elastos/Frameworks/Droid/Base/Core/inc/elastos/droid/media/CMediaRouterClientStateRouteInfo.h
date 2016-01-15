#ifndef _ELASTOS_DROID_MEDIA_CMEDIAROUTERCLIENTSTATEROUTEINFO_H__
#define _ELASTOS_DROID_MEDIA_CMEDIAROUTERCLIENTSTATEROUTEINFO_H__

#include "_Elastos_Droid_Media_CMediaRouterClientStateRouteInfo.h"
#include "elastos/droid/media/MediaRouterClientState.h"

namespace Elastos{
namespace Droid {
namespace Media {

CarClass(CMediaRouterClientStateRouteInfo)
    , public MediaRouterClientState::RouteInfo
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // _ELASTOS_DROID_MEDIA_CMEDIAROUTERCLIENTSTATEROUTEINFO_H__