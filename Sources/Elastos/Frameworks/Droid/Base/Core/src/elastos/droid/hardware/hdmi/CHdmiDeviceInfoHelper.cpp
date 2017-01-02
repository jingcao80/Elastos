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

#include "elastos/droid/hardware/hdmi/CHdmiDeviceInfoHelper.h"
#include "elastos/droid/hardware/hdmi/HdmiDeviceInfo.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

CAR_INTERFACE_IMPL(CHdmiDeviceInfoHelper, Singleton, IHdmiDeviceInfoHelper)

CAR_SINGLETON_IMPL(CHdmiDeviceInfoHelper)

ECode CHdmiDeviceInfoHelper::IdForCecDevice(
    /* [in] */ Int32 address,
    /* [out] */ Int32* device)
{
    VALIDATE_NOT_NULL(device);

    return HdmiDeviceInfo::IdForCecDevice(address, device);
}

ECode CHdmiDeviceInfoHelper::IdForMhlDevice(
    /* [in] */ Int32 portId,
    /* [out] */ Int32* device)
{
    VALIDATE_NOT_NULL(device);

    return HdmiDeviceInfo::IdForMhlDevice(portId, device);
}

ECode CHdmiDeviceInfoHelper::IdForHardware(
    /* [in] */ Int32 portId,
    /* [out] */ Int32* hardware)
{
    VALIDATE_NOT_NULL(hardware);

    return HdmiDeviceInfo::IdForHardware(portId, hardware);
}

} // namespace Hdmi
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
