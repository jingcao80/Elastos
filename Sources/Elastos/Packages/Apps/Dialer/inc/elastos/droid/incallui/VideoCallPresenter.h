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

#ifndef __ELASTOS_DROID_INCALLUI_VIDEOCALLPRESENTER_H__
#define __ELASTOS_DROID_INCALLUI_VIDEOCALLPRESENTER_H__

#include "elastos/droid/incallui/Presenter.h"
#include "elastos/droid/incallui/Call.h"
#include <elastos/core/Runnable.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;
using Elastos::Droid::Telecom::IInCallServiceVideoCall;
using Elastos::Droid::Telecom::ICallDetails;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class VideoCallPresenter
    : public Presenter
    , public IIncomingCallListener
    , public IInCallOrientationListener
    , public IInCallStateListener
    , public IInCallDetailsListener
    , public ISurfaceChangeListener
    , public IVideoEventListener
    , public ISessionModificationListener
{
private:
    /**
     * Defines the state of the preview surface negotiation with the telephony layer.
     */
    class PreviewSurfaceState
    {
    private:
        /**
         * The camera has not yet been set on the {@link VideoCall}; negotiation has not yet
         * started.
         */
        static const Int32 NONE = 0;

        /**
         * The camera has been set on the {@link VideoCall}, but camera capabilities have not yet
         * been received.
         */
        static const Int32 CAMERA_SET = 1;

        /**
         * The camera capabilties have been received from telephony, but the surface has not yet
         * been set on the {@link VideoCall}.
         */
        static const Int32 CAPABILITIES_RECEIVED = 2;

        /**
         * The surface has been set on the {@link VideoCall}.
         */
        static const Int32 SURFACE_SET = 3;

        friend class VideoCallPresenter;
    };

    class OnUpgradeToVideoFailRunnable : public Runnable
    {
    public:
        OnUpgradeToVideoFailRunnable(
            /* [in] */ VideoCallPresenter* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        VideoCallPresenter* mHost;
    };

public:
    VideoCallPresenter();

    CAR_INTERFACE_DECL()

    /**
     * Determines the device orientation (portrait/lanscape).
     */
    CARAPI_(Int32) GetDeviceOrientation();

    /**
     * Initializes the presenter.
     *
     * @param context The current context.
     */
    CARAPI_(void) Init(
        /* [in] */ IContext* context);

    /**
     * Called when the user interface is ready to be used.
     *
     * @param ui The Ui implementation that is now ready to be used.
     */
    // @Override
    CARAPI OnUiReady(
        /* [in] */ IUi* ui);

    /**
     * Called when the user interface is no longer ready to be used.
     *
     * @param ui The Ui implementation that is no longer ready to be used.
     */
    // @Override
    CARAPI OnUiUnready(
        /* [in] */ IUi* ui);

    /**
     * Handles the creation of a surface in the {@link VideoCallFragment}.
     *
     * @param surface The surface which was created.
     */
    CARAPI_(void) OnSurfaceCreated(
        /* [in] */ Int32 surface);

    /**
     * Handles structural changes (format or size) to a surface.
     *
     * @param surface The surface which changed.
     * @param format The new PixelFormat of the surface.
     * @param width The new width of the surface.
     * @param height The new height of the surface.
     */
    CARAPI_(void) OnSurfaceChanged(
        /* [in] */ Int32 surface,
        /* [in] */ Int32 format,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Handles the destruction of a surface in the {@link VideoCallFragment}.
     *
     * @param surface The surface which was destroyed.
     */
    CARAPI_(void) OnSurfaceDestroyed(
        /* [in] */ Int32 surface);

    /**
     * Handles clicks on the video surfaces by toggling full screen state.
     * Informs the {@link InCallPresenter} of the change so that it can inform the
     * {@link CallCardPresenter} of the change.
     *
     * @param surfaceId The video surface receiving the click.
     */
    CARAPI_(void) OnSurfaceClick(
        /* [in] */ Int32 surfaceId);

    /**
     * Handles incoming calls.
     *
     * @param state The in call state.
     * @param call The call.
     */
    // @Override
    CARAPI OnIncomingCall(
        /* [in] */ InCallState oldState,
        /* [in] */ InCallState newState,
        /* [in] */ ICall* call);

    /**
     * Handles state changes (including incoming calls)
     *
     * @param newState The in call state.
     * @param callList The call list.
     */
    // @Override
    CARAPI OnStateChange(
        /* [in] */ InCallState oldState,
        /* [in] */ InCallState newState,
        /* [in] */ ICallList* callList);

    /**
     * Handles changes to the details of the call.  The {@link VideoCallPresenter} is interested in
     * changes to the video state.
     *
     * @param call The call for which the details changed.
     * @param details The new call details.
     */
    // @Override
    CARAPI OnDetailsChanged(
        /* [in] */ ICall* call,
        /* [in] */ ICallDetails* details);

    /**
     * Handles peer video pause state changes.
     *
     * @param call The call which paused or un-pausedvideo transmission.
     * @param paused {@code True} when the video transmission is paused, {@code false} when video
     *               transmission resumes.
     */
    // @Override
    CARAPI OnPeerPauseStateChanged(
        /* [in] */ ICall* call,
        /* [in] */ Boolean paused);

    /**
     * Handles peer video dimension changes.
     *
     * @param call The call which experienced a peer video dimension change.
     * @param width The new peer video width .
     * @param height The new peer video height.
     */
    // @Override
    CARAPI OnUpdatePeerDimensions(
        /* [in] */ ICall* call,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Handles a change to the dimensions of the local camera.  Receiving the camera capabilities
     * triggers the creation of the video
     *
     * @param call The call which experienced the camera dimension change.
     * @param width The new camera video width.
     * @param height The new camera video height.
     */
    // @Override
    CARAPI OnCameraDimensionsChange(
        /* [in] */ ICall* call,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Handles hanges to the device orientation.
     * See: {@link Configuration.ORIENTATION_LANDSCAPE}, {@link Configuration.ORIENTATION_PORTRAIT}
     * @param orientation The device orientation.
     */
    // @Override
    CARAPI OnDeviceOrientationChanged(
        /* [in] */ Int32 orientation);

    // @Override
    CARAPI OnUpgradeToVideoRequest(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnUpgradeToVideoSuccess(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnUpgradeToVideoFail(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnDowngradeToAudio(
        /* [in] */ ICall* call);

private:
    /**
     * @return The {@link VideoCall}.
     */
    CARAPI_(AutoPtr<IInCallServiceVideoCall>) GetVideoCall();

    /**
     * Checks for a change to the video call and changes it if required.
     */
    CARAPI_(void) CheckForVideoCallChange();

    /**
     * Checks to see if the current video state has changed and updates the UI if required.
     */
    CARAPI_(void) CheckForVideoStateChange();

    /**
     * Handles a change to the video call.  Sets the surfaces on the previous call to null and sets
     * the surfaces on the new video call accordingly.
     *
     * @param videoCall The new video call.
     */
    CARAPI_(void) ChangeVideoCall(
        /* [in] */ IInCallServiceVideoCall* videoCall);

    /**
     * Enters video mode by showing the video surfaces and making other adjustments (eg. audio).
     * TODO(vt): Need to adjust size and orientation of preview surface here.
     */
    CARAPI_(void) EnterVideoMode();

    /**
     * Exits video mode by hiding the video surfaces  and making other adjustments (eg. audio).
     */
    CARAPI_(void) ExitVideoMode();

    /**
     * Sets the preview surface size based on the current device orientation.
     * See: {@link Configuration.ORIENTATION_LANDSCAPE}, {@link Configuration.ORIENTATION_PORTRAIT}
     *
     * @param orientation The device orientation.
     * @param aspectRatio The aspect ratio of the camera (width / height).
     */
    CARAPI_(void) SetPreviewSize(
        /* [in] */ Int32 orientation,
        /* [in] */ Float aspectRatio);

private:
    static const Int64 SESSION_MODIFICATION_RESET_DELAY_MS = 3000;

    /**
     * The minimum width or height of the preview surface.  Used when re-sizing the preview surface
     * to match the aspect ratio of the currently selected camera.
     */
    Float mMinimumVideoDimension;

    /**
     * The current context.
     */
    AutoPtr<IContext> mContext;

    /**
     * The call the video surfaces are currently related to
     */
    AutoPtr<Call> mPrimaryCall;

    /**
     * The {@link VideoCall} used to inform the video telephony layer of changes to the video
     * surfaces.
     */
    AutoPtr<IInCallServiceVideoCall> mVideoCall;

    /**
     * Determines if the current UI state represents a video call.
     */
    Boolean mIsVideoCall;

    /**
     * Determines the device orientation (portrait/lanscape).
     */
    Int32 mDeviceOrientation;

    /**
     * Tracks the state of the preview surface negotiation with the telephony layer.
     */
    Int32 mPreviewSurfaceState;

    /**
     * Determines whether the video surface is in full-screen mode.
     */
    Boolean mIsFullScreen;

    /**
     * Saves the audio mode which was selected prior to going into a video call.
     */
    Int32 mPreVideoAudioMode;

    /** Handler which resets request state to NO_REQUEST after an interval. */
    AutoPtr<IHandler> mSessionModificationResetHandler;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_VIDEOCALLPRESENTER_H__
