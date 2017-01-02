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

#include "elastos/droid/media/CAudioDevicePort.h"
#include "elastos/droid/media/CAudioManager.h"
#include "elastos/droid/media/CAudioDevicePortConfig.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioDevicePort, AudioPort, IAudioDevicePort)

CAR_OBJECT_IMPL(CAudioDevicePort)

CAudioDevicePort::CAudioDevicePort()
    : mType(0)
{
}

CAudioDevicePort::~CAudioDevicePort()
{
}

ECode CAudioDevicePort::constructor(
    /* [in] */ IAudioHandle* handle,
    /* [in] */ ArrayOf<Int32>* samplingRates,
    /* [in] */ ArrayOf<Int32>* channelMasks,
    /* [in] */ ArrayOf<Int32>* formats,
    /* [in] */ ArrayOf<IAudioGain*>* gains,
    /* [in] */ Int32 type,
    /* [in] */ const String& address)
{
    Boolean b;
    CAudioManager::IsInputDevice(type, &b);
    Int32 val;
    if (b) {
        val = IAudioPort::ROLE_SOURCE;
    }
    else {
        val = IAudioPort::ROLE_SINK;
    }
    AudioPort::constructor(handle, val, samplingRates, channelMasks, formats, gains);
    mType = type;
    mAddress = address;
    return NOERROR;
}

ECode CAudioDevicePort::Type(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mType;
    return NOERROR;
}

ECode CAudioDevicePort::Address(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAddress;
    return NOERROR;
}

ECode CAudioDevicePort::BuildConfig(
    /* [in] */ Int32 samplingRate,
    /* [in] */ Int32 channelMask,
    /* [in] */ Int32 format,
    /* [in] */ IAudioGainConfig* gain,
    /* [out] */ IAudioDevicePortConfig** result)
{
    VALIDATE_NOT_NULL(result)
    return CAudioDevicePortConfig::New(this, samplingRate, channelMask, format, gain, result);
}

ECode CAudioDevicePort::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (obj == NULL || IAudioDevicePort::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    return AudioPort::Equals(obj, result);
}

ECode CAudioDevicePort::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String str;
    AudioPort::ToString(&str);
    StringBuilder sb("{");
    sb.Append(str);
    sb.Append(", mType:");
    sb.Append(mType);
    sb.Append(", mAddress:");
    sb.Append(mAddress);
    sb.Append("}");
    return sb.ToString(result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
