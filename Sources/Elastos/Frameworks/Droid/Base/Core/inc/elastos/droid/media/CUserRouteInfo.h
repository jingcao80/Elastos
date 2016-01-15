
#ifndef __ELASTOS_DROID_MEDIA_CUSERROUTEINFO_H__
#define __ELASTOS_DROID_MEDIA_CUSERROUTEINFO_H__

#include "_Elastos_Droid_Media_CUserRouteInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/MediaRouteInfo.h"

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
CarClass(CUserRouteInfo), public MediaRouteInfo
{
public:
    CUserRouteInfo();

    CARAPI constructor(
        /* [in] */ IRouteCategory* category);

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    /**
     * Set the user-visible name of this route.
     * @param name Name to display to the user to describe this route
     */
    CARAPI SetName(
        /* [in] */ ICharSequence* name);

    /**
     * Set the user-visible name of this route.
     * @param resId Resource ID of the name to display to the user to describe this route
     */
    CARAPI SetName(
        /* [in] */ Int32 resId);

    /**
     * Set the current user-visible status for this route.
     * @param status Status to display to the user to describe what the endpoint
     * of this route is currently doing
     */
    CARAPI SetStatus(
        /* [in] */ ICharSequence* status);

    /**
     * Set the RemoteControlClient responsible for reporting playback info for this
     * user route.
     *
     * <p>If this route manages remote playback, the data exposed by this
     * RemoteControlClient will be used to reflect and update information
     * such as route volume info in related UIs.</p>
     *
     * <p>The RemoteControlClient must have been previously registered with
     * {@link AudioManager#registerRemoteControlClient(RemoteControlClient)}.</p>
     *
     * @param rcc RemoteControlClient associated with this route
     */
    CARAPI SetRemoteControlClient(
        /* [in] */ IRemoteControlClient* rcc);

    /**
     * Retrieve the RemoteControlClient associated with this route, if one has been set.
     *
     * @return the RemoteControlClient associated with this route
     * @see #setRemoteControlClient(RemoteControlClient)
     */
    CARAPI GetRemoteControlClient(
        /* [out] */ IRemoteControlClient** result);

    /**
     * Set an icon that will be used to represent this route.
     * The system may use this icon in picker UIs or similar.
     *
     * @param icon icon drawable to use to represent this route
     */
    CARAPI SetIconDrawable(
        /* [in] */ IDrawable* icon);

    /**
     * Set an icon that will be used to represent this route.
     * The system may use this icon in picker UIs or similar.
     *
     * @param resId Resource ID of an icon drawable to use to represent this route
     */
    CARAPI SetIconResource(
        /* [in] */ Int32 resId);

    /**
     * Set a callback to be notified of volume update requests
     * @param vcb
     */
    CARAPI SetVolumeCallback(
        /* [in] */ IVolumeCallback* vcb);

    /**
     * Defines whether playback associated with this route is "local"
     *    ({@link RouteInfo#PLAYBACK_TYPE_LOCAL}) or "remote"
     *    ({@link RouteInfo#PLAYBACK_TYPE_REMOTE}).
     * @param type
     */
    CARAPI SetPlaybackType(
        /* [in] */ Int32 type);

    /**
     * Defines whether volume for the playback associated with this route is fixed
     * ({@link RouteInfo#PLAYBACK_VOLUME_FIXED}) or can modified
     * ({@link RouteInfo#PLAYBACK_VOLUME_VARIABLE}).
     * @param volumeHandling
     */
    CARAPI SetVolumeHandling(
        /* [in] */ Int32 volumeHandling);

    /**
     * Defines at what volume the playback associated with this route is performed (for user
     * feedback purposes). This information is only used when the playback is not local.
     * @param volume
     */
    CARAPI SetVolume(
        /* [in] */ Int32 volume);

    CARAPI RequestSetVolume(
        /* [in] */ Int32 volume);

    CARAPI RequestUpdateVolume(
        /* [in] */ Int32 direction);

    /**
     * Defines the maximum volume at which the playback associated with this route is performed
     * (for user feedback purposes). This information is only used when the playback is not
     * local.
     * @param volumeMax
     */
    CARAPI SetVolumeMax(
        /* [in] */ Int32 volumeMax);

    /**
     * Defines over what stream type the media is presented.
     * @param stream
     */
    CARAPI SetPlaybackStream(
        /* [in] */ Int32 stream);

//-------------------------------
//    MediaRouteInfo
//-------------------------------

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

    CARAPI SetSupportedTypes(
        /* [in] */ Int32 supportedTypes);

    CARAPI SetPresentationDisplay(
        /* [in] */ IDisplay* presentationDisplay);

    CARAPI GetDeviceAddress(
        /* [out] */ String* result);

    CARAPI SetDeviceAddress(
        /* [in] */ const String& deviceAddress);

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

private:
    CARAPI_(void) UpdatePlaybackInfoOnRcc();

    CARAPI_(void) SetPlaybackInfoOnRcc(
        /* [in] */ Int32 what,
        /* [in] */ Int32 value);

private:
    AutoPtr<IRemoteControlClient> mRcc;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __CMEDIAROUTER_H__
