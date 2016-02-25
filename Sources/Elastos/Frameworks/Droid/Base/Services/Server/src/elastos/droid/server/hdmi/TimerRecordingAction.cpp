
#include "elastos/droid/server/hdmi/TimerRecordingAction.h"
#include <elastos/utility/Arrays.h>

// import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION;
// import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_RESULT_EXTRA_FAIL_TO_RECORD_SELECTED_SOURCE;
// import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_TYPE_ANALOGUE;
// import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_TYPE_DIGITAL;
// import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_TYPE_EXTERNAL;

using Elastos::Droid::Utility::ISlog;

using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String TimerRecordingAction::TAG("TimerRecordingAction");
const Int32 TimerRecordingAction::TIMER_STATUS_TIMEOUT_MS = 120000;
const Int32 TimerRecordingAction::STATE_WAITING_FOR_TIMER_STATUS = 1;

TimerRecordingAction::TimerRecordingAction()
    : mRecorderAddress(0)
    , mSourceType(0)
{}

ECode TimerRecordingAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 recorderAddress,
    /* [in] */ Int32 sourceType,
    /* [in] */ ArrayOf<Byte>* recordSource)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Super(source);
        mRecorderAddress = recorderAddress;
        mSourceType = sourceType;
        mRecordSource = recordSource;
#endif
}

ECode TimerRecordingAction::Start(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        SendTimerMessage();
        return TRUE;
#endif
}

ECode TimerRecordingAction::SendTimerMessage()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        HdmiCecMessage message = NULL;
        switch (mSourceType) {
            case TIMER_RECORDING_TYPE_DIGITAL:
                message = HdmiCecMessageBuilder->BuildSetDigitalTimer(GetSourceAddress(),
                        mRecorderAddress, mRecordSource);
                break;
            case TIMER_RECORDING_TYPE_ANALOGUE:
                message = HdmiCecMessageBuilder->BuildSetAnalogueTimer(GetSourceAddress(),
                        mRecorderAddress, mRecordSource);
                break;
            case TIMER_RECORDING_TYPE_EXTERNAL:
                message = HdmiCecMessageBuilder->BuildSetExternalTimer(GetSourceAddress(),
                        mRecorderAddress, mRecordSource);
                break;
            default:
                Tv()->AnnounceTimerRecordingResult(
                        TIMER_RECORDING_RESULT_EXTRA_FAIL_TO_RECORD_SELECTED_SOURCE);
                Finish();
                return;
        }
        SendCommand(message, new SendMessageCallback() {
            //@Override
            CARAPI OnSendCompleted(Int32 error) {
                if (error != Constants::SEND_RESULT_SUCCESS) {
                    Tv()->AnnounceTimerRecordingResult(
                            TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION);
                    Finish();
                    return;
                }
                mState = STATE_WAITING_FOR_TIMER_STATUS;
                AddTimer(mState, TIMER_STATUS_TIMEOUT_MS);
            }
        });
#endif
}

ECode TimerRecordingAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != STATE_WAITING_FOR_TIMER_STATUS) {
            return FALSE;
        }

        if (cmd->GetSource() != mRecorderAddress) {
            return FALSE;
        }

        switch (cmd->GetOpcode()) {
            case Constants::MESSAGE_TIMER_STATUS:
                return HandleTimerStatus(cmd);
            case Constants::MESSAGE_FEATURE_ABORT:
                return HandleFeatureAbort(cmd);
        }
        return FALSE;
#endif
}

ECode TimerRecordingAction::HandleTimerStatus(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Byte[] timerStatusData = cmd->GetParams();
        // [Timer Status Data] should be one or three bytes.
        if (timerStatusData.length == 1 || timerStatusData.length == 3) {
            Tv()->AnnounceTimerRecordingResult(BytesToInt(timerStatusData));
            Slogger::I(TAG, "Received [Timer Status Data]:" + Arrays->ToString(timerStatusData));
        } else {
            Slogger::W(TAG, "Invalid [Timer Status Data]:" + Arrays->ToString(timerStatusData));
        }

        // Unlike one touch record, finish timer record when <Timer Status> is received.
        Finish();
        return TRUE;
#endif
}

ECode TimerRecordingAction::HandleFeatureAbort(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Byte[] params = cmd->GetParams();
        Int32 messageType = params[0] & 0xFF;
        switch (messageType) {
            case Constants::MESSAGE_SET_DIGITAL_TIMER: // fall through
            case Constants::MESSAGE_SET_ANALOG_TIMER: // fall through
            case Constants::MESSAGE_SET_EXTERNAL_TIMER: // fall through
                break;
            default:
                return FALSE;
        }
        Int32 reason = params[1] & 0xFF;
        Slogger::I(TAG, "[Feature Abort] for " + messageType + " reason:" + reason);
        Tv()->AnnounceTimerRecordingResult(TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION);
        Finish();
        return TRUE;
#endif
}

ECode TimerRecordingAction::BytesToInt(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (data.length > 4) {
            throw new IllegalArgumentException("Invalid data size:" + Arrays->ToString(data));
        }
        Int32 result = 0;
        for (Int32 i = 0; i < data.length; ++i) {
            Int32 shift = (3 - i) * 8;
            result |= ((data[i] & 0xFF) << shift);
        }
        return result;
#endif
}

ECode TimerRecordingAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != state) {
            Slogger::W(TAG, "Timeout in invalid state:[Expected:" + mState + ", Actual:" + state + "]");
            return;
        }

        Tv()->AnnounceTimerRecordingResult(TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION);
        Finish();
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
