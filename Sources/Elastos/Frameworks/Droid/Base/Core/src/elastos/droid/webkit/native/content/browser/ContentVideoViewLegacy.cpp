
#include "elastos/droid/webkit/native/content/browser/ContentVideoViewLegacy.h"

using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::Media::Session::EIID_IMediaController;
using Elastos::Droid::Widget::EIID_IMediaPlayerControl;

namespace Elastos {
namespace Droid {
namespace Webkit {
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
    assert(0);
//    super(context);
}

CAR_INTERFACE_IMPL(ContentVideoViewLegacy::FullScreenMediaController, Object, IMediaController);

//@Override
ECode ContentVideoViewLegacy::FullScreenMediaController::Show()
{
    assert(0);
#if 0
    super.show();
    if (mListener != NULL) {
        mListener->OnMediaControlsVisibilityChanged(TRUE);
    }

    if (mVideoView != NULL) {
        mVideoView->SetSystemUiVisibility(IView::SYSTEM_UI_FLAG_VISIBLE);
    }
#endif

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

    assert(0);
//    super.hide();

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
    assert(0);
#if 0
    VALIDATE_NOT_NULL(v);
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

    if (IsInPlaybackState() && isKeyCodeSupported && mMediaController != NULL) {
        if (keyCode == IKeyEvent::KEYCODE_HEADSETHOOK ||
                keyCode == IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE) {
            if (IsPlaying()) {
                Pause();
                mMediaController->Show();
            }
            else {
                Start();
                mMediaController->Hide();
            }

            *result = TRUE;
            return NOERROR;
        }
        else if (keyCode == IKeyEvent::KEYCODE_MEDIA_PLAY) {
            if (!IsPlaying()) {
                Start();
                mMediaController->Hide();
            }
            *result = TRUE;
            return NOERROR;
        }
        else if (keyCode == IKeyEvent::KEYCODE_MEDIA_STOP
                || keyCode == IKeyEvent::KEYCODE_MEDIA_PAUSE) {
            if (IsPlaying()) {
                Pause();
                mMediaController->Show();
            }

            *result = TRUE;
            return NOERROR;
        }
        else {
            ToggleMediaControlsVisiblity();
        }
    }
    else if (keyCode == IKeyEvent::KEYCODE_BACK &&
            (event->GetAction(&action), action == IKeyEvent::ACTION_UP)) {
        ExitFullscreen(FALSE);
        *result = TRUE;
            return NOERROR;
    }
    else if (keyCode == IKeyEvent::KEYCODE_MENU || keyCode == IKeyEvent::KEYCODE_SEARCH) {
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
#endif
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
    assert(0);
#if 0
    VALIDATE_NOT_NULL(v);
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);

    Int32 action;
    if (IsInPlaybackState() && mMediaController != NULL &&
            (event->GetAction(&action), action == IMotionEvent::ACTION_DOWN)) {
        ToggleMediaControlsVisiblity();
    }

    *result = TRUE;
#endif
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
    /* [in] */ Int64 nativeContentVideoView,
    /* [in] */ ContentVideoViewClient* client)
    : ContentVideoView(context, nativeContentVideoView, client)
    , mCanPause(FALSE)
    , mCanSeekBackward(FALSE)
    , mCanSeekForward(FALSE)
{
    assert(0);
//    SetBackgroundColor(IColor::BLACK);
    mCurrentBufferPercentage = 0;
}

//@Override
void ContentVideoViewLegacy::ShowContentVideoView()
{
    assert(0);
#if 0
    AutoPtr<ISurfaceView> surfaceView = GetSurfaceView();
    surfaceView->SetZOrderOnTop(TRUE);
    AutoPtr<IViewOnKeyListener> listener = new InnerViewOnKeyListener(this);
    surfaceView->SetOnKeyListener(listener);

    AutoPtr<IViewOnTouchListener> touchListener = new InnerViewOnTouchListener(this);
    SetOnTouchListener(touchListener);
    surfaceView->SetFocusable(TRUE);
    surfaceView->SetFocusableInTouchMode(TRUE);
    surfaceView->RequestFocus();
    ContentVideoView::ShowContentVideoView();
#endif
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
    assert(0);
#if 0
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
        mMediaController->Show(0);
    }
#endif
}

//@Override
ECode ContentVideoViewLegacy::SurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    assert(0);
#if 0
    ContentVideoView::SurfaceChanged(holder, format, width, height);
    AutoPtr<ISurfaceView> surfaceView = GetSurfaceView();
    surfaceView->SetFocusable(TRUE);
    surfaceView->SetFocusableInTouchMode(TRUE);
    if (IsInPlaybackState() && mMediaController != NULL) {
        mMediaController->Show();
    }
#endif

    return NOERROR;
}

//@Override
void ContentVideoViewLegacy::OpenVideo()
{
    assert(0);
#if 0
    ContentVideoView::OpenVideo();

    mCurrentBufferPercentage = 0;

    if (mMediaController != NULL) {
        return;
    }

    mMediaController = new FullScreenMediaController(GetContext(), this, mListener);
    AutoPtr<IMediaController> controller = new InnerMediaPlayerControl(this);
    mMediaController->SetMediaPlayer(controller);
    mMediaController->SetAnchorView(GetSurfaceView());
    mMediaController->SetEnabled(FALSE);
#endif
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
    assert(0);
#if 0
    if (mMediaController != NULL) {
        mMediaController->SetEnabled(FALSE);
        mMediaController->Hide();
        mMediaController = NULL;
    }

    ContentVideoView::DestroyContentVideoView(nativeViewDestroyed);
#endif
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
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
