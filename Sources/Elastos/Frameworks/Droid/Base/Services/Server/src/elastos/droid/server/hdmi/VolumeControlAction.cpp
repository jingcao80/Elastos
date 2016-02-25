
#include "elastos/droid/server/hdmi/VolumeControlAction.h"
#include <Elastos.CoreLibrary.Utility.h>

// import static com.android.server.hdmi.Constants.IRT_MS;
// import static com.android.server.hdmi.Constants.MESSAGE_FEATURE_ABORT;
// import static com.android.server.hdmi.Constants.MESSAGE_REPORT_AUDIO_STATUS;
// import static com.android.server.hdmi.Constants.MESSAGE_USER_CONTROL_PRESSED;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String VolumeControlAction::TAG("VolumeControlAction");
const Int32 VolumeControlAction::STATE_WAIT_FOR_NEXT_VOLUME_PRESS = 1;
const Int32 VolumeControlAction::MAX_VOLUME = 100;
const Int32 VolumeControlAction::UNKNOWN_AVR_VOLUME = -1;

VolumeControlAction::VolumeControlAction()
    : mAvrAddress(0)
    , mIsVolumeUp(FALSE)
    , mLastKeyUpdateTime(0)
    , mLastAvrVolume(0)
    , mSentKeyPressed(FALSE)
{}

ECode VolumeControlAction::ScaleToCecVolume(
    /* [in] */ Int32 volume,
    /* [in] */ Int32 scale,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return (volume * MAX_VOLUME) / scale;
#endif
}

ECode VolumeControlAction::ScaleToCustomVolume(
    /* [in] */ Int32 cecVolume,
    /* [in] */ Int32 scale,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return (cecVolume * scale) / MAX_VOLUME;

#endif
}

ECode VolumeControlAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 avrAddress,
    /* [in] */ Boolean isVolumeUp)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Super(source);
        mAvrAddress = avrAddress;
        mIsVolumeUp = isVolumeUp;
        mLastAvrVolume = UNKNOWN_AVR_VOLUME;
        mSentKeyPressed = FALSE;

        UpdateLastKeyUpdateTime();
#endif
}

ECode VolumeControlAction::UpdateLastKeyUpdateTime()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mLastKeyUpdateTime = System->CurrentTimeMillis();
#endif
}

ECode VolumeControlAction::Start(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mState = STATE_WAIT_FOR_NEXT_VOLUME_PRESS;
        SendVolumeKeyPressed();
        ResetTimer();
        return TRUE;
#endif
}

ECode VolumeControlAction::SendVolumeKeyPressed()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        SendCommand(HdmiCecMessageBuilder->BuildUserControlPressed(GetSourceAddress(), mAvrAddress,
                mIsVolumeUp ? HdmiCecKeycode.CEC_KEYCODE_VOLUME_UP
                        : HdmiCecKeycode.CEC_KEYCODE_VOLUME_DOWN));
        mSentKeyPressed = TRUE;
#endif
}

ECode VolumeControlAction::ResetTimer()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mActionTimer->ClearTimerMessage();
        AddTimer(STATE_WAIT_FOR_NEXT_VOLUME_PRESS, IRT_MS);
#endif
}

ECode VolumeControlAction::HandleVolumeChange(
    /* [in] */ Boolean isVolumeUp)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mIsVolumeUp != isVolumeUp) {
            HdmiLogger->Debug("Volume Key Status Changed[old:%b new:%b]", mIsVolumeUp, isVolumeUp);
            SendVolumeKeyReleased();
            mIsVolumeUp = isVolumeUp;
        }
        UpdateLastKeyUpdateTime();
#endif
}

ECode VolumeControlAction::SendVolumeKeyReleased()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        SendCommand(HdmiCecMessageBuilder->BuildUserControlReleased(
                GetSourceAddress(), mAvrAddress));
        mSentKeyPressed = FALSE;
#endif
}

ECode VolumeControlAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != STATE_WAIT_FOR_NEXT_VOLUME_PRESS || cmd->GetSource() != mAvrAddress) {
            return FALSE;
        }

        switch (cmd->GetOpcode()) {
            case MESSAGE_REPORT_AUDIO_STATUS:
                return HandleReportAudioStatus(cmd);
            case MESSAGE_FEATURE_ABORT:
                return HandleFeatureAbort(cmd);
            default:
                return FALSE;
        }
#endif
}

ECode VolumeControlAction::HandleReportAudioStatus(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Byte params[] = cmd->GetParams();
        Boolean mute = (params[0] & 0x80) == 0x80;
        Int32 volume = params[0] & 0x7F;
        mLastAvrVolume = volume;
        if (ShouldUpdateAudioVolume(mute)) {
            HdmiLogger->Debug("Force volume change[mute:%b, volume=%d]", mute, volume);
            Tv()->SetAudioStatus(mute, volume);
        }
        return TRUE;
#endif
}

ECode VolumeControlAction::ShouldUpdateAudioVolume(
    /* [in] */ Boolean mute,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Do nothing if in mute.
        if (mute) {
            return TRUE;
        }

        // Update audio status if current volume position is edge of volume bar,
        // i.e max or min volume.
        AudioManager audioManager = Tv()->GetService().GetAudioManager();
        Int32 currentVolume = audioManager->GetStreamVolume(AudioManager.STREAM_MUSIC);
        if (mIsVolumeUp) {
            Int32 maxVolume = audioManager->GetStreamMaxVolume(AudioManager.STREAM_MUSIC);
            return currentVolume == maxVolume;
        } else {
            return currentVolume == 0;
        }
#endif
}

ECode VolumeControlAction::HandleFeatureAbort(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Int32 originalOpcode = cmd->GetParams()[0] & 0xFF;
        // Since it sends <User Control Released> only when it finishes this action,
        // it takes care of <User Control Pressed> only here.
        if (originalOpcode == MESSAGE_USER_CONTROL_PRESSED) {
            Finish();
            return TRUE;
        }
        return FALSE;
#endif
}

ECode VolumeControlAction::Clear()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super->Clear();
        if (mSentKeyPressed) {
            SendVolumeKeyReleased();
        }
        if (mLastAvrVolume != UNKNOWN_AVR_VOLUME) {
            Tv()->SetAudioStatus(FALSE, mLastAvrVolume);
            mLastAvrVolume = UNKNOWN_AVR_VOLUME;
        }
#endif
}

ECode VolumeControlAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (state != STATE_WAIT_FOR_NEXT_VOLUME_PRESS) {
            return;
        }

        if (System->CurrentTimeMillis() - mLastKeyUpdateTime >= IRT_MS) {
            Finish();
        } else {
            SendVolumeKeyPressed();
            ResetTimer();
        }
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
