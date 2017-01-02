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

#include "elastos/droid/media/CAudioDevice.h"
#include "elastos/droid/media/CAudioDevicePortConfig.h"
#include "elastos/droid/utility/CSparseInt32Array.h"

using Elastos::Droid::Utility::CSparseInt32Array;

namespace Elastos {
namespace Droid {
namespace Media {

static AutoPtr<ISparseInt32Array> Init_INT()
{
    AutoPtr<ISparseInt32Array> INT_TO_EXT_DEVICE_MAPPING;
    CSparseInt32Array::New((ISparseInt32Array**)&INT_TO_EXT_DEVICE_MAPPING);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_EARPIECE, IAudioDevice::TYPE_BUILTIN_EARPIECE);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_SPEAKER, IAudioDevice::TYPE_BUILTIN_SPEAKER);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_WIRED_HEADSET, IAudioDevice::TYPE_WIRED_HEADSET);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_WIRED_HEADPHONE, IAudioDevice::TYPE_WIRED_HEADPHONES);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_BLUETOOTH_SCO, IAudioDevice::TYPE_BLUETOOTH_SCO);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET, IAudioDevice::TYPE_BLUETOOTH_SCO);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT, IAudioDevice::TYPE_BLUETOOTH_SCO);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP, IAudioDevice::TYPE_BLUETOOTH_A2DP);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES, IAudioDevice::TYPE_BLUETOOTH_A2DP);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER, IAudioDevice::TYPE_BLUETOOTH_A2DP);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_HDMI, IAudioDevice::TYPE_HDMI);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET, IAudioDevice::TYPE_DOCK);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_DGTL_DOCK_HEADSET, IAudioDevice::TYPE_DOCK);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_USB_ACCESSORY, IAudioDevice::TYPE_USB_ACCESSORY);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_USB_DEVICE, IAudioDevice::TYPE_USB_DEVICE);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_TELEPHONY_TX, IAudioDevice::TYPE_TELEPHONY);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_LINE, IAudioDevice::TYPE_LINE_ANALOG);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_HDMI_ARC, IAudioDevice::TYPE_HDMI_ARC);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_SPDIF, IAudioDevice::TYPE_LINE_DIGITAL);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_FM, IAudioDevice::TYPE_FM);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_OUT_AUX_LINE, IAudioDevice::TYPE_AUX_LINE);

    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_IN_BUILTIN_MIC, IAudioDevice::TYPE_BUILTIN_MIC);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_IN_BLUETOOTH_SCO_HEADSET, IAudioDevice::TYPE_BLUETOOTH_SCO);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_IN_WIRED_HEADSET, IAudioDevice::TYPE_WIRED_HEADSET);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_IN_HDMI, IAudioDevice::TYPE_HDMI);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_IN_TELEPHONY_RX, IAudioDevice::TYPE_TELEPHONY);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_IN_BACK_MIC, IAudioDevice::TYPE_BUILTIN_MIC);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_IN_ANLG_DOCK_HEADSET, IAudioDevice::TYPE_DOCK);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_IN_DGTL_DOCK_HEADSET, IAudioDevice::TYPE_DOCK);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_IN_USB_ACCESSORY, IAudioDevice::TYPE_USB_ACCESSORY);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_IN_USB_DEVICE, IAudioDevice::TYPE_USB_DEVICE);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_IN_FM_TUNER, IAudioDevice::TYPE_FM_TUNER);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_IN_TV_TUNER, IAudioDevice::TYPE_TV_TUNER);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_IN_LINE, IAudioDevice::TYPE_LINE_ANALOG);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_IN_SPDIF, IAudioDevice::TYPE_LINE_DIGITAL);
    INT_TO_EXT_DEVICE_MAPPING->Put(IAudioSystem::DEVICE_IN_BLUETOOTH_A2DP, IAudioDevice::TYPE_BLUETOOTH_A2DP);

    return INT_TO_EXT_DEVICE_MAPPING;
}

