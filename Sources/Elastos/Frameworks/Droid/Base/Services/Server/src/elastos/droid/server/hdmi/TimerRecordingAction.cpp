
#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDeviceTv.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/TimerRecordingAction.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Utility::ISlog;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// TimerRecordingAction::InnerSub_SendMessageCallback
//=============================================================================
CAR_INTERFACE_IMPL(TimerRecordingAction::InnerSub_SendMessageCallback, Object, IHdmiControlServiceSendMessageCallback)

TimerRecordingAction::InnerSub_SendMessageCallback::InnerSub_SendMessageCallback(
    /* [in] */ TimerRecordingAction* host)
    : mHost(host)
{}

ECode TimerRecordingAction::InnerSub_SendMessageCallback::OnSendCompleted(
    /* [in] */ Int32 error)
{
    if (error != Constants::SEND_RESULT_SUCCESS) {
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
        ((HdmiCecLocalDeviceTv*) tv.Get())->AnnounceTimerRecordingResult(
                IHdmiControlManager::TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION);
        mHost->Finish();
        return NOERROR;
    }
    mHost->mState = STATE_WAITING_FOR_TIMER_STATUS;
    mHost->AddTimer(mHost->mState, TIMER_STATUS_TIMEOUT_MS);
    return NOERROR;
}

//=============================================================================
// TimerRecordingAction
//=============================================================================
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
    HdmiCecFeatureAction::constructor(source);
    mRecorderAddress = recorderAddress;
    mSourceType = sourceType;
    mRecordSource = recordSource;
    return NOERROR;
}

ECode TimerRecordingAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    SendTimerMessage();
    *result = TRUE;
    return NOERROR;
}

ECode TimerRecordingAction::SendTimerMessage()
{
    AutoPtr<IHdmiCecMessage> message;
    switch (mSourceType) {
        case IHdmiControlManager::TIMER_RECORDING_TYPE_DIGITAL:
            {
                Int32 srcAddr;
                GetSourceAddress(&srcAddr);
                HdmiCecMessageBuilder::BuildSetDigitalTimer(srcAddr,
                        mRecorderAddress, mRecordSource, (IHdmiCecMessage**)&message);
            }
            break;
        case IHdmiControlManager::TIMER_RECORDING_TYPE_ANALOGUE:
            {
                Int32 srcAddr;
                GetSourceAddress(&srcAddr);
                HdmiCecMessageBuilder::BuildSetAnalogueTimer(srcAddr,
                        mRecorderAddress, mRecordSource, (IHdmiCecMessage**)&message);
            }
            break;
        case IHdmiControlManager::TIMER_RECORDING_TYPE_EXTERNAL:
            {
                Int32 srcAddr;
                GetSourceAddress(&srcAddr);
                HdmiCecMessageBuilder::BuildSetExternalTimer(srcAddr,
                        mRecorderAddress, mRecordSource, (IHdmiCecMessage**)&message);
            }
            break;
        default:
            {
                AutoPtr<IHdmiCecLocalDeviceTv> tv;
                Tv((IHdmiCecLocalDeviceTv**)&tv);
                ((HdmiCecLocalDeviceTv*) tv.Get())->AnnounceTimerRecordingResult(
                        IHdmiControlManager::TIMER_RECORDING_RESULT_EXTRA_FAIL_TO_RECORD_SELECTED_SOURCE);
                Finish();
            }
            return NOERROR;
    }
    SendCommand(message, new InnerSub_SendMessageCallback(this));
    return NOERROR;
}

ECode TimerRecordingAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

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
    if (opcode == Constants::MESSAGE_TIMER_STATUS) {
        *result = HandleTimerStatus(cmd);
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_FEATURE_ABORT) {
        *result = HandleFeatureAbort(cmd);
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

Boolean TimerRecordingAction::HandleTimerStatus(
    /* [in] */ IHdmiCecMessage* cmd)
{
    AutoPtr<ArrayOf<Byte> > timerStatusData;
    cmd->GetParams((ArrayOf<Byte>**)&timerStatusData);
    // [Timer Status Data] should be one or three bytes.
    if (timerStatusData->GetLength() == 1 || timerStatusData->GetLength() == 3) {
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        ((HdmiCecLocalDeviceTv*) tv.Get())->AnnounceTimerRecordingResult(BytesToInt32(timerStatusData));
        Slogger::I(TAG, "Received [Timer Status Data]:%s", Arrays::ToString(timerStatusData).string());
    } else {
        Slogger::W(TAG, "Invalid [Timer Status Data]:%s", Arrays::ToString(timerStatusData).string());
    }

    // Unlike one touch record, finish timer record when <Timer Status> is received.
    Finish();
    return TRUE;
}

Boolean TimerRecordingAction::HandleFeatureAbort(
    /* [in] */ IHdmiCecMessage* cmd)
{
    AutoPtr<ArrayOf<Byte> > params;
    cmd->GetParams((ArrayOf<Byte>**)&params);
    Int32 messageType = (*params)[0] & 0xFF;
    if (messageType == Constants::MESSAGE_SET_DIGITAL_TIMER             // fall through
            || messageType == Constants::MESSAGE_SET_ANALOG_TIMER       // fall through
            || messageType == Constants::MESSAGE_SET_EXTERNAL_TIMER) {  // fall through
    }
    else {
        return FALSE;
    }
    Int32 reason = (*params)[1] & 0xFF;
    Slogger::I(TAG, "[Feature Abort] for %d reason:%d", messageType, reason);
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    ((HdmiCecLocalDeviceTv*) tv.Get())->AnnounceTimerRecordingResult(IHdmiControlManager::TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION);
    Finish();
    return TRUE;
}

Int32 TimerRecordingAction::BytesToInt32(
    /* [in] */ ArrayOf<Byte>* data)
{
    if (data->GetLength() > 4) {
        Logger::E(TAG, "Invalid data size:%s", Arrays::ToString(data).string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 result = 0;
    for (Int32 i = 0; i < data->GetLength(); ++i) {
        Int32 shift = (3 - i) * 8;
        result |= (((*data)[i] & 0xFF) << shift);
    }
    return result;
}

ECode TimerRecordingAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    if (mState != state) {
        Slogger::W(TAG, "Timeout in invalid state:[Expected:%d, Actual:%d]", mState, state);
        return NOERROR;
    }

    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    ((HdmiCecLocalDeviceTv*) tv.Get())->AnnounceTimerRecordingResult(IHdmiControlManager::TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION);
    Finish();
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
