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
#include "elastos/droid/server/hdmi/HdmiCecLocalDeviceTv.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/PowerStatusMonitorAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Hardware.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Server::Hdmi::IHdmiControlServiceSendMessageCallback;
using Elastos::Droid::Utility::CSparseInt32Array;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// PowerStatusMonitorAction::InnerSub_SendMessageCallback
//=============================================================================
CAR_INTERFACE_IMPL(PowerStatusMonitorAction::InnerSub_SendMessageCallback, Object, IHdmiControlServiceSendMessageCallback)

PowerStatusMonitorAction::InnerSub_SendMessageCallback::InnerSub_SendMessageCallback(
    /* [in] */ PowerStatusMonitorAction* host,
    /* [in] */ Int32 logicalAddress)
    : mHost(host)
    , mLogicalAddress(logicalAddress)
{}

ECode PowerStatusMonitorAction::InnerSub_SendMessageCallback::OnSendCompleted(
    /* [in] */ Int32 error)
{
    // If fails to send <Give Device Power Status>,
    // update power status into UNKNOWN.
    if (error != Constants::SEND_RESULT_SUCCESS) {
       mHost->UpdatePowerStatus(mLogicalAddress, IHdmiControlManager::POWER_STATUS_UNKNOWN, TRUE);
    }
    return NOERROR;
}

//=============================================================================
// PowerStatusMonitorAction
//=============================================================================
CAR_INTERFACE_IMPL(PowerStatusMonitorAction, HdmiCecFeatureAction, IPowerStatusMonitorAction)

const String PowerStatusMonitorAction::TAG("PowerStatusMonitorAction");
const Int32 PowerStatusMonitorAction::STATE_WAIT_FOR_REPORT_POWER_STATUS = 1;
const Int32 PowerStatusMonitorAction::STATE_WAIT_FOR_NEXT_MONITORING = 2;
const Int32 PowerStatusMonitorAction::INVALID_POWER_STATUS = IHdmiControlManager::POWER_STATUS_UNKNOWN - 1;
const Int32 PowerStatusMonitorAction::MONITIROING_INTERNAL_MS = 60000;
const Int32 PowerStatusMonitorAction::REPORT_POWER_STATUS_TIMEOUT_MS = 5000;

PowerStatusMonitorAction::PowerStatusMonitorAction()
{
    CSparseInt32Array::New((ISparseInt32Array**)&mPowerStatus);
}

ECode PowerStatusMonitorAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source)
{
    return HdmiCecFeatureAction::constructor(source);
}

ECode PowerStatusMonitorAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    QueryPowerStatus();
    *result = TRUE;
    return NOERROR;
}

ECode PowerStatusMonitorAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mState != STATE_WAIT_FOR_REPORT_POWER_STATUS) {
        *result = FALSE;
        return NOERROR;
    }
    *result = HandleReportPowerStatus(cmd);
    return NOERROR;
}

Boolean PowerStatusMonitorAction::HandleReportPowerStatus(
    /* [in] */ IHdmiCecMessage* cmd)
{
    Int32 sourceAddress;
    cmd->GetSource(&sourceAddress);
    Int32 oldStatus;
    mPowerStatus->Get(sourceAddress, INVALID_POWER_STATUS, &oldStatus);
    if (oldStatus == INVALID_POWER_STATUS) {
        // if no device exists for incoming message, hands it over to other actions.
        return FALSE;
    }
    AutoPtr<ArrayOf<Byte> > params;
    cmd->GetParams((ArrayOf<Byte>**)&params);
    Int32 newStatus = (*params)[0] & 0xFF;
    UpdatePowerStatus(sourceAddress, newStatus, TRUE);
    return TRUE;
}

ECode PowerStatusMonitorAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    switch (mState) {
        case STATE_WAIT_FOR_NEXT_MONITORING:
            QueryPowerStatus();
            break;
        case STATE_WAIT_FOR_REPORT_POWER_STATUS:
            HandleTimeout();
            break;
    }
    return NOERROR;
}

ECode PowerStatusMonitorAction::HandleTimeout()
{
    Int32 size;
    mPowerStatus->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        Int32 logicalAddress;
        mPowerStatus->KeyAt(i, &logicalAddress);
        UpdatePowerStatus(logicalAddress, IHdmiControlManager::POWER_STATUS_UNKNOWN, FALSE);
    }
    mPowerStatus->Clear();
    mState = STATE_WAIT_FOR_NEXT_MONITORING;
    return NOERROR;
}

ECode PowerStatusMonitorAction::ResetPowerStatus(
    /* [in] */ IList* deviceInfos)
{
    mPowerStatus->Clear();
    AutoPtr<IIterator> it;
    deviceInfos->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IHdmiDeviceInfo> info = IHdmiDeviceInfo::Probe(obj);
        Int32 logicalAddr;
        info->GetLogicalAddress(&logicalAddr);
        Int32 devicePowerStatus;
        info->GetDevicePowerStatus(&devicePowerStatus);
        mPowerStatus->Append(logicalAddr, devicePowerStatus);
    }
    return NOERROR;
}

ECode PowerStatusMonitorAction::QueryPowerStatus()
{
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    AutoPtr<IList> deviceInfos;
    ((HdmiCecLocalDeviceTv*) tv.Get())->GetDeviceInfoList(FALSE, (IList**)&deviceInfos);
    ResetPowerStatus(deviceInfos);
    AutoPtr<IIterator> it;
    deviceInfos->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IHdmiDeviceInfo> info = IHdmiDeviceInfo::Probe(obj);
        Int32 logicalAddress;
        info->GetLogicalAddress(&logicalAddress);
        Int32 srcAddr;
        GetSourceAddress(&srcAddr);
        AutoPtr<IHdmiCecMessage> cmd;
        HdmiCecMessageBuilder::BuildGiveDevicePowerStatus(srcAddr,
                logicalAddress, (IHdmiCecMessage**)&cmd);
        SendCommand(cmd, new InnerSub_SendMessageCallback(this, logicalAddress));
    }

    mState = STATE_WAIT_FOR_REPORT_POWER_STATUS;

    // Add both timers, monitoring and timeout.
    AddTimer(STATE_WAIT_FOR_NEXT_MONITORING, MONITIROING_INTERNAL_MS);
    AddTimer(STATE_WAIT_FOR_REPORT_POWER_STATUS, REPORT_POWER_STATUS_TIMEOUT_MS);
    return NOERROR;
}

ECode PowerStatusMonitorAction::UpdatePowerStatus(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 newStatus,
    /* [in] */ Boolean remove)
{
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    ((HdmiCecLocalDeviceTv*) tv.Get())->UpdateDevicePowerStatus(logicalAddress, newStatus);

    if (remove) {
        mPowerStatus->Delete(logicalAddress);
    }
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
