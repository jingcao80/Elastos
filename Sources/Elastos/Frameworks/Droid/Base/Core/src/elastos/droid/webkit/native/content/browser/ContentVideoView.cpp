
#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/native/content/browser/ContentVideoView.h"
#include "elastos/droid/webkit/native/content/api/ContentVideoView_dec.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::EIID_ISurfaceView;
using Elastos::Droid::Widget::EIID_ILinearLayout;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

//===============================================================
//               ContentVideoView::VideoSurfaceView
//===============================================================

ContentVideoView::VideoSurfaceView::VideoSurfaceView(
    /* [in] */ ContentVideoView* owner,
    /* [in] */ IContext* context)
    : mOwner(owner)
{
    assert(0);
//    super(context);
}

CAR_INTERFACE_IMPL(ContentVideoView::VideoSurfaceView, Object, ISurfaceView);

//@Override
ECode ContentVideoView::VideoSurfaceView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    assert(0);
#if 0
    // set the default surface view size to (1, 1) so that it won't block
    // the infobar. (0, 0) is not a valid size for surface view.
    Int32 width = 1;
    Int32 height = 1;
    if (mOwner->mVideoWidth > 0 && mOwner->mVideoHeight > 0) {
        width = GetDefaultSize(mOwner->mVideoWidth, widthMeasureSpec);
        height = GetDefaultSize(mOwner->mVideoHeight, heightMeasureSpec);
        if (mOwner->mVideoWidth * height  > width * mOwner->mVideoHeight) {
            height = width * mOwner->mVideoHeight / mOwner->mVideoWidth;
        }
        else if (mOwner->mVideoWidth * height  < width * mOwner->mVideoHeight) {
            width = height * mOwner->mVideoWidth / mOwner->mVideoHeight;
        }
    }

    SetMeasuredDimension(width, height);
#endif
    return NOERROR;
}

//===============================================================
//                ContentVideoView::ProgressView
//===============================================================

ContentVideoView::ProgressView::ProgressView(
    /* [in] */ IContext* context,
    /* [in] */ const String& videoLoadingText)
{
    assert(0);
#if 0
    super(context);
    SetOrientation(ILinearLayout::VERTICAL);
    AutoPtr<ILinearLayoutLayoutParams> params;
    CLinearLayoutLayoutParams::New(
            ILinearLayoutLayoutParams::WRAP_CONTENT,
            ILinearLayoutLayoutParams::WRAP_CONTENT,
            (ILinearLayoutLayoutParams**)&params);
    SetLayoutParams(params);
    CProgressBar::New(context, NULL, android::R::attr::progressBarStyleLarge,
        (IProgressBar**)&mProgressBar);
    CTextView::New(context, (ITextView**)&mTextView);
    mTextView->SetText(videoLoadingText);
    AddView(mProgressBar);
    AddView(mTextView);
#endif
}

CAR_INTERFACE_IMPL(ContentVideoView::ProgressView, Object, ILinearLayout);

//===============================================================
//               ContentVideoView::InnerRunnable
//===============================================================

ContentVideoView::InnerRunnable::InnerRunnable(
    /* [in] */ ContentVideoView* owner)
    : mOwner(owner)
{
}

CAR_INTERFACE_IMPL(ContentVideoView::InnerRunnable, Runnable, IRunnable);

ECode ContentVideoView::InnerRunnable::Run()
{
    mOwner->ExitFullscreen(TRUE);
    return NOERROR;
}

//===============================================================
//                     ContentVideoView
//===============================================================

const String ContentVideoView::TAG("ContentVideoView");

/* Do not change these values without updating their counterparts
 * in include/media/mediaplayer.h!
 */
const Int32 ContentVideoView::MEDIA_NOP;
const Int32 ContentVideoView::MEDIA_PREPARED;
const Int32 ContentVideoView::MEDIA_PLAYBACK_COMPLETE;
const Int32 ContentVideoView::MEDIA_BUFFERING_UPDATE;
const Int32 ContentVideoView::MEDIA_SEEK_COMPLETE;
const Int32 ContentVideoView::MEDIA_SET_VIDEO_SIZE;
const Int32 ContentVideoView::MEDIA_ERROR;
const Int32 ContentVideoView::MEDIA_INFO;

