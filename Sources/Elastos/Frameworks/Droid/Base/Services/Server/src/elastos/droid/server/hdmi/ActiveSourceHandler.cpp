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

#include "elastos/droid/server/hdmi/ActiveSourceHandler.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDeviceTv.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/RoutingControlAction.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String ActiveSourceHandler::TAG("ActiveSourceHandler");

ECode ActiveSourceHandler::Create(
    /* [in] */ IHdmiCecLocalDeviceTv* source,
    /* [in] */ IIHdmiControlCallback* callback,
    /* [out] */ ActiveSourceHandler** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (source == NULL) {
        Slogger::E(TAG, "Wrong arguments");
        *result = NULL;
        return NOERROR;
    }
    *result = new ActiveSourceHandler(source, callback);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ActiveSourceHandler::ActiveSourceHandler(
    /* [in] */ IHdmiCecLocalDeviceTv* source,
    /* [in] */ IIHdmiControlCallback* callback)
    : mSource(source)
    , mCallback(callback)
{
    ((HdmiCecLocalDeviceTv*) mSource.Get())->GetService((IHdmiControlService**)&mService);
}

ECode ActiveSourceHandler::Process(
    /* [in] */ IHdmiCecLocalDeviceActiveSource* newActive)
{
    // Seq #17
    AutoPtr<IHdmiCecLocalDeviceTv> tv = mSource;
    AutoPtr<IHdmiCecLocalDeviceActiveSource> activeSource;
    ((HdmiCecLocalDeviceTv*) tv.Get())->GetActiveSource((IHdmiCecLocalDeviceActiveSource**)&activeSource);
    Boolean isEquals;
    IObject::Probe(activeSource)->Equals(newActive, &isEquals);
    if (isEquals) {
        InvokeCallback(IHdmiControlManager::RESULT_SUCCESS);
        return NOERROR;
    }
    AutoPtr<IHdmiDeviceInfo> device;
    ((HdmiControlService*)mService.Get())->GetDeviceInfo(((HdmiCecLocalDevice::ActiveSource*)newActive)->mLogicalAddress, (IHdmiDeviceInfo**)&device);
    if (device == NULL) {
        ((HdmiCecLocalDeviceTv*) tv.Get())->StartNewDeviceAction(newActive);
    }
    Boolean isProhibitMode;
    ((HdmiCecLocalDeviceTv*) tv.Get())->IsProhibitMode(&isProhibitMode);
    if (!isProhibitMode) {
        ((HdmiCecLocalDeviceTv*) tv.Get())->UpdateActiveSource(newActive);
        Boolean notifyInputChange = (mCallback == NULL);
        ((HdmiCecLocalDeviceTv*) tv.Get())->UpdateActiveInput(((HdmiCecLocalDevice::ActiveSource*)newActive)->mPhysicalAddress, notifyInputChange);
        InvokeCallback(IHdmiControlManager::RESULT_SUCCESS);
    } else {
        // TV is in a mode that should keep its current source/input from
        // being changed for its operation. Reclaim the active source
        // or switch the port back to the one used for the current mode.
        AutoPtr<IHdmiCecLocalDeviceActiveSource> current;
        ((HdmiCecLocalDeviceTv*) tv.Get())->GetActiveSource((IHdmiCecLocalDeviceActiveSource**)&current);
        Int32 sourceAddress;
        GetSourceAddress(&sourceAddress);
        if (((HdmiCecLocalDevice::ActiveSource*)current.Get())->mLogicalAddress == sourceAddress) {
            AutoPtr<IHdmiCecMessage> activeSourceCommand;
            HdmiCecMessageBuilder::BuildActiveSource(
                    ((HdmiCecLocalDevice::ActiveSource*)current.Get())->mLogicalAddress, ((HdmiCecLocalDevice::ActiveSource*)current.Get())->mPhysicalAddress, (IHdmiCecMessage**)&activeSourceCommand);
            ((HdmiControlService*)mService.Get())->SendCecCommand(activeSourceCommand);
            ((HdmiCecLocalDeviceTv*) tv.Get())->UpdateActiveSource(current);
            InvokeCallback(IHdmiControlManager::RESULT_SUCCESS);
        } else {
            AutoPtr<IHdmiCecMessage> routingChange;
            Int32 address;
            GetSourceAddress(&address);
            HdmiCecMessageBuilder::BuildRoutingChange(
                    address, ((HdmiCecLocalDevice::ActiveSource*)newActive)->mPhysicalAddress, ((HdmiCecLocalDevice::ActiveSource*)current.Get())->mPhysicalAddress, (IHdmiCecMessage**)&routingChange);
            ((HdmiControlService*)mService.Get())->SendCecCommand(routingChange);
            AutoPtr<RoutingControlAction> action = new RoutingControlAction();
            action->constructor(IHdmiCecLocalDevice::Probe(tv), ((HdmiCecLocalDevice::ActiveSource*)current.Get())->mPhysicalAddress, TRUE, mCallback);
            ((HdmiCecLocalDeviceTv*) tv.Get())->AddAndStartAction(action.Get());
        }
    }
    return NOERROR;
}

ECode ActiveSourceHandler::GetSourceAddress(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IHdmiDeviceInfo> info;
    ((HdmiCecLocalDeviceTv*) mSource.Get())->GetDeviceInfo((IHdmiDeviceInfo**)&info);
    return info->GetLogicalAddress(result);
}

ECode ActiveSourceHandler::InvokeCallback(
    /* [in] */ Int32 result)
{
    if (mCallback == NULL) {
        return NOERROR;
    }
    // try {
    ECode ec = mCallback->OnComplete(result);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            Slogger::E(TAG, "Callback failed: %d", ec);
        }
        else
            return ec;
    }
    // }
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
