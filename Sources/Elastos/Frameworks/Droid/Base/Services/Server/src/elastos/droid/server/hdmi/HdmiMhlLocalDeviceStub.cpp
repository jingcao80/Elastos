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

#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/HdmiMhlLocalDeviceStub.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const AutoPtr<IHdmiDeviceInfo> HdmiMhlLocalDeviceStub::INFO = InitINFO();

HdmiMhlLocalDeviceStub::HdmiMhlLocalDeviceStub()
    : mPortId(0)
{}

ECode HdmiMhlLocalDeviceStub::constructor(
    /* [in] */ IHdmiControlService* service,
    /* [in] */ Int32 portId)
{
    mService = service;
    mPortId = portId;
    return NOERROR;
}

ECode HdmiMhlLocalDeviceStub::OnDeviceRemoved()
{
    return NOERROR;
}

ECode HdmiMhlLocalDeviceStub::GetInfo(
    /* [out] */ IHdmiDeviceInfo** result)
{
    VALIDATE_NOT_NULL(result)

    *result = INFO;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiMhlLocalDeviceStub::SetBusMode(
    /* [in] */ Int32 cbusmode)
{
    return NOERROR;
}

ECode HdmiMhlLocalDeviceStub::OnBusOvercurrentDetected(
    /* [in] */ Boolean on)
{
    return NOERROR;
}

ECode HdmiMhlLocalDeviceStub::SetDeviceStatusChange(
    /* [in] */ Int32 adopterId,
    /* [in] */ Int32 deviceId)
{
    return NOERROR;
}

ECode HdmiMhlLocalDeviceStub::GetPortId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPortId;
    return NOERROR;
}

ECode HdmiMhlLocalDeviceStub::TurnOn(
    /* [in] */ IIHdmiControlCallback* callback)
{
    return NOERROR;
}

ECode HdmiMhlLocalDeviceStub::SendKeyEvent(
    /* [in] */ Int32 keycode,
    /* [in] */ Boolean isPressed)
{
    return NOERROR;
}

AutoPtr<IHdmiDeviceInfo> HdmiMhlLocalDeviceStub::InitINFO()
{
    AutoPtr<IHdmiDeviceInfo> rev;
    // TODO: Waiting for CHdmiDeviceInfo
    // assert(0);
    // CHdmiDeviceInfo::New(Constants::INVALID_PHYSICAL_ADDRESS, Constants::INVALID_PORT_ID, -1, -1, (IHdmiDeviceInfo**)&rev);
    return rev;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