/**
 * Keep these error codes in sync with the code we defined in
 * MediaPlayerListener.java.
 */
const Int32 ContentVideoView::MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK;
const Int32 ContentVideoView::MEDIA_ERROR_INVALID_CODE;

// all possible internal states
const Int32 ContentVideoView::STATE_ERROR;
const Int32 ContentVideoView::STATE_IDLE;
const Int32 ContentVideoView::STATE_PLAYING;
const Int32 ContentVideoView::STATE_PAUSED;
const Int32 ContentVideoView::STATE_PLAYBACK_COMPLETED;

ContentVideoView::ContentVideoView(
    /* [in] */ IContext* context,
    /* [in] */ Int64 nativeContentVideoView,
    /* [in] */ ContentVideoViewClient* client)
    : mVideoWidth(0)
    , mVideoHeight(0)
    , mDuration(0)
    , mCurrentState(0)
{
    assert(0);
#if 0
    super(context);
    mExitFullscreenRunnable = new InnerRunnable(this);
    mNativeContentVideoView = nativeContentVideoView;
    mViewAndroid = new ViewAndroid(new WindowAndroid(context.getApplicationContext()), this);
    mClient = client;
    initResources(context);
    mVideoSurfaceView = new VideoSurfaceView(context);
    showContentVideoView();
    setVisibility(View.VISIBLE);
#endif
}

AutoPtr<ContentVideoViewClient> ContentVideoView::GetContentVideoViewClient()
{
    return mClient;
}

void ContentVideoView::InitResources(
    /* [in] */ IContext* context)
{
    assert(0);
#if 0
    if (mPlaybackErrorText != null) return;
    mPlaybackErrorText = context.getString(
            org.chromium.content.R.string.media_player_error_text_invalid_progressive_playback);
    mUnknownErrorText = context.getString(
            org.chromium.content.R.string.media_player_error_text_unknown);
    mErrorButton = context.getString(
            org.chromium.content.R.string.media_player_error_button);
    mErrorTitle = context.getString(
            org.chromium.content.R.string.media_player_error_title);
    mVideoLoadingText = context.getString(
            org.chromium.content.R.string.media_player_loading_video);
#endif
}

void ContentVideoView::ShowContentVideoView()
{
    assert(0);
#if 0
    mVideoSurfaceView.getHolder().addCallback(this);
    this.addView(mVideoSurfaceView, new FrameLayout.LayoutParams(
            ViewGroup.LayoutParams.WRAP_CONTENT,
            ViewGroup.LayoutParams.WRAP_CONTENT,
            Gravity.CENTER));

    mProgressView = mClient.getVideoLoadingProgressView();
    if (mProgressView == null) {
        mProgressView = new ProgressView(getContext(), mVideoLoadingText);
    }
    this.addView(mProgressView, new FrameLayout.LayoutParams(
            ViewGroup.LayoutParams.WRAP_CONTENT,
            ViewGroup.LayoutParams.WRAP_CONTENT,
            Gravity.CENTER));
#endif
}

AutoPtr<ISurfaceView> ContentVideoView::GetSurfaceView()
{
    return mVideoSurfaceView;
}

