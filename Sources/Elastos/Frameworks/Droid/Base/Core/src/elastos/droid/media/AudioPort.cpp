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

#include "elastos/droid/media/AudioPort.h"
#include "elastos/droid/media/CAudioPortConfig.h"
#include "elastos/droid/media/CAudioHandle.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(AudioPort, Object, IAudioPort)

AudioPort::AudioPort()
    : mRole(0)
{
}

AudioPort::~AudioPort()
{
}

ECode AudioPort::constructor(
    /* [in] */ IAudioHandle* handle,
    /* [in] */ Int32 role,
    /* [in] */ ArrayOf<Int32>* samplingRates,
    /* [in] */ ArrayOf<Int32>* channelMasks,
    /* [in] */ ArrayOf<Int32>* formats,
    /* [in] */ ArrayOf<IAudioGain*>* gains)
{
    mHandle = handle;
    mRole = role;
    mSamplingRates = samplingRates;
    mChannelMasks = channelMasks;
    mFormats = formats;
    mGains = gains;
    return NOERROR;
}

ECode AudioPort::Role(
    /* [out]*/ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRole;
    return NOERROR;
}

ECode AudioPort::SamplingRates(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSamplingRates;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode AudioPort::ChannelMasks(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mChannelMasks;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode AudioPort::Formats(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFormats;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode AudioPort::Gains(
    /* [out, callee] */ ArrayOf<IAudioGain*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mGains;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode AudioPort::BuildConfig(
    /* [in] */ Int32 samplingRate,
    /* [in] */ Int32 channelMask,
    /* [in] */ Int32 format,
    /* [in] */ IAudioGainConfig* gain,
    /* [out] */ IAudioPortConfig** result)
{
    VALIDATE_NOT_NULL(result)
    return CAudioPortConfig::New(this, samplingRate,
        channelMask, format, gain, result);
}

ECode AudioPort::ActiveConfig(
    /* [out] */ IAudioPortConfig** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mActiveConfig;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode AudioPort::SetActiveConfig(
    /* [in] */ IAudioPortConfig* config)
{
    mActiveConfig = config;
    return NOERROR;
}

ECode AudioPort::Handle(
    /* [out]*/ IAudioHandle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHandle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode AudioPort::Gain(
    /* [in] */ Int32 index,
    /* [out]*/ IAudioGain** result)
{
    VALIDATE_NOT_NULL(result)
    if (index < 0 || index >= mGains->GetLength()) {
        *result = NULL;
        return NOERROR;
    }
    *result = (*mGains)[index];
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode AudioPort::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return ((CAudioHandle*)mHandle.Get())->GetHashCode(result);
}

ECode AudioPort::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (obj == NULL || IAudioPort::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<AudioPort> ap = (AudioPort*)(IObject*)obj;
    return ((CAudioHandle*)mHandle.Get())->Equals(ap->mHandle, result);
}

ECode AudioPort::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder sb("{mHandle:");
    sb.Append(mHandle);
    sb.Append(", mRole:");
    sb.Append(mRole);
    sb.Append("}");
    return sb.ToString(result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
