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
