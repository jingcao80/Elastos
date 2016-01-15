
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_CONTENTVIDEOVIEW_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_CONTENTVIDEOVIEW_H__

#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/content/browser/ContentVideoViewClient.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Core::IRunnable;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContextWrapper;
using Elastos::Droid::Content::IDialogInterface;
// import android.util.Log;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Droid::View::ISurfaceView;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import org.chromium.base.ThreadUtils;
// import org.chromium.ui.base.ViewAndroid;
// import org.chromium.ui.base.ViewAndroidDelegate;
// import org.chromium.ui.base.WindowAndroid;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

/**
 * This class implements accelerated fullscreen video playback using surface view.
 */
//@JNINamespace("content")
class ContentVideoView
    : public Object
//    : public FrameLayout
//    , public ISurfaceHolderCallback
//    , public ViewAndroidDelegate
{
private:
    class VideoSurfaceView
        : public Object
        , public ISurfaceView
    {
    public:
        VideoSurfaceView(
            /* [in] */ ContentVideoView* owner,
            /* [in] */ IContext* context);

        CAR_INTERFACE_DECL()

    protected:
        //@Override
        CARAPI OnMeasure(
            /* [in] */ Int32 widthMeasureSpec,
            /* [in] */ Int32 heightMeasureSpec);

    private:
        ContentVideoView* mOwner;
    };

    class ProgressView
        : public Object
        , public ILinearLayout
    {
    public:
        ProgressView(
            /* [in] */ IContext* context,
            /* [in] */ const String& videoLoadingText);

        CAR_INTERFACE_DECL()

    private:
        const AutoPtr<IProgressBar> mProgressBar;
        const AutoPtr<ITextView> mTextView;
    };

    class InnerRunnable
        : public Runnable
    {
    public:
        InnerRunnable(
            /* [in] */ ContentVideoView* owner);

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        ContentVideoView* mOwner;
    };

public:
    //@CalledByNative
    virtual CARAPI_(void) OnMediaPlayerError(
        /* [in] */ Int32 errorType);

    //@Override
    CARAPI SurfaceChanged(
        /* [in] */ ISurfaceHolder* holder,
        /* [in] */ Int32 format,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    //@Override
    CARAPI SurfaceCreated(
        /* [in] */ ISurfaceHolder* holder);

    //@Override
    CARAPI SurfaceDestroyed(
        /* [in] */ ISurfaceHolder* holder);

    virtual CARAPI_(Boolean) IsPlaying();

    virtual CARAPI_(void) RemoveSurfaceView();

    virtual CARAPI_(void) ExitFullscreen(
        /* [in] */ Boolean relaseMediaPlayer);

    static CARAPI_(AutoPtr<ContentVideoView>) GetContentVideoView();

    //@Override
    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI AcquireAnchorView(
        /* [out] */ IView** view);

    //@Override
    CARAPI SetAnchorViewPosition(
        /* [in] */ IView* view,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float width,
        /* [in] */ Float height);

    //@Override
    CARAPI ReleaseAnchorView(
        /* [in] */ IView* anchorView);

    static CARAPI_(void*) ElaContentVideoViewCallback_Init();

protected:
    ContentVideoView(
        /* [in] */ IContext* context,
        /* [in] */ Int64 nativeContentVideoView,
        /* [in] */ ContentVideoViewClient* client);

    virtual CARAPI_(AutoPtr<ContentVideoViewClient>) GetContentVideoViewClient();

    virtual CARAPI_(void) ShowContentVideoView();

    virtual CARAPI_(AutoPtr<ISurfaceView>) GetSurfaceView();

    //@CalledByNative
    virtual CARAPI_(void) OnBufferingUpdate(
        /* [in] */ Int32 percent);

    //@CalledByNative
    virtual CARAPI_(void) OnUpdateMediaMetadata(
        /* [in] */ Int32 videoWidth,
        /* [in] */ Int32 videoHeight,
        /* [in] */ Int32 duration,
        /* [in] */ Boolean canPause,
        /* [in] */ Boolean canSeekBack,
        /* [in] */ Boolean canSeekForward);


    //@CalledByNative
    virtual CARAPI_(void) OpenVideo();

    virtual CARAPI_(void) OnCompletion();

    virtual CARAPI_(Boolean) IsInPlaybackState();

    virtual CARAPI_(void) Start();

    virtual CARAPI_(void) Pause();

    // cache duration as mDuration for faster access
    virtual CARAPI_(Int32) GetDuration();

    virtual CARAPI_(Int32) GetCurrentPosition();

    virtual CARAPI_(void) SeekTo(
        /* [in] */ Int32 msec);

    /**
     * This method shall only be called by native and exitFullscreen,
     * To exit fullscreen, use exitFullscreen in Java.
     */
    //@CalledByNative
    virtual CARAPI_(void) DestroyContentVideoView(
        /* [in] */ Boolean nativeViewDestroyed);

private:

    static CARAPI_(void) OnMediaPlayerError(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 errorType);

    static CARAPI_(void) OnVideoSizeChanged(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    static CARAPI_(void) OnBufferingUpdate(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 percent);

    static CARAPI_(void) OnPlaybackComplete(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) OnUpdateMediaMetadata(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 videoWidth,
        /* [in] */ Int32 videoHeight,
        /* [in] */ Int32 duration,
        /* [in] */ Boolean canPause,
        /* [in] */ Boolean canSeekBack,
        /* [in] */ Boolean canSeekForward);

    static CARAPI_(void) OpenVideo(
        /* [in] */ IInterface* obj);

    static CARAPI_(AutoPtr<IInterface>) CreateContentVideoView(
        /* [in] */ IInterface* context,
        /* [in] */ Int64 nativeContentVideoView,
        /* [in] */ IInterface* client,
        /* [in] */ Boolean legacy);

    static CARAPI_(void) OnExitFullscreen(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) DestroyContentVideoView(
        /* [in] */ IInterface* obj,
        /* [in] */ Boolean nativeViewDestroyed);

    static CARAPI_(Int64) GetNativeViewAndroid(
        /* [in] */ IInterface* obj);

    CARAPI_(void) InitResources(
        /* [in] */ IContext* context);

    //@CalledByNative
    CARAPI_(void) OnVideoSizeChanged(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    //@CalledByNative
    CARAPI_(void) OnPlaybackComplete();

    //@CalledByNative
    static CARAPI_(AutoPtr<ContentVideoView>) CreateContentVideoView(
        /* [in] */ IContext* context,
        /* [in] */ Int64 nativeContentVideoView,
        /* [in] */ ContentVideoViewClient* client,
        /* [in] */ Boolean legacy);

    static CARAPI_(Boolean) IsActivityContext(
        /* [in] */ IContext* context);

    //@CalledByNative
    CARAPI_(void) OnExitFullscreen();

    //@CalledByNative
    CARAPI_(Int64) GetNativeViewAndroid();

    static CARAPI_(AutoPtr<ContentVideoView>) NativeGetSingletonJavaContentVideoView();

    CARAPI_(void) NativeExitFullscreen(
        /* [in] */ Int64 nativeContentVideoView,
        /* [in] */ Boolean relaseMediaPlayer);

    CARAPI_(Int32) NativeGetCurrentPosition(
        /* [in] */ Int64 nativeContentVideoView);

    CARAPI_(Int32) NativeGetDurationInMilliSeconds(
        /* [in] */ Int64 nativeContentVideoView);

    CARAPI_(void) NativeRequestMediaMetadata(
        /* [in] */ Int64 nativeContentVideoView);

    CARAPI_(Int32) NativeGetVideoWidth(
        /* [in] */ Int64 nativeContentVideoView);

    CARAPI_(Int32) NativeGetVideoHeight(
        /* [in] */ Int64 nativeContentVideoView);

    CARAPI_(Boolean) NativeIsPlaying(
        /* [in] */ Int64 nativeContentVideoView);

    CARAPI_(void) NativePause(
        /* [in] */ Int64 nativeContentVideoView);

    CARAPI_(void) NativePlay(
        /* [in] */ Int64 nativeContentVideoView);

    CARAPI_(void) NativeSeekTo(
        /* [in] */ Int64 nativeContentVideoView,
        /* [in] */ Int32 msec);

    CARAPI_(void) NativeSetSurface(
        /* [in] */ Int64 nativeContentVideoView,
        /* [in] */ ISurface* surface);

protected:
    static const Int32 MEDIA_ERROR_INVALID_CODE = 3;

private:
    static const String TAG;

    /* Do not change these values without updating their counterparts
     * in include/media/mediaplayer.h!
     */
    static const Int32 MEDIA_NOP = 0; // interface test message
    static const Int32 MEDIA_PREPARED = 1;
    static const Int32 MEDIA_PLAYBACK_COMPLETE = 2;
    static const Int32 MEDIA_BUFFERING_UPDATE = 3;
    static const Int32 MEDIA_SEEK_COMPLETE = 4;
    static const Int32 MEDIA_SET_VIDEO_SIZE = 5;
    static const Int32 MEDIA_ERROR = 100;
    static const Int32 MEDIA_INFO = 200;

    /**
     * Keep these error codes in sync with the code we defined in
     * MediaPlayerListener.java.
     */
    static const Int32 MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK = 2;

    // all possible internal states
    static const Int32 STATE_ERROR              = -1;
    static const Int32 STATE_IDLE               = 0;
    static const Int32 STATE_PLAYING            = 1;
    static const Int32 STATE_PAUSED             = 2;
    static const Int32 STATE_PLAYBACK_COMPLETED = 3;

    AutoPtr<ISurfaceHolder> mSurfaceHolder;
    Int32 mVideoWidth;
    Int32 mVideoHeight;
    Int32 mDuration;

    // Native pointer to C++ ContentVideoView object.
    Int64 mNativeContentVideoView;

    // webkit should have prepared the media
    Int32 mCurrentState;

    // Strings for displaying media player errors
    String mPlaybackErrorText;
    String mUnknownErrorText;
    String mErrorButton;
    String mErrorTitle;
    String mVideoLoadingText;

    // This view will contain the video.
    AutoPtr<VideoSurfaceView> mVideoSurfaceView;

    // Progress view when the video is loading.
    AutoPtr<IView> mProgressView;

    // The ViewAndroid is used to keep screen on during video playback.
//    AutoPtr<ViewAndroid> mViewAndroid;

    const AutoPtr<ContentVideoViewClient> mClient;

    const AutoPtr<IRunnable> mExitFullscreenRunnable;
};

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_CONTENTVIDEOVIEW_H__
