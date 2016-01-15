
#ifndef __ELASTOS_DROID_MEDIA_CMEDIAROUTEINFO_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAROUTEINFO_H__

#include "_Elastos_Droid_Media_CMediaRouteInfo.h"
#include "MediaRouteInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/CMediaRouter.h"

using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Information about a media route.
 */
CarClass(CMediaRouteInfo), public MediaRouteInfo
{
public:
    CARAPI constructor(
        /* [in] */ IRouteCategory* category);

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    /**
     * @return The user-friendly name of a media route. This is the string presented
     * to users who may select this as the active route.
     */
    CARAPI GetName(
        /* [out] */ ICharSequence** result);

    /**
     * Return the properly localized/resource selected name of this route.
     *
     * @param context Context used to resolve the correct configuration to load
     * @return The user-friendly name of the media route. This is the string presented
     * to users who may select this as the active route.
     */
    CARAPI GetName(
        /* [in] */ IContext* context,
        /* [out] */ ICharSequence** result);

    /**
     * @return The user-friendly status for a media route. This may include a description
     * of the currently playing media, if available.
     */
    CARAPI GetStatus(
        /* [out] */ ICharSequence** result);

    /**
     * @hide
     */
    CARAPI GetStatusCode(
        /* [out] */ Int32* result);

    /**
     * @return A media type flag set describing which types this route supports.
     */
    CARAPI GetSupportedTypes(
        /* [out] */ Int32* result);

    CARAPI SetGroup(
        /* [in] */ IRouteGroup* group);

    /**
     * @return The group that this route belongs to.
     */
    CARAPI GetGroup(
        /* [out] */ IRouteGroup** result);

    CARAPI SetCategory(
        /* [in] */ IRouteCategory* category);

    /**
     * @return the category this route belongs to.
     */
    CARAPI GetCategory(
        /* [out] */ IRouteCategory** result);

    /**
     * Get the icon representing this route.
     * This icon will be used in picker UIs if available.
     *
     * @return the icon representing this route or null if no icon is available
     */
    CARAPI GetIconDrawable(
        /* [out] */ IDrawable** result);

    /**
     * Set an application-specific tag object for this route.
     * The application may use this to store arbitrary data associated with the
     * route for internal tracking.
     *
     * <p>Note that the lifespan of a route may be well past the lifespan of
     * an Activity or other Context; take care that objects you store here
     * will not keep more data in memory alive than you intend.</p>
     *
     * @param tag Arbitrary, app-specific data for this route to hold for later use
     */
    CARAPI SetTag(
        /* [in] */ IInterface* tag);

    /**
     * @return The tag object previously set by the application
     * @see #setTag(Object)
     */
    CARAPI GetTag(
        /* [out] */ IInterface** result);

    /**
     * @return the type of playback associated with this route
     * @see UserRouteInfo#setPlaybackType(int)
     */
    CARAPI GetPlaybackType(
        /* [out] */ Int32* result);

    /**
     * @return the stream over which the playback associated with this route is performed
     * @see UserRouteInfo#setPlaybackStream(int)
     */
    CARAPI GetPlaybackStream(
        /* [out] */ Int32* result);

    /**
     * Return the current volume for this route. Depending on the route, this may only
     * be valid if the route is currently selected.
     *
     * @return the volume at which the playback associated with this route is performed
     * @see UserRouteInfo#setVolume(int)
     */
    CARAPI GetVolume(
        /* [out] */ Int32* result);

    /**
     * Request a volume change for this route.
     * @param volume value between 0 and getVolumeMax
     */
    CARAPI RequestSetVolume(
        /* [in] */ Int32 volume);

    /**
     * Request an incremental volume update for this route.
     * @param direction Delta to apply to the current volume
     */
    CARAPI RequestUpdateVolume(
        /* [in] */ Int32 direction);

    /**
     * @return the maximum volume at which the playback associated with this route is performed
     * @see UserRouteInfo#setVolumeMax(int)
     */
    CARAPI GetVolumeMax(
        /* [out] */ Int32* result);

    /**
     * @return how volume is handling on the route
     * @see UserRouteInfo#setVolumeHandling(int)
     */
    CARAPI GetVolumeHandling(
        /* [out] */ Int32* result);

    /**
     * Gets the {@link Display} that should be used by the application to show
     * a {@link android.app.Presentation} on an external display when this route is selected.
     * Depending on the route, this may only be valid if the route is currently
     * selected.
     * <p>
     * The preferred presentation display may change independently of the route
     * being selected or unselected.  For example, the presentation display
     * of the default system route may change when an external HDMI display is connected
     * or disconnected even though the route itself has not changed.
     * </p><p>
     * This method may return null if there is no external display associated with
     * the route or if the display is not ready to show UI yet.
     * </p><p>
     * The application should listen for changes to the presentation display
     * using the {@link Callback#onRoutePresentationDisplayChanged} callback and
     * show or dismiss its {@link android.app.Presentation} accordingly when the display
     * becomes available or is removed.
     * </p><p>
     * This method only makes sense for {@link #ROUTE_TYPE_LIVE_VIDEO live video} routes.
     * </p>
     *
     * @return The preferred presentation display to use when this route is
     * selected or null if none.
     *
     * @see #ROUTE_TYPE_LIVE_VIDEO
     * @see android.app.Presentation
     */
    CARAPI GetPresentationDisplay(
        /* [out] */ IDisplay** result);

    /**
     * @return true if this route is enabled and may be selected
     */
    CARAPI IsEnabled(
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI SetName(
        /* [in] */ Int32 nameResId);

    CARAPI SetSupportedTypes(
        /* [in] */ Int32 supportedTypes);

    CARAPI SetPresentationDisplay(
        /* [in] */ IDisplay* presentationDisplay);

    CARAPI SetName(
        /* [in] */ ICharSequence* name);

    CARAPI GetDeviceAddress(
        /* [out] */ String* result);

    CARAPI SetDeviceAddress(
        /* [in] */ const String& deviceAddress);

    CARAPI SetVolumeHandling(
        /* [in] */ Int32 volumeHandling);

    CARAPI SetPlaybackType(
        /* [in] */ Int32 playbackType);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI GetEnabled(
        /* [out] */ Boolean* result);

    /**
     * Set this route's status by predetermined status code. If the caller
     * should dispatch a route changed event this call will return true;
     */
    CARAPI SetStatusCode(
        /* [in] */ Int32 statusCode,
        /* [out] */ Boolean* result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIAROUTEINFO_H__
