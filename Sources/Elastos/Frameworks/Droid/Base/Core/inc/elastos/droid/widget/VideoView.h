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

#ifndef __ELASTOS_DROID_WIDGET_VideoView_H__
#define __ELASTOS_DROID_WIDGET_VideoView_H__

#include "elastos/droid/view/SurfaceView.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Media::IMediaPlayer;
using Elastos::Droid::Media::IMediaPlayerOnPreparedListener;
using Elastos::Droid::Media::IMediaPlayerOnCompletionListener;
using Elastos::Droid::Media::IMediaPlayerOnErrorListener;
using Elastos::Droid::Media::IMediaPlayerOnInfoListener;
using Elastos::Droid::Media::IMediaPlayerOnVideoSizeChangedListener;
using Elastos::Droid::Media::IMediaPlayerOnBufferingUpdateListener;
using Elastos::Droid::Media::ISubtitleControllerAnchor;
using Elastos::Droid::Media::IMediaFormat;
using Elastos::Droid::Media::ISubtitleTrackRenderingWidgetOnChangedListener;
using Elastos::Droid::Media::ISubtitleTrackRenderingWidget;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Droid::View::ISurfaceHolderCallback;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::SurfaceView;
using Elastos::IO::IInputStream;
using Elastos::Utility::IVector;

namespace Elastos {
namespace Droid {
namespace Widget {

class VideoView
    : public SurfaceView
    , public IVideoView
    , public IMediaPlayerControl
    , public ISubtitleControllerAnchor
{
private:
    class VVOnVideoSizeChangedListener
        : public Object
        , public IMediaPlayerOnVideoSizeChangedListener
    {
    public:
        CAR_INTERFACE_DECL()

        VVOnVideoSizeChangedListener(
            /* [in] */ VideoView* host);

        CARAPI OnVideoSizeChanged(
            /* [in] */ IMediaPlayer* mp,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

    private:
        VideoView* mHost;
    };

    class VVOnPreparedListener
      : public Object
      , public IMediaPlayerOnPreparedListener
    {
    public:
        CAR_INTERFACE_DECL()

        VVOnPreparedListener(
            /* [in] */ VideoView* host);

        CARAPI OnPrepared(
            /* [in] */ IMediaPlayer* mp);

    private:
        VideoView* mHost;
    };

    class VVOnCompletionListener
        : public Object
        , public IMediaPlayerOnCompletionListener
    {
    public:
        CAR_INTERFACE_DECL()

        VVOnCompletionListener(
            /* [in] */ VideoView* host);

        CARAPI OnCompletion(
            /* [in] */ IMediaPlayer* mp);

    private:
        VideoView* mHost;
    };

    class VVOnErrorListener
        : public Object
        , public IMediaPlayerOnErrorListener
    {
    public:
        CAR_INTERFACE_DECL()

        VVOnErrorListener(
            /* [in] */ VideoView* host);

        CARAPI OnError(
            /* [in] */ IMediaPlayer* mp,
            /* [in] */ Int32 what,
            /* [in] */ Int32 extra,
            /* [out] */ Boolean* res);

    private:
        VideoView* mHost;
    };

    class VVDialogOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        VVDialogOnClickListener(
            /* [in] */ VideoView* host);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        VideoView* mHost;
    };

    class VVOnBufferingUpdateListener
        : public Object
        , public IMediaPlayerOnBufferingUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        VVOnBufferingUpdateListener(
            /* [in] */ VideoView* host);

        CARAPI OnBufferingUpdate(
            /* [in] */ IMediaPlayer* mp,
            /* [in] */ Int32 percent);

    private:
        VideoView* mHost;
    };

