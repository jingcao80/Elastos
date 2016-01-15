#include "elastos/droid/media/CMediaPlayerHelper.h"
#include "elastos/droid/media/CMediaPlayer.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaPlayerHelper, Singleton, IMediaPlayerHelper)

CAR_SINGLETON_IMPL(CMediaPlayerHelper)

ECode CMediaPlayerHelper::Create(
    /* [in] */ IContext* ctx,
    /* [in] */ IUri* uri,
    /* [out] */ IMediaPlayer** player)
{
    return CMediaPlayer::Create(ctx, uri, player);
}

ECode CMediaPlayerHelper::Create(
    /* [in] */ IContext* ctx,
    /* [in] */ IUri* uri,
    /* [in] */ ISurfaceHolder* holder,
    /* [out] */ IMediaPlayer** player)
{
    return CMediaPlayer::Create(ctx, uri, holder, player);
}

ECode CMediaPlayerHelper::Create(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 resid,
    /* [out] */ IMediaPlayer** player)
{
    return CMediaPlayer::Create(ctx, resid, player);
}

ECode CMediaPlayerHelper::SetRawDataMode(
    /* [in] */ Int32 rawDataMode)
{
    return CMediaPlayer::SetRawDataMode(rawDataMode);
}

ECode CMediaPlayerHelper::GetRawDataMode(
    /* [out] */ Int32* rawDataMode)
{
    VALIDATE_NOT_NULL(rawDataMode);
    *rawDataMode = CMediaPlayer::GetRawDataMode();
    return NOERROR;
}

/* Rotate the video. */
ECode CMediaPlayerHelper::IsRotatable(
    /* [out] */ Boolean* rotatable)
{
    return CMediaPlayer::IsRotatable(rotatable);
}

ECode CMediaPlayerHelper::SetRotation(
    /* [in] */ Int32 value)
{
    return CMediaPlayer::SetRotation(value);
}

/*  Notify hdmi status. */
ECode CMediaPlayerHelper::SetHdmiState(
    /* [in] */ Boolean bHdmiPlugged)
{
    return CMediaPlayer::SetHdmiState(bHdmiPlugged);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