//@CalledByNative
void ContentVideoView::OnMediaPlayerError(
    /* [in] */ Int32 errorType)
{
    assert(0);
#if 0
    Log.d(TAG, "OnMediaPlayerError: " + errorType);
    if (mCurrentState == STATE_ERROR || mCurrentState == STATE_PLAYBACK_COMPLETED) {
        return;
    }

    // Ignore some invalid error codes.
    if (errorType == MEDIA_ERROR_INVALID_CODE) {
        return;
    }

    mCurrentState = STATE_ERROR;

    /* Pop up an error dialog so the user knows that
     * something bad has happened. Only try and pop up the dialog
     * if we're attached to a window. When we're going away and no
     * longer have a window, don't bother showing the user an error.
     *
     * TODO(qinmin): We need to review whether this Dialog is OK with
     * the rest of the browser UI elements.
     */
    if (getWindowToken() != null) {
        String message;

        if (errorType == MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK) {
            message = mPlaybackErrorText;
        } else {
            message = mUnknownErrorText;
        }

        try {
            new AlertDialog.Builder(getContext())
                .setTitle(mErrorTitle)
                .setMessage(message)
                .setPositiveButton(mErrorButton,
                        new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        /* Inform that the video is over.
                         */
                        onCompletion();
                    }
                })
                .setCancelable(false)
                .show();
        } catch (RuntimeException e) {
            Log.e(TAG, "Cannot show the alert dialog, error message: " + message, e);
        }
    }
#endif
}

//@CalledByNative
void ContentVideoView::OnVideoSizeChanged(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    assert(0);
#if 0
    mVideoWidth = width;
    mVideoHeight = height;
    // This will trigger the SurfaceView.onMeasure() call.
    mVideoSurfaceView.getHolder().setFixedSize(mVideoWidth, mVideoHeight);
#endif
}

//@CalledByNative
void ContentVideoView::OnBufferingUpdate(
    /* [in] */ Int32 percent)
{
}

//@CalledByNative
void ContentVideoView::OnPlaybackComplete()
{
    OnCompletion();
}

//@CalledByNative
void ContentVideoView::OnUpdateMediaMetadata(
    /* [in] */ Int32 videoWidth,
    /* [in] */ Int32 videoHeight,
    /* [in] */ Int32 duration,
    /* [in] */ Boolean canPause,
    /* [in] */ Boolean canSeekBack,
    /* [in] */ Boolean canSeekForward)
{
    mDuration = duration;
    mProgressView->SetVisibility(IView::GONE);
    mCurrentState = IsPlaying() ? STATE_PLAYING : STATE_PAUSED;
    OnVideoSizeChanged(videoWidth, videoHeight);
}

//@Override
ECode ContentVideoView::SurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return NOERROR;
}

//@Override
ECode ContentVideoView::SurfaceCreated(
    /* [in] */ ISurfaceHolder* holder)
{
    mSurfaceHolder = holder;
    OpenVideo();
    return NOERROR;
}

//@Override
ECode ContentVideoView::SurfaceDestroyed(
    /* [in] */ ISurfaceHolder* holder)
{
    if (mNativeContentVideoView != 0) {
        NativeSetSurface(mNativeContentVideoView, NULL);
    }

    mSurfaceHolder = NULL;
    assert(0);
//    Post(mExitFullscreenRunnable);

    return NOERROR;
}

//@CalledByNative
void ContentVideoView::OpenVideo()
{
    if (mSurfaceHolder != NULL) {
        mCurrentState = STATE_IDLE;
        if (mNativeContentVideoView != 0) {
            NativeRequestMediaMetadata(mNativeContentVideoView);
            AutoPtr<ISurface> surface;
            mSurfaceHolder->GetSurface((ISurface**)&surface);
            NativeSetSurface(mNativeContentVideoView,
                    surface);
        }
    }
}

void ContentVideoView::OnCompletion()
{
    mCurrentState = STATE_PLAYBACK_COMPLETED;
}

Boolean ContentVideoView::IsInPlaybackState()
{
    return (mCurrentState != STATE_ERROR && mCurrentState != STATE_IDLE);
}

void ContentVideoView::Start()
{
    if (IsInPlaybackState()) {
        if (mNativeContentVideoView != 0) {
            NativePlay(mNativeContentVideoView);
        }
        mCurrentState = STATE_PLAYING;
    }
}

void ContentVideoView::Pause()
{
    if (IsInPlaybackState()) {
        if (IsPlaying()) {
            if (mNativeContentVideoView != 0) {
                NativePause(mNativeContentVideoView);
            }
            mCurrentState = STATE_PAUSED;
        }
    }
}

