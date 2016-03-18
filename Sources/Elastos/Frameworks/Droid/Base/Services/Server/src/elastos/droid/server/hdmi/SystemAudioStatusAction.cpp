
#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/HdmiCecKeycode.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDeviceTv.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiConfig.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/SystemAudioStatusAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Utility::ISlog;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// SystemAudioStatusAction::InnerSub_SendMessageCallback
//=============================================================================
CAR_INTERFACE_IMPL(SystemAudioStatusAction::InnerSub_SendMessageCallback, Object, IHdmiControlServiceSendMessageCallback)

SystemAudioStatusAction::InnerSub_SendMessageCallback::InnerSub_SendMessageCallback(
    /* [in] */ SystemAudioStatusAction* host)
    : mHost(host)
{}

ECode SystemAudioStatusAction::InnerSub_SendMessageCallback::OnSendCompleted(Int32 error) {
    if (error != Constants::SEND_RESULT_SUCCESS) {
        mHost->HandleSendGiveAudioStatusFailure();
    }
    return NOERROR;
}

//=============================================================================
// SystemAudioStatusAction
//=============================================================================
CAR_INTERFACE_IMPL(SystemAudioStatusAction, HdmiCecFeatureAction, ISystemAudioStatusAction)

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
    HdmiCecFeatureAction::constructor(source);
    mAvrAddress = avrAddress;
    mCallback = callback;
    return NOERROR;
}

ECode SystemAudioStatusAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mState = STATE_WAIT_FOR_REPORT_AUDIO_STATUS;
    AddTimer(mState, HdmiConfig::TIMEOUT_MS);
    SendGiveAudioStatus();
    *result = TRUE;
    return NOERROR;
}

ECode SystemAudioStatusAction::SendGiveAudioStatus()
{
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildGiveAudioStatus(srcAddr, mAvrAddress, (IHdmiCecMessage**)&cmd);
    SendCommand(cmd, new InnerSub_SendMessageCallback(this));
    return NOERROR;
}

ECode SystemAudioStatusAction::HandleSendGiveAudioStatusFailure()
{
    // Inform to all application that the audio status (volumn, mute) of
    // the audio amplifier is unknown.
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    ((HdmiCecLocalDeviceTv*) tv.Get())->SetAudioStatus(FALSE, Constants::UNKNOWN_VOLUME);

    Boolean isSystemAudioActivated;
    ((HdmiCecLocalDeviceTv*) tv.Get())->IsSystemAudioActivated(&isSystemAudioActivated);
    Int32 uiCommand = isSystemAudioActivated
            ? HdmiCecKeycode::CEC_KEYCODE_RESTORE_VOLUME_FUNCTION  // SystemAudioMode: ON
            : HdmiCecKeycode::CEC_KEYCODE_MUTE_FUNCTION;           // SystemAudioMode: OFF
    SendUserControlPressedAndReleased(mAvrAddress, uiCommand);

    // Still return SUCCESS to callback.
    FinishWithCallback(IHdmiControlManager::RESULT_SUCCESS);
    return NOERROR;
}

ECode SystemAudioStatusAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mState != STATE_WAIT_FOR_REPORT_AUDIO_STATUS) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 opcode;
    cmd->GetOpcode(&opcode);
    if (opcode == Constants::MESSAGE_REPORT_AUDIO_STATUS) {
        HandleReportAudioStatus(cmd);
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode SystemAudioStatusAction::HandleReportAudioStatus(
    /* [in] */ IHdmiCecMessage* cmd)
{
    AutoPtr<ArrayOf<Byte> > params;
    cmd->GetParams((ArrayOf<Byte>**)&params);
    Boolean mute = ((*params)[0] & 0x80) == 0x80;
    Int32 volume = (*params)[0] & 0x7F;
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    ((HdmiCecLocalDeviceTv*) tv.Get())->SetAudioStatus(mute, volume);

    Boolean isSystemAudioActivated;
    ((HdmiCecLocalDeviceTv*) tv.Get())->IsSystemAudioActivated(&isSystemAudioActivated);
    if (!(isSystemAudioActivated ^ mute)) {
        // Toggle AVR's mute status to match with the system audio status.
        SendUserControlPressedAndReleased(mAvrAddress, HdmiCecKeycode::CEC_KEYCODE_MUTE);
    }
    FinishWithCallback(IHdmiControlManager::RESULT_SUCCESS);
    return NOERROR;
}

ECode SystemAudioStatusAction::FinishWithCallback(
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

ECode SystemAudioStatusAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    if (mState != state) {
        return NOERROR;
    }

    HandleSendGiveAudioStatusFailure();
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
