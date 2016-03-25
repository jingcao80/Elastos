
#include "elastos/droid/widget/CVideoView.h"

using Elastos::Droid::View::EIID_View;

namespace Elastos {
namespace Droid {
namespace Widget {

IVIEW_METHODS_IMPL(CVideoView, VideoView)
IDRAWABLECALLBACK_METHODS_IMPL(CVideoView, VideoView)
IKEYEVENTCALLBACK_METHODS_IMPL(CVideoView, VideoView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CVideoView, VideoView)
ISURFACEVIEW_METHODS_IMPL(CVideoView, VideoView)

ECode CVideoView::constructor(
    /* [in] */ IContext* context)
{
    return VideoView::Init(context);
}

ECode CVideoView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return VideoView::Init(context, attrs);
}

ECode CVideoView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return VideoView::Init(context, attrs, defStyle);
}

PInterface CVideoView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CVideoView::Probe(riid);
}

ECode CVideoView::ResolveAdjustedSize(
    /* [in] */ Int32 desiredSize,
    /* [in] */ Int32 measureSpec,
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    *size = VideoView::ResolveAdjustedSize(desiredSize, measureSpec);
    return NOERROR;
}

ECode CVideoView::SetVideoPath(
    /* [in] */ const String& path)
{
    return VideoView::SetVideoPath(path);
}

ECode CVideoView::SetVideoURI(
    /* [in] */ IUri* uri)
{
    return VideoView::SetVideoURI(uri);
}

ECode CVideoView::StopPlayback()
{
    return VideoView::StopPlayback();
}

ECode CVideoView::SetMediaController(
    /* [in] */ IMediaController* controller)
{
    return VideoView::SetMediaController(controller);
}

ECode CVideoView::SetOnPreparedListener(
    /* [in] */ IMediaPlayerOnPreparedListener* l)
{
    return VideoView::SetOnPreparedListener(l);
}

ECode CVideoView::SetOnCompletionListener(
    /* [in] */ IMediaPlayerOnCompletionListener* l)
{
    return VideoView::SetOnCompletionListener(l);
}

ECode CVideoView::SetOnErrorListener(
    /* [in] */ IMediaPlayerOnErrorListener* l)
{
    return VideoView::SetOnErrorListener(l);
}

ECode CVideoView::SetOnInfoListener(
    /* [in] */ IMediaPlayerOnInfoListener* l)
{
    return VideoView::SetOnInfoListener(l);
}

ECode CVideoView::Suspend()
{
    return VideoView::Suspend();
}

ECode CVideoView::Resume()
{
    return VideoView::Resume();
}

// interface IMediaPlayerControl

ECode CVideoView::Start()
{
    return VideoView::Start();
}

ECode CVideoView::Pause()
{
    return VideoView::Pause();
}

ECode CVideoView::GetDuration(
    /* [out] */ Int32* duration)
{
    VALIDATE_NOT_NULL(duration);
    *duration = VideoView::GetDuration();
    return NOERROR;
}

ECode CVideoView::GetCurrentPosition(
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position);
    *position = VideoView::GetCurrentPosition();
    return NOERROR;
}

ECode CVideoView::SeekTo(
    /* [in] */ Int32 position)
{
    return VideoView::SeekTo(position);
}

ECode CVideoView::IsPlaying(
    /* [out] */ Boolean* isPlaying)
{
    VALIDATE_NOT_NULL(isPlaying);
    *isPlaying = VideoView::IsPlaying();
    return NOERROR;
}

ECode CVideoView::GetBufferPercentage(
    /* [out] */ Int32* percentage)
{
    VALIDATE_NOT_NULL(percentage);
    *percentage = VideoView::GetBufferPercentage();
    return NOERROR;
}

ECode CVideoView::CanPause(
    /* [out] */ Boolean* canPause)
{
    VALIDATE_NOT_NULL(canPause);
    *canPause = VideoView::CanPause();
    return NOERROR;
}

ECode CVideoView::CanSeekBackward(
    /* [out] */ Boolean* canSeekBackward)
{
    VALIDATE_NOT_NULL(canSeekBackward);
    *canSeekBackward = VideoView::CanSeekBackward();
    return NOERROR;
}

ECode CVideoView::CanSeekForward(
    /* [out] */ Boolean* canSeekForward)
{
    VALIDATE_NOT_NULL(canSeekForward);
    *canSeekForward = VideoView::CanSeekForward();
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
