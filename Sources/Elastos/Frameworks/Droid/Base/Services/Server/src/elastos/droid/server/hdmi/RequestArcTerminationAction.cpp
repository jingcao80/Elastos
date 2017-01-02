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

#include "elastos/droid/server/hdmi/RequestArcTerminationAction.h"
#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/HdmiCecMessage.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiConfig.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"

using Elastos::Droid::Server::Hdmi::IHdmiControlServiceSendMessageCallback;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// RequestArcTerminationAction::InnerSub_SendMessageCallback
//=============================================================================
CAR_INTERFACE_IMPL(RequestArcTerminationAction::InnerSub_SendMessageCallback, Object, IHdmiControlServiceSendMessageCallback)

RequestArcTerminationAction::InnerSub_SendMessageCallback::InnerSub_SendMessageCallback(
    /* [in] */ RequestArcTerminationAction* host)
    : mHost(host)
{}

ECode RequestArcTerminationAction::InnerSub_SendMessageCallback::OnSendCompleted(
    /* [in] */ Int32 error)
{
    if (error != Constants::SEND_RESULT_SUCCESS) {
        // If failed to send <Request ARC Termination>, start "Disabled" ARC
        // transmission action.
        mHost->DisableArcTransmission();
        mHost->Finish();
    }
    return NOERROR;
}

//=============================================================================
// RequestArcTerminationAction
//=============================================================================
CAR_INTERFACE_IMPL(RequestArcTerminationAction, RequestArcAction, IRequestArcTerminationAction)

const String RequestArcTerminationAction::TAG("RequestArcTerminationAction");

ECode RequestArcTerminationAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 avrAddress)
{
    return RequestArcAction::constructor(source, avrAddress);
}

ECode RequestArcTerminationAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mState = STATE_WATING_FOR_REQUEST_ARC_REQUEST_RESPONSE;
    AddTimer(mState, HdmiConfig::TIMEOUT_MS);

    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> command;
    HdmiCecMessageBuilder::BuildRequestArcTermination(srcAddr, mAvrAddress, (IHdmiCecMessage**)&command);
    SendCommand(command, new InnerSub_SendMessageCallback(this));
    *result = TRUE;
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
