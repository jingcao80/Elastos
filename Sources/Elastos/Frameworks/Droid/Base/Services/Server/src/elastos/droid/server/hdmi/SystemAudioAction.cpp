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
#include "elastos/droid/server/hdmi/HdmiLogger.h"
#include "elastos/droid/server/hdmi/HdmiUtils.h"
#include "elastos/droid/server/hdmi/RoutingControlAction.h"
#include "elastos/droid/server/hdmi/SystemAudioAction.h"
#include "elastos/droid/server/hdmi/SystemAudioActionFromAvr.h"
#include "elastos/droid/server/hdmi/SystemAudioStatusAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Utility::ISlog;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// SystemAudioAction::InnerSub_Runnable
//=============================================================================
CAR_INTERFACE_IMPL(SystemAudioAction::InnerSub_Runnable, Object, IRunnable)

SystemAudioAction::InnerSub_Runnable::InnerSub_Runnable(
    /* [in] */ SystemAudioAction* host)
    : mHost(host)
{}

ECode SystemAudioAction::InnerSub_Runnable::Run()
{
    return mHost->SendSystemAudioModeRequestInternal();
}

//=============================================================================
// SystemAudioAction::InnerSub_SendMessageCallback
//=============================================================================
CAR_INTERFACE_IMPL(SystemAudioAction::InnerSub_SendMessageCallback, Object, IHdmiControlServiceSendMessageCallback)

SystemAudioAction::InnerSub_SendMessageCallback::InnerSub_SendMessageCallback(
    /* [in] */ SystemAudioAction* host)
    : mHost(host)
{}

ECode SystemAudioAction::InnerSub_SendMessageCallback::OnSendCompleted(
    /* [in] */ Int32 error)
{
    if (error != Constants::SEND_RESULT_SUCCESS) {
        HdmiLogger::Debug("Failed to send <System Audio Mode Request>:%s", error);
        mHost->SetSystemAudioMode(FALSE);
        mHost->FinishWithCallback(IHdmiControlManager::RESULT_COMMUNICATION_FAILED);
    }
    return NOERROR;
}

//=============================================================================
// SystemAudioAction
//=============================================================================
CAR_INTERFACE_IMPL(SystemAudioAction, HdmiCecFeatureAction, ISystemAudioAction)

const String SystemAudioAction::TAG("SystemAudioAction");
const Int32 SystemAudioAction::STATE_CHECK_ROUTING_IN_PRGRESS = 1;
const Int32 SystemAudioAction::STATE_WAIT_FOR_SET_SYSTEM_AUDIO_MODE = 2;
const Int32 SystemAudioAction::MAX_SEND_RETRY_COUNT = 2;
const Int32 SystemAudioAction::ON_TIMEOUT_MS = 5000;
const Int32 SystemAudioAction::OFF_TIMEOUT_MS = HdmiConfig::TIMEOUT_MS;

SystemAudioAction::SystemAudioAction()
    : mAvrLogicalAddress(0)
    , mTargetAudioStatus(FALSE)
    , mSendRetryCount(0)
{}

ECode SystemAudioAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 avrAddress,
    /* [in] */ Boolean targetStatus,
    /* [in] */ IIHdmiControlCallback* callback)
{
    HdmiCecFeatureAction::constructor(source);
    HdmiUtils::VerifyAddressType(avrAddress, IHdmiDeviceInfo::DEVICE_AUDIO_SYSTEM);
    mAvrLogicalAddress = avrAddress;
    mTargetAudioStatus = targetStatus;
    mCallback = callback;
    return NOERROR;
}

ECode SystemAudioAction::SendSystemAudioModeRequest()
{
    AutoPtr<IList> routingActions;
    GetActions(ECLSID_CRoutingControlAction, (IList**)&routingActions);
    Boolean isEmpty;
    routingActions->IsEmpty(&isEmpty);
    if (!isEmpty) {
        mState = STATE_CHECK_ROUTING_IN_PRGRESS;
        // Should have only one Routing Control Action
        AutoPtr<IInterface> obj;
        routingActions->Get(0, (IInterface**)&obj);
        AutoPtr<RoutingControlAction> routingAction = (RoutingControlAction*) IObject::Probe(obj);
        routingAction->AddOnFinishedCallback(this, new InnerSub_Runnable(this));
        return NOERROR;
    }
    SendSystemAudioModeRequestInternal();
    return NOERROR;
}

ECode SystemAudioAction::SendSystemAudioModeRequestInternal()
{
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    AutoPtr<IHdmiDeviceInfo> info;
    ((HdmiCecLocalDeviceTv*) tv.Get())->GetAvrDeviceInfo((IHdmiDeviceInfo**)&info);
    Int32 avrPhysicalAddress;
    info->GetPhysicalAddress(&avrPhysicalAddress);
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> command;
    HdmiCecMessageBuilder::BuildSystemAudioModeRequest(srcAddr,
            mAvrLogicalAddress, avrPhysicalAddress, mTargetAudioStatus, (IHdmiCecMessage**)&command);
    SendCommand(command, new InnerSub_SendMessageCallback(this));
    mState = STATE_WAIT_FOR_SET_SYSTEM_AUDIO_MODE;
    AddTimer(mState, mTargetAudioStatus ? ON_TIMEOUT_MS : OFF_TIMEOUT_MS);
    return NOERROR;
}

ECode SystemAudioAction::HandleSendSystemAudioModeRequestTimeout()
{
    if (!mTargetAudioStatus  // Don't retry for Off case.
            || mSendRetryCount++ >= MAX_SEND_RETRY_COUNT) {
        HdmiLogger::Debug("[T]:wait for <Set System Audio Mode>.");
        SetSystemAudioMode(FALSE);
        FinishWithCallback(IHdmiControlManager::RESULT_TIMEOUT);
        return NOERROR;
    }
    SendSystemAudioModeRequest();
    return NOERROR;
}

ECode SystemAudioAction::SetSystemAudioMode(
    /* [in] */ Boolean mode)
{
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    ((HdmiCecLocalDeviceTv*) tv.Get())->SetSystemAudioMode(mode, TRUE);
    return NOERROR;
}

ECode SystemAudioAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    switch (mState) {
        case STATE_WAIT_FOR_SET_SYSTEM_AUDIO_MODE:
        {
            Int32 opcode;
            cmd->GetOpcode(&opcode);
            AutoPtr<ArrayOf<Byte> > params;
            cmd->GetParams((ArrayOf<Byte>**)&params);
            if (opcode == Constants::MESSAGE_FEATURE_ABORT
                    && ((*params)[0] & 0xFF) == Constants::MESSAGE_SYSTEM_AUDIO_MODE_REQUEST) {
                HdmiLogger::Debug("Failed to start system audio mode request.");
                SetSystemAudioMode(FALSE);
                FinishWithCallback(IHdmiControlManager::RESULT_EXCEPTION);
                *result = TRUE;
                return NOERROR;
            }
            if (opcode != Constants::MESSAGE_SET_SYSTEM_AUDIO_MODE
                    || !HdmiUtils::CheckCommandSource(cmd, mAvrLogicalAddress, TAG)) {
                *result = FALSE;
                return NOERROR;
            }
            Boolean receivedStatus = HdmiUtils::ParseCommandParamSystemAudioStatus(cmd);
            if (receivedStatus == mTargetAudioStatus) {
                SetSystemAudioMode(receivedStatus);
                StartAudioStatusAction();
                *result = TRUE;
                return NOERROR;
            } else {
                HdmiLogger::Debug("Unexpected system audio mode request:%s", receivedStatus ? "true" : "false");
                // Unexpected response, consider the request is newly initiated by AVR.
                // To return 'FALSE' will initiate new SystemAudioActionFromAvr by the control
                // service.
                FinishWithCallback(IHdmiControlManager::RESULT_EXCEPTION);
                *result = FALSE;
                return NOERROR;
            }
        }
        default:
            *result = FALSE;
            return NOERROR;
    }
    return NOERROR;
}

ECode SystemAudioAction::StartAudioStatusAction()
{
    AutoPtr<SystemAudioStatusAction> newSystemAudioStatusAction = new SystemAudioStatusAction();
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    newSystemAudioStatusAction->constructor(IHdmiCecLocalDevice::Probe(tv), mAvrLogicalAddress, mCallback);
    AddAndStartAction(newSystemAudioStatusAction);
    Finish();
    return NOERROR;
}

ECode SystemAudioAction::RemoveSystemAudioActionInProgress()
{
    RemoveActionExcept(ECLSID_CSystemAudioActionFromTv, this);
    RemoveActionExcept(ECLSID_CSystemAudioActionFromAvr, this);
    return NOERROR;
}

ECode SystemAudioAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    if (mState != state) {
        return NOERROR;
    }
    switch (mState) {
        case STATE_WAIT_FOR_SET_SYSTEM_AUDIO_MODE:
            HandleSendSystemAudioModeRequestTimeout();
            return NOERROR;
    }
    return NOERROR;
}

ECode SystemAudioAction::FinishWithCallback(
    /* [in] */ Int32 returnCode)
{
    if (mCallback != NULL) {
        // try {
        ECode ec = mCallback->OnComplete(returnCode);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "Failed to invoke callback.%d", ec);
            }
            else
                return ec;
        }
        // }
    }
    Finish();
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
