//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_MEDIA_TV_CTVVIEW_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVVIEW_H__

#include "_Elastos_Droid_Media_Tv_CTvView.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/SurfaceView.h"
#include "elastos/droid/view/ViewGroup.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Droid::View::ISurfaceHolderCallback;
using Elastos::Droid::View::ISurfaceView;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::SurfaceView;
using Elastos::Droid::View::ViewGroup;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * Displays TV contents. The TvView class provides a high level interface for applications to show
 * TV programs from various TV sources that implement {@link TvInputService}. (Note that the list of
 * TV inputs available on the system can be obtained by calling
 * {@link TvInputManager#getTvInputList() TvInputManager.getTvInputList()}.)
 * <p>
 * Once the application supplies the URI for a specific TV channel to {@link #tune(String, Uri)}
 * method, it takes care of underlying service binding (and unbinding if the current TvView is
 * already bound to a service) and automatically allocates/deallocates resources needed. In addition
 * to a few essential methods to control how the contents are presented, it also provides a way to
 * dispatch input events to the connected TvInputService in order to enable custom key actions for
 * the TV input.
 * </p>
 */
CarClass(CTvView)
    , public ViewGroup
    , public ITvView
{
private:
    class SurfaceHolderCallback
        : public Object
        , public ISurfaceHolderCallback
    {
    public:
        CAR_INTERFACE_DECL()

        SurfaceHolderCallback(
            /* [in] */ CTvView* host)
            : mHost(host)
        {}

        /**
         * This is called immediately after the surface is first created.
         * Implementations of this should start up whatever rendering code
         * they desire.  Note that only one thread can ever draw into
         * a {@link Surface}, so you should not draw into the Surface here
         * if your normal rendering will be in another thread.
         *
         * @param holder The SurfaceHolder whose surface is being created.
         */
        CARAPI SurfaceCreated(
            /* [in] */ ISurfaceHolder* holder);

        /**
         * This is called immediately after any structural changes (format or
         * size) have been made to the surface.  You should at this point update
         * the imagery in the surface.  This method is always called at least
         * once, after {@link #surfaceCreated}.
         *
         * @param holder The SurfaceHolder whose surface has changed.
         * @param format The new PixelFormat of the surface.
         * @param width The new width of the surface.
         * @param height The new height of the surface.
         */
        CARAPI SurfaceChanged(
            /* [in] */ ISurfaceHolder* holder,
            /* [in] */ Int32 format,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        /**
         * This is called immediately before a surface is being destroyed. After
         * returning from this call, you should no longer try to access this
         * surface.  If you have a rendering thread that directly accesses
         * the surface, you must ensure that thread is no longer touching the
         * Surface before returning from this function.
         *
         * @param holder The SurfaceHolder whose surface is being destroyed.
         */
        CARAPI SurfaceDestroyed(
            /* [in] */ ISurfaceHolder* holder);

    private:
        CTvView* mHost;
    };

    class FinishedInputEventCallback
        : public Object
        , public ITvInputManagerSessionFinishedInputEventCallback
    {
    public:
        CAR_INTERFACE_DECL()

        FinishedInputEventCallback(
            /* [in] */ CTvView* host)
            : mHost(host)
        {}

        /**
         * Called when the dispatched input event is finished.
         *
         * @param token A token passed to {@link #dispatchInputEvent}.
         * @param handled {@code true} if the dispatched input event was handled properly.
         *            {@code false} otherwise.
         */
        CARAPI OnFinishedInputEvent(
            /* [in] */ IInterface * token,
            /* [in] */ Boolean handled);

    private:
        CTvView* mHost;
    };

    class MySurfaceView
        : public SurfaceView
    {
    public:
        MySurfaceView(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs,
            /* [in] */ Int32 defStyle,
            /* [in] */ CTvView* host);

        CARAPI UpdateWindow(
            /* [in] */ Boolean force,
            /* [in] */ Boolean redrawNeeded);

    private:
        CTvView* mHost;
    };

    class MySessionCallback
        : public Object
        , public ITvInputManagerSessionCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MySessionCallback(
            /* [in] */ const String& inputId,
            /* [in] */ IUri* channelUri,
            /* [in] */ IBundle* tuneParams,
            /* [in] */ CTvView* host);

        /**
         * This is called after {@link TvInputManager#createSession} has been processed.
         *
         * @param session A {@link TvInputManager.Session} instance created. This can be
         *            {@code null} if the creation request failed.
         */
        CARAPI OnSessionCreated(
            /* [in] */ ITvInputManagerSession* session);

        /**
         * This is called when {@link TvInputManager.Session} is released.
         * This typically happens when the process hosting the session has crashed or been killed.
         *
         * @param session A {@link TvInputManager.Session} instance released.
         */
        CARAPI OnSessionReleased(
            /* [in] */ ITvInputManagerSession* session);

        /**
         * This is called when the channel of this session is changed by the underlying TV input
         * with out any {@link TvInputManager.Session#tune(Uri)} request.
         *
         * @param session A {@link TvInputManager.Session} associated with this callback.
         * @param channelUri The URI of a channel.
         */
        CARAPI OnChannelRetuned(
            /* [in] */ ITvInputManagerSession* session,
            /* [in] */ IUri* channelUri);

        /**
         * This is called when the track information of the session has been changed.
         *
         * @param session A {@link TvInputManager.Session} associated with this callback.
         * @param tracks A list which includes track information.
         */
        CARAPI OnTracksChanged(
            /* [in] */ ITvInputManagerSession* session,
            /* [in] */ IList* tracks);

        /**
         * This is called when a track for a given type is selected.
         *
         * @param session A {@link TvInputManager.Session} associated with this callback
         * @param type The type of the selected track. The type can be
         *            {@link TvTrackInfo#TYPE_AUDIO}, {@link TvTrackInfo#TYPE_VIDEO} or
         *            {@link TvTrackInfo#TYPE_SUBTITLE}.
         * @param trackId The ID of the selected track. When {@code null} the currently selected
         *            track for a given type should be unselected.
         */
        CARAPI OnTrackSelected(
            /* [in] */ ITvInputManagerSession* session,
            /* [in] */ Int32 type,
            /* [in] */ const String& trackId);

        /**
         * This is called when the video is available, so the TV input starts the playback.
         *
         * @param session A {@link TvInputManager.Session} associated with this callback.
         */
        CARAPI OnVideoAvailable(
            /* [in] */ ITvInputManagerSession* session);

        /**
         * This is called when the video is not available, so the TV input stops the playback.
         *
         * @param session A {@link TvInputManager.Session} associated with this callback
         * @param reason The reason why the TV input stopped the playback:
         * <ul>
         * <li>{@link TvInputManager#VIDEO_UNAVAILABLE_REASON_UNKNOWN}
         * <li>{@link TvInputManager#VIDEO_UNAVAILABLE_REASON_TUNING}
         * <li>{@link TvInputManager#VIDEO_UNAVAILABLE_REASON_WEAK_SIGNAL}
         * <li>{@link TvInputManager#VIDEO_UNAVAILABLE_REASON_BUFFERING}
         * </ul>
         */
        CARAPI OnVideoUnavailable(
            /* [in] */ ITvInputManagerSession* session,
            /* [in] */ Int32 reason);

        /**
         * This is called when the current program content turns out to be allowed to watch since
         * its content rating is not blocked by parental controls.
         *
         * @param session A {@link TvInputManager.Session} associated with this callback
         */
        CARAPI OnContentAllowed(
            /* [in] */ ITvInputManagerSession* session);

        /**
         * This is called when the current program content turns out to be not allowed to watch
         * since its content rating is blocked by parental controls.
         *
         * @param session A {@link TvInputManager.Session} associated with this callback
         * @param rating The content ration of the blocked program.
         */
        CARAPI OnContentBlocked(
            /* [in] */ ITvInputManagerSession* session,
            /* [in] */ ITvContentRating* rating);

        /**
         * This is called when {@link TvInputService.Session#layoutSurface} is called to change the
         * layout of surface.
         *
         * @param session A {@link TvInputManager.Session} associated with this callback
         * @param left Left position.
         * @param top Top position.
         * @param right Right position.
         * @param bottom Bottom position.
         * @hide
         */
        // @SystemApi
        CARAPI OnLayoutSurface(
            /* [in] */ ITvInputManagerSession* session,
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom);

        /**
         * This is called when a custom event has been sent from this session.
         *
         * @param session A {@link TvInputManager.Session} associated with this callback
         * @param eventType The type of the event.
         * @param eventArgs Optional arguments of the event.
         * @hide
         */
        // @SystemApi
        CARAPI OnSessionEvent(
            /* [in] */ ITvInputManagerSession* session,
            /* [in] */ const String& eventType,
            /* [in] */ IBundle* eventArgs);

    public:
        String mInputId;
        AutoPtr<IUri> mChannelUri;
        AutoPtr<IBundle> mTuneParams;
        CTvView* mHost;
    };

public:
    CTvView();

    virtual ~CTvView();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    /**
     * Sets the callback to be invoked when an event is dispatched to this TvView.
     *
     * @param callback The callback to receive events. A value of {@code null} removes any existing
     *            callbacks.
     */
    CARAPI SetCallback(
        /* [in] */ ITvViewTvInputCallback* cb);

    /**
     * Sets this as the main {@link TvView}.
     * <p>
     * The main {@link TvView} is a {@link TvView} whose corresponding TV input determines the
     * HDMI-CEC active source device. For an HDMI port input, one of source devices that is
     * connected to that HDMI port becomes the active source. For an HDMI-CEC logical device input,
     * the corresponding HDMI-CEC logical device becomes the active source. For any non-HDMI input
     * (including the tuner, composite, S-Video, etc.), the internal device (= TV itself) becomes
     * the active source.
     * </p><p>
     * First tuned {@link TvView} becomes main automatically, and keeps to be main until either
     * {@link #reset} is called for the main {@link TvView} or {@link #setMain} is called for other
     * {@link TvView}.
     * </p>
     * @hide
     */
    CARAPI SetMain();

    /**
     * Sets the Z order of a window owning the surface of this TvView above the normal TvView
     * but below an application.
     *
     * @see SurfaceView#setZOrderMediaOverlay
     * @hide
     */
    CARAPI SetZOrderMediaOverlay(
        /* [in] */ Boolean isMediaOverlay);

    /**
     * Sets the Z order of a window owning the surface of this TvView on top of an application.
     *
     * @see SurfaceView#setZOrderOnTop
     * @hide
     */
    CARAPI SetZOrderOnTop(
        /* [in] */ Boolean onTop);

    /**
     * Sets the relative stream volume of this session to handle a change of audio focus.
     *
     * @param volume A volume value between 0.0f to 1.0f.
     */
    CARAPI SetStreamVolume(
        /* [in] */ Float volume);

    /**
     * Tunes to a given channel.
     *
     * @param inputId The ID of TV input which will play the given channel.
     * @param channelUri The URI of a channel.
     */
    CARAPI Tune(
        /* [in] */ const String& inputId,
        /* [in] */ IUri* channelUri);

    /**
     * Tunes to a given channel.
     *
     * @param inputId The ID of TV input which will play the given channel.
     * @param channelUri The URI of a channel.
     * @param params Extra parameters which might be handled with the tune event.
     * @hide
     */
    CARAPI Tune(
        /* [in] */ const String& inputId,
        /* [in] */ IUri* channelUri,
        /* [in] */ IBundle* params);

    /**
     * Resets this TvView.
     * <p>
     * This method is primarily used to un-tune the current TvView.
     */
    CARAPI Reset();

    /**
     * Requests to unblock TV content according to the given rating.
     * <p>
     * This notifies TV input that blocked content is now OK to play.
     * </p>
     *
     * @param unblockedRating A TvContentRating to unblock.
     * @see TvInputService.Session#notifyContentBlocked(TvContentRating)
     * @hide
     */
    CARAPI RequestUnblockContent(
        /* [in] */ ITvContentRating* unblockedRating);

    /**
     * Enables or disables the caption in this TvView.
     * <p>
     * Note that this method does not take any effect unless the current TvView is tuned.
     *
     * @param enabled {@code true} to enable, {@code false} to disable.
     */
    CARAPI SetCaptionEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Selects a track.
     *
     * @param type The type of the track to select. The type can be {@link TvTrackInfo#TYPE_AUDIO},
     *            {@link TvTrackInfo#TYPE_VIDEO} or {@link TvTrackInfo#TYPE_SUBTITLE}.
     * @param trackId The ID of the track to select. {@code null} means to unselect the current
     *            track for a given type.
     * @see #getTracks
     * @see #getSelectedTrack
     */
    CARAPI SelectTrack(
        /* [in] */ Int32 type,
        /* [in] */ const String& trackId);

    /**
     * Returns the list of tracks. Returns {@code null} if the information is not available.
     *
     * @param type The type of the tracks. The type can be {@link TvTrackInfo#TYPE_AUDIO},
     *            {@link TvTrackInfo#TYPE_VIDEO} or {@link TvTrackInfo#TYPE_SUBTITLE}.
     * @see #selectTrack
     * @see #getSelectedTrack
     */
    CARAPI GetTracks(
        /* [in] */ Int32 type,
        /* [out] */ IList** result);

    /**
     * Returns the ID of the selected track for a given type. Returns {@code null} if the
     * information is not available or the track is not selected.
     *
     * @param type The type of the selected tracks. The type can be {@link TvTrackInfo#TYPE_AUDIO},
     *            {@link TvTrackInfo#TYPE_VIDEO} or {@link TvTrackInfo#TYPE_SUBTITLE}.
     * @see #selectTrack
     * @see #getTracks
     */
    CARAPI GetSelectedTrack(
        /* [in] */ Int32 type,
        /* [out] */ String* result);

    /**
     * Calls {@link TvInputService.Session#appPrivateCommand(String, Bundle)
     * TvInputService.Session.appPrivateCommand()} on the current TvView.
     *
     * @param action Name of the command to be performed. This <em>must</em> be a scoped name, i.e.
     *            prefixed with a package name you own, so that different developers will not create
     *            conflicting commands.
     * @param data Any data to include with the command.
     * @hide
     */
    CARAPI SendAppPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle* data);

    /**
     * Dispatches an unhandled input event to the next receiver.
     * <p>
     * Except system keys, TvView always consumes input events in the normal flow. This is called
     * asynchronously from where the event is dispatched. It gives the host application a chance to
     * dispatch the unhandled input events.
     *
     * @param event The input event.
     * @return {@code true} if the event was handled by the view, {@code false} otherwise.
     */
    CARAPI DispatchUnhandledInputEvent(
        /* [in] */ IInputEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Called when an unhandled input event also has not been handled by the user provided
     * callback. This is the last chance to handle the unhandled input event in the TvView.
     *
     * @param event The input event.
     * @return If you handled the event, return {@code true}. If you want to allow the event to be
     *         handled by the next receiver, return {@code false}.
     */
    CARAPI OnUnhandledInputEvent(
        /* [in] */ IInputEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Registers a callback to be invoked when an input event is not handled by the bound TV input.
     *
     * @param listener The callback to be invoked when the unhandled input event is received.
     */
    CARAPI SetOnUnhandledInputEventListener(
        /* [in] */ ITvViewTvInputOnUnhandledInputEventListener* listener);

    virtual CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    CARAPI DispatchGenericMotionEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI DispatchWindowFocusChanged(
        /* [in] */ Boolean hasFocus);

protected:
    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnDetachedFromWindow();

    virtual CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    virtual CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    virtual CARAPI OnVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 visibility);

private:
    CARAPI_(void) ResetInternal();

    CARAPI_(void) ResetSurfaceView();

    CARAPI_(void) ReleaseResources();

    CARAPI_(void) SetSessionSurface(
        /* [in] */ ISurface* surface);

    CARAPI_(void) DispatchSurfaceChanged(
        /* [in] */ Int32 format,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI_(void) CreateSessionOverlayView();

    CARAPI_(void) RemoveSessionOverlayView();

    CARAPI_(void) RelayoutSessionOverlayView();

    CARAPI_(AutoPtr<IRect>) GetViewFrameOnScreen();

public:
    static String TAG;
    static Boolean DEBUG;

    static Int32 VIDEO_SIZE_VALUE_UNKNOWN;

    static Int32 ZORDER_MEDIA;
    static Int32 ZORDER_MEDIA_OVERLAY;
    static Int32 ZORDER_ON_TOP;

    static Int32 CAPTION_DEFAULT;
    static Int32 CAPTION_ENABLED;
    static Int32 CAPTION_DISABLED;

    static AutoPtr<IWeakReference> NULL_TV_VIEW;
    static Object sMainTvViewLock;
    static AutoPtr<IWeakReference> sMainTvView;

    AutoPtr<IHandler> mHandler;
    AutoPtr<ITvInputManagerSession> mSession;
    AutoPtr<ISurfaceView> mSurfaceView;
    AutoPtr<ISurface> mSurface;
    Boolean mOverlayViewCreated;
    AutoPtr<IRect> mOverlayViewFrame;
    AutoPtr<ITvInputManager> mTvInputManager;
    AutoPtr<MySessionCallback> mSessionCallback;
    AutoPtr<ITvViewTvInputCallback> mCallback;
    AutoPtr<ITvViewTvInputOnUnhandledInputEventListener> mOnUnhandledInputEventListener;
    Boolean mHasStreamVolume;
    Float mStreamVolume;
    Int32 mVideoWidth;
    Int32 mVideoHeight;
    Boolean mSurfaceChanged;
    Int32 mSurfaceFormat;
    Int32 mSurfaceWidth;
    Int32 mSurfaceHeight;
    AutoPtr<IAttributeSet> mAttrs;
    Int32 mDefStyleAttr;
    Int32 mWindowZOrder;
    Boolean mUseRequestedSurfaceLayout;
    Int32 mSurfaceViewLeft;
    Int32 mSurfaceViewRight;
    Int32 mSurfaceViewTop;
    Int32 mSurfaceViewBottom;
    Int32 mCaptionEnabled;

    AutoPtr<SurfaceHolderCallback> mSurfaceHolderCallback;
    AutoPtr<FinishedInputEventCallback> mFinishedInputEventCallback;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVVIEW_H__