    class VVSurfaceHodlerCallback
        : public Object
        , public ISurfaceHolderCallback
    {
    public:
        CAR_INTERFACE_DECL()

        VVSurfaceHodlerCallback(
            /* [in] */ VideoView* host);

        CARAPI SurfaceCreated(
            /* [in] */ ISurfaceHolder* holder);

        CARAPI SurfaceChanged(
            /* [in] */ ISurfaceHolder* holder,
            /* [in] */ Int32 format,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI SurfaceDestroyed(
            /* [in] */ ISurfaceHolder* holder);

    private:
        VideoView* mHost;
    };

    class InfoListener
        : public Object
        , public IMediaPlayerOnInfoListener
    {
    public:
        CAR_INTERFACE_DECL()

        InfoListener(
            /* [in] */ VideoView* host);

        CARAPI OnInfo(
            /* [in] */ IMediaPlayer* mp,
            /* [in] */ Int32 arg1,
            /* [in] */ Int32 arg2,
            /* [out] */ Boolean* result);

    private:
        VideoView* mHost;
    };

    class ChangedListener
        : public Object
        , public ISubtitleTrackRenderingWidgetOnChangedListener
    {
    public:
        CAR_INTERFACE_DECL()

        ChangedListener(
            /* [in] */ VideoView* host);

        // @Override
        CARAPI OnChanged(
            /* [in] */ ISubtitleTrackRenderingWidget* renderingWidget);

    private:
        VideoView* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    VideoView();

    CARAPI constructor(
        /* [in] */ IContext* ctx);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    virtual ~VideoView();

    virtual CARAPI ResolveAdjustedSize(
        /* [in] */ Int32 desiredSize,
        /* [in] */ Int32 measureSpec,
        /* [out] */ Int32* size);

    virtual CARAPI SetVideoPath(
        /* [in] */ const String& path);

    virtual CARAPI SetVideoURI(
        /* [in] */ IUri* uri);

    virtual CARAPI SetVideoURI(
        /* [in] */ IUri* uri,
        /* [in] */ IMap* headers);

    CARAPI AddSubtitleSource(
        /* [in] */ IInputStream* is,
        /* [in] */ IMediaFormat* format);

    virtual CARAPI StopPlayback();

    virtual CARAPI SetMediaController(
        /* [in] */ IMediaController* controller);

    virtual CARAPI SetOnPreparedListener(
        /* [in] */ IMediaPlayerOnPreparedListener* l);

    virtual CARAPI SetOnCompletionListener(
        /* [in] */ IMediaPlayerOnCompletionListener* l);

    virtual CARAPI SetOnErrorListener(
        /* [in] */ IMediaPlayerOnErrorListener* l);

    /**
     * Register a callback to be invoked when an informational event
     * occurs during playback or setup.
     *
     * @param l The callback that will be run
     */
    virtual CARAPI SetOnInfoListener(
        /* [in] */ IMediaPlayerOnInfoListener* l);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    virtual CARAPI Start();

    // @Override
    virtual CARAPI Pause();

    virtual CARAPI Suspend();

    virtual CARAPI Resume();

    // @Override
    virtual CARAPI GetDuration(
        /* [out] */ Int32* duration);

    virtual CARAPI GetCurrentPosition(
        /* [out] */ Int32* pos);

    // @Override
    virtual CARAPI SeekTo(
        /* [in] */ Int32 msec);

    // @Override
    virtual CARAPI IsPlaying(
        /* [out] */ Boolean* playing);

    // @Override
    virtual CARAPI GetBufferPercentage(
        /* [out] */ Int32* percentage);

    // @Override
    virtual CARAPI CanPause(
        /* [out] */ Boolean* can);

    // @Override
    virtual CARAPI CanSeekBackward(
        /* [out] */ Boolean* can);

    // @Override
    virtual CARAPI CanSeekForward(
        /* [out] */ Boolean* can);

    // @Override
    CARAPI GetAudioSessionId(
        /* [out] */ Int32* id);

    // @Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    /** @hide */
    // @Override
    CARAPI SetSubtitleWidget(
        /* [in] */ ISubtitleTrackRenderingWidget* subtitleWidget);

    /** @hide */
    // @Override
    CARAPI GetSubtitleLooper(
        /* [out] */ ILooper** looper);

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

protected:
    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnDetachedFromWindow();

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

private:
    CARAPI_(void) InitVideoView();

    CARAPI_(void) OpenVideo();

    CARAPI_(void) AttachMediaController();

    CARAPI_(void) ReleaseResources(
        /* [in] */ Boolean cleartargetstate);

    CARAPI_(void) ToggleMediaControlsVisiblity();

    CARAPI_(Boolean) IsInPlaybackState();

    /**
     * Forces a measurement and layout pass for all overlaid views.
     *
     * @see #setSubtitleWidget(RenderingWidget)
     */
    CARAPI_(void) MeasureAndLayoutSubtitleWidget();

    CARAPI_(Boolean) IsHTTPStreaming(
        /* [in] */ IUri* uri);

private:
    static const String TAG;
    // settable by the client
    AutoPtr<IUri> mUri;
    AutoPtr<IMap> mHeaders;

    // all possible internal states
    static const Int32 STATE_ERROR;
    static const Int32 STATE_IDLE;
    static const Int32 STATE_PREPARING;
    static const Int32 STATE_PREPARED;
    static const Int32 STATE_PLAYING;
    static const Int32 STATE_PAUSED;
    static const Int32 STATE_PLAYBACK_COMPLETED;
    static const Int32 STATE_SUSPENDED;

    // mCurrentState is a VideoView object's current state.
    // mTargetState is the state that a method caller intends to reach.
    // For instance, regardless the VideoView object's current state,
    // calling pause() intends to bring the object to a target state
    // of STATE_PAUSED.
    Int32 mCurrentState;
    Int32 mTargetState;

    // All the stuff we need for playing and showing a video
    AutoPtr<ISurfaceHolder> mSurfaceHolder;
    AutoPtr<IMediaPlayer> mMediaPlayer;
    Int32 mAudioSession;
    Int32 mVideoWidth;
    Int32 mVideoHeight;
    Int32 mSurfaceWidth;
    Int32 mSurfaceHeight;
    AutoPtr<IMediaController> mMediaController;
    AutoPtr<IMediaPlayerOnCompletionListener> mOnCompletionListener;
    AutoPtr<IMediaPlayerOnPreparedListener> mOnPreparedListener;
    Int32 mCurrentBufferPercentage;
    AutoPtr<IMediaPlayerOnErrorListener> mOnErrorListener;
    AutoPtr<IMediaPlayerOnInfoListener>  mOnInfoListener;
    Int32 mSeekWhenPrepared; // recording the seek position while preparing
    Boolean mCanPause;
    Boolean mCanSeekBack;
    Boolean mCanSeekForward;

    /** Subtitle rendering widget overlaid on top of the video. */
    AutoPtr<ISubtitleTrackRenderingWidget> mSubtitleWidget;

    /** Listener for changes to subtitle data, used to redraw when needed. */
    AutoPtr<ISubtitleTrackRenderingWidgetOnChangedListener> mSubtitlesChangedListener;

    AutoPtr<IMediaPlayerOnVideoSizeChangedListener> mSizeChangedListener;
    AutoPtr<IMediaPlayerOnPreparedListener> mPreparedListener;
    AutoPtr<IMediaPlayerOnCompletionListener> mCompletionListener;

    AutoPtr<IMediaPlayerOnInfoListener> mInfoListener;

    AutoPtr<IMediaPlayerOnErrorListener> mErrorListener;
    AutoPtr<IMediaPlayerOnBufferingUpdateListener> mBufferingUpdateListener;
    AutoPtr<ISurfaceHolderCallback> mSHCallback;
    // private Vector<Pair<InputStream, MediaFormat>> mPendingSubtitleTracks;
    AutoPtr<IVector> mPendingSubtitleTracks;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_VideoView_H__
