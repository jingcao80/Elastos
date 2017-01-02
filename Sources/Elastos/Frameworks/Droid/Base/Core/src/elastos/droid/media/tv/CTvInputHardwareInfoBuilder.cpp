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

#include "elastos/droid/media/tv/CTvInputHardwareInfoBuilder.h"
#include "elastos/droid/media/tv/CTvInputHardwareInfo.h"

using Elastos::Core::CInteger32;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_INTERFACE_IMPL(CTvInputHardwareInfoBuilder, Object, ITvInputHardwareInfoBuilder)

CAR_OBJECT_IMPL(CTvInputHardwareInfoBuilder)

CTvInputHardwareInfoBuilder::CTvInputHardwareInfoBuilder()
    : mAudioType(0)
{
}

CTvInputHardwareInfoBuilder::~CTvInputHardwareInfoBuilder()
{
}

ECode CTvInputHardwareInfoBuilder::constructor()
{
    return NOERROR;
}

ECode CTvInputHardwareInfoBuilder::DeviceId(
    /* [in] */ Int32 deviceId)
{
    CInteger32::New(deviceId, (IInteger32**)&mDeviceId);
    return NOERROR;
}

ECode CTvInputHardwareInfoBuilder::Type(
    /* [in] */ Int32 type)
{
    CInteger32::New(type, (IInteger32**)&mType);
    return NOERROR;
}

ECode CTvInputHardwareInfoBuilder::AudioType(
    /* [in] */ Int32 audioType)
{
    mAudioType = audioType;
    return NOERROR;
}

ECode CTvInputHardwareInfoBuilder::AudioAddress(
    /* [in] */ const String& audioAddress)
{
    mAudioAddress = audioAddress;
    return NOERROR;
}

ECode CTvInputHardwareInfoBuilder::HdmiPortId(
    /* [in] */ Int32 hdmiPortId)
{
    CInteger32::New(hdmiPortId, (IInteger32**)&mHdmiPortId);
    return NOERROR;
}

ECode CTvInputHardwareInfoBuilder::Build(
    /* [out] */ ITvInputHardwareInfo ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (mDeviceId == NULL || mType == NULL) {
        // throw new UnsupportedOperationException();
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    Int32 type;
    mType->GetValue(&type);
    if ((type == ITvInputHardwareInfo::TV_INPUT_TYPE_HDMI && mHdmiPortId == NULL) ||
            (type != ITvInputHardwareInfo::TV_INPUT_TYPE_HDMI && mHdmiPortId != NULL)) {
        // throw new UnsupportedOperationException();
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    AutoPtr<CTvInputHardwareInfo> info;
    CTvInputHardwareInfo::NewByFriend((CTvInputHardwareInfo**)&info);
    mDeviceId->GetValue(&info->mDeviceId);
    mType->GetValue(&info->mType);
    info->mAudioType = mAudioType;
    if (info->mAudioType != IAudioManager::DEVICE_NONE) {
        info->mAudioAddress = mAudioAddress;
    }
    if (mHdmiPortId != NULL) {
        mHdmiPortId->GetValue(&info->mHdmiPortId);
    }
    *result = ITvInputHardwareInfo::Probe(info);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
