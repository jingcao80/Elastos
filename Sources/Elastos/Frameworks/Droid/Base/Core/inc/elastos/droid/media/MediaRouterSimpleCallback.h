#ifndef __ELASTOS_DROID_MEDIA_MEDIAROUTERSIMPLECALLBACK_H__
#define __ELASTOS_DROID_MEDIA_MEDIAROUTERSIMPLECALLBACK_H__

#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "Elastos.Droid.Media.h"

namespace Elastos {
namespace Droid {
namespace Media {

class ECO_PUBLIC MediaRouterSimpleCallback
    : public Object
    , public IMediaRouterCallback
    , public IMediaRouterSimpleCallback
{
public:
    CAR_INTERFACE_DECL()

    MediaRouterSimpleCallback();

    virtual ~MediaRouterSimpleCallback();

    CARAPI constructor();

    CARAPI OnRouteSelected(
        /* [in] */ IMediaRouter* router,
        /* [in] */ Int32 type,
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI OnRouteUnselected(
        /* [in] */ IMediaRouter* router,
        /* [in] */ Int32 type,
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI OnRouteAdded(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI OnRouteRemoved(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI OnRouteChanged(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI OnRouteGrouped(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info,
        /* [in] */ IMediaRouterRouteGroup* group,
        /* [in] */ Int32 index);

    CARAPI OnRouteUngrouped(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info,
        /* [in] */ IMediaRouterRouteGroup* group);

    CARAPI OnRouteVolumeChanged(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI OnRoutePresentationDisplayChanged(
        /* [in] */ IMediaRouter * router,
        /* [in] */ IMediaRouterRouteInfo * info);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_MEDIAROUTERSIMPLECALLBACK_H__