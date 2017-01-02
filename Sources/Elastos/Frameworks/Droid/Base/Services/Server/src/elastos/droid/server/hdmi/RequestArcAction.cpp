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
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/HdmiLogger.h"
#include "elastos/droid/server/hdmi/HdmiUtils.h"
#include "elastos/droid/server/hdmi/RequestArcAction.h"
#include "elastos/droid/server/hdmi/SetArcTransmissionStateAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/droid/internal/utility/State.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Utility::ISlog;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CAR_INTERFACE_IMPL(RequestArcAction, HdmiCecFeatureAction, IRequestArcAction)

const String RequestArcAction::TAG("RequestArcAction");
const Int32 RequestArcAction::STATE_WATING_FOR_REQUEST_ARC_REQUEST_RESPONSE = 1;

RequestArcAction::RequestArcAction()
    : mAvrAddress(0)
{}

ECode RequestArcAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 avrAddress)
{
    HdmiCecFeatureAction::constructor(source);
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    HdmiUtils::VerifyAddressType(srcAddr, IHdmiDeviceInfo::DEVICE_TV);
    HdmiUtils::VerifyAddressType(avrAddress, IHdmiDeviceInfo::DEVICE_AUDIO_SYSTEM);
    mAvrAddress = avrAddress;
    return NOERROR;
}

ECode RequestArcAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mState != STATE_WATING_FOR_REQUEST_ARC_REQUEST_RESPONSE
            || !HdmiUtils::CheckCommandSource(cmd, mAvrAddress, TAG)) {
        *result = FALSE;
        return NOERROR;
    }
    Int32 opcode;
    cmd->GetOpcode(&opcode);
    // Handles only <Feature Abort> here and, both <Initiate ARC> and <Terminate ARC>
    // are handled in HdmiControlService itself because both can be
    // received without <Request ARC Initiation> or <Request ARC Termination>.
    if (opcode == Constants::MESSAGE_FEATURE_ABORT) {
        AutoPtr<ArrayOf<Byte> > params;
        cmd->GetParams((ArrayOf<Byte>**)&params);
        Int32 originalOpcode = (*params)[0] & 0xFF;
        if (originalOpcode == Constants::MESSAGE_REQUEST_ARC_INITIATION
                || originalOpcode == Constants::MESSAGE_REQUEST_ARC_TERMINATION) {
            DisableArcTransmission();
            Finish();
            *result = TRUE;
            return NOERROR;
        } else {
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode RequestArcAction::DisableArcTransmission()
{
    // Start Set ARC Transmission State action.
    AutoPtr<IHdmiCecLocalDevice> localDevice;
    LocalDevice((IHdmiCecLocalDevice**)&localDevice);
    AutoPtr<SetArcTransmissionStateAction> action = new SetArcTransmissionStateAction();
    action->constructor(localDevice, mAvrAddress, FALSE);
    AddAndStartAction(action);
    return NOERROR;
}

ECode RequestArcAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    if (mState != state || state != STATE_WATING_FOR_REQUEST_ARC_REQUEST_RESPONSE) {
        return NOERROR;
    }
    HdmiLogger::Debug("[T]RequestArcAction.");
    DisableArcTransmission();
    Finish();
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
