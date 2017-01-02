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

#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentVideoViewLegacy.h"

using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::Media::Session::EIID_IMediaController;
using Elastos::Droid::Widget::EIID_IMediaPlayerControl;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//===============================================================
//      ContentVideoViewLegacy::FullScreenMediaController
//===============================================================

/**
 * @param context The context.
 * @param video The full screen video container view.
 * @param listener A listener that listens to the visibility of media controllers.
 */
ContentVideoViewLegacy::FullScreenMediaController::FullScreenMediaController(
    /* [in] */ IContext* context,
    /* [in] */ IView* video,
    /* [in] */ MediaControlsVisibilityListener* listener)
    : mVideoView(video)
    , mListener(listener)
{
    FrameLayout::constructor(context);
}

//CAR_INTERFACE_IMPL(ContentVideoViewLegacy::FullScreenMediaController, Object, IMediaController);

//@Override
ECode ContentVideoViewLegacy::FullScreenMediaController::Show()
{
    MediaController::Show();
    if (mListener != NULL) {
        mListener->OnMediaControlsVisibilityChanged(TRUE);
    }

    if (mVideoView != NULL) {
        mVideoView->SetSystemUiVisibility(IView::SYSTEM_UI_FLAG_VISIBLE);
    }
    return NOERROR;
}

//@Override
ECode ContentVideoViewLegacy::FullScreenMediaController::Hide()
{
    if (mVideoView != NULL) {
        mVideoView->SetSystemUiVisibility(IView::SYSTEM_UI_FLAG_LOW_PROFILE);
    }

    if (mListener != NULL) {
        mListener->OnMediaControlsVisibilityChanged(FALSE);
    }

    MediaController::Hide();

    return NOERROR;
}

//===============================================================
//         ContentVideoViewLegacy::InnerViewOnKeyListener
//===============================================================

ContentVideoViewLegacy::InnerViewOnKeyListener::InnerViewOnKeyListener(
    /* [in] */ ContentVideoViewLegacy* owner)
    : mOwner(owner)
{
}

CAR_INTERFACE_IMPL(ContentVideoViewLegacy::InnerViewOnKeyListener, Object, IViewOnKeyListener);

ECode ContentVideoViewLegacy::InnerViewOnKeyListener::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 action;

    Boolean isKeyCodeSupported = (
            keyCode != IKeyEvent::KEYCODE_BACK &&
            keyCode != IKeyEvent::KEYCODE_VOLUME_UP &&
            keyCode != IKeyEvent::KEYCODE_VOLUME_DOWN &&
            keyCode != IKeyEvent::KEYCODE_VOLUME_MUTE &&
            keyCode != IKeyEvent::KEYCODE_CALL &&
            keyCode != IKeyEvent::KEYCODE_MENU &&
            keyCode != IKeyEvent::KEYCODE_SEARCH &&
            keyCode != IKeyEvent::KEYCODE_ENDCALL);

    if (mOwner->IsInPlaybackState() && isKeyCodeSupported && mOwner->mMediaController != NULL) {
        if (keyCode == IKeyEvent::KEYCODE_HEADSETHOOK ||
                keyCode == IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE) {
            if (mOwner->IsPlaying()) {
                mOwner->Pause();
                mOwner->mMediaController->Show();
            }
            else {
                mOwner->Start();
                mOwner->mMediaController->Hide();
            }

            *result = TRUE;
            return NOERROR;
        }
        else if (keyCode == IKeyEvent::KEYCODE_MEDIA_PLAY) {
            if (!mOwner->IsPlaying()) {
                mOwner->Start();
                mOwner->mMediaController->Hide();
            }
            *result = TRUE;
            return NOERROR;
        }
        else if (keyCode == IKeyEvent::KEYCODE_MEDIA_STOP
                || keyCode == IKeyEvent::KEYCODE_MEDIA_PAUSE) {
            if (mOwner->IsPlaying()) {
                mOwner->Pause();
                mOwner->mMediaController->Show();
            }

            *result = TRUE;
            return NOERROR;
        }
        else {
            mOwner->ToggleMediaControlsVisiblity();
        }
    }
    else if (keyCode == IKeyEvent::KEYCODE_BACK &&
            (event->GetAction(&action), action == IKeyEvent::ACTION_UP)) {
        mOwner->ExitFullscreen(FALSE);
        *result = TRUE;
        return NOERROR;
    }
    else if (keyCode == IKeyEvent::KEYCODE_MENU || keyCode == IKeyEvent::KEYCODE_SEARCH) {
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

//===============================================================
//       ContentVideoViewLegacy::InnerViewOnTouchListener
//===============================================================

ContentVideoViewLegacy::InnerViewOnTouchListener::InnerViewOnTouchListener(
    /* [in] */ ContentVideoViewLegacy* owner)
    : mOwner(owner)
{
}

CAR_INTERFACE_IMPL(ContentVideoViewLegacy::InnerViewOnTouchListener, Object, IViewOnTouchListener);

ECode ContentVideoViewLegacy::InnerViewOnTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 action;
    if (mOwner->IsInPlaybackState() && mOwner->mMediaController != NULL &&
            (event->GetAction(&action), action == IMotionEvent::ACTION_DOWN)) {
        mOwner->ToggleMediaControlsVisiblity();
    }

    *result = TRUE;
    return NOERROR;
}

