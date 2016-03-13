
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentVideoView.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentVideoViewLegacy.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/ContentVideoView_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/content/R_Content.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

//using Elastos::Droid::View::EIID_ISurfaceView;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::View::EIID_ISurfaceHolderCallback;
using Elastos::Droid::View::CView;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::ContentVideoViewLegacy;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Droid::Widget::EIID_ILinearLayout;
using Elastos::Droid::Widget::ILinearLayoutLayoutParams;
using Elastos::Droid::Widget::CLinearLayoutLayoutParams;
using Elastos::Droid::Widget::CTextView;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::CProgressBar;
using Elastos::Droid::Widget::CFrameLayoutLayoutParams;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
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
    SurfaceView::constructor(context);
}

//CAR_INTERFACE_IMPL(ContentVideoView::VideoSurfaceView, Object, ISurfaceView);

//@Override
void ContentVideoView::VideoSurfaceView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
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
}

//===============================================================
//                ContentVideoView::ProgressView
//===============================================================

ContentVideoView::ProgressView::ProgressView(
    /* [in] */ IContext* context,
    /* [in] */ const String& videoLoadingText)
{
    LinearLayout::constructor(context);
    SetOrientation(ILinearLayout::VERTICAL);
    AutoPtr<ILinearLayoutLayoutParams> params;
    CLinearLayoutLayoutParams::New(
            IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::WRAP_CONTENT,
            (ILinearLayoutLayoutParams**)&params);
    SetLayoutParams(IViewGroupLayoutParams::Probe(params));
    CProgressBar::New(context, NULL, Elastos::Droid::R::attr::progressBarStyleLarge,
        (IProgressBar**)&mProgressBar);
    CTextView::New(context, (ITextView**)&mTextView);
    AutoPtr<ICharSequence> vdText;
    CString::New(videoLoadingText, (ICharSequence**)&vdText);
    mTextView->SetText(vdText);
    AddView(IView::Probe(mProgressBar));
    AddView(IView::Probe(mTextView));
}

//CAR_INTERFACE_IMPL(ContentVideoView::ProgressView, Object, ILinearLayout);

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

//==============================================================
//           ContentVideoView::PositiveButtonClickListener
//==============================================================
CAR_INTERFACE_IMPL(ContentVideoView::PositiveButtonClickListener, Object, IDialogInterfaceOnClickListener);

ContentVideoView::PositiveButtonClickListener::PositiveButtonClickListener(
    /* [in] */ ContentVideoView* owner)
    : mOwner(owner)
{
}

ECode ContentVideoView::PositiveButtonClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    /* Inform that the video is over.
    */
    mOwner->OnCompletion();
    return NOERROR;
}

//==============================================================

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

CAR_INTERFACE_IMPL(ContentVideoView, FrameLayout, ISurfaceHolderCallback);

ContentVideoView::ContentVideoView(
    /* [in] */ IContext* context,
    /* [in] */ Handle64 nativeContentVideoView,
    /* [in] */ ContentVideoViewClient* client)
    : mVideoWidth(0)
    , mVideoHeight(0)
    , mDuration(0)
    , mCurrentState(0)
{
    FrameLayout::constructor(context);
    mExitFullscreenRunnable = new InnerRunnable(this);
    mNativeContentVideoView = nativeContentVideoView;
    AutoPtr<IContext> ctx;
    context->GetApplicationContext((IContext**)&ctx);
    mViewAndroid = new ViewElastos(new WindowElastos(ctx), this);
    mClient = client;
    InitResources(context);
    mVideoSurfaceView = new VideoSurfaceView(this, context);
    ShowContentVideoView();
    SetVisibility(IView::VISIBLE);
}

AutoPtr<ContentVideoViewClient> ContentVideoView::GetContentVideoViewClient()
{
    return mClient;
}

