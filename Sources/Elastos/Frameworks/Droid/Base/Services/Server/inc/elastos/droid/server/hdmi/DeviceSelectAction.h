/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.server.hdmi;

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiTvClient;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::Server::Ihdmi.HdmiControlService.SendMessageCallback;

/**
 * Handles an action that selects a logical device as a new active source.
 *
 * Triggered by {@link HdmiTvClient}, attempts to select the given target device
 * for a new active source. It does its best to wake up the target in standby mode
 * before issuing the command &gt;Set Stream path&lt;.
 */
final class DeviceSelectAction extends HdmiCecFeatureAction {
    private static const String TAG = "DeviceSelect";

    // Time in milliseconds we wait for the device power status to switch to 'Standby'
    private static const Int32 TIMEOUT_TRANSIT_TO_STANDBY_MS = 5 * 1000;

    // Time in milliseconds we wait for the device power status to turn to 'On'.
    private static const Int32 TIMEOUT_POWER_ON_MS = 5 * 1000;

    // The number of times we try to wake up the target device before we give up
    // and just send <Set Stream Path>.
    private static const Int32 LOOP_COUNTER_MAX = 20;

    // State in which we wait for <Report Power Status> to come in response to the command
    // <Give Device Power Status> we have sent.
    private static const Int32 STATE_WAIT_FOR_REPORT_POWER_STATUS = 1;

    // State in which we wait for the device power status to switch to 'Standby'.
    // We wait till the status becomes 'Standby' before we send <Set Stream Path>
    // to wake up the device again.
    private static const Int32 STATE_WAIT_FOR_DEVICE_TO_TRANSIT_TO_STANDBY = 2;

    // State in which we wait for the device power status to switch to 'on'. We wait
    // maximum 100 seconds (20 * 5) before we give up and just send <Set Stream Path>.
    private static const Int32 STATE_WAIT_FOR_DEVICE_POWER_ON = 3;

    private final HdmiDeviceInfo mTarget;
    private final IHdmiControlCallback mCallback;
    private final HdmiCecMessage mGivePowerStatus;

    private Int32 mPowerStatusCounter = 0;

    /**
     * Constructor.
     *
     * @param source {@link HdmiCecLocalDevice} instance
     * @param target target logical device that will be a new active source
     * @param callback callback object
     */
    public DeviceSelectAction(HdmiCecLocalDeviceTv source,
            HdmiDeviceInfo target, IHdmiControlCallback callback) {
        Super(source);
        mCallback = callback;
        mTarget = target;
        mGivePowerStatus = HdmiCecMessageBuilder->BuildGiveDevicePowerStatus(
                GetSourceAddress(), GetTargetAddress());
    }

    Int32 GetTargetAddress() {
        return mTarget->GetLogicalAddress();
    }

    //@Override
    public Boolean Start() {
        // Seq #9
        QueryDevicePowerStatus();
        return TRUE;
    }

    private void QueryDevicePowerStatus() {
        SendCommand(mGivePowerStatus, new SendMessageCallback() {
            //@Override
            CARAPI OnSendCompleted(Int32 error) {
                if (error == Constants.SEND_RESULT_NAK) {
                    InvokeCallback(HdmiControlManager.RESULT_COMMUNICATION_FAILED);
                    Finish();
                    return;
                }
            }
        });
        mState = STATE_WAIT_FOR_REPORT_POWER_STATUS;
        AddTimer(mState, HdmiConfig.TIMEOUT_MS);
    }

    //@Override
    public Boolean ProcessCommand(HdmiCecMessage cmd) {
        if (cmd->GetSource() != GetTargetAddress()) {
            return FALSE;
        }
        Int32 opcode = cmd->GetOpcode();
        Byte[] params = cmd->GetParams();

        switch (mState) {
            case STATE_WAIT_FOR_REPORT_POWER_STATUS:
                if (opcode == Constants.MESSAGE_REPORT_POWER_STATUS) {
                    return HandleReportPowerStatus(params[0]);
                }
                return FALSE;
            default:
                break;
        }
        return FALSE;
    }

    private Boolean HandleReportPowerStatus(Int32 powerStatus) {
        switch (powerStatus) {
            case HdmiControlManager.POWER_STATUS_ON:
                SendSetStreamPath();
                return TRUE;
            case HdmiControlManager.POWER_STATUS_TRANSIENT_TO_STANDBY:
                if (mPowerStatusCounter < 4) {
                    mState = STATE_WAIT_FOR_DEVICE_TO_TRANSIT_TO_STANDBY;
                    AddTimer(mState, TIMEOUT_TRANSIT_TO_STANDBY_MS);
                } else {
                    SendSetStreamPath();
                }
                return TRUE;
            case HdmiControlManager.POWER_STATUS_STANDBY:
                if (mPowerStatusCounter == 0) {
                    TurnOnDevice();
                } else {
                    SendSetStreamPath();
                }
                return TRUE;
            case HdmiControlManager.POWER_STATUS_TRANSIENT_TO_ON:
                if (mPowerStatusCounter < LOOP_COUNTER_MAX) {
                    mState = STATE_WAIT_FOR_DEVICE_POWER_ON;
                    AddTimer(mState, TIMEOUT_POWER_ON_MS);
                } else {
                    SendSetStreamPath();
                }
                return TRUE;
        }
        return FALSE;
    }

    private void TurnOnDevice() {
        SendUserControlPressedAndReleased(mTarget->GetLogicalAddress(),
                HdmiCecKeycode.CEC_KEYCODE_POWER);
        SendUserControlPressedAndReleased(mTarget->GetLogicalAddress(),
                HdmiCecKeycode.CEC_KEYCODE_POWER_ON_FUNCTION);
        mState = STATE_WAIT_FOR_DEVICE_POWER_ON;
        AddTimer(mState, TIMEOUT_POWER_ON_MS);
    }

    private void SendSetStreamPath() {
        SendCommand(HdmiCecMessageBuilder->BuildSetStreamPath(
                GetSourceAddress(), mTarget->GetPhysicalAddress()));
        InvokeCallback(HdmiControlManager.RESULT_SUCCESS);
        Finish();
    }

    //@Override
    CARAPI HandleTimerEvent(Int32 timeoutState) {
        if (mState != timeoutState) {
            Slogger::W(TAG, "Timer in a wrong state. Ignored.");
            return;
        }
        switch (mState) {
            case STATE_WAIT_FOR_REPORT_POWER_STATUS:
                if (Tv()->IsPowerStandbyOrTransient()) {
                    InvokeCallback(HdmiControlManager.RESULT_INCORRECT_MODE);
                    Finish();
                    return;
                }
                SendSetStreamPath();
                break;
            case STATE_WAIT_FOR_DEVICE_TO_TRANSIT_TO_STANDBY:
            case STATE_WAIT_FOR_DEVICE_POWER_ON:
                mPowerStatusCounter++;
                QueryDevicePowerStatus();
                break;
        }
    }

    private void InvokeCallback(Int32 result) {
        if (mCallback == NULL) {
            return;
        }
        try {
            mCallback->OnComplete(result);
        } catch (RemoteException e) {
            Slogger::E(TAG, "Callback failed:" + e);
        }
    }
}
