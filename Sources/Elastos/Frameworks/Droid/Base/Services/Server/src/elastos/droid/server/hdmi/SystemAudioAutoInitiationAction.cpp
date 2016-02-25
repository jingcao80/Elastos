
#include "elastos/droid/server/hdmi/SystemAudioAutoInitiationAction.h"
#include <Elastos.CoreLibrary.Utility.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

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
        Super(source);
        mAvrAddress = avrAddress;
#endif
}

ECode SystemAudioAutoInitiationAction::Start(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mState = STATE_WAITING_FOR_SYSTEM_AUDIO_MODE_STATUS;

        AddTimer(mState, HdmiConfig.TIMEOUT_MS);
        SendGiveSystemAudioModeStatus();
        return TRUE;
#endif
}

ECode SystemAudioAutoInitiationAction::SendGiveSystemAudioModeStatus()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        SendCommand(HdmiCecMessageBuilder->BuildGiveSystemAudioModeStatus(GetSourceAddress(),
                mAvrAddress), new SendMessageCallback() {
            //@Override
            CARAPI OnSendCompleted(Int32 error) {
                if (error != Constants::SEND_RESULT_SUCCESS) {
                    Tv()->SetSystemAudioMode(FALSE, TRUE);
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
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != STATE_WAITING_FOR_SYSTEM_AUDIO_MODE_STATUS) {
            return FALSE;
        }

        switch (cmd->GetOpcode()) {
            case Constants::MESSAGE_SYSTEM_AUDIO_MODE_STATUS:
                HandleSystemAudioModeStatusMessage();
                return TRUE;
            default:
                return FALSE;
        }
#endif
}

ECode SystemAudioAutoInitiationAction::HandleSystemAudioModeStatusMessage()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // If the last setting is system audio, turn on system audio whatever AVR status is.
        if (Tv()->GetSystemAudioModeSetting()) {
            if (CanChangeSystemAudio()) {
                AddAndStartAction(new SystemAudioActionFromTv(Tv(), mAvrAddress, TRUE, NULL));
            }
        } else {
            // If the last setting is non-system audio, turn off system audio mode
            // and update system audio status (volume or mute).
            Tv()->SetSystemAudioMode(FALSE, TRUE);
            if (CanChangeSystemAudio()) {
                AddAndStartAction(new SystemAudioStatusAction(Tv(), mAvrAddress, NULL));
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
            return;
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
        if (Tv()->GetSystemAudioModeSetting()) {
            if (CanChangeSystemAudio()) {
                AddAndStartAction(new SystemAudioActionFromTv(Tv(), mAvrAddress, TRUE, NULL));
            }
        } else {
            Tv()->SetSystemAudioMode(FALSE, TRUE);
        }
        Finish();
#endif
}

ECode SystemAudioAutoInitiationAction::CanChangeSystemAudio(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return !(Tv()->HasAction(SystemAudioActionFromTv.class)
               || Tv()->HasAction(SystemAudioActionFromAvr.class));
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