void ContentVideoView::InitResources(
    /* [in] */ IContext* context)
{
    if (mPlaybackErrorText != NULL) return;
    //mPlaybackErrorText =
    context->GetString(Elastos::Droid::Webkit::Webview::Chromium::Content::R::string::media_player_error_text_invalid_progressive_playback,
            &mPlaybackErrorText);
    //mUnknownErrorText =
    context->GetString(Elastos::Droid::Webkit::Webview::Chromium::Content::R::string::media_player_error_text_unknown,
            &mUnknownErrorText);
    //mErrorButton =
    context->GetString(Elastos::Droid::Webkit::Webview::Chromium::Content::R::string::media_player_error_button,
            &mErrorButton);
    //mErrorTitle =
    context->GetString(Elastos::Droid::Webkit::Webview::Chromium::Content::R::string::media_player_error_title,
            &mErrorTitle);
    //mVideoLoadingText =
    context->GetString(Elastos::Droid::Webkit::Webview::Chromium::Content::R::string::media_player_loading_video,
            &mVideoLoadingText);
}

void ContentVideoView::ShowContentVideoView()
{
    AutoPtr<ISurfaceHolder> surfaceHolder;
    mVideoSurfaceView->GetHolder((ISurfaceHolder**)&surfaceHolder);
    surfaceHolder->AddCallback(this);

    AutoPtr<IFrameLayoutLayoutParams> flp;
    CFrameLayoutLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
                                  IViewGroupLayoutParams::WRAP_CONTENT,
                                  IGravity::CENTER,
                                  (IFrameLayoutLayoutParams**)&flp);
    this->AddView(mVideoSurfaceView, IViewGroupLayoutParams::Probe(flp));

    mProgressView = mClient->GetVideoLoadingProgressView();
    if (mProgressView == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        mProgressView = new ProgressView(context, mVideoLoadingText);
    }
    this->AddView(mProgressView, IViewGroupLayoutParams::Probe(flp));
}

AutoPtr<ISurfaceView> ContentVideoView::GetSurfaceView()
{
    return mVideoSurfaceView;
}

//@CalledByNative
void ContentVideoView::OnMediaPlayerError(
    /* [in] */ Int32 errorType)
{
    Logger::D(TAG, "OnMediaPlayerError: %d", errorType);
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
    AutoPtr<IBinder> windowToken;
    if ((GetWindowToken((IBinder**)&windowToken), windowToken) != NULL) {
        String message;

        if (errorType == MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK) {
            message = mPlaybackErrorText;
        } else {
            message = mUnknownErrorText;
        }

        //try {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&builder);
            //new AlertDialog.Builder(getContext())
        AutoPtr<ICharSequence> title;
        CString::New(mErrorTitle, (ICharSequence**)&title);
        builder->SetTitle(title);
        AutoPtr<ICharSequence> msg;
        CString::New(message, (ICharSequence**)&msg);
        builder->SetMessage(msg);
        AutoPtr<ICharSequence> pButton;
        CString::New(mErrorButton, (ICharSequence**)&pButton);
        builder->SetPositiveButton(pButton,
                        new PositiveButtonClickListener(this));
        builder->SetCancelable(FALSE);
        AutoPtr<IAlertDialog> ad;
        builder->Show((IAlertDialog**)&ad);
        //} catch (RuntimeException e) {
        //    Log.e(TAG, "Cannot show the alert dialog, error message: " + message, e);
        //}
    }
}

//@CalledByNative
void ContentVideoView::OnVideoSizeChanged(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mVideoWidth = width;
    mVideoHeight = height;
    // This will trigger the SurfaceView.onMeasure() call.
    AutoPtr<ISurfaceHolder> surfaceHolder;
    mVideoSurfaceView->GetHolder((ISurfaceHolder**)&surfaceHolder);
    surfaceHolder->SetFixedSize(mVideoWidth, mVideoHeight);
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
    Boolean res;
    Post(mExitFullscreenRunnable, &res);

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
    /* [in] */ Handle64 nativeContentVideoView,
    /* [in] */ ContentVideoViewClient* client,
    /* [in] */ Boolean legacy)
{
    ThreadUtils::AssertOnUiThread();
    // The context needs be Activity to create the ContentVideoView correctly.
    if (!IsActivityContext(context)) {
        Logger::E(TAG, "Wrong type of context, can't create fullscreen video");
        return NULL;
    }
    AutoPtr<ContentVideoView> videoView = NULL;
    if (legacy) {
        videoView = new ContentVideoViewLegacy(context, nativeContentVideoView, client);
    } else {
        videoView = new ContentVideoView(context, nativeContentVideoView, client);
    }

    if (videoView->GetContentVideoViewClient()->OnShowCustomView(videoView)) {
        return videoView;
    }
    return NULL;
}

