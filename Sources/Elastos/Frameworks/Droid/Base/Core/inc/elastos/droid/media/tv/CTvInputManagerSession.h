#ifndef __ELASTOS_DROID_MEDIA_TV_CTVINPUTMANAGERSESSION_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVINPUTMANAGERSESSION_H__

#include "_Elastos_Droid_Media_Tv_CTvInputManagerSession.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/utility/Pools.h"
#include "elastos/droid/view/InputEventSender.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::Pools;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::InputEventSender;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::IView;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * The Session provides the per-session functionality of TV inputs.
 * @hide
 */
CarClass(CTvInputManagerSession)
    , public Object
    , public ITvInputManagerSession
{
private:
    class InputEventHandler
        : public Handler
    {
    public:
        InputEventHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ CTvInputManagerSession* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        const static Int32 MSG_SEND_INPUT_EVENT;
        const static Int32 MSG_TIMEOUT_INPUT_EVENT;
        const static Int32 MSG_FLUSH_INPUT_EVENT;

    private:
        CTvInputManagerSession* mHost;
    };

    class TvInputEventSender
        : public InputEventSender
    {
    public:
        TvInputEventSender(
            /* [in] */ IInputChannel* inputChannel,
            /* [in] */ ILooper* looper,
            /* [in] */ CTvInputManagerSession* host);

        CARAPI OnInputEventFinished(
            /* [in] */ Int32 seq,
            /* [in] */ Boolean handled);

    private:
        CTvInputManagerSession* mHost;
    };

    class PendingEvent
        : public Runnable
    {
    public:
        PendingEvent(
            /* [in] */ CTvInputManagerSession* host);

        CARAPI Recycle();

        CARAPI Run();

    public:
        AutoPtr<IInputEvent> mEvent;
        AutoPtr<IInterface> mEventToken;
        AutoPtr<ITvInputManagerSessionFinishedInputEventCallback> mCallback;
        AutoPtr<IHandler> mEventHandler;
        Boolean mHandled;

    private:
        CTvInputManagerSession* mHost;
    };

public:
    CTvInputManagerSession();

    virtual ~CTvInputManagerSession();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IBinder* token,
        /* [in] */ IInputChannel* channel,
        /* [in] */ IITvInputManager* service,
        /* [in] */ Int32 userId,
        /* [in] */ Int32 seq,
        /* [in] */ ISparseArray* sessionCallbackRecordMap);

    /**
     * Releases this session.
     */
    CARAPI ReleaseResources();

    /**
     * Sets the {@link android.view.Surface} for this session.
     *
     * @param surface A {@link android.view.Surface} used to render video.
     */
    CARAPI SetSurface(
        /* [in] */ ISurface* surface);

    /**
     * Notifies of any structural changes (format or size) of the {@link Surface}
     * passed by {@link #setSurface}.
     *
     * @param format The new PixelFormat of the {@link Surface}.
     * @param width The new width of the {@link Surface}.
     * @param height The new height of the {@link Surface}.
     * @hide
     */
    CARAPI DispatchSurfaceChanged(
        /* [in] */ Int32 format,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Sets the relative stream volume of this session to handle a change of audio focus.
     *
     * @param volume A volume value between 0.0f to 1.0f.
     * @throws IllegalArgumentException if the volume value is out of range.
     */
    CARAPI SetStreamVolume(
        /* [in] */ Float volume);

    /**
     * Tunes to a given channel.
     *
     * @param channelUri The URI of a channel.
     * @throws IllegalArgumentException if the argument is {@code null}.
     */
    CARAPI Tune(
        /* [in] */ IUri* channelUri);

    /**
     * Tunes to a given channel.
     *
     * @param channelUri The URI of a channel.
     * @param params A set of extra parameters which might be handled with this tune event.
     * @throws IllegalArgumentException if {@code channelUri} is {@code null}.
     * @hide
     */
    CARAPI Tune(
        /* [in] */ IUri* channelUri,
        /* [in] */ IBundle* params);

    /**
     * Enables or disables the caption for this session.
     *
     * @param enabled {@code true} to enable, {@code false} to disable.
     */
    CARAPI SetCaptionEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Selects a track.
     *
     * @param type The type of the track to select. The type can be
     *            {@link TvTrackInfo#TYPE_AUDIO}, {@link TvTrackInfo#TYPE_VIDEO} or
     *            {@link TvTrackInfo#TYPE_SUBTITLE}.
     * @param trackId The ID of the track to select. When {@code null}, the currently selected
     *            track of the given type will be unselected.
     * @see #getTracks
     */
    CARAPI SelectTrack(
        /* [in] */ Int32 type,
        /* [in] */ const String& trackId);

    /**
     * Returns the list of tracks for a given type. Returns {@code null} if the information is
     * not available.
     *
     * @param type The type of the tracks. The type can be {@link TvTrackInfo#TYPE_AUDIO},
     *            {@link TvTrackInfo#TYPE_VIDEO} or {@link TvTrackInfo#TYPE_SUBTITLE}.
     * @return the list of tracks for the given type.
     */
    CARAPI GetTracks(
        /* [in] */ Int32 type,
        /* [out] */ IList** result);

    /**
     * Returns the selected track for a given type. Returns {@code null} if the information is
     * not available or any of the tracks for the given type is not selected.
     *
     * @return the ID of the selected track.
     * @see #selectTrack
     */
    CARAPI GetSelectedTrack(
        /* [in] */ Int32 type,
        /* [out] */ String* result);

    /**
     * Calls {@link TvInputService.Session#appPrivateCommand(String, Bundle)
     * TvInputService.Session.appPrivateCommand()} on the current TvView.
     *
     * @param action Name of the command to be performed. This <em>must</em> be a scoped name,
     *            i.e. prefixed with a package name you own, so that different developers will
     *            not create conflicting commands.
     * @param data Any data to include with the command.
     * @hide
     */
    CARAPI SendAppPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle * data);

    /**
     * Dispatches an input event to this session.
     *
     * @param event An {@link InputEvent} to dispatch.
     * @param token A token used to identify the input event later in the callback.
     * @param callback A callback used to receive the dispatch result.
     * @param handler A {@link Handler} that the dispatch result will be delivered to.
     * @return Returns {@link #DISPATCH_HANDLED} if the event was handled. Returns
     *         {@link #DISPATCH_NOT_HANDLED} if the event was not handled. Returns
     *         {@link #DISPATCH_IN_PROGRESS} if the event is in progress and the callback will
     *         be invoked later.
     * @throws IllegalArgumentException if any of the necessary arguments is {@code null}.
     * @hide
     */
    CARAPI DispatchInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ IInterface* token,
        /* [in] */ ITvInputManagerSessionFinishedInputEventCallback* cb,
        /* [in] */ IHandler* handler,
        /* [out] */ Int32* result);

    /**
     * Sets this as the main session. The main session is a session whose corresponding TV
     * input determines the HDMI-CEC active source device.
     *
     * @see TvView#setMain
     */
    CARAPI SetMain();

    /**
     * Creates an overlay view. Once the overlay view is created, {@link #relayoutOverlayView}
     * should be called whenever the layout of its containing view is changed.
     * {@link #removeOverlayView()} should be called to remove the overlay view.
     * Since a session can have only one overlay view, this method should be called only once
     * or it can be called again after calling {@link #removeOverlayView()}.
     *
     * @param view A view playing TV.
     * @param frame A position of the overlay view.
     * @throws IllegalArgumentException if any of the arguments is {@code null}.
     * @throws IllegalStateException if {@code view} is not attached to a window.
     */
    CARAPI CreateOverlayView(
        /* [in] */ IView* view,
        /* [in] */ IRect* frame);

    /**
     * Relayouts the current overlay view.
     *
     * @param frame A new position of the overlay view.
     * @throws IllegalArgumentException if the arguments is {@code null}.
     */
    CARAPI RelayoutOverlayView(
        /* [in] */ IRect* frame);

    /**
     * Removes the current overlay view.
     */
    CARAPI RemoveOverlayView();

    /**
     * Requests to unblock content blocked by parental controls.
     */
    CARAPI RequestUnblockContent(
        /* [in] */ ITvContentRating* unblockedRating);

    CARAPI FinishedInputEvent(
        /* [in] */ Int32 seq,
        /* [in] */ Boolean handled,
        /* [in] */ Boolean timeout);

    CARAPI GetToken(
        /* [out] */ IBinder** result);

    CARAPI ReleaseInternal();

private:
    CARAPI_(Boolean) ContainsTrack(
        /* [in] */ IList* tracks,
        /* [in] */ const String& trackId);

    // Must be called on the main looper
    CARAPI_(void) SendInputEventAndReportResultOnMainLooper(
        /* [in] */ PendingEvent* p);

    CARAPI_(Int32) SendInputEventOnMainLooperLocked(
        /* [in] */ PendingEvent* p);

    // Assumes the event has already been removed from the queue.
    CARAPI_(void) InvokeFinishedInputEventCallback(
        /* [in] */ PendingEvent* p,
        /* [in] */ Boolean handled);

    CARAPI_(void) FlushPendingEventsLocked();

    CARAPI_(AutoPtr<PendingEvent>) ObtainPendingEventLocked(
        /* [in] */ IInputEvent* event,
        /* [in] */ IInterface* token,
        /* [in] */ ITvInputManagerSessionFinishedInputEventCallback* callback,
        /* [in] */ IHandler* handler);

    CARAPI_(void) RecyclePendingEventLocked(
        /* [in] */ PendingEvent* p);

public:
    AutoPtr<IArrayList> mAudioTracks;
    AutoPtr<IArrayList> mVideoTracks;
    AutoPtr<IArrayList> mSubtitleTracks;
    String mSelectedAudioTrackId;
    String mSelectedVideoTrackId;
    String mSelectedSubtitleTrackId;

private:
    static Int64 INPUT_SESSION_NOT_RESPONDING_TIMEOUT;

    AutoPtr<IITvInputManager> mService;
    Int32 mUserId;
    Int32 mSeq;

    // For scheduling input event handling on the main thread. This also serves as a lock to
    // protect pending input events and the input channel.
    AutoPtr<InputEventHandler> mHandler;

    AutoPtr<Pools::SimplePool<PendingEvent> > mPendingEventPool;
    AutoPtr<ISparseArray> mPendingEvents;
    AutoPtr<ISparseArray> mSessionCallbackRecordMap;

    AutoPtr<IBinder> mToken;
    AutoPtr<TvInputEventSender> mSender;
    AutoPtr<IInputChannel> mChannel;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVINPUTMANAGERSESSION_H__
