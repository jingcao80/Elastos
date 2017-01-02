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
#include "elastos/droid/server/hdmi/HdmiConfig.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/SystemAudioActionFromTv.h"
#include "elastos/droid/server/hdmi/SystemAudioAutoInitiationAction.h"
#include "elastos/droid/server/hdmi/SystemAudioStatusAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>

using Elastos::Droid::Server::Hdmi::IHdmiControlServiceSendMessageCallback;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// SystemAudioAutoInitiationAction::InnerSub_SendMessageCallback
//=============================================================================
CAR_INTERFACE_IMPL(SystemAudioAutoInitiationAction::InnerSub_SendMessageCallback, Object, IHdmiControlServiceSendMessageCallback)

SystemAudioAutoInitiationAction::InnerSub_SendMessageCallback::InnerSub_SendMessageCallback(
    /* [in] */ SystemAudioAutoInitiationAction* host)
    : mHost(host)
{}

ECode SystemAudioAutoInitiationAction::InnerSub_SendMessageCallback::OnSendCompleted(
    /* [in] */ Int32 error)
{
    if (error != Constants::SEND_RESULT_SUCCESS) {
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
        ((HdmiCecLocalDeviceTv*) tv.Get())->SetSystemAudioMode(FALSE, TRUE);
        mHost->Finish();
    }
    return NOERROR;
}

//=============================================================================
// SystemAudioAutoInitiationAction
//=============================================================================
CAR_INTERFACE_IMPL(SystemAudioAutoInitiationAction, HdmiCecFeatureAction, ISystemAudioAutoInitiationAction)

const Int32 SystemAudioAutoInitiationAction::STATE_WAITING_FOR_SYSTEM_AUDIO_MODE_STATUS = 1;

SystemAudioAutoInitiationAction::SystemAudioAutoInitiationAction()
    : mAvrAddress(0)
{}

ECode SystemAudioAutoInitiationAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 avrAddress)
{
    HdmiCecFeatureAction::constructor(source);
    mAvrAddress = avrAddress;
    return NOERROR;
}

ECode SystemAudioAutoInitiationAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mState = STATE_WAITING_FOR_SYSTEM_AUDIO_MODE_STATUS;

    AddTimer(mState, HdmiConfig::TIMEOUT_MS);
    SendGiveSystemAudioModeStatus();
    *result = TRUE;
    return NOERROR;
}

ECode SystemAudioAutoInitiationAction::SendGiveSystemAudioModeStatus()
{
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildGiveSystemAudioModeStatus(srcAddr,
            mAvrAddress, (IHdmiCecMessage**)&cmd);
    SendCommand(cmd, new InnerSub_SendMessageCallback(this));
    return NOERROR;
}

ECode SystemAudioAutoInitiationAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mState != STATE_WAITING_FOR_SYSTEM_AUDIO_MODE_STATUS) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 opcode;
    cmd->GetOpcode(&opcode);
    if (opcode == Constants::MESSAGE_SYSTEM_AUDIO_MODE_STATUS) {
        HandleSystemAudioModeStatusMessage();
        *result = TRUE;
        return NOERROR;
    }
    else {
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode SystemAudioAutoInitiationAction::HandleSystemAudioModeStatusMessage()
{
    // If the last setting is system audio, turn on system audio whatever AVR status is.
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    Boolean bSystemAudioModeSetting;
    ((HdmiCecLocalDeviceTv*) tv.Get())->GetSystemAudioModeSetting(&bSystemAudioModeSetting);
    if (bSystemAudioModeSetting) {
        Boolean canChangeSystemAudio;
        CanChangeSystemAudio(&canChangeSystemAudio);
        if (canChangeSystemAudio) {
            AutoPtr<SystemAudioActionFromTv> newSystemAudioActionFromTv = new SystemAudioActionFromTv();
            newSystemAudioActionFromTv->constructor(IHdmiCecLocalDevice::Probe(tv), mAvrAddress, TRUE, NULL);
            AddAndStartAction(newSystemAudioActionFromTv);
        }
    } else {
        // If the last setting is non-system audio, turn off system audio mode
        // and update system audio status (volume or mute).
        ((HdmiCecLocalDeviceTv*) tv.Get())->SetSystemAudioMode(FALSE, TRUE);
        Boolean canChangeSystemAudio;
        CanChangeSystemAudio(&canChangeSystemAudio);
        if (canChangeSystemAudio) {
            AutoPtr<SystemAudioStatusAction> newSystemAudioStatusAction = new SystemAudioStatusAction();
            newSystemAudioStatusAction->constructor(IHdmiCecLocalDevice::Probe(tv), mAvrAddress, NULL);
            AddAndStartAction(newSystemAudioStatusAction);
        }
    }
    Finish();
    return NOERROR;
}

ECode SystemAudioAutoInitiationAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    if (mState != state) {
        return NOERROR;
    }

    switch (mState) {
        case STATE_WAITING_FOR_SYSTEM_AUDIO_MODE_STATUS:
            HandleSystemAudioModeStatusTimeout();
            break;
    }
    return NOERROR;
}

ECode SystemAudioAutoInitiationAction::HandleSystemAudioModeStatusTimeout()
{
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    Boolean bSystemAudioModeSetting;
    ((HdmiCecLocalDeviceTv*) tv.Get())->GetSystemAudioModeSetting(&bSystemAudioModeSetting);
    if (bSystemAudioModeSetting) {
        Boolean canChangeSystemAudio;
        CanChangeSystemAudio(&canChangeSystemAudio);
        if (canChangeSystemAudio) {
            AutoPtr<SystemAudioActionFromTv> newSystemAudioActionFromTv = new SystemAudioActionFromTv();
            newSystemAudioActionFromTv->constructor(IHdmiCecLocalDevice::Probe(tv), mAvrAddress, TRUE, NULL);
            AddAndStartAction(newSystemAudioActionFromTv);
        }
    } else {
        ((HdmiCecLocalDeviceTv*) tv.Get())->SetSystemAudioMode(FALSE, TRUE);
    }
    Finish();
    return NOERROR;
}

ECode SystemAudioAutoInitiationAction::CanChangeSystemAudio(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    Boolean hasAction;
    ((HdmiCecLocalDeviceTv*) tv.Get())->HasAction(ECLSID_CSystemAudioActionFromTv, &hasAction);
    Boolean hasAvrAction;
    ((HdmiCecLocalDeviceTv*) tv.Get())->HasAction(ECLSID_CSystemAudioActionFromAvr, &hasAvrAction);
    *result = !hasAction || hasAvrAction;
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
