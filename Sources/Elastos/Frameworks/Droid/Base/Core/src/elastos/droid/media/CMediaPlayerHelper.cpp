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
    /* [in] */ IUri* uri,
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ IAudioAttributes* audioAttributes,
    /* [in] */ Int32 audioSessionId,
    /* [out] */ IMediaPlayer** result)
{
    return CMediaPlayer::Create(ctx, uri, holder, audioAttributes, audioSessionId, result);
}

ECode CMediaPlayerHelper::Create(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 resid,
    /* [out] */ IMediaPlayer** player)
{
    return CMediaPlayer::Create(ctx, resid, player);
}

ECode CMediaPlayerHelper::Create(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 resid,
    /* [in] */ IAudioAttributes* audioAttributes,
    /* [in] */ Int32 audioSessionId,
    /* [out] */ IMediaPlayer** result)
{
    return CMediaPlayer::Create(ctx, resid, audioAttributes, audioSessionId, result);
}

ECode CMediaPlayerHelper::NativePullBatteryData(
    /* [in] */ IParcel* reply,
    /* [out] */ Int32* result)
{
    return CMediaPlayer::NativePullBatteryData(reply, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
