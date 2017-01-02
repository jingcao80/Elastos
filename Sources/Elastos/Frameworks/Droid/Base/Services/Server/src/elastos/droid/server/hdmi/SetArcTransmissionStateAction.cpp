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
#include "elastos/droid/server/hdmi/HdmiCecMessage.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiConfig.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/HdmiLogger.h"
#include "elastos/droid/server/hdmi/HdmiUtils.h"
#include "elastos/droid/server/hdmi/SetArcTransmissionStateAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Server::Hdmi::IHdmiControlServiceSendMessageCallback;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// SetArcTransmissionStateAction::InnerSub_SendMessageCallback
//=============================================================================
CAR_INTERFACE_IMPL(SetArcTransmissionStateAction::InnerSub_SendMessageCallback, Object, IHdmiControlServiceSendMessageCallback)

SetArcTransmissionStateAction::InnerSub_SendMessageCallback::InnerSub_SendMessageCallback(
    /* [in] */ SetArcTransmissionStateAction* host)
    : mHost(host)
{}

ECode SetArcTransmissionStateAction::InnerSub_SendMessageCallback::OnSendCompleted(
    /* [in] */ Int32 error)
{
    if (error != Constants::SEND_RESULT_SUCCESS) {
        // If fails to send <Report ARC Initiated>, disable ARC and
        // send <Report ARC Terminated> directly.
        mHost->SetArcStatus(FALSE);
        HdmiLogger::Debug("Failed to send <Report Arc Initiated>.");
        mHost->Finish();
    }
    return NOERROR;
}

//=============================================================================
// SetArcTransmissionStateAction
//=============================================================================
CAR_INTERFACE_IMPL(SetArcTransmissionStateAction, HdmiCecFeatureAction, ISetArcTransmissionStateAction)

const String SetArcTransmissionStateAction::TAG("SetArcTransmissionStateAction");
const Int32 SetArcTransmissionStateAction::STATE_WAITING_TIMEOUT = 1;

SetArcTransmissionStateAction::SetArcTransmissionStateAction()
    : mEnabled(FALSE)
    , mAvrAddress(0)
{}

ECode SetArcTransmissionStateAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 avrAddress,
    /* [in] */ Boolean enabled)
{
    HdmiCecFeatureAction::constructor(source);
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    HdmiUtils::VerifyAddressType(srcAddr, IHdmiDeviceInfo::DEVICE_TV);
    HdmiUtils::VerifyAddressType(avrAddress, IHdmiDeviceInfo::DEVICE_AUDIO_SYSTEM);
    mAvrAddress = avrAddress;
    mEnabled = enabled;
    return NOERROR;
}

ECode SetArcTransmissionStateAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mEnabled) {
        // Enable ARC status immediately after sending <Report Arc Initiated>.
        // If AVR responds with <Feature Abort>, disable ARC status again.
        // This is different from spec that says that turns ARC status to
        // "Enabled" if <Report ARC Initiated> is acknowledged and no
        // <Feature Abort> is received.
        // But implemented this way to save the time having to wait for
        // <Feature Abort>.
        SetArcStatus(TRUE);
        // If succeeds to send <Report ARC Initiated>, wait general timeout
        // to check whether there is no <Feature Abort> for <Report ARC Initiated>.
        mState = STATE_WAITING_TIMEOUT;
        AddTimer(mState, HdmiConfig::TIMEOUT_MS);
        SendReportArcInitiated();
    } else {
        SetArcStatus(FALSE);
        Finish();
    }
    *result = TRUE;
    return NOERROR;
}

ECode SetArcTransmissionStateAction::SendReportArcInitiated()
{
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> command;
    HdmiCecMessageBuilder::BuildReportArcInitiated(srcAddr, mAvrAddress, (IHdmiCecMessage**)&command);
    SendCommand(command, new InnerSub_SendMessageCallback(this));
    return NOERROR;
}

ECode SetArcTransmissionStateAction::SetArcStatus(
    /* [in] */ Boolean enabled)
{
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    Boolean wasEnabled;
    ((HdmiCecLocalDeviceTv*) tv.Get())->SetArcStatus(enabled, &wasEnabled);
    Slogger::I(TAG, "Change arc status [old:%s, new:%s]",
        wasEnabled ? "true" : "false", enabled ? "true" : "false");

    // If enabled before and set to "disabled" and send <Report Arc Terminated> to
    // av reciever.
    if (!enabled && wasEnabled) {
        Int32 srcAddr;
        GetSourceAddress(&srcAddr);
        AutoPtr<IHdmiCecMessage> cmd;
        HdmiCecMessageBuilder::BuildReportArcTerminated(srcAddr, mAvrAddress, (IHdmiCecMessage**)&cmd);
        SendCommand(cmd);
    }
    return NOERROR;
}

ECode SetArcTransmissionStateAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mState != STATE_WAITING_TIMEOUT) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 opcode;
    cmd->GetOpcode(&opcode);
    if (opcode == Constants::MESSAGE_FEATURE_ABORT) {
        AutoPtr<ArrayOf<Byte> > params;
        cmd->GetParams((ArrayOf<Byte>**)&params);
        Int32 originalOpcode = (*params)[0] & 0xFF;
        if (originalOpcode == Constants::MESSAGE_REPORT_ARC_INITIATED) {
            HdmiLogger::Debug("Feature aborted for <Report Arc Initiated>");
            SetArcStatus(FALSE);
            Finish();
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode SetArcTransmissionStateAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    if (mState != state || mState != STATE_WAITING_TIMEOUT) {
        return NOERROR;
    }
    // Expire timeout for <Feature Abort>.
    Finish();
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
