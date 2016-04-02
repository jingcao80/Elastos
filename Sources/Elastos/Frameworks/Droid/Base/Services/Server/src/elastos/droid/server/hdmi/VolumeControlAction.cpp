
#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/HdmiCecKeycode.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDeviceTv.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/HdmiLogger.h"
#include "elastos/droid/server/hdmi/VolumeControlAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <Elastos.Droid.Media.h>

using Elastos::Core::CSystem;
using Elastos::Core::ISystem;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CAR_INTERFACE_IMPL(VolumeControlAction, HdmiCecFeatureAction, IVolumeControlAction)

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
    VALIDATE_NOT_NULL(result)

    *result = (volume * MAX_VOLUME) / scale;
    return NOERROR;
}

ECode VolumeControlAction::ScaleToCustomVolume(
    /* [in] */ Int32 cecVolume,
    /* [in] */ Int32 scale,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (cecVolume * scale) / MAX_VOLUME;
    return NOERROR;
}

ECode VolumeControlAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 avrAddress,
    /* [in] */ Boolean isVolumeUp)
{
    HdmiCecFeatureAction::constructor(source);
    mAvrAddress = avrAddress;
    mIsVolumeUp = isVolumeUp;
    mLastAvrVolume = UNKNOWN_AVR_VOLUME;
    mSentKeyPressed = FALSE;

    UpdateLastKeyUpdateTime();
    return NOERROR;
}

ECode VolumeControlAction::UpdateLastKeyUpdateTime()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mLastKeyUpdateTime);
    return NOERROR;
}

ECode VolumeControlAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mState = STATE_WAIT_FOR_NEXT_VOLUME_PRESS;
    SendVolumeKeyPressed();
    ResetTimer();
    *result = TRUE;
    return NOERROR;
}

ECode VolumeControlAction::SendVolumeKeyPressed()
{
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildUserControlPressed(srcAddr, mAvrAddress,
            mIsVolumeUp ? HdmiCecKeycode::CEC_KEYCODE_VOLUME_UP
                    : HdmiCecKeycode::CEC_KEYCODE_VOLUME_DOWN, (IHdmiCecMessage**)&cmd);
    SendCommand(cmd);
    mSentKeyPressed = TRUE;
    return NOERROR;
}

ECode VolumeControlAction::ResetTimer()
{
    mActionTimer->ClearTimerMessage();
    AddTimer(STATE_WAIT_FOR_NEXT_VOLUME_PRESS, Constants::IRT_MS);
    return NOERROR;
}

ECode VolumeControlAction::HandleVolumeChange(
    /* [in] */ Boolean isVolumeUp)
{
    if (mIsVolumeUp != isVolumeUp) {
        HdmiLogger::Debug("Volume Key Status Changed[old:%s new:%s]", mIsVolumeUp ? "true" : "false", isVolumeUp ? "true" : "false");
        SendVolumeKeyReleased();
        mIsVolumeUp = isVolumeUp;
    }
    UpdateLastKeyUpdateTime();
    return NOERROR;
}

ECode VolumeControlAction::SendVolumeKeyReleased()
{
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildUserControlReleased(srcAddr, mAvrAddress, (IHdmiCecMessage**)&cmd);
    SendCommand(cmd);
    mSentKeyPressed = FALSE;
    return NOERROR;
}

ECode VolumeControlAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 srcAddr;
    cmd->GetSource(&srcAddr);
    if (mState != STATE_WAIT_FOR_NEXT_VOLUME_PRESS || srcAddr != mAvrAddress) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 opcode;
    cmd->GetOpcode(&opcode);
    if (opcode == Constants::MESSAGE_REPORT_AUDIO_STATUS) {
        *result = HandleReportAudioStatus(cmd);
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_FEATURE_ABORT) {
        *result = HandleFeatureAbort(cmd);
        return NOERROR;
    }
    else {
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

Boolean VolumeControlAction::HandleReportAudioStatus(
    /* [in] */ IHdmiCecMessage* cmd)
{
    AutoPtr<ArrayOf<Byte> > params;
    cmd->GetParams((ArrayOf<Byte>**)&params);
    Boolean mute = ((*params)[0] & 0x80) == 0x80;
    Int32 volume = (*params)[0] & 0x7F;
    mLastAvrVolume = volume;
    if (ShouldUpdateAudioVolume(mute)) {
        HdmiLogger::Debug("Force volume change[mute:%s, volume=%d]", mute ? "true" : "false", volume);
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        ((HdmiCecLocalDeviceTv*) tv.Get())->SetAudioStatus(mute, volume);
    }
    return TRUE;
}

Boolean VolumeControlAction::ShouldUpdateAudioVolume(
    /* [in] */ Boolean mute)
{
    // Do nothing if in mute.
    if (mute) {
        return TRUE;
    }

    // Update audio status if current volume position is edge of volume bar,
    // i.e max or min volume.
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    AutoPtr<IHdmiControlService> service;
    ((HdmiCecLocalDeviceTv*) tv.Get())->GetService((IHdmiControlService**)&service);
    AutoPtr<IAudioManager> audioManager;
    ((HdmiControlService*) service.Get())->GetAudioManager((IAudioManager**)&audioManager);
    Int32 currentVolume;
    audioManager->GetStreamVolume(IAudioManager::STREAM_MUSIC, &currentVolume);
    if (mIsVolumeUp) {
        Int32 maxVolume;
        audioManager->GetStreamMaxVolume(IAudioManager::STREAM_MUSIC, &maxVolume);
        return currentVolume == maxVolume;
    } else {
        return currentVolume == 0;
    }
    return FALSE;
}

Boolean VolumeControlAction::HandleFeatureAbort(
    /* [in] */ IHdmiCecMessage* cmd)
{
    AutoPtr<ArrayOf<Byte> > originalOpcodes;
    cmd->GetParams((ArrayOf<Byte>**)&originalOpcodes);
    Int32 originalOpcode = (*originalOpcodes)[0] & 0xFF;
    // Since it sends <User Control Released> only when it finishes this action,
    // it takes care of <User Control Pressed> only here.
    if (originalOpcode == Constants::MESSAGE_USER_CONTROL_PRESSED) {
        Finish();
        return TRUE;
    }
    return FALSE;
}

ECode VolumeControlAction::Clear()
{
    HdmiCecFeatureAction::Clear();
    if (mSentKeyPressed) {
        SendVolumeKeyReleased();
    }
    if (mLastAvrVolume != UNKNOWN_AVR_VOLUME) {
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        ((HdmiCecLocalDeviceTv*) tv.Get())->SetAudioStatus(FALSE, mLastAvrVolume);
        mLastAvrVolume = UNKNOWN_AVR_VOLUME;
    }
    return NOERROR;
}

ECode VolumeControlAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    if (state != STATE_WAIT_FOR_NEXT_VOLUME_PRESS) {
        return NOERROR;
    }

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 curTimeMillis;
    system->GetCurrentTimeMillis(&curTimeMillis);
    if (curTimeMillis - mLastKeyUpdateTime >= Constants::IRT_MS) {
        Finish();
    } else {
        SendVolumeKeyPressed();
        ResetTimer();
    }
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
