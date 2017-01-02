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

#include "elastos/droid/media/tv/CTvInputHardwareInfo.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

String CTvInputHardwareInfo::TAG("TvInputHardwareInfo");

CAR_INTERFACE_IMPL_2(CTvInputHardwareInfo, Object, ITvInputHardwareInfo, IParcelable)

CAR_OBJECT_IMPL(CTvInputHardwareInfo)

CTvInputHardwareInfo::CTvInputHardwareInfo()
    : mDeviceId(0)
    , mType(0)
    , mAudioType(0)
    , mHdmiPortId(0)
{
}

CTvInputHardwareInfo::~CTvInputHardwareInfo()
{
}

ECode CTvInputHardwareInfo::constructor()
{
    return NOERROR;
}

ECode CTvInputHardwareInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mDeviceId);
    source->ReadInt32(&mType);
    source->ReadInt32(&mAudioType);
    source->ReadInt32(&mHdmiPortId);
    source->ReadString(&mAudioAddress);
    return NOERROR;
}

ECode CTvInputHardwareInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mDeviceId);
    dest->WriteInt32(mType);
    dest->WriteInt32(mAudioType);
    dest->WriteInt32(mHdmiPortId);
    dest->WriteString(mAudioAddress);
    return NOERROR;
}

ECode CTvInputHardwareInfo::GetDeviceId(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDeviceId;
    return NOERROR;
}

ECode CTvInputHardwareInfo::GetType(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mType;
    return NOERROR;
}

ECode CTvInputHardwareInfo::GetAudioType(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAudioType;
    return NOERROR;
}

ECode CTvInputHardwareInfo::GetAudioAddress(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAudioAddress;
    return NOERROR;
}

ECode CTvInputHardwareInfo::GetHdmiPortId(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mType != ITvInputHardwareInfo::TV_INPUT_TYPE_HDMI) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *result = mHdmiPortId;
    return NOERROR;
}

ECode CTvInputHardwareInfo::ToString(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder b;
    b.Append("TvInputHardwareInfo {id=");
    b.Append(mDeviceId);
    b.Append(", type=");
    b.Append(mType);
    b.Append(", audio_type=");
    b.Append(mAudioType);
    b.Append(", audio_addr=");
    b.Append(mAudioAddress);
    if (mType == ITvInputHardwareInfo::TV_INPUT_TYPE_HDMI) {
        b.Append(", hdmi_port=");
        b.Append(mHdmiPortId);
    }
    b.Append("}");
    return b.ToString(result);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
