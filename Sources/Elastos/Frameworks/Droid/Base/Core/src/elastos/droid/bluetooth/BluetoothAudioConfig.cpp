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

#include "elastos/droid/bluetooth/BluetoothAudioConfig.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//                         BluetoothAudioConfig
//=====================================================================
CAR_INTERFACE_IMPL_2(BluetoothAudioConfig, Object, IBluetoothAudioConfig, IParcelable);

BluetoothAudioConfig::BluetoothAudioConfig()
{
}

ECode BluetoothAudioConfig::constructor()
{
    return NOERROR;
}

ECode BluetoothAudioConfig::constructor(
    /* [in] */ Int32 sampleRate,
    /* [in] */ Int32 channelConfig,
    /* [in] */ Int32 audioFormat)
{
    mSampleRate = sampleRate;
    mChannelConfig = channelConfig;
    mAudioFormat = audioFormat;
    return NOERROR;
}

ECode BluetoothAudioConfig::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    IBluetoothAudioConfig* ibac = IBluetoothAudioConfig::Probe(other);
    if (ibac != NULL) {
        BluetoothAudioConfig* bac = (BluetoothAudioConfig*)ibac;
        *result = (bac->mSampleRate == mSampleRate &&
                bac->mChannelConfig == mChannelConfig &&
                bac->mAudioFormat == mAudioFormat);
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothAudioConfig::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = mSampleRate | (mChannelConfig << 24) | (mAudioFormat << 28);
    return NOERROR;
}

ECode BluetoothAudioConfig::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("{mSampleRate:") + StringUtils::ToString(mSampleRate)
            + String(",mChannelConfig:") + StringUtils::ToString(mChannelConfig)
            + String(",mAudioFormat:") + StringUtils::ToString(mAudioFormat)
            + String("}");
    return NOERROR;
}

ECode BluetoothAudioConfig::WriteToParcel(
    /* [in] */ IParcel* out)
{
    out->WriteInt32(mSampleRate);
    out->WriteInt32(mChannelConfig);
    out->WriteInt32(mAudioFormat);
    return NOERROR;
}

CARAPI BluetoothAudioConfig::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mSampleRate);
    source->ReadInt32(&mChannelConfig);
    source->ReadInt32(&mAudioFormat);
    return NOERROR;
}

ECode BluetoothAudioConfig::GetSampleRate(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSampleRate;
    return NOERROR;
}

ECode BluetoothAudioConfig::GetChannelConfig(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mChannelConfig;
    return NOERROR;
}

ECode BluetoothAudioConfig::GetAudioFormat(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAudioFormat;
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
