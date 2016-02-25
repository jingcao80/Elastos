
#include "elastos/droid/server/hdmi/DeviceSelectAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Hardware.h>

using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String DeviceSelectAction::TAG("DeviceSelect");
const Int32 DeviceSelectAction::TIMEOUT_TRANSIT_TO_STANDBY_MS = 5 * 1000;
const Int32 DeviceSelectAction::TIMEOUT_POWER_ON_MS = 5 * 1000;
const Int32 DeviceSelectAction::LOOP_COUNTER_MAX = 20;
const Int32 DeviceSelectAction::STATE_WAIT_FOR_REPORT_POWER_STATUS = 1;
const Int32 DeviceSelectAction::STATE_WAIT_FOR_DEVICE_TO_TRANSIT_TO_STANDBY = 2;
const Int32 DeviceSelectAction::STATE_WAIT_FOR_DEVICE_POWER_ON = 3;

DeviceSelectAction::DeviceSelectAction()
    : mPowerStatusCounter(0)
{}

ECode DeviceSelectAction::constructor(
    /* [in] */ HdmiCecLocalDeviceTv* source,
    /* [in] */ IHdmiDeviceInfo* target,
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super(source);
        mCallback = callback;
        mTarget = target;
        mGivePowerStatus = HdmiCecMessageBuilder.buildGiveDevicePowerStatus(
                getSourceAddress(), getTargetAddress());

#endif
}

ECode DeviceSelectAction::GetTargetAddress(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mTarget.getLogicalAddress();

#endif
}

ECode DeviceSelectAction::Start(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Seq #9
        queryDevicePowerStatus();
        return true;

#endif
}

ECode DeviceSelectAction::QueryDevicePowerStatus()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendCommand(mGivePowerStatus, new SendMessageCallback() {
            @Override
            public void onSendCompleted(int error) {
                if (error == Constants::SEND_RESULT_NAK) {
                    invokeCallback(HdmiControlManager.RESULT_COMMUNICATION_FAILED);
                    finish();
                    return;
                }
            }
        });
        mState = STATE_WAIT_FOR_REPORT_POWER_STATUS;
        addTimer(mState, HdmiConfig.TIMEOUT_MS);

#endif
}

ECode DeviceSelectAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (cmd.getSource() != getTargetAddress()) {
            return FALSE;
        }
        int opcode = cmd.getOpcode();
        byte[] params = cmd.getParams();
        switch (mState) {
            case STATE_WAIT_FOR_REPORT_POWER_STATUS:
                if (opcode == Constants::MESSAGE_REPORT_POWER_STATUS) {
                    return handleReportPowerStatus(params[0]);
                }
                return FALSE;
            default:
                break;
        }
        return FALSE;

#endif
}

ECode DeviceSelectAction::HandleReportPowerStatus(
    /* [in] */ Int32 powerStatus,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        switch (powerStatus) {
            case HdmiControlManager.POWER_STATUS_ON:
                sendSetStreamPath();
                return true;
            case HdmiControlManager.POWER_STATUS_TRANSIENT_TO_STANDBY:
                if (mPowerStatusCounter < 4) {
                    mState = STATE_WAIT_FOR_DEVICE_TO_TRANSIT_TO_STANDBY;
                    addTimer(mState, TIMEOUT_TRANSIT_TO_STANDBY_MS);
                } else {
                    sendSetStreamPath();
                }
                return true;
            case HdmiControlManager.POWER_STATUS_STANDBY:
                if (mPowerStatusCounter == 0) {
                    turnOnDevice();
                } else {
                    sendSetStreamPath();
                }
                return true;
            case HdmiControlManager.POWER_STATUS_TRANSIENT_TO_ON:
                if (mPowerStatusCounter < LOOP_COUNTER_MAX) {
                    mState = STATE_WAIT_FOR_DEVICE_POWER_ON;
                    addTimer(mState, TIMEOUT_POWER_ON_MS);
                } else {
                    sendSetStreamPath();
                }
                return true;
        }
        return FALSE;

#endif
}

ECode DeviceSelectAction::TurnOnDevice()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendUserControlPressedAndReleased(mTarget.getLogicalAddress(),
                HdmiCecKeycode.CEC_KEYCODE_POWER);
        sendUserControlPressedAndReleased(mTarget.getLogicalAddress(),
                HdmiCecKeycode.CEC_KEYCODE_POWER_ON_FUNCTION);
        mState = STATE_WAIT_FOR_DEVICE_POWER_ON;
        addTimer(mState, TIMEOUT_POWER_ON_MS);

#endif
}

ECode DeviceSelectAction::SendSetStreamPath()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendCommand(HdmiCecMessageBuilder.buildSetStreamPath(
                getSourceAddress(), mTarget.getPhysicalAddress()));
        invokeCallback(HdmiControlManager.RESULT_SUCCESS);
        finish();

#endif
}

ECode DeviceSelectAction::HandleTimerEvent(
    /* [in] */ Int32 timeoutState)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != timeoutState) {
            Slogger::W(TAG, "Timer in a wrong state. Ignored.");
            return;
        }
        switch (mState) {
            case STATE_WAIT_FOR_REPORT_POWER_STATUS:
                if (tv().isPowerStandbyOrTransient()) {
                    invokeCallback(HdmiControlManager.RESULT_INCORRECT_MODE);
                    finish();
                    return;
                }
                sendSetStreamPath();
                break;
            case STATE_WAIT_FOR_DEVICE_TO_TRANSIT_TO_STANDBY:
            case STATE_WAIT_FOR_DEVICE_POWER_ON:
                mPowerStatusCounter++;
                queryDevicePowerStatus();
                break;
        }

#endif
}

ECode DeviceSelectAction::InvokeCallback(
    /* [in] */ Int32 result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mCallback == NULL) {
            return;
        }
        try {
            mCallback.onComplete(result);
        } catch (RemoteException e) {
            Slogger::E(TAG, "Callback failed:" + e);
        }

#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