// cache duration as mDuration for faster access
Int32 ContentVideoView::GetDuration()
{
    if (IsInPlaybackState()) {
        if (mDuration > 0) {
            return mDuration;
        }

        if (mNativeContentVideoView != 0) {
            mDuration = NativeGetDurationInMilliSeconds(mNativeContentVideoView);
        }
        else {
            mDuration = 0;
        }

        return mDuration;
    }

    mDuration = -1;
    return mDuration;
}

Int32 ContentVideoView::GetCurrentPosition()
{
    if (IsInPlaybackState() && mNativeContentVideoView != 0) {
        return NativeGetCurrentPosition(mNativeContentVideoView);
    }

    return 0;
}

void ContentVideoView::SeekTo(
    /* [in] */ Int32 msec)
{
    if (mNativeContentVideoView != 0) {
        NativeSeekTo(mNativeContentVideoView, msec);
    }
}

Boolean ContentVideoView::IsPlaying()
{
    return mNativeContentVideoView != 0 && NativeIsPlaying(mNativeContentVideoView);
}

//@CalledByNative
AutoPtr<ContentVideoView> ContentVideoView::CreateContentVideoView(
    /* [in] */ IContext* context,
    /* [in] */ Int64 nativeContentVideoView,
    /* [in] */ ContentVideoViewClient* client,
    /* [in] */ Boolean legacy)
{
    assert(0);
#if 0
    ThreadUtils::AssertOnUiThread();
    // The context needs be Activity to create the ContentVideoView correctly.
    if (!isActivityContext(context)) {
        Log.e(TAG, "Wrong type of context, can't create fullscreen video");
        return null;
    }
    ContentVideoView videoView = null;
    if (legacy) {
        videoView = new ContentVideoViewLegacy(context, nativeContentVideoView, client);
    } else {
        videoView = new ContentVideoView(context, nativeContentVideoView, client);
    }

    if (videoView.getContentVideoViewClient().onShowCustomView(videoView)) {
        return videoView;
    }
    return null;
#endif
    return NULL;
}

Boolean ContentVideoView::IsActivityContext(
    /* [in] */ IContext* context)
{
    assert(0);
#if 0
    // Only retrieve the base context if the supplied context is a ContextWrapper but not
    // an Activity, given that Activity is already a subclass of ContextWrapper.
    if (context instanceof ContextWrapper && !(context instanceof Activity)) {
        context = ((ContextWrapper) context).getBaseContext();
    }
    return context instanceof Activity;
#endif

    return FALSE;
}

void ContentVideoView::RemoveSurfaceView()
{
    assert(0);
//    RemoveView(mVideoSurfaceView);
//    RemoveView(mProgressView);
    mVideoSurfaceView = NULL;
    mProgressView = NULL;
}

void ContentVideoView::ExitFullscreen(
    /* [in] */ Boolean relaseMediaPlayer)
{
    DestroyContentVideoView(FALSE);
    if (mNativeContentVideoView != 0) {
        NativeExitFullscreen(mNativeContentVideoView, relaseMediaPlayer);
        mNativeContentVideoView = 0;
    }
}

//@CalledByNative
void ContentVideoView::OnExitFullscreen()
{
    ExitFullscreen(FALSE);
}

/**
 * This method shall only be called by native and exitFullscreen,
 * To exit fullscreen, use exitFullscreen in Java.
 */
//@CalledByNative
void ContentVideoView::DestroyContentVideoView(
    /* [in] */ Boolean nativeViewDestroyed)
{
    if (mVideoSurfaceView != NULL) {
        RemoveSurfaceView();
        assert(0);
//        SetVisibility(IView::GONE);

        // To prevent re-entrance, call this after removeSurfaceView.
        mClient->OnDestroyContentVideoView();
    }

    if (nativeViewDestroyed) {
        mNativeContentVideoView = 0;
    }
}

AutoPtr<ContentVideoView> ContentVideoView::GetContentVideoView()
{
    return NativeGetSingletonJavaContentVideoView();
}