Boolean ContentVideoView::IsActivityContext(
    /* [in] */ IContext* context)
{
    // Only retrieve the base context if the supplied context is a ContextWrapper but not
    // an Activity, given that Activity is already a subclass of ContextWrapper.
    AutoPtr<IActivity> activity = IActivity::Probe(context);
    AutoPtr<IContextWrapper> cw = IContextWrapper::Probe(context);
    if (cw != NULL && !(activity == NULL)) {
        //context = ((ContextWrapper) context).getBaseContext();
        AutoPtr<IContext> baseContext;
        cw->GetBaseContext((IContext**)&baseContext);
        context = baseContext;
    }
    AutoPtr<IActivity> a = IActivity::Probe(context);
    return a != NULL;
}

void ContentVideoView::RemoveSurfaceView()
{
    RemoveView(mVideoSurfaceView);
    RemoveView(mProgressView);
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
        SetVisibility(IView::GONE);

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
    if (keyCode == IKeyEvent::KEYCODE_BACK) {
        ExitFullscreen(FALSE);
        *result = TRUE;
        return NOERROR;
    }
    return FrameLayout::OnKeyUp(keyCode, event, result);
}

//@Override
ECode ContentVideoView::AcquireAnchorView(
    /* [out] */ IView** view)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IView> anchorView;
     CView::New(context, (IView**)&anchorView);
    AddView(anchorView);
    return NOERROR;
}

AutoPtr<IView> ContentVideoView::AcquireAnchorView()
{
    AutoPtr<IView> view;
    AcquireAnchorView((IView**)&view);
    return view;
}

//@Override
ECode ContentVideoView::SetAnchorViewPosition(
    /* [in] */ IView* view,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float width,
    /* [in] */ Float height)
{
    Logger::E(TAG, "setAnchorViewPosition isn't implemented");
    return NOERROR;
}

//@Override
ECode ContentVideoView::ReleaseAnchorView(
    /* [in] */ IView* anchorView)
{
    RemoveView(anchorView);
    return NOERROR;
}

//@CalledByNative
Handle64 ContentVideoView::GetNativeViewAndroid()
{
    return mViewAndroid->GetNativePointer();
}

AutoPtr<ContentVideoView> ContentVideoView::NativeGetSingletonJavaContentVideoView()
{
    AutoPtr<IInterface> cvv = Elastos_ContentVideoView_nativeGetSingletonJavaContentVideoView();
    return (ContentVideoView*)(FrameLayout*)(IObject::Probe(cvv));
}

void ContentVideoView::NativeExitFullscreen(
    /* [in] */ Handle64 nativeContentVideoView,
    /* [in] */ Boolean relaseMediaPlayer)
{
    Elastos_ContentVideoView_nativeExitFullscreen(THIS_PROBE(IInterface), nativeContentVideoView, relaseMediaPlayer);
}

Int32 ContentVideoView::NativeGetCurrentPosition(
    /* [in] */ Handle64 nativeContentVideoView)
{
    return Elastos_ContentVideoView_nativeGetCurrentPosition(THIS_PROBE(IInterface), nativeContentVideoView);
}

Int32 ContentVideoView::NativeGetDurationInMilliSeconds(
    /* [in] */ Handle64 nativeContentVideoView)
{
    return Elastos_ContentVideoView_nativeGetDurationInMilliSeconds(THIS_PROBE(IInterface), nativeContentVideoView);
}

void ContentVideoView::NativeRequestMediaMetadata(
    /* [in] */ Handle64 nativeContentVideoView)
{
    Elastos_ContentVideoView_nativeRequestMediaMetadata(THIS_PROBE(IInterface), nativeContentVideoView);
}

