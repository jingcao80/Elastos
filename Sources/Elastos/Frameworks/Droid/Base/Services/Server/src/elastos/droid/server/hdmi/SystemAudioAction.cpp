
#include "elastos/droid/server/hdmi/SystemAudioAction.h"
#include "elastos/droid/server/hdmi/HdmiConfig.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Hardware.h>
#include "elastos/droid/server/hdmi/HdmiControlService.h"

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Utility::ISlog;

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

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
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super::constructor(source);
        HdmiUtils->VerifyAddressType(avrAddress, IHdmiDeviceInfo::DEVICE_AUDIO_SYSTEM);
        mAvrLogicalAddress = avrAddress;
        mTargetAudioStatus = targetStatus;
        mCallback = callback;
#endif
}

ECode SystemAudioAction::SendSystemAudioModeRequest()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        List<RoutingControlAction> routingActions = GetActions(ECLSID_CRoutingControlAction);
        Boolean isEmpty;
        routingActions->IsEmpty(&isEmpty);
        if (!isEmpty) {
            mState = STATE_CHECK_ROUTING_IN_PRGRESS;
            // Should have only one Routing Control Action
            AutoPtr<IInterface> obj;
            routingActions->Get(0, (IInterface**)&obj);
            RoutingControlAction routingAction = I::Probe(obj);
            routingAction->AddOnFinishedCallback(this, new Runnable() {
                //@Override
                CARAPI Run() {
                    SendSystemAudioModeRequestInternal();
                }
            });
            return NOERROR;
        }
        SendSystemAudioModeRequestInternal();
#endif
}

ECode SystemAudioAction::SendSystemAudioModeRequestInternal()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        AutoPtr<IHdmiDeviceInfo> info;
        tv->GetAvrDeviceInfo((IHdmiDeviceInfo**)&info);
        Int32 avrPhysicalAddress;
        info->GetPhysicalAddress(&avrPhysicalAddress);
        Int32 srcAddr;
        GetSourceAddress(&srcAddr);
        HdmiCecMessage command = HdmiCecMessageBuilder->BuildSystemAudioModeRequest(srcAddr,
                mAvrLogicalAddress, avrPhysicalAddress, mTargetAudioStatus);
        SendCommand(command, new HdmiControlService->SendMessageCallback() {
            //@Override
            CARAPI OnSendCompleted(Int32 error) {
                if (error != Constants::SEND_RESULT_SUCCESS) {
                    HdmiLogger->Debug("Failed to send <System Audio Mode Request>:" + error);
                    SetSystemAudioMode(FALSE);
                    FinishWithCallback(IHdmiControlManager::RESULT_COMMUNICATION_FAILED);
                }
            }
        });
        mState = STATE_WAIT_FOR_SET_SYSTEM_AUDIO_MODE;
        AddTimer(mState, mTargetAudioStatus ? ON_TIMEOUT_MS : OFF_TIMEOUT_MS);
#endif
}

ECode SystemAudioAction::HandleSendSystemAudioModeRequestTimeout()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (!mTargetAudioStatus  // Don't retry for Off case.
                || mSendRetryCount++ >= MAX_SEND_RETRY_COUNT) {
            HdmiLogger->Debug("[T]:wait for <Set System Audio Mode>.");
            SetSystemAudioMode(FALSE);
            FinishWithCallback(IHdmiControlManager::RESULT_TIMEOUT);
            return NOERROR;
        }
        SendSystemAudioModeRequest();
#endif
}

ECode SystemAudioAction::SetSystemAudioMode(
    /* [in] */ Boolean mode)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        tv->SetSystemAudioMode(mode, TRUE);
#endif
}

ECode SystemAudioAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        switch (mState) {
            case STATE_WAIT_FOR_SET_SYSTEM_AUDIO_MODE:
            {
                Int32 opcode;
                cmd->GetOpcode(&opcode);
                if (opcode == Constants::MESSAGE_FEATURE_ABORT
                        AutoPtr<ArrayOf<Byte> > params;
                        cmd->GetParams((ArrayOf<Byte>**)&params);
                        && ((*params)[0] & 0xFF)
                                == Constants::MESSAGE_SYSTEM_AUDIO_MODE_REQUEST) {
                    HdmiLogger->Debug("Failed to start system audio mode request.");
                    SetSystemAudioMode(FALSE);
                    FinishWithCallback(IHdmiControlManager::RESULT_EXCEPTION);
                    *result = TRUE;
                    return NOERROR;
                }
                if (opcode != Constants::MESSAGE_SET_SYSTEM_AUDIO_MODE
                        || !HdmiUtils->CheckCommandSource(cmd, mAvrLogicalAddress, TAG)) {
                    *result = FALSE;
                    return NOERROR;
                }
                Boolean receivedStatus = HdmiUtils->ParseCommandParamSystemAudioStatus(cmd);
                if (receivedStatus == mTargetAudioStatus) {
                    SetSystemAudioMode(receivedStatus);
                    StartAudioStatusAction();
                    *result = TRUE;
                    return NOERROR;
                } else {
                    HdmiLogger->Debug("Unexpected system audio mode request:" + receivedStatus);
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
#endif
}

ECode SystemAudioAction::StartAudioStatusAction()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        AutoPtr<SystemAudioStatusAction> newSystemAudioStatusAction = new SystemAudioStatusAction();
        newSystemAudioStatusAction->constructor(Tv(), mAvrLogicalAddress, mCallback);
        AddAndStartAction(newSystemAudioStatusAction);
        Finish();
#endif
}

ECode SystemAudioAction::RemoveSystemAudioActionInProgress()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        RemoveActionExcept(ECLSID_CSystemAudioActionFromTv, this);
        RemoveActionExcept(ECLSID_CSystemAudioActionFromAvr, this);
#endif
}

ECode SystemAudioAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != state) {
            return NOERROR;
        }
        switch (mState) {
            case STATE_WAIT_FOR_SET_SYSTEM_AUDIO_MODE:
                HandleSendSystemAudioModeRequestTimeout();
                return NOERROR;
        }
#endif
}

ECode SystemAudioAction::FinishWithCallback(
    /* [in] */ Int32 returnCode)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mCallback != NULL) {
            try {
                mCallback->OnComplete(returnCode);
            } catch (RemoteException e) {
                Slogger::E(TAG, "Failed to invoke callback.", e);
            }
        }
        Finish();
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
