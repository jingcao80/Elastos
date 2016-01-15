#ifndef __ELASTOS_DROID_MEDIA_MEDIAROUTERSIMPLECALLBACK_H__
#define __ELASTOS_DROID_MEDIA_MEDIAROUTERSIMPLECALLBACK_H__

#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Stub implementation of {@link MediaRouter.Callback}.
 * Each abstract method is defined as a no-op. Override just the ones
 * you need.
 */
class MediaRouterSimpleCallback
    : public Object
    , public IMediaRouterSimpleCallback
    , public IMediaRouterCallback
{
public:
    MediaRouterSimpleCallback();

    CAR_INTERFACE_DECL()

    virtual ~MediaRouterSimpleCallback();

    CARAPI constructor();

    /**
     * Called when the supplied route becomes selected as the active route
     * for the given route type.
     *
     * @param router the MediaRouter reporting the event
     * @param type Type flag set indicating the routes that have been selected
     * @param info Route that has been selected for the given route types
     */
    CARAPI OnRouteSelected(
        /* [in] */ IMediaRouter* router,
        /* [in] */ Int32 type,
        /* [in] */ IMediaRouterRouteInfo* info);

    /**
     * Called when the supplied route becomes unselected as the active route
     * for the given route type.
     *
     * @param router the MediaRouter reporting the event
     * @param type Type flag set indicating the routes that have been unselected
     * @param info Route that has been unselected for the given route types
     */
    CARAPI OnRouteUnselected(
        /* [in] */ IMediaRouter* router,
        /* [in] */ Int32 type,
        /* [in] */ IMediaRouterRouteInfo* info);

    /**
     * Called when a route for the specified type was added.
     *
     * @param router the MediaRouter reporting the event
     * @param info Route that has become available for use
     */
    CARAPI OnRouteAdded(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info);

    /**
     * Called when a route for the specified type was removed.
     *
     * @param router the MediaRouter reporting the event
     * @param info Route that has been removed from availability
     */
    CARAPI OnRouteRemoved(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info);

    /**
     * Called when an aspect of the indicated route has changed.
     *
     * <p>This will not indicate that the types supported by this route have
     * changed, only that cosmetic info such as name or status have been updated.</p>
     *
     * @param router the MediaRouter reporting the event
     * @param info The route that was changed
     */
    CARAPI OnRouteChanged(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info);

    /**
     * Called when a route is added to a group.
     *
     * @param router the MediaRouter reporting the event
     * @param info The route that was added
     * @param group The group the route was added to
     * @param index The route index within group that info was added at
     */
    CARAPI OnRouteGrouped(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info,
        /* [in] */ IMediaRouterRouteGroup* group,
        /* [in] */ Int32 index);

    /**
     * Called when a route is removed from a group.
     *
     * @param router the MediaRouter reporting the event
     * @param info The route that was removed
     * @param group The group the route was removed from
     */
    CARAPI OnRouteUngrouped(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info,
        /* [in] */ IMediaRouterRouteGroup* group);

    /**
     * Called when a route's volume changes.
     *
     * @param router the MediaRouter reporting the event
     * @param info The route with altered volume
     */
    CARAPI OnRouteVolumeChanged(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info);

    /**
     * Called when a route's presentation display changes.
     * <p>
     * This method is called whenever the route's presentation display becomes
     * available, is removes or has changes to some of its properties (such as its size).
     * </p>
     *
     * @param router the MediaRouter reporting the event
     * @param info The route whose presentation display changed
     *
     * @see RouteInfo#getPresentationDisplay()
     */
    CARAPI OnRoutePresentationDisplayChanged(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_MEDIAROUTERSIMPLECALLBACK_H__
