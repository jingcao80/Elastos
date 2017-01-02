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

#ifndef ___AUDIOFORMAT_H__
#define ___AUDIOFORMAT_H__

#include "_Elastos.Droid.Media.h"
#include <system/audio.h>

#define ENCODING_AMR_NB     100
#define ENCODING_AMR_WB     101
#define ENCODING_EVRC       102
#define ENCODING_EVRC_B     103
#define ENCODING_EVRC_WB    104
#define ENCODING_EVRC_NW    105

namespace Elastos {
namespace Droid {
namespace Media {

static inline audio_format_t AudioFormatToNative(int audioFormat)
{
    switch (audioFormat) {
    case IAudioFormat::ENCODING_PCM_16BIT:
        return AUDIO_FORMAT_PCM_16_BIT;
    case IAudioFormat::ENCODING_PCM_8BIT:
        return AUDIO_FORMAT_PCM_8_BIT;
    case IAudioFormat::ENCODING_PCM_FLOAT:
        return AUDIO_FORMAT_PCM_FLOAT;
    case IAudioFormat::ENCODING_AC3:
        return AUDIO_FORMAT_AC3;
    case IAudioFormat::ENCODING_E_AC3:
        return AUDIO_FORMAT_E_AC3;
#ifdef QCOM_HARDWARE
    case ENCODING_AMR_NB:
        return AUDIO_FORMAT_AMR_NB;
    case ENCODING_AMR_WB:
        return AUDIO_FORMAT_AMR_WB;
    case ENCODING_EVRC:
        return AUDIO_FORMAT_EVRC;
    case ENCODING_EVRC_B:
        return AUDIO_FORMAT_EVRCB;
    case ENCODING_EVRC_WB:
        return AUDIO_FORMAT_EVRCWB;
    case ENCODING_EVRC_NW:
        return AUDIO_FORMAT_EVRCNW;
#endif
    case IAudioFormat::ENCODING_DEFAULT:
        return AUDIO_FORMAT_DEFAULT;
    default:
        return AUDIO_FORMAT_INVALID;
    }
}

static inline Int32 AudioFormatFromNative(audio_format_t nativeFormat)
{
    switch (nativeFormat) {
    case AUDIO_FORMAT_PCM_16_BIT:
        return IAudioFormat::ENCODING_PCM_16BIT;
    case AUDIO_FORMAT_PCM_8_BIT:
        return IAudioFormat::ENCODING_PCM_8BIT;
    case AUDIO_FORMAT_PCM_FLOAT:
        return IAudioFormat::ENCODING_PCM_FLOAT;
    case AUDIO_FORMAT_AC3:
        return IAudioFormat::ENCODING_AC3;
    case AUDIO_FORMAT_E_AC3:
        return IAudioFormat::ENCODING_E_AC3;
#ifdef QCOM_HARDWARE
    case AUDIO_FORMAT_AMR_NB:
        return ENCODING_AMR_NB;
    case AUDIO_FORMAT_AMR_WB:
        return ENCODING_AMR_WB;
    case AUDIO_FORMAT_EVRC:
        return ENCODING_EVRC;
    case AUDIO_FORMAT_EVRCB:
        return ENCODING_EVRC_B;
    case AUDIO_FORMAT_EVRCWB:
        return ENCODING_EVRC_WB;
    case AUDIO_FORMAT_EVRCNW:
        return ENCODING_EVRC_NW;
#endif
    case AUDIO_FORMAT_DEFAULT:
        return IAudioFormat::ENCODING_DEFAULT;
    default:
        return IAudioFormat::ENCODING_INVALID;
    }
}

static inline audio_channel_mask_t OutChannelMaskToNative(Int32 channelMask)
{
    switch (channelMask) {
    case IAudioFormat::CHANNEL_OUT_DEFAULT:
    case IAudioFormat::CHANNEL_INVALID:
        return AUDIO_CHANNEL_NONE;
    default:
        return (audio_channel_mask_t)(channelMask>>2);
    }
}

static inline Int32 OutChannelMaskFromNative(audio_channel_mask_t nativeMask)
{
    switch (nativeMask) {
    case AUDIO_CHANNEL_NONE:
        return IAudioFormat::CHANNEL_OUT_DEFAULT;
    default:
        return (Int32)nativeMask << 2;
    }
}

static inline audio_channel_mask_t InChannelMaskToNative(Int32 channelMask)
{
    return (audio_channel_mask_t)channelMask;
}

static inline Int32 InChannelMaskFromNative(audio_channel_mask_t nativeMask)
{
    return (Int32)nativeMask;
}

} // namespace Media
} // namespace Droid
} // namespace Elastos

#endif // ___AUDIOFORMAT_H__