//===============================================================
//       ContentVideoViewLegacy::InnerMediaPlayerControl
//===============================================================

ContentVideoViewLegacy::InnerMediaPlayerControl::InnerMediaPlayerControl(
    /* [in] */ ContentVideoViewLegacy* owner)
    : mOwner(owner)
{
}

CAR_INTERFACE_IMPL(ContentVideoViewLegacy::InnerMediaPlayerControl, Object, IMediaPlayerControl);

ECode ContentVideoViewLegacy::InnerMediaPlayerControl::Start()
{
    mOwner->Start();
    return NOERROR;
}

ECode ContentVideoViewLegacy::InnerMediaPlayerControl::Pause()
{
    mOwner->Pause();
    return NOERROR;
}

ECode ContentVideoViewLegacy::InnerMediaPlayerControl::GetDuration(
    /* [out] */ Int32* duration)
{
    VALIDATE_NOT_NULL(duration);
    *duration = mOwner->GetDuration();
    return NOERROR;
}

ECode ContentVideoViewLegacy::InnerMediaPlayerControl::GetCurrentPosition(
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position);
    *position = mOwner->GetCurrentPosition();
    return NOERROR;
}

ECode ContentVideoViewLegacy::InnerMediaPlayerControl::SeekTo(
    /* [in] */ Int32 pos)
{
    mOwner->SeekTo(pos);
    return NOERROR;
}

ECode ContentVideoViewLegacy::InnerMediaPlayerControl::IsPlaying(
    /* [out] */ Boolean* isPlaying)
{
    VALIDATE_NOT_NULL(isPlaying);
    *isPlaying = mOwner->IsPlaying();
    return NOERROR;
}

ECode ContentVideoViewLegacy::InnerMediaPlayerControl::GetBufferPercentage(
    /* [out] */ Int32* percentage)
{
    VALIDATE_NOT_NULL(percentage);
    *percentage = mOwner->mCurrentBufferPercentage;
    return NOERROR;
}

ECode ContentVideoViewLegacy::InnerMediaPlayerControl::CanPause(
    /* [out] */ Boolean* canPause)
{
    VALIDATE_NOT_NULL(canPause);
    *canPause = mOwner->mCanPause;
    return NOERROR;
}

ECode ContentVideoViewLegacy::InnerMediaPlayerControl::CanSeekBackward(
    /* [out] */ Boolean* canSeekBackward)
{
    VALIDATE_NOT_NULL(canSeekBackward);
    *canSeekBackward = mOwner->mCanSeekBackward;
    return NOERROR;
}

ECode ContentVideoViewLegacy::InnerMediaPlayerControl::CanSeekForward(
    /* [out] */ Boolean* canSeekForward)
{
    VALIDATE_NOT_NULL(canSeekForward);
    *canSeekForward = mOwner->mCanSeekForward;
    return NOERROR;
}

ECode ContentVideoViewLegacy::InnerMediaPlayerControl::GetAudioSessionId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = 0;
    return NOERROR;
}

//===============================================================
//                ContentVideoViewLegacy
//===============================================================

ContentVideoViewLegacy::ContentVideoViewLegacy(
    /* [in] */ IContext* context,
    /* [in] */ Handle64 nativeContentVideoView,
    /* [in] */ ContentVideoViewClient* client)
    : ContentVideoView(context, nativeContentVideoView, client)
    , mCanPause(FALSE)
    , mCanSeekBackward(FALSE)
    , mCanSeekForward(FALSE)
{
    SetBackgroundColor(IColor::BLACK);
    mCurrentBufferPercentage = 0;
}

