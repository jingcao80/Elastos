
#include "elastos/droid/server/hdmi/SystemAudioStatusAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Hardware.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Utility::ISlog;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String SystemAudioStatusAction::TAG("SystemAudioStatusAction");
const Int32 SystemAudioStatusAction::STATE_WAIT_FOR_REPORT_AUDIO_STATUS = 1;

SystemAudioStatusAction::SystemAudioStatusAction()
    : mAvrAddress(0)
{}

ECode SystemAudioStatusAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 avrAddress,
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Super(source);
        mAvrAddress = avrAddress;
        mCallback = callback;
#endif
}

ECode SystemAudioStatusAction::Start(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mState = STATE_WAIT_FOR_REPORT_AUDIO_STATUS;
        AddTimer(mState, HdmiConfig.TIMEOUT_MS);
        SendGiveAudioStatus();
        return TRUE;
#endif
}

ECode SystemAudioStatusAction::SendGiveAudioStatus()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        SendCommand(HdmiCecMessageBuilder->BuildGiveAudioStatus(GetSourceAddress(), mAvrAddress),
                new SendMessageCallback() {
            //@Override
            CARAPI OnSendCompleted(Int32 error) {
                if (error != Constants::SEND_RESULT_SUCCESS) {
                    HandleSendGiveAudioStatusFailure();
                }
            }
        });
#endif
}

ECode SystemAudioStatusAction::HandleSendGiveAudioStatusFailure()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Inform to all application that the audio status (volumn, mute) of
        // the audio amplifier is unknown.
        Tv()->SetAudioStatus(FALSE, Constants::UNKNOWN_VOLUME);

        Int32 uiCommand = Tv()->IsSystemAudioActivated()
                ? HdmiCecKeycode.CEC_KEYCODE_RESTORE_VOLUME_FUNCTION  // SystemAudioMode: ON
                : HdmiCecKeycode.CEC_KEYCODE_MUTE_FUNCTION;           // SystemAudioMode: OFF
        SendUserControlPressedAndReleased(mAvrAddress, uiCommand);

        // Still return SUCCESS to callback.
        FinishWithCallback(HdmiControlManager.RESULT_SUCCESS);
#endif
}

ECode SystemAudioStatusAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != STATE_WAIT_FOR_REPORT_AUDIO_STATUS) {
            return FALSE;
        }

        switch (cmd->GetOpcode()) {
            case Constants::MESSAGE_REPORT_AUDIO_STATUS:
                HandleReportAudioStatus(cmd);
                return TRUE;
        }

        return FALSE;
#endif
}

ECode SystemAudioStatusAction::HandleReportAudioStatus(
    /* [in] */ IHdmiCecMessage* cmd)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Byte[] params = cmd->GetParams();
        Boolean mute = (params[0] & 0x80) == 0x80;
        Int32 volume = params[0] & 0x7F;
        Tv()->SetAudioStatus(mute, volume);

        if (!(Tv()->IsSystemAudioActivated() ^ mute)) {
            // Toggle AVR's mute status to match with the system audio status.
            SendUserControlPressedAndReleased(mAvrAddress, HdmiCecKeycode.CEC_KEYCODE_MUTE);
        }
        FinishWithCallback(HdmiControlManager.RESULT_SUCCESS);
#endif
}

ECode SystemAudioStatusAction::FinishWithCallback(
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

ECode SystemAudioStatusAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != state) {
            return;
        }

        HandleSendGiveAudioStatusFailure();
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
