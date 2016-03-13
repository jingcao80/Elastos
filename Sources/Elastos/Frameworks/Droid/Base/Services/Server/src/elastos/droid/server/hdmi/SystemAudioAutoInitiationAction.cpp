
#include "elastos/droid/server/hdmi/SystemAudioAutoInitiationAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/server/hdmi/HdmiControlService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CAR_INTERFACE_IMPL(SystemAudioAutoInitiationAction, HdmiCecFeatureAction, ISystemAudioAutoInitiationAction)

const Int32 SystemAudioAutoInitiationAction::STATE_WAITING_FOR_SYSTEM_AUDIO_MODE_STATUS = 1;

SystemAudioAutoInitiationAction::SystemAudioAutoInitiationAction()
    : mAvrAddress(0)
{}

ECode SystemAudioAutoInitiationAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 avrAddress)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super::constructor(source);
        mAvrAddress = avrAddress;
#endif
}

ECode SystemAudioAutoInitiationAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mState = STATE_WAITING_FOR_SYSTEM_AUDIO_MODE_STATUS;

        AddTimer(mState, HdmiConfig::TIMEOUT_MS);
        SendGiveSystemAudioModeStatus();
        *result = TRUE;
        return NOERROR;
#endif
}

ECode SystemAudioAutoInitiationAction::SendGiveSystemAudioModeStatus()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Int32 srcAddr;
        GetSourceAddress(&srcAddr);
        SendCommand(HdmiCecMessageBuilder->BuildGiveSystemAudioModeStatus(srcAddr,
                mAvrAddress), new SendMessageCallback() {
            //@Override
            CARAPI OnSendCompleted(Int32 error) {
                if (error != Constants::SEND_RESULT_SUCCESS) {
                    AutoPtr<IHdmiCecLocalDeviceTv> tv;
                    Tv((IHdmiCecLocalDeviceTv**)&tv);
                    tv->SetSystemAudioMode(FALSE, TRUE);
                    Finish();
                }
            }
        });
#endif
}

ECode SystemAudioAutoInitiationAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != STATE_WAITING_FOR_SYSTEM_AUDIO_MODE_STATUS) {
            *result = FALSE;
            return NOERROR;
        }

        Int32 opcode;
        cmd->GetOpcode(&opcode);
        switch (opcode) {
            case Constants::MESSAGE_SYSTEM_AUDIO_MODE_STATUS:
                HandleSystemAudioModeStatusMessage();
                *result = TRUE;
                return NOERROR;
            default:
                *result = FALSE;
                return NOERROR;
        }
#endif
}

ECode SystemAudioAutoInitiationAction::HandleSystemAudioModeStatusMessage()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // If the last setting is system audio, turn on system audio whatever AVR status is.
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        if (tv->GetSystemAudioModeSetting()) {
            if (CanChangeSystemAudio()) {
                AutoPtr<SystemAudioActionFromTv> newSystemAudioActionFromTv = new SystemAudioActionFromTv();
                newSystemAudioActionFromTv->constructor(Tv(), mAvrAddress, TRUE, NULL);
                AddAndStartAction(newSystemAudioActionFromTv);
            }
        } else {
            // If the last setting is non-system audio, turn off system audio mode
            // and update system audio status (volume or mute).
            AutoPtr<IHdmiCecLocalDeviceTv> tv;
            Tv((IHdmiCecLocalDeviceTv**)&tv);
            tv->SetSystemAudioMode(FALSE, TRUE);
            if (CanChangeSystemAudio()) {
                AutoPtr<SystemAudioStatusAction> newSystemAudioStatusAction = new SystemAudioStatusAction();
                newSystemAudioStatusAction->constructor(Tv(), mAvrAddress, NULL);
                AddAndStartAction(newSystemAudioStatusAction);
            }
        }
        Finish();
#endif
}

ECode SystemAudioAutoInitiationAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != state) {
            return NOERROR;
        }

        switch (mState) {
            case STATE_WAITING_FOR_SYSTEM_AUDIO_MODE_STATUS:
                HandleSystemAudioModeStatusTimeout();
                break;
        }
#endif
}

ECode SystemAudioAutoInitiationAction::HandleSystemAudioModeStatusTimeout()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        if (tv->GetSystemAudioModeSetting()) {
            if (CanChangeSystemAudio()) {
                AutoPtr<SystemAudioActionFromTv> newSystemAudioActionFromTv = new SystemAudioActionFromTv();
                newSystemAudioActionFromTv->constructor(Tv(), mAvrAddress, TRUE, NULL);
                AddAndStartAction(newSystemAudioActionFromTv);
            }
        } else {
            AutoPtr<IHdmiCecLocalDeviceTv> tv;
            Tv((IHdmiCecLocalDeviceTv**)&tv);
            tv->SetSystemAudioMode(FALSE, TRUE);
        }
        Finish();
#endif
}

ECode SystemAudioAutoInitiationAction::CanChangeSystemAudio(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        Boolean hasAction;
        HasAction(ECLSID_CSystemAudioActionFromTv, &hasAction);
        Boolean hasAvrAction;
        HasAction(ECLSID_CSystemAudioActionFromAvr, &hasAvrAction);
        return !(tv->hasAction || tv->hasAvrAction);
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
