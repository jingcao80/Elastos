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

#include "elastos/droid/media/CAudioFormat.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioFormat, Object, IAudioFormat)

CAR_OBJECT_IMPL(CAudioFormat)

CAudioFormat::CAudioFormat()
    : mEncoding(0)
    , mSampleRate(0)
    , mChannelMask(0)
    , mPropertySetMask(0)
{
}

CAudioFormat::~CAudioFormat()
{
}

ECode CAudioFormat::constructor()
{
    // throw new UnsupportedOperationException("There is no valid usage of this constructor");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CAudioFormat::constructor(
    /* [in] */ Int32 ignoredArgument)
{
    return NOERROR;
}

ECode CAudioFormat::constructor(
    /* [in] */ Int32 encoding,
    /* [in] */ Int32 sampleRate,
    /* [in] */ Int32 channelMask)
{
    mEncoding = encoding;
    mSampleRate = sampleRate;
    mChannelMask = channelMask;
    mPropertySetMask = IAudioFormat::AUDIO_FORMAT_HAS_PROPERTY_ENCODING |
            IAudioFormat::AUDIO_FORMAT_HAS_PROPERTY_SAMPLE_RATE |
            IAudioFormat::AUDIO_FORMAT_HAS_PROPERTY_CHANNEL_MASK;
    return NOERROR;
}

ECode CAudioFormat::GetEncoding(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if ((mPropertySetMask & IAudioFormat::AUDIO_FORMAT_HAS_PROPERTY_ENCODING) == 0) {
        *result = IAudioFormat::ENCODING_INVALID;
        return NOERROR;
    }
    *result = mEncoding;
    return NOERROR;
}

ECode CAudioFormat::GetSampleRate(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if ((mPropertySetMask & IAudioFormat::AUDIO_FORMAT_HAS_PROPERTY_SAMPLE_RATE) == 0) {
        *result = 0;
        return NOERROR;
    }
    *result = mSampleRate;
    return NOERROR;
}

ECode CAudioFormat::GetChannelMask(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if ((mPropertySetMask & IAudioFormat::AUDIO_FORMAT_HAS_PROPERTY_CHANNEL_MASK) == 0) {
        *result = CHANNEL_INVALID;
        return NOERROR;
    }
    *result = mChannelMask;
    return NOERROR;
}

ECode CAudioFormat::GetPropertySetMask(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPropertySetMask;
    return NOERROR;
}

ECode CAudioFormat::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder sb("AudioFormat:");
    sb.Append(" props=");
    sb.Append(mPropertySetMask);
    sb.Append(" enc=");
    sb.Append(mEncoding);
    sb.Append(" chan=0x=");
    sb.Append(StringUtils::ToHexString(mChannelMask));
    sb.Append(" rate=");
    sb.Append(mSampleRate);
    return sb.ToString(result);
}

ECode CAudioFormat::ChannelCountFromInChannelMask(
    /* [in] */ Int32 mask,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = Elastos::Core::Math::BitCount(mask);
    return NOERROR;
}

ECode CAudioFormat::ChannelCountFromOutChannelMask(
    /* [in] */ Int32 mask,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = Elastos::Core::Math::BitCount(mask);
    return NOERROR;
}

ECode CAudioFormat::ConvertChannelOutMaskToNativeMask(
    /* [in] */ Int32 javaMask,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (javaMask >> 2);
    return NOERROR;
}

ECode CAudioFormat::ConvertNativeChannelMaskToOutMask(
    /* [in] */ Int32 nativeMask,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (nativeMask << 2);
    return NOERROR;
}

ECode CAudioFormat::GetBytesPerSample(
    /* [in] */ Int32 audioFormat,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    switch (audioFormat) {
    case ENCODING_PCM_8BIT:
        *result = 1;
        return NOERROR;
    case ENCODING_PCM_16BIT:
    case ENCODING_DEFAULT:
        *result = 2;
        return NOERROR;
    case ENCODING_PCM_FLOAT:
        *result = 4;
        return NOERROR;
    case ENCODING_AMRNB:
        *result = 32;
        return NOERROR;
    case ENCODING_AMRWB:
        *result = 61;
        return NOERROR;
    case ENCODING_EVRC:
    case ENCODING_EVRCB:
    case ENCODING_EVRCWB:
    case ENCODING_EVRCNW:
        *result = 23;
        return NOERROR;
    case ENCODING_INVALID:
    default:
        // throw new IllegalArgumentException("Bad audio format " + audioFormat);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

ECode CAudioFormat::IsValidEncoding(
    /* [in] */ Int32 audioFormat,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    switch (audioFormat) {
    case ENCODING_PCM_8BIT:
    case ENCODING_PCM_16BIT:
    case ENCODING_PCM_FLOAT:
    case ENCODING_AC3:
    case ENCODING_E_AC3:
    case ENCODING_AMRNB:
    case ENCODING_AMRWB:
    case ENCODING_EVRC:
    case ENCODING_EVRCB:
    case ENCODING_EVRCWB:
    case ENCODING_EVRCNW:
        *result = TRUE;
    default:
        *result = FALSE;
    }
    return NOERROR;
}

ECode CAudioFormat::IsEncodingLinearPcm(
    /* [in] */ Int32 audioFormat,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    switch (audioFormat) {
    case ENCODING_PCM_8BIT:
    case ENCODING_PCM_16BIT:
    case ENCODING_PCM_FLOAT:
    case ENCODING_DEFAULT:
        *result = TRUE;
        return NOERROR;
    case ENCODING_AC3:
    case ENCODING_E_AC3:
    case ENCODING_AMRNB:
    case ENCODING_AMRWB:
    case ENCODING_EVRC:
    case ENCODING_EVRCB:
    case ENCODING_EVRCWB:
    case ENCODING_EVRCNW:
        *result = FALSE;
        return NOERROR;
    case ENCODING_INVALID:
    default:
        // throw new IllegalArgumentException("Bad audio format " + audioFormat);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