Int32 ContentVideoView::NativeGetVideoWidth(
    /* [in] */ Handle64 nativeContentVideoView)
{
    return Elastos_ContentVideoView_nativeGetVideoWidth(THIS_PROBE(IInterface), nativeContentVideoView);
}

Int32 ContentVideoView::NativeGetVideoHeight(
    /* [in] */ Handle64 nativeContentVideoView)
{
    return Elastos_ContentVideoView_nativeGetVideoHeight(THIS_PROBE(IInterface), nativeContentVideoView);
}

Boolean ContentVideoView::NativeIsPlaying(
    /* [in] */ Handle64 nativeContentVideoView)
{
    return Elastos_ContentVideoView_nativeIsPlaying(THIS_PROBE(IInterface), nativeContentVideoView);
}

void ContentVideoView::NativePause(
    /* [in] */ Handle64 nativeContentVideoView)
{
    Elastos_ContentVideoView_nativePause(THIS_PROBE(IInterface), nativeContentVideoView);
}

void ContentVideoView::NativePlay(
    /* [in] */ Handle64 nativeContentVideoView)
{
    Elastos_ContentVideoView_nativePlay(THIS_PROBE(IInterface), nativeContentVideoView);
}

void ContentVideoView::NativeSeekTo(
    /* [in] */ Handle64 nativeContentVideoView,
    /* [in] */ Int32 msec)
{
    Elastos_ContentVideoView_nativeSeekTo(THIS_PROBE(IInterface), nativeContentVideoView, msec);
}

void ContentVideoView::NativeSetSurface(
    /* [in] */ Handle64 nativeContentVideoView,
    /* [in] */ ISurface* surface)
{
    Elastos_ContentVideoView_nativeSetSurface(THIS_PROBE(IInterface), nativeContentVideoView, TO_IINTERFACE(surface));
}

void ContentVideoView::OnMediaPlayerError(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 errorType)
{
    AutoPtr<ContentVideoView> mObj = (ContentVideoView*)(FrameLayout*)(IObject::Probe(obj));
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
    AutoPtr<ContentVideoView> mObj = (ContentVideoView*)(FrameLayout*)(IObject::Probe(obj));
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
    AutoPtr<ContentVideoView> mObj = (ContentVideoView*)(FrameLayout*)(IObject::Probe(obj));
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
    AutoPtr<ContentVideoView> mObj = (ContentVideoView*)(FrameLayout*)(IObject::Probe(obj));
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
    AutoPtr<ContentVideoView> mObj = (ContentVideoView*)(FrameLayout*)(IObject::Probe(obj));
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
    AutoPtr<ContentVideoView> mObj = (ContentVideoView*)(FrameLayout*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentVideoView::OpenVideo, mObj is NULL");
        return;
    }
    mObj->OpenVideo();
}

AutoPtr<IInterface> ContentVideoView::CreateContentVideoView(
    /* [in] */ IInterface* context,
    /* [in] */ Handle64 nativeContentVideoView,
    /* [in] */ IInterface* client,
    /* [in] */ Boolean legacy)
{
    AutoPtr<ContentVideoViewClient> cvvc = (ContentVideoViewClient*)(FrameLayout*)(IObject::Probe(client));
    AutoPtr<IContext> c = IContext::Probe(context);
    return TO_IINTERFACE(CreateContentVideoView(c, nativeContentVideoView, cvvc, legacy));
}

void ContentVideoView::OnExitFullscreen(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentVideoView> mObj = (ContentVideoView*)(FrameLayout*)(IObject::Probe(obj));
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
    AutoPtr<ContentVideoView> mObj = (ContentVideoView*)(FrameLayout*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentVideoView::DestroyContentVideoView, mObj is NULL");
        return;
    }
    mObj->DestroyContentVideoView(nativeViewDestroyed);
}

Handle64 ContentVideoView::GetNativeViewAndroid(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentVideoView> mObj = (ContentVideoView*)(FrameLayout*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentVideoView::GetNativeViewAndroid, mObj is NULL");
        return 0;
    }
    return mObj->GetNativeViewAndroid();
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
