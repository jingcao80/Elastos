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
#include "elastos/droid/server/hdmi/OneTouchRecordAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Server::Hdmi::IHdmiControlServiceSendMessageCallback;
using Elastos::Droid::Utility::ISlog;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// OneTouchRecordAction::InnerSub_SendMessageCallback
//=============================================================================
CAR_INTERFACE_IMPL(OneTouchRecordAction::InnerSub_SendMessageCallback, Object, IHdmiControlServiceSendMessageCallback)

OneTouchRecordAction::InnerSub_SendMessageCallback::InnerSub_SendMessageCallback(
    /* [in] */ OneTouchRecordAction* host)
    : mHost(host)
{}

ECode OneTouchRecordAction::InnerSub_SendMessageCallback::OnSendCompleted(
    /* [in] */ Int32 error)
{
    // if failed to send <Record On>, display error message and finish action.
    if (error != Constants::SEND_RESULT_SUCCESS) {
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
        ((HdmiCecLocalDeviceTv*) tv.Get())->AnnounceOneTouchRecordResult(
                IHdmiControlManager::ONE_TOUCH_RECORD_CHECK_RECORDER_CONNECTION);
        mHost->Finish();
        return NOERROR;
    }

    mHost->mState = STATE_WAITING_FOR_RECORD_STATUS;
    mHost->AddTimer(mHost->mState, RECORD_STATUS_TIMEOUT_MS);
    return NOERROR;
}

//=============================================================================
// OneTouchRecordAction
//=============================================================================
CAR_INTERFACE_IMPL(OneTouchRecordAction, HdmiCecFeatureAction, IOneTouchRecordAction)

const String OneTouchRecordAction::TAG("OneTouchRecordAction");
const Int32 OneTouchRecordAction::RECORD_STATUS_TIMEOUT_MS = 120000;
const Int32 OneTouchRecordAction::STATE_WAITING_FOR_RECORD_STATUS = 1;
const Int32 OneTouchRecordAction::STATE_RECORDING_IN_PROGRESS = 2;

OneTouchRecordAction::OneTouchRecordAction()
    : mRecorderAddress(0)
{}

ECode OneTouchRecordAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 recorderAddress,
    /* [in] */ ArrayOf<Byte>* recordSource)
{
    HdmiCecFeatureAction::constructor(source);
    mRecorderAddress = recorderAddress;
    mRecordSource = recordSource;
    return NOERROR;
}

ECode OneTouchRecordAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    SendRecordOn();
    *result = TRUE;
    return NOERROR;
}

ECode OneTouchRecordAction::SendRecordOn()
{
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildRecordOn(srcAddr, mRecorderAddress,
            mRecordSource, (IHdmiCecMessage**)&cmd);
    SendCommand(cmd, new InnerSub_SendMessageCallback(this));
    return NOERROR;
}

ECode OneTouchRecordAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mState != STATE_WAITING_FOR_RECORD_STATUS) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 opcode;
    cmd->GetOpcode(&opcode);
    if (opcode == Constants::MESSAGE_RECORD_STATUS) {
            *result = HandleRecordStatus(cmd);
            return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

Boolean OneTouchRecordAction::HandleRecordStatus(
    /* [in] */ IHdmiCecMessage* cmd)
{
    // Only handle message coming from original recorder.
    Int32 srcAddr;
    cmd->GetSource(&srcAddr);
    if (srcAddr != mRecorderAddress) {
        return FALSE;
    }

    AutoPtr<ArrayOf<Byte> > params;
    cmd->GetParams((ArrayOf<Byte>**)&params);
    Int32 recordStatus = (*params)[0];
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    ((HdmiCecLocalDeviceTv*) tv.Get())->AnnounceOneTouchRecordResult(recordStatus);
    Slogger::I(TAG, "Got record status:%d from %d", recordStatus, srcAddr);

    // If recording started successfully, change state and keep this action until <Record Off>
    // received. Otherwise, finish action.
    switch (recordStatus) {
        case IHdmiControlManager::ONE_TOUCH_RECORD_RECORDING_CURRENTLY_SELECTED_SOURCE:
        case IHdmiControlManager::ONE_TOUCH_RECORD_RECORDING_DIGITAL_SERVICE:
        case IHdmiControlManager::ONE_TOUCH_RECORD_RECORDING_ANALOGUE_SERVICE:
        case IHdmiControlManager::ONE_TOUCH_RECORD_RECORDING_EXTERNAL_INPUT:
            mState = STATE_RECORDING_IN_PROGRESS;
            mActionTimer->ClearTimerMessage();
            break;
        default:
            Finish();
            break;
    }
    return TRUE;
}

ECode OneTouchRecordAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    if (mState != state) {
        Slogger::W(TAG, "Timeout in invalid state:[Expected:%d, Actual:%d]", mState, state);
        return NOERROR;
    }

    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    ((HdmiCecLocalDeviceTv*) tv.Get())->AnnounceOneTouchRecordResult(IHdmiControlManager::ONE_TOUCH_RECORD_CHECK_RECORDER_CONNECTION);
    Finish();
    return NOERROR;
}


ECode OneTouchRecordAction::GetRecorderAddress(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mRecorderAddress;
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