//@Override
ECode ContentVideoView::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    assert(0);
#if 0
    if (keyCode == IKeyEvent::KEYCODE_BACK) {
        ExitFullscreen(FALSE);
        return TRUE;
    }
    return super.onKeyUp(keyCode, event);
#endif

    return NOERROR;
}

//@Override
ECode ContentVideoView::AcquireAnchorView(
    /* [out] */ IView** view)
{
    assert(0);
#if 0
    View anchorView = new View(getContext());
    addView(anchorView);
    return anchorView;
#endif

    return NOERROR;
}

//@Override
ECode ContentVideoView::SetAnchorViewPosition(
    /* [in] */ IView* view,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float width,
    /* [in] */ Float height)
{
//    Log.e(TAG, "setAnchorViewPosition isn't implemented");
    return NOERROR;
}

//@Override
ECode ContentVideoView::ReleaseAnchorView(
    /* [in] */ IView* anchorView)
{
    assert(0);
//    RemoveView(anchorView);
    return NOERROR;
}

//@CalledByNative
Int64 ContentVideoView::GetNativeViewAndroid()
{
    assert(0);
//    return mViewAndroid->GetNativePointer();
    return -1;
}

AutoPtr<ContentVideoView> ContentVideoView::NativeGetSingletonJavaContentVideoView()
{
    AutoPtr<IInterface> cvv = Elastos_ContentVideoView_nativeGetSingletonJavaContentVideoView();
    return (ContentVideoView*)(IObject::Probe(cvv));
}

void ContentVideoView::NativeExitFullscreen(
    /* [in] */ Int64 nativeContentVideoView,
    /* [in] */ Boolean relaseMediaPlayer)
{
    Elastos_ContentVideoView_nativeExitFullscreen(THIS_PROBE(IInterface), (Handle32)nativeContentVideoView, relaseMediaPlayer);
}

Int32 ContentVideoView::NativeGetCurrentPosition(
    /* [in] */ Int64 nativeContentVideoView)
{
    return Elastos_ContentVideoView_nativeGetCurrentPosition(THIS_PROBE(IInterface), (Handle32)nativeContentVideoView);
}

Int32 ContentVideoView::NativeGetDurationInMilliSeconds(
    /* [in] */ Int64 nativeContentVideoView)
{
    return Elastos_ContentVideoView_nativeGetDurationInMilliSeconds(THIS_PROBE(IInterface), (Handle32)nativeContentVideoView);
}

void ContentVideoView::NativeRequestMediaMetadata(
    /* [in] */ Int64 nativeContentVideoView)
{
    Elastos_ContentVideoView_nativeRequestMediaMetadata(THIS_PROBE(IInterface), (Handle32)nativeContentVideoView);
}

Int32 ContentVideoView::NativeGetVideoWidth(
    /* [in] */ Int64 nativeContentVideoView)
{
    return Elastos_ContentVideoView_nativeGetVideoWidth(THIS_PROBE(IInterface), (Handle32)nativeContentVideoView);
}

Int32 ContentVideoView::NativeGetVideoHeight(
    /* [in] */ Int64 nativeContentVideoView)
{
    return Elastos_ContentVideoView_nativeGetVideoHeight(THIS_PROBE(IInterface), (Handle32)nativeContentVideoView);
}

Boolean ContentVideoView::NativeIsPlaying(
    /* [in] */ Int64 nativeContentVideoView)
{
    return Elastos_ContentVideoView_nativeIsPlaying(THIS_PROBE(IInterface), (Handle32)nativeContentVideoView);
}

void ContentVideoView::NativePause(
    /* [in] */ Int64 nativeContentVideoView)
{
    Elastos_ContentVideoView_nativePause(THIS_PROBE(IInterface), (Handle32)nativeContentVideoView);
}

void ContentVideoView::NativePlay(
    /* [in] */ Int64 nativeContentVideoView)
{
    Elastos_ContentVideoView_nativePlay(THIS_PROBE(IInterface), (Handle32)nativeContentVideoView);
}