//@Override
void ContentVideoViewLegacy::ShowContentVideoView()
{
    AutoPtr<ISurfaceView> surfaceView = GetSurfaceView();
    surfaceView->SetZOrderOnTop(TRUE);
    AutoPtr<IViewOnKeyListener> listener = new InnerViewOnKeyListener(this);
    IView::Probe(surfaceView)->SetOnKeyListener(listener);

    AutoPtr<IViewOnTouchListener> touchListener = new InnerViewOnTouchListener(this);
    SetOnTouchListener(touchListener);
    IView::Probe(surfaceView)->SetFocusable(TRUE);
    IView::Probe(surfaceView)->SetFocusableInTouchMode(TRUE);
    Boolean res;
    IView::Probe(surfaceView)->RequestFocus(&res);
    ContentVideoView::ShowContentVideoView();
}

//@Override
void ContentVideoViewLegacy::OnMediaPlayerError(
    /* [in] */ Int32 errorType)
{
    ContentVideoView::OnMediaPlayerError(errorType);

    if (errorType == MEDIA_ERROR_INVALID_CODE) {
        return;
    }

    if (mMediaController != NULL) {
        mMediaController->Hide();
    }
}

//@Override
void ContentVideoViewLegacy::OnBufferingUpdate(
    /* [in] */ Int32 percent)
{
    ContentVideoView::OnBufferingUpdate(percent);
    mCurrentBufferPercentage = percent;
}

//@Override
void ContentVideoViewLegacy::OnUpdateMediaMetadata(
    /* [in] */ Int32 videoWidth,
    /* [in] */ Int32 videoHeight,
    /* [in] */ Int32 duration,
    /* [in] */ Boolean canPause,
    /* [in] */ Boolean canSeekBack,
    /* [in] */ Boolean canSeekForward)
{
    ContentVideoView::OnUpdateMediaMetadata(videoWidth, videoHeight, duration,
            canPause, canSeekBack, canSeekForward);
    mCanPause = canPause;
    mCanSeekBackward = canSeekBack;
    mCanSeekForward = canSeekForward;

    if (mMediaController == NULL) return;

    mMediaController->SetEnabled(TRUE);
    // If paused , should show the controller forever.
    if (IsPlaying()) {
        mMediaController->Show();
    }
    else {
        mMediaController->MediaController::Show(0);
    }
}

//@Override
ECode ContentVideoViewLegacy::SurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    ContentVideoView::SurfaceChanged(holder, format, width, height);
    AutoPtr<ISurfaceView> surfaceView = GetSurfaceView();
    IView::Probe(surfaceView)->SetFocusable(TRUE);
    IView::Probe(surfaceView)->SetFocusableInTouchMode(TRUE);
    if (IsInPlaybackState() && mMediaController != NULL) {
        mMediaController->Show();
    }

    return NOERROR;
}

//@Override
void ContentVideoViewLegacy::OpenVideo()
{
    ContentVideoView::OpenVideo();

    mCurrentBufferPercentage = 0;

    if (mMediaController != NULL) {
        return;
    }

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    mMediaController = new FullScreenMediaController(context, this, mListener);
    AutoPtr<IMediaPlayerControl> control = new InnerMediaPlayerControl(this);
    mMediaController->SetMediaPlayer(control);
    mMediaController->SetAnchorView(IView::Probe(GetSurfaceView()));
    mMediaController->SetEnabled(FALSE);
}

//@Override
void ContentVideoViewLegacy::OnCompletion()
{
    ContentVideoView::OnCompletion();
    if (mMediaController != NULL) {
        mMediaController->Hide();
    }
}

//@Override
Boolean ContentVideoViewLegacy::OnTrackballEvent(
    /* [in] */ IMotionEvent* ev)
{
    if (IsInPlaybackState() && mMediaController != NULL) {
        ToggleMediaControlsVisiblity();
    }

    return FALSE;
}

void ContentVideoViewLegacy::ToggleMediaControlsVisiblity()
{
    Boolean isShowing = FALSE;
    mMediaController->IsShowing(&isShowing);
    if (isShowing) {
        mMediaController->Hide();
    }
    else {
        mMediaController->Show();
    }
}

//@Override
void ContentVideoViewLegacy::DestroyContentVideoView(
    /* [in] */ Boolean nativeViewDestroyed)
{
    if (mMediaController != NULL) {
        mMediaController->SetEnabled(FALSE);
        mMediaController->Hide();
        mMediaController = NULL;
    }

    ContentVideoView::DestroyContentVideoView(nativeViewDestroyed);
}

//@Override
Boolean ContentVideoViewLegacy::OnTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    return TRUE;
}

/**
 * Sets the MediaControlsVisibilityListener that wants to listen to visibility change events.
 *
 * @param listener the listener to send the events to.
 */
void ContentVideoViewLegacy::SetListener(
    /* [in] */ MediaControlsVisibilityListener* listener)
{
    mListener = listener;
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
