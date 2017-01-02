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

#include "elastos/droid/media/AudioPortConfig.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Media {

Int32 AudioPortConfig::SAMPLE_RATE  = 0x1;
Int32 AudioPortConfig::CHANNEL_MASK = 0x2;
Int32 AudioPortConfig::FORMAT       = 0x4;
Int32 AudioPortConfig::GAIN         = 0x8;

CAR_INTERFACE_IMPL(AudioPortConfig, Object, IAudioPortConfig)

AudioPortConfig::AudioPortConfig()
    : mConfigMask(0)
    , mSamplingRate(0)
    , mChannelMask(0)
    , mFormat(0)
{
}

AudioPortConfig::~AudioPortConfig()
{
}

ECode AudioPortConfig::constructor(
    /* [in] */ IAudioPort* port,
    /* [in] */ Int32 samplingRate,
    /* [in] */ Int32 channelMask,
    /* [in] */ Int32 format,
    /* [in] */ IAudioGainConfig* gain)
{
    mPort = port;
    mSamplingRate = samplingRate;
    mChannelMask = channelMask;
    mFormat = format;
    mGain = gain;
    mConfigMask = 0;
    return NOERROR;
}

ECode AudioPortConfig::Port(
    /* [out] */ IAudioPort** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPort;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode AudioPortConfig::SamplingRate(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSamplingRate;
    return NOERROR;
}

ECode AudioPortConfig::ChannelMask(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mChannelMask;
    return NOERROR;
}

ECode AudioPortConfig::Format(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFormat;
    return NOERROR;
}

ECode AudioPortConfig::Gain(
    /* [out] */ IAudioGainConfig** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mGain;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode AudioPortConfig::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder sb("{mPort:");
    sb.Append(mPort);
    sb.Append(", mSamplingRate:");
    sb.Append(mSamplingRate);
    sb.Append(", mChannelMask: ");
    sb.Append(mChannelMask);
    sb.Append(", mFormat:");
    sb.Append(mFormat);
    sb.Append(", mGain:");
    sb.Append(mGain);
    sb.Append("}");
    return sb.ToString(result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
