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

#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/HdmiUtils.h"
#include "elastos/droid/server/hdmi/SystemAudioActionFromTv.h"

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CAR_INTERFACE_IMPL(SystemAudioActionFromTv, SystemAudioAction, ISystemAudioActionFromTv)

ECode SystemAudioActionFromTv::constructor(
    /* [in] */ IHdmiCecLocalDevice* sourceAddress,
    /* [in] */ Int32 avrAddress,
    /* [in] */ Boolean targetStatus,
    /* [in] */ IIHdmiControlCallback* callback)
{
    SystemAudioAction::constructor(sourceAddress, avrAddress, targetStatus, callback);
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    HdmiUtils::VerifyAddressType(srcAddr, IHdmiDeviceInfo::DEVICE_TV);
    return NOERROR;
}

ECode SystemAudioActionFromTv::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    RemoveSystemAudioActionInProgress();
    SendSystemAudioModeRequest();
    *result = TRUE;
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
