
#include "elastos/droid/server/hdmi/TimerRecordingAction.h"
#include <elastos/utility/Arrays.h>
#include "elastos/droid/server/hdmi/HdmiControlService.h"

// import static android.hardware.hdmi.IHdmiControlManager::TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION;
// import static android.hardware.hdmi.IHdmiControlManager::TIMER_RECORDING_RESULT_EXTRA_FAIL_TO_RECORD_SELECTED_SOURCE;
// import static android.hardware.hdmi.IHdmiControlManager::TIMER_RECORDING_TYPE_ANALOGUE;
// import static android.hardware.hdmi.IHdmiControlManager::TIMER_RECORDING_TYPE_DIGITAL;
// import static android.hardware.hdmi.IHdmiControlManager::TIMER_RECORDING_TYPE_EXTERNAL;

using Elastos::Droid::Utility::ISlog;

using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CAR_INTERFACE_IMPL(TimerRecordingAction, HdmiCecFeatureAction, ITimerRecordingAction)

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
        super::constructor(source);
        mRecorderAddress = recorderAddress;
        mSourceType = sourceType;
        mRecordSource = recordSource;
#endif
}

ECode TimerRecordingAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        SendTimerMessage();
        *result = TRUE;
        return NOERROR;
#endif
}

ECode TimerRecordingAction::SendTimerMessage()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        HdmiCecMessage message = NULL;
        switch (mSourceType) {
            case TIMER_RECORDING_TYPE_DIGITAL:
                Int32 srcAddr;
                GetSourceAddress(&srcAddr);
                message = HdmiCecMessageBuilder->BuildSetDigitalTimer(srcAddr,
                        mRecorderAddress, mRecordSource);
                break;
            case TIMER_RECORDING_TYPE_ANALOGUE:
                Int32 srcAddr;
                GetSourceAddress(&srcAddr);
                message = HdmiCecMessageBuilder->BuildSetAnalogueTimer(srcAddr,
                        mRecorderAddress, mRecordSource);
                break;
            case TIMER_RECORDING_TYPE_EXTERNAL:
                Int32 srcAddr;
                GetSourceAddress(&srcAddr);
                message = HdmiCecMessageBuilder->BuildSetExternalTimer(srcAddr,
                        mRecorderAddress, mRecordSource);
                break;
            default:
                AutoPtr<IHdmiCecLocalDeviceTv> tv;
                Tv((IHdmiCecLocalDeviceTv**)&tv);
                tv->AnnounceTimerRecordingResult(
                        TIMER_RECORDING_RESULT_EXTRA_FAIL_TO_RECORD_SELECTED_SOURCE);
                Finish();
                return NOERROR;
        }
        SendCommand(message, new SendMessageCallback() {
            //@Override
            CARAPI OnSendCompleted(Int32 error) {
                if (error != Constants::SEND_RESULT_SUCCESS) {
                    AutoPtr<IHdmiCecLocalDeviceTv> tv;
                    Tv((IHdmiCecLocalDeviceTv**)&tv);
                    tv->AnnounceTimerRecordingResult(
                            TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION);
                    Finish();
                    return NOERROR;
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
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != STATE_WAITING_FOR_TIMER_STATUS) {
            *result = FALSE;
            return NOERROR;
        }

        Int32 srcAddr;
        cmd->GetSource(&srcAddr);
        if (srcAddr != mRecorderAddress) {
            *result = FALSE;
            return NOERROR;
        }

        Int32 opcode;
        cmd->GetOpcode(&opcode);
        switch (opcode) {
            case Constants::MESSAGE_TIMER_STATUS:
                return HandleTimerStatus(cmd);
            case Constants::MESSAGE_FEATURE_ABORT:
                return HandleFeatureAbort(cmd);
        }
        *result = FALSE;
        return NOERROR;
#endif
}

ECode TimerRecordingAction::HandleTimerStatus(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        AutoPtr<ArrayOf<Byte> > timerStatusData;
        cmd->GetParams((ArrayOf<Byte>**)&timerStatusData);
        // [Timer Status Data] should be one or three bytes.
        if (timerStatusData->GetLength() == 1 || timerStatusData->GetLength() == 3) {
            AutoPtr<IHdmiCecLocalDeviceTv> tv;
            Tv((IHdmiCecLocalDeviceTv**)&tv);
            tv->AnnounceTimerRecordingResult(BytesToInt32(timerStatusData));
            Slogger::I(TAG, "Received [Timer Status Data]:" + Arrays->ToString(timerStatusData));
        } else {
            Slogger::W(TAG, "Invalid [Timer Status Data]:" + Arrays->ToString(timerStatusData));
        }

        // Unlike one touch record, finish timer record when <Timer Status> is received.
        Finish();
        *result = TRUE;
        return NOERROR;
#endif
}

ECode TimerRecordingAction::HandleFeatureAbort(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        AutoPtr<ArrayOf<Byte> > params;
        cmd->GetParams((ArrayOf<Byte>**)&params);
        Int32 messageType = (*params)[0] & 0xFF;
        switch (messageType) {
            case Constants::MESSAGE_SET_DIGITAL_TIMER: // fall through
            case Constants::MESSAGE_SET_ANALOG_TIMER: // fall through
            case Constants::MESSAGE_SET_EXTERNAL_TIMER: // fall through
                break;
            default:
                *result = FALSE;
                return NOERROR;
        }
        Int32 reason = (*params)[1] & 0xFF;
        Slogger::I(TAG, "[Feature Abort] for " + messageType + " reason:" + reason);
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        tv->AnnounceTimerRecordingResult(TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION);
        Finish();
        *result = TRUE;
        return NOERROR;
#endif
}

ECode TimerRecordingAction::BytesToInt32(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (data->GetLength() > 4) {
            Logger::E(TAG, "Invalid data size:" + Arrays->ToString(data));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        Int32 result = 0;
        for (Int32 i = 0; i < data->GetLength(); ++i) {
            Int32 shift = (3 - i) * 8;
            result |= (((*data)[i] & 0xFF) << shift);
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
            return NOERROR;
        }

        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        tv->AnnounceTimerRecordingResult(TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION);
        Finish();
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