void ContentVideoView::NativeSeekTo(
    /* [in] */ Int64 nativeContentVideoView,
    /* [in] */ Int32 msec)
{
    Elastos_ContentVideoView_nativeSeekTo(THIS_PROBE(IInterface), (Handle32)nativeContentVideoView, msec);
}

void ContentVideoView::NativeSetSurface(
    /* [in] */ Int64 nativeContentVideoView,
    /* [in] */ ISurface* surface)
{
    Elastos_ContentVideoView_nativeSetSurface(THIS_PROBE(IInterface), (Handle32)nativeContentVideoView, TO_IINTERFACE(surface));
}

void ContentVideoView::OnMediaPlayerError(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 errorType)
{
    AutoPtr<ContentVideoView> mObj = (ContentVideoView*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentVideoView::OnMediaPlayerError, mObj is NULL");
        return;
    }
    mObj->OnMediaPlayerError(errorType);
}

void ContentVideoView::OnVideoSizeChanged(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    AutoPtr<ContentVideoView> mObj = (ContentVideoView*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentVideoView::OnVideoSizeChanged, mObj is NULL");
        return;
    }
    mObj->OnVideoSizeChanged(width, height);
}

void ContentVideoView::OnBufferingUpdate(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 percent)
{
    AutoPtr<ContentVideoView> mObj = (ContentVideoView*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentVideoView::OnBufferingUpdate, mObj is NULL");
        return;
    }
    mObj->OnBufferingUpdate(percent);
}

void ContentVideoView::OnPlaybackComplete(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentVideoView> mObj = (ContentVideoView*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentVideoView::OnPlaybackComplete, mObj is NULL");
        return;
    }
    mObj->OnPlaybackComplete();
}

void ContentVideoView::OnUpdateMediaMetadata(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 videoWidth,
    /* [in] */ Int32 videoHeight,
    /* [in] */ Int32 duration,
    /* [in] */ Boolean canPause,
    /* [in] */ Boolean canSeekBack,
    /* [in] */ Boolean canSeekForward)
{
    AutoPtr<ContentVideoView> mObj = (ContentVideoView*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentVideoView::OnUpdateMediaMetadata, mObj is NULL");
        return;
    }
    mObj->OnUpdateMediaMetadata(videoWidth, videoHeight, duration, canPause, canSeekBack, canSeekForward);
}

void ContentVideoView::OpenVideo(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentVideoView> mObj = (ContentVideoView*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentVideoView::OpenVideo, mObj is NULL");
        return;
    }
    mObj->OpenVideo();
}

AutoPtr<IInterface> ContentVideoView::CreateContentVideoView(
    /* [in] */ IInterface* context,
    /* [in] */ Int64 nativeContentVideoView,
    /* [in] */ IInterface* client,
    /* [in] */ Boolean legacy)
{
    AutoPtr<ContentVideoViewClient> cvvc = (ContentVideoViewClient*)(IObject::Probe(client));
    AutoPtr<IContext> c = IContext::Probe(context);
    return TO_IINTERFACE(CreateContentVideoView(c, nativeContentVideoView, cvvc, legacy));
}

void ContentVideoView::OnExitFullscreen(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentVideoView> mObj = (ContentVideoView*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentVideoView::OnExitFullscreen, mObj is NULL");
        return;
    }
    mObj->OnExitFullscreen();
}

void ContentVideoView::DestroyContentVideoView(
    /* [in] */ IInterface* obj,
    /* [in] */ Boolean nativeViewDestroyed)
{
    AutoPtr<ContentVideoView> mObj = (ContentVideoView*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentVideoView::DestroyContentVideoView, mObj is NULL");
        return;
    }
    mObj->DestroyContentVideoView(nativeViewDestroyed);
}

Int64 ContentVideoView::GetNativeViewAndroid(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentVideoView> mObj = (ContentVideoView*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentVideoView::GetNativeViewAndroid, mObj is NULL");
        return 0;
    }
    return mObj->GetNativeViewAndroid();
}

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
