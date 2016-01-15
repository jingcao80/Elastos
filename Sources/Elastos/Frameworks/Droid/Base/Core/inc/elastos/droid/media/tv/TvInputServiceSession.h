#ifndef __ELASTOS_DROID_MEDIA_TV_TVINPUTSERVICESESSION_H__
#define __ELASTOS_DROID_MEDIA_TV_TVINPUTSERVICESESSION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IDispatcherState;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IInputEventReceiver;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Core::IRunnable;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * Base class for derived classes to implement to provide a TV input session.
 */
class TvInputServiceSession
    : public Object
    , public ITvInputServiceSession
    , public IKeyEventCallback
{
private:
    class SetOverlayViewEnabledRunnable
        : public Runnable
    {
    public:
        SetOverlayViewEnabledRunnable(
            /* [in] */ TvInputServiceSession* host,
            /* [in] */ Boolean value)
            : mHost(host)
            , mEnable(value)
        {}

        CARAPI Run();

    private:
        TvInputServiceSession* mHost;
        Boolean mEnable;
    };

    class NotifySessionEventRunnable
        : public Runnable
    {
    public:
        NotifySessionEventRunnable(
            /* [in] */ TvInputServiceSession* host,
            /* [in] */ const String& eventType,
            /* [in] */ IBundle* eventArgs)
            : mHost(host)
            , mEventType(eventType)
            , mEventArgs(eventArgs)
        {}

        CARAPI Run();

    private:
        TvInputServiceSession* mHost;
        String mEventType;
        AutoPtr<IBundle> mEventArgs;
    };

    class NotifyChannelRetunedRunnable
        : public Runnable
    {
    public:
        NotifyChannelRetunedRunnable(
            /* [in] */ TvInputServiceSession* host,
            /* [in] */ IUri* channelUri)
            : mHost(host)
            , mChannelUri(channelUri)
        {}

        CARAPI Run();

    private:
        TvInputServiceSession* mHost;
        AutoPtr<IUri> mChannelUri;
    };

    class NotifyTracksChangedRunnable
        : public Runnable
    {
    public:
        NotifyTracksChangedRunnable(
            /* [in] */ TvInputServiceSession* host,
            /* [in] */ IList* tracks)
            : mHost(host)
            , mTracks(tracks)
        {}

        CARAPI Run();

    private:
        TvInputServiceSession* mHost;
        AutoPtr<IList> mTracks;
    };

    class NotifyTrackSelectedRunnable
        : public Runnable
    {
    public:
        NotifyTrackSelectedRunnable(
            /* [in] */ TvInputServiceSession* host,
            /* [in] */ Int32 type,
            /* [in] */ const String& trackId)
            : mHost(host)
            , mType(type)
            , mTrackId(trackId)
        {}

        CARAPI Run();

    private:
        TvInputServiceSession* mHost;
        Int32 mType;
        String mTrackId;
    };

    class NotifyVideoAvailableRunnable
        : public Runnable
    {
    public:
        NotifyVideoAvailableRunnable(
            /* [in] */ TvInputServiceSession* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        TvInputServiceSession* mHost;
    };

    class NotifyVideoUnavailableRunnable
        : public Runnable
    {
    public:
        NotifyVideoUnavailableRunnable(
            /* [in] */ TvInputServiceSession* host,
            /* [in] */ Int32 reason)
            : mHost(host)
            , mReason(reason)
        {}

        CARAPI Run();

    private:
        TvInputServiceSession* mHost;
        Int32 mReason;
    };

    class NotifyContentAllowedRunnable
        : public Runnable
    {
    public:
        NotifyContentAllowedRunnable(
            /* [in] */ TvInputServiceSession* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        TvInputServiceSession* mHost;
    };

    class NotifyContentBlockedRunnable
        : public Runnable
    {
    public:
        NotifyContentBlockedRunnable(
            /* [in] */ TvInputServiceSession* host,
            /* [in] */ ITvContentRating* rating)
            : mHost(host)
            , mRating(rating)
        {}

        CARAPI Run();

    private:
        TvInputServiceSession* mHost;
        AutoPtr<ITvContentRating> mRating;
    };

    class LayoutSurfaceRunnable
        : public Runnable
    {
    public:
        LayoutSurfaceRunnable(
            /* [in] */ TvInputServiceSession* host,
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottm)
            : mHost(host)
            , mLeft(left)
            , mTop(top)
            , mRight(right)
            , mBottm(bottm)
        {}

        CARAPI Run();

    private:
        TvInputServiceSession* mHost;
        Int32 mLeft;
        Int32 mTop;
        Int32 mRight;
        Int32 mBottm;
    };

public:
    friend class TvInputService;

    TvInputServiceSession();

    virtual ~TvInputServiceSession();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Creates a new Session.
     *
     * @param context The context of the application
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Enables or disables the overlay view. By default, the overlay view is disabled. Must be
     * called explicitly after the session is created to enable the overlay view.
     *
     * @param enable {@code true} if you want to enable the overlay view. {@code false}
     *            otherwise.
     */
    CARAPI SetOverlayViewEnabled(
        /* [in] */ Boolean value);

    /**
     * Dispatches an event to the application using this session.
     *
     * @param eventType The type of the event.
     * @param eventArgs Optional arguments of the event.
     * @hide
     */
    CARAPI NotifySessionEvent(
        /* [in] */ const String& eventType,
        /* [in] */ IBundle* eventArgs);

    /**
     * Notifies the channel of the session is retuned by TV input.
     *
     * @param channelUri The URI of a channel.
     */
    CARAPI NotifyChannelRetuned(
        /* [in] */ IUri* channelUri);

    /**
     * Sends the change on the track information. This is expected to be called whenever a track
     * is added/removed and the metadata of a track is modified.
     *
     * @param tracks A list which includes track information.
     * @throws IllegalArgumentException if {@code tracks} contains redundant tracks.
     */
    CARAPI NotifyTracksChanged(
        /* [in] */ IList* tracks);

    /**
     * Sends the ID of the selected track for a given track type. This is expected to be called
     * whenever there is a change on track selection.
     *
     * @param type The type of the selected track. The type can be
     *            {@link TvTrackInfo#TYPE_AUDIO}, {@link TvTrackInfo#TYPE_VIDEO} or
     *            {@link TvTrackInfo#TYPE_SUBTITLE}.
     * @param trackId The ID of the selected track.
     * @see #onSelectTrack
     */
    CARAPI NotifyTrackSelected(
        /* [in] */ Int32 type,
        /* [in] */ const String& trackId);

    /**
     * Informs the application that the video is now available for watching. This is primarily
     * used to signal the application to unblock the screen. The TV input service must call this
     * method as soon as the content rendered onto its surface gets ready for viewing.
     *
     * @see #notifyVideoUnavailable
     */
    CARAPI NotifyVideoAvailable();

    /**
     * Informs the application that the video became unavailable for some reason. This is
     * primarily used to signal the application to block the screen not to show any intermittent
     * video artifacts.
     *
     * @param reason The reason why the video became unavailable:
     *            <ul>
     *            <li>{@link TvInputManager#VIDEO_UNAVAILABLE_REASON_UNKNOWN}
     *            <li>{@link TvInputManager#VIDEO_UNAVAILABLE_REASON_TUNING}
     *            <li>{@link TvInputManager#VIDEO_UNAVAILABLE_REASON_WEAK_SIGNAL}
     *            <li>{@link TvInputManager#VIDEO_UNAVAILABLE_REASON_BUFFERING}
     *            </ul>
     * @see #notifyVideoAvailable
     */
    CARAPI NotifyVideoUnavailable(
        /* [in] */ Int32 reason);

    /**
     * Informs the application that the user is allowed to watch the current program content.
     * <p>
     * Each TV input service is required to query the system whether the user is allowed to
     * watch the current program before showing it to the user if the parental controls is
     * enabled (i.e. {@link TvInputManager#isParentalControlsEnabled
     * TvInputManager.isParentalControlsEnabled()} returns {@code true}). Whether the TV input
     * service should block the content or not is determined by invoking
     * {@link TvInputManager#isRatingBlocked TvInputManager.isRatingBlocked(TvContentRating)}
     * with the content rating for the current program. Then the {@link TvInputManager} makes a
     * judgment based on the user blocked ratings stored in the secure settings and returns the
     * result. If the rating in question turns out to be allowed by the user, the TV input
     * service must call this method to notify the application that is permitted to show the
     * content.
     * </p><p>
     * Each TV input service also needs to continuously listen to any changes made to the
     * parental controls settings by registering a broadcast receiver to receive
     * {@link TvInputManager#ACTION_BLOCKED_RATINGS_CHANGED} and
     * {@link TvInputManager#ACTION_PARENTAL_CONTROLS_ENABLED_CHANGED} and immediately
     * reevaluate the current program with the new parental controls settings.
     * </p>
     *
     * @see #notifyContentBlocked
     * @see TvInputManager
     */
    CARAPI NotifyContentAllowed();

    /**
     * Informs the application that the current program content is blocked by parent controls.
     * <p>
     * Each TV input service is required to query the system whether the user is allowed to
     * watch the current program before showing it to the user if the parental controls is
     * enabled (i.e. {@link TvInputManager#isParentalControlsEnabled
     * TvInputManager.isParentalControlsEnabled()} returns {@code true}). Whether the TV input
     * service should block the content or not is determined by invoking
     * {@link TvInputManager#isRatingBlocked TvInputManager.isRatingBlocked(TvContentRating)}
     * with the content rating for the current program. Then the {@link TvInputManager} makes a
     * judgment based on the user blocked ratings stored in the secure settings and returns the
     * result. If the rating in question turns out to be blocked, the TV input service must
     * immediately block the content and call this method with the content rating of the current
     * program to prompt the PIN verification screen.
     * </p><p>
     * Each TV input service also needs to continuously listen to any changes made to the
     * parental controls settings by registering a broadcast receiver to receive
     * {@link TvInputManager#ACTION_BLOCKED_RATINGS_CHANGED} and
     * {@link TvInputManager#ACTION_PARENTAL_CONTROLS_ENABLED_CHANGED} and immediately
     * reevaluate the current program with the new parental controls settings.
     * </p>
     *
     * @param rating The content rating for the current TV program.
     * @see #notifyContentAllowed
     * @see TvInputManager
     */
    CARAPI NotifyContentBlocked(
        /* [in] */ ITvContentRating* rating);

    /**
     * Assigns a position of the {@link Surface} passed by {@link #onSetSurface}. The position
     * is relative to an overlay view.
     *
     * @param left Left position in pixels, relative to the overlay view.
     * @param top Top position in pixels, relative to the overlay view.
     * @param right Right position in pixels, relative to the overlay view.
     * @param bottm Bottom position in pixels, relative to the overlay view.
     * @see #onOverlayViewSizeChanged
     * @hide
     */
    CARAPI LayoutSurface(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottm);

    /**
     * Called when the session is released.
     */
    CARAPI OnRelease();

    /**
     * Sets the current session as the main session. The main session is a session whose
     * corresponding TV input determines the HDMI-CEC active source device.
     * <p>
     * TV input service that manages HDMI-CEC logical device should implement {@link
     * #onSetMain} to (1) select the corresponding HDMI logical device as the source device
     * when {@code isMain} is {@code true}, and to (2) select the internal device (= TV itself)
     * as the source device when {@code isMain} is {@code false} and the session is still main.
     * Also, if a surface is passed to a non-main session and active source is changed to
     * initiate the surface, the active source should be returned to the main session.
     * </p><p>
     * {@link TvView} guarantees that, when tuning involves a session transition, {@code
     * onSetMain(true)} for new session is called first, {@code onSetMain(false)} for old
     * session is called afterwards. This allows {@code onSetMain(false)} to be no-op when TV
     * input service knows that the next main session corresponds to another HDMI logical
     * device. Practically, this implies that one TV input service should handle all HDMI port
     * and HDMI-CEC logical devices for smooth active source transition.
     * </p>
     *
     * @param isMain If true, session should become main.
     * @see TvView#setMain
     * @hide
     */
    CARAPI OnSetMain(
        /* [in] */ Boolean isMain);

    /**
     * Sets the {@link Surface} for the current input session on which the TV input renders
     * video.
     *
     * @param surface {@link Surface} an application passes to this TV input session.
     * @return {@code true} if the surface was set, {@code false} otherwise.
     */
    CARAPI OnSetSurface(
        /* [in] */ ISurface* surface,
        /* [out] */ Boolean* result);

    /**
     * Called after any structural changes (format or size) have been made to the
     * {@link Surface} passed by {@link #onSetSurface}. This method is always called
     * at least once, after {@link #onSetSurface} with non-null {@link Surface} is called.
     *
     * @param format The new PixelFormat of the {@link Surface}.
     * @param width The new width of the {@link Surface}.
     * @param height The new height of the {@link Surface}.
     */
    CARAPI OnSurfaceChanged(
        /* [in] */ Int32 format,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Called when a size of an overlay view is changed by an application. Even when the overlay
     * view is disabled by {@link #setOverlayViewEnabled}, this is called. The size is same as
     * the size of {@link Surface} in general. Once {@link #layoutSurface} is called, the sizes
     * of {@link Surface} and the overlay view can be different.
     *
     * @param width The width of the overlay view.
     * @param height The height of the overlay view.
     * @hide
     */
    CARAPI OnOverlayViewSizeChanged(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Sets the relative stream volume of the current TV input session to handle the change of
     * audio focus by setting.
     *
     * @param volume Volume scale from 0.0 to 1.0.
     */
    CARAPI OnSetStreamVolume(
        /* [in] */ Float volume);

    /**
     * Tunes to a given channel. When the video is available, {@link #notifyVideoAvailable()}
     * should be called. Also, {@link #notifyVideoUnavailable(int)} should be called when the
     * TV input cannot continue playing the given channel.
     *
     * @param channelUri The URI of the channel.
     * @return {@code true} the tuning was successful, {@code false} otherwise.
     */
    CARAPI OnTune(
        /* [in] */ IUri* channelUri);

    /**
     * Calls {@link #onTune(Uri)}. Override this method in order to handle {@code params}.
     *
     * @param channelUri The URI of the channel.
     * @param params The extra parameters from other applications.
     * @return {@code true} the tuning was successful, {@code false} otherwise.
     * @hide
     */
    CARAPI OnTune(
        /* [in] */ IUri* channelUri,
        /* [in] */ IBundle* params);

    /**
     * Enables or disables the caption.
     * <p>
     * The locale for the user's preferred captioning language can be obtained by calling
     * {@link CaptioningManager#getLocale CaptioningManager.getLocale()}.
     *
     * @param enabled {@code true} to enable, {@code false} to disable.
     * @see CaptioningManager
     */
    CARAPI OnSetCaptionEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Requests to unblock the content according to the given rating.
     * <p>
     * The implementation should unblock the content.
     * TV input service has responsibility to decide when/how the unblock expires
     * while it can keep previously unblocked ratings in order not to ask a user
     * to unblock whenever a content rating is changed.
     * Therefore an unblocked rating can be valid for a channel, a program,
     * or certain amount of time depending on the implementation.
     * </p>
     *
     * @param unblockedRating An unblocked content rating
     */
    CARAPI OnUnblockContent(
        /* [in] */ ITvContentRating* unblockedRating);

    /**
     * Select a given track.
     * <p>
     * If this is done successfully, the implementation should call {@link #notifyTrackSelected}
     * to help applications maintain the selcted track lists.
     * </p>
     *
     * @param trackId The ID of the track to select. {@code null} means to unselect the current
     *            track for a given type.
     * @param type The type of the track to select. The type can be
     *            {@link TvTrackInfo#TYPE_AUDIO}, {@link TvTrackInfo#TYPE_VIDEO} or
     *            {@link TvTrackInfo#TYPE_SUBTITLE}.
     * @see #notifyTrackSelected
     */
    CARAPI OnSelectTrack(
        /* [in] */ Int32 type,
        /* [in] */ const String& trackId);

    /**
     * Processes a private command sent from the application to the TV input. This can be used
     * to provide domain-specific features that are only known between certain TV inputs and
     * their clients.
     *
     * @param action Name of the command to be performed. This <em>must</em> be a scoped name,
     *            i.e. prefixed with a package name you own, so that different developers will
     *            not create conflicting commands.
     * @param data Any data to include with the command.
     * @hide
     */
    CARAPI OnAppPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle* data);

    /**
     * Called when an application requests to create an overlay view. Each session
     * implementation can override this method and return its own view.
     *
     * @return a view attached to the overlay window
     */
    CARAPI OnCreateOverlayView(
        /* [out] */ IView** result);

    /**
     * Implement this method to handle touch screen motion events on the current input session.
     *
     * @param event The motion event being received.
     * @return If you handled the event, return {@code true}. If you want to allow the event to
     *         be handled by the next receiver, return {@code false}.
     * @see View#onTouchEvent
     */
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Implement this method to handle trackball events on the current input session.
     *
     * @param event The motion event being received.
     * @return If you handled the event, return {@code true}. If you want to allow the event to
     *         be handled by the next receiver, return {@code false}.
     * @see View#onTrackballEvent
     */
    CARAPI OnTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Implement this method to handle generic motion events on the current input session.
     *
     * @param event The motion event being received.
     * @return If you handled the event, return {@code true}. If you want to allow the event to
     *         be handled by the next receiver, return {@code false}.
     * @see View#onGenericMotionEvent
     */
    CARAPI OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /**
     * This method is called when the application would like to stop using the current input
     * session.
     */
    CARAPI ReleaseResources();

    /**
     * Calls {@link #onSetMain}.
     */
    CARAPI SetMain(
        /* [in] */ Boolean isMain);

    /**
     * Calls {@link #onSetSurface}.
     */
    CARAPI SetSurface(
        /* [in] */ ISurface* surface);

    /**
     * Calls {@link #onSurfaceChanged}.
     */
    CARAPI DispatchSurfaceChanged(
        /* [in] */ Int32 format,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Calls {@link #onSetStreamVolume}.
     */
    CARAPI SetStreamVolume(
        /* [in] */ Float volume);

    /**
     * Calls {@link #onTune}.
     */
    CARAPI Tune(
        /* [in] */ IUri* channelUri,
        /* [in] */ IBundle* params);

    /**
     * Calls {@link #onSetCaptionEnabled}.
     */
    CARAPI SetCaptionEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Calls {@link #onSelectTrack}.
     */
    CARAPI SelectTrack(
        /* [in] */ Int32 type,
        /* [in] */ const String& trackId);

    /**
     * Calls {@link #onUnblockContent}.
     */
    CARAPI UnblockContent(
        /* [in] */ const String& unblockedRating);

    /**
     * Calls {@link #onAppPrivateCommand}.
     */
    CARAPI AppPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle* data);

    /**
     * Creates an overlay view. This calls {@link #onCreateOverlayView} to get a view to attach
     * to the overlay window.
     *
     * @param windowToken A window token of an application.
     * @param frame A position of the overlay view.
     */
    CARAPI CreateOverlayView(
        /* [in] */ IBinder* windowToken,
        /* [in] */ IRect* frame);

    /**
     * Relayouts the current overlay view.
     *
     * @param frame A new position of the overlay view.
     */
    CARAPI RelayoutOverlayView(
        /* [in] */ IRect* frame);

    /**
     * Removes the current overlay view.
     */
    CARAPI RemoveOverlayView(
        /* [in] */ Boolean clearWindowToken);

    /**
     * Takes care of dispatching incoming input events and tells whether the event was handled.
     */
    CARAPI DispatchInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ IInputEventReceiver* receiver,
        /* [out] */ Int32* result);

private:
    CARAPI_(void) SetSessionCallback(
        /* [in] */ IITvInputSessionCallback* callback);

    CARAPI_(void) RunOnMainThread(
        /* [in] */ IRunnable* action);

private:
    AutoPtr<IDispatcherState> mDispatcherState;
    AutoPtr<IWindowManager> mWindowManager;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IWindowManagerLayoutParams> mWindowParams;
    AutoPtr<ISurface> mSurface;
    AutoPtr<IView> mOverlayView;
    Boolean mOverlayViewEnabled;
    AutoPtr<IBinder> mWindowToken;
    AutoPtr<IRect> mOverlayFrame;
    AutoPtr<IITvInputSessionCallback> mSessionCallback;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_TVINPUTSERVICESESSION_H__