static AutoPtr<ISparseInt32Array> Init_EXT()
{
    // not covered here, legacy
    //AudioSystem.DEVICE_OUT_REMOTE_SUBMIX
    //AudioSystem.DEVICE_IN_REMOTE_SUBMIX

    // privileges mapping to output device
    AutoPtr<ISparseInt32Array> EXT_TO_INT_DEVICE_MAPPING;
    CSparseInt32Array::New((ISparseInt32Array**)&EXT_TO_INT_DEVICE_MAPPING);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_BUILTIN_EARPIECE, IAudioSystem::DEVICE_OUT_EARPIECE);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_BUILTIN_SPEAKER, IAudioSystem::DEVICE_OUT_SPEAKER);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_WIRED_HEADSET, IAudioSystem::DEVICE_OUT_WIRED_HEADSET);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_WIRED_HEADPHONES, IAudioSystem::DEVICE_OUT_WIRED_HEADPHONE);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_LINE_ANALOG, IAudioSystem::DEVICE_OUT_LINE);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_LINE_DIGITAL, IAudioSystem::DEVICE_OUT_SPDIF);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_BLUETOOTH_SCO, IAudioSystem::DEVICE_OUT_BLUETOOTH_SCO);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_BLUETOOTH_A2DP, IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_HDMI, IAudioSystem::DEVICE_OUT_HDMI);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_HDMI_ARC, IAudioSystem::DEVICE_OUT_HDMI_ARC);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_USB_DEVICE, IAudioSystem::DEVICE_OUT_USB_DEVICE);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_USB_ACCESSORY, IAudioSystem::DEVICE_OUT_USB_ACCESSORY);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_DOCK, IAudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_FM, IAudioSystem::DEVICE_OUT_FM);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_BUILTIN_MIC, IAudioSystem::DEVICE_IN_BUILTIN_MIC);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_FM_TUNER, IAudioSystem::DEVICE_IN_FM_TUNER);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_TV_TUNER, IAudioSystem::DEVICE_IN_TV_TUNER);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_TELEPHONY, IAudioSystem::DEVICE_OUT_TELEPHONY_TX);
    EXT_TO_INT_DEVICE_MAPPING->Put(IAudioDevice::TYPE_AUX_LINE, IAudioSystem::DEVICE_OUT_AUX_LINE);

    return EXT_TO_INT_DEVICE_MAPPING;
}

AutoPtr<ISparseInt32Array> CAudioDevice::INT_TO_EXT_DEVICE_MAPPING = Init_INT();
AutoPtr<ISparseInt32Array> CAudioDevice::EXT_TO_INT_DEVICE_MAPPING = Init_EXT();

CAR_INTERFACE_IMPL(CAudioDevice, Object, IAudioDevice)

CAR_OBJECT_IMPL(CAudioDevice)

CAudioDevice::CAudioDevice()
{
}

CAudioDevice::~CAudioDevice()
{
}

ECode CAudioDevice::constructor(
    /* [in] */ IAudioDevicePortConfig* config)
{
    return CAudioDevicePortConfig::New(config, (IAudioDevicePortConfig**)&mConfig);
}

ECode CAudioDevice::IsInputDevice(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IAudioDevicePort> port;
    mConfig->Port((IAudioDevicePort**)&port);
    Int32 role;
    IAudioPort::Probe(port)->Role(&role);
    *result = (role == IAudioPort::ROLE_SOURCE);
    return NOERROR;
}

ECode CAudioDevice::IsOutputDevice(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IAudioDevicePort> port;
    mConfig->Port((IAudioDevicePort**)&port);
    Int32 role;
    IAudioPort::Probe(port)->Role(&role);
    *result = (role == IAudioPort::ROLE_SINK);
    return NOERROR;
}

ECode CAudioDevice::GetDeviceType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IAudioDevicePort> port;
    mConfig->Port((IAudioDevicePort**)&port);
    Int32 role;
    IAudioPort::Probe(port)->Role(&role);
    return INT_TO_EXT_DEVICE_MAPPING->Get(role, IAudioDevice::TYPE_UNKNOWN, result);
}

ECode CAudioDevice::GetAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IAudioDevicePort> port;
    mConfig->Port((IAudioDevicePort**)&port);

    return port->Address(result);
}

ECode CAudioDevice::ConvertDeviceTypeToInternalDevice(
    /* [in] */ Int32 deviceType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return EXT_TO_INT_DEVICE_MAPPING->Get(deviceType, IAudioSystem::DEVICE_NONE, result);
}

ECode CAudioDevice::ConvertInternalDeviceToDeviceType(
    /* [in] */ Int32 intDevice,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return INT_TO_EXT_DEVICE_MAPPING->Get(intDevice, IAudioDevice::TYPE_UNKNOWN, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
