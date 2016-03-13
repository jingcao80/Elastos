
#include "elastos/droid/server/hdmi/OneTouchRecordAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/server/hdmi/HdmiControlService.h"

// import static android.hardware.hdmi.IHdmiControlManager::ONE_TOUCH_RECORD_CHECK_RECORDER_CONNECTION;
// import static android.hardware.hdmi.IHdmiControlManager::ONE_TOUCH_RECORD_RECORDING_ANALOGUE_SERVICE;
// import static android.hardware.hdmi.IHdmiControlManager::ONE_TOUCH_RECORD_RECORDING_CURRENTLY_SELECTED_SOURCE;
// import static android.hardware.hdmi.IHdmiControlManager::ONE_TOUCH_RECORD_RECORDING_DIGITAL_SERVICE;
// import static android.hardware.hdmi.IHdmiControlManager::ONE_TOUCH_RECORD_RECORDING_EXTERNAL_INPUT;

using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::Server::Hdmi::IHdmiControlServiceSendMessageCallback;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String OneTouchRecordAction::TAG("OneTouchRecordAction");
const Int32 OneTouchRecordAction::RECORD_STATUS_TIMEOUT_MS = 120000;
const Int32 OneTouchRecordAction::STATE_WAITING_FOR_RECORD_STATUS = 1;
const Int32 OneTouchRecordAction::STATE_RECORDING_IN_PROGRESS = 2;

OneTouchRecordAction::OneTouchRecordAction()
    : mRecorderAddress(0)
{}

ECode OneTouchRecordAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 recorderAddress,
    /* [in] */ ArrayOf<Byte>* recordSource)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super::constructor(source);
        mRecorderAddress = recorderAddress;
        mRecordSource = recordSource;

#endif
}

ECode OneTouchRecordAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        SendRecordOn();
        *result = TRUE;
        return NOERROR;
#endif
}

ECode OneTouchRecordAction::SendRecordOn()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Int32 srcAddr;
        GetSourceAddress(&srcAddr);
        SendCommand(HdmiCecMessageBuilder->BuildRecordOn(srcAddr, mRecorderAddress,
                mRecordSource),
                new SendMessageCallback() {
                //@Override
                    CARAPI OnSendCompleted(Int32 error) {
                        // if failed to send <Record On>, display error message and finish action.
                        if (error != Constants::SEND_RESULT_SUCCESS) {
                            AutoPtr<IHdmiCecLocalDeviceTv> tv;
                            Tv((IHdmiCecLocalDeviceTv**)&tv);
                            tv->AnnounceOneTouchRecordResult(
                                    ONE_TOUCH_RECORD_CHECK_RECORDER_CONNECTION);
                            Finish();
                            return NOERROR;
                        }

                        mState = STATE_WAITING_FOR_RECORD_STATUS;
                        AddTimer(mState, RECORD_STATUS_TIMEOUT_MS);
                    }
                });
#endif
}

ECode OneTouchRecordAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != STATE_WAITING_FOR_RECORD_STATUS) {
            *result = FALSE;
            return NOERROR;
        }

        Int32 opcode;
        cmd->GetOpcode(&opcode);
        switch (opcode) {
            case Constants::MESSAGE_RECORD_STATUS:
                return HandleRecordStatus(cmd);

        }
        *result = FALSE;
        return NOERROR;
#endif
}

ECode OneTouchRecordAction::HandleRecordStatus(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Only handle message coming from original recorder.
        Int32 srcAddr;
        cmd->GetSource(&srcAddr);
        if (srcAddr != mRecorderAddress) {
            *result = FALSE;
            return NOERROR;
        }

        AutoPtr<ArrayOf<Byte> > params;
        cmd->GetParams((ArrayOf<Byte>**)&params);
        Int32 recordStatus = (*params)[0];
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        tv->AnnounceOneTouchRecordResult(recordStatus);
        Slogger::I(TAG, "Got record status:" + recordStatus + " from " + srcAddr);

        // If recording started successfully, change state and keep this action until <Record Off>
        // received. Otherwise, finish action.
        switch (recordStatus) {
            case ONE_TOUCH_RECORD_RECORDING_CURRENTLY_SELECTED_SOURCE:
            case ONE_TOUCH_RECORD_RECORDING_DIGITAL_SERVICE:
            case ONE_TOUCH_RECORD_RECORDING_ANALOGUE_SERVICE:
            case ONE_TOUCH_RECORD_RECORDING_EXTERNAL_INPUT:
                mState = STATE_RECORDING_IN_PROGRESS;
                mActionTimer->ClearTimerMessage();
                break;
            default:
                Finish();
                break;
        }
        *result = TRUE;
        return NOERROR;
#endif
}

ECode OneTouchRecordAction::HandleTimerEvent(
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
        tv->AnnounceOneTouchRecordResult(ONE_TOUCH_RECORD_CHECK_RECORDER_CONNECTION);
        Finish();
    }

    Int32 GetRecorderAddress() {
        return mRecorderAddress;
#endif
}

ECode OneTouchRecordAction::GetRecorderAddress(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mRecorderAddress;

#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
