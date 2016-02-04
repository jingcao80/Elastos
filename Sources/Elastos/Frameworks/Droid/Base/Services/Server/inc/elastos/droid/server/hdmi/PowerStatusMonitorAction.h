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

import static android.hardware.hdmi.HdmiControlManager.POWER_STATUS_UNKNOWN;

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Utility::ISparseIntArray;

using Elastos::Droid::Server::Ihdmi.HdmiControlService.SendMessageCallback;

using Elastos::Utility::IList;

/**
 * Action that check each device's power status.
 */
public class PowerStatusMonitorAction extends HdmiCecFeatureAction {
    private static const String TAG = "PowerStatusMonitorAction";

    // State that waits for <Report Power Status> once sending <Give Device Power Status>
    // to all external devices.
    private static const Int32 STATE_WAIT_FOR_REPORT_POWER_STATUS = 1;
    // State that waits for next monitoring
    private static const Int32 STATE_WAIT_FOR_NEXT_MONITORING = 2;

    private static const Int32 INVALID_POWER_STATUS = POWER_STATUS_UNKNOWN - 1;

    // Monitoring interval (60s)
    private static const Int32 MONITIROING_INTERNAL_MS = 60000;

    // Timeout once sending <Give Device Power Status>
    private static const Int32 REPORT_POWER_STATUS_TIMEOUT_MS = 5000;

    // Container for current power status of all external devices.
    // The key is a logical address a device and the value is current power status of it
    // Whenever the action receives <Report Power Status> from a device,
    // it removes an entry of the given device.
    // If this is non-empty when timeout for STATE_WAIT_FOR_REPORT_POWER_STATUS happens,
    // updates power status of all remaining devices into POWER_STATUS_UNKNOWN.
    private final SparseIntArray mPowerStatus = new SparseIntArray();

    PowerStatusMonitorAction(HdmiCecLocalDevice source) {
        Super(source);
    }

    //@Override
    Boolean Start() {
        QueryPowerStatus();
        return TRUE;
    }

    //@Override
    Boolean ProcessCommand(HdmiCecMessage cmd) {
        if (mState != STATE_WAIT_FOR_REPORT_POWER_STATUS) {
            return FALSE;
        }
        return HandleReportPowerStatus(cmd);
    }

    private Boolean HandleReportPowerStatus(HdmiCecMessage cmd) {
        Int32 sourceAddress = cmd->GetSource();
        Int32 oldStatus = mPowerStatus->Get(sourceAddress, INVALID_POWER_STATUS);
        if (oldStatus == INVALID_POWER_STATUS) {
            // if no device exists for incoming message, hands it over to other actions.
            return FALSE;
        }
        Int32 newStatus = cmd->GetParams()[0] & 0xFF;
        UpdatePowerStatus(sourceAddress, newStatus, TRUE);
        return TRUE;
    }

    //@Override
    void HandleTimerEvent(Int32 state) {
        switch (mState) {
            case STATE_WAIT_FOR_NEXT_MONITORING:
                QueryPowerStatus();
                break;
            case STATE_WAIT_FOR_REPORT_POWER_STATUS:
                HandleTimeout();
                break;
        }
    }

    private void HandleTimeout() {
        for (Int32 i = 0; i < mPowerStatus->Size(); ++i) {
            Int32 logicalAddress = mPowerStatus->KeyAt(i);
            UpdatePowerStatus(logicalAddress, POWER_STATUS_UNKNOWN, FALSE);
        }
        mPowerStatus->Clear();
        mState = STATE_WAIT_FOR_NEXT_MONITORING;
    }

    private void ResetPowerStatus(List<HdmiDeviceInfo> deviceInfos) {
        mPowerStatus->Clear();
        for (HdmiDeviceInfo info : deviceInfos) {
            mPowerStatus->Append(info->GetLogicalAddress(), info->GetDevicePowerStatus());
        }
    }

    private void QueryPowerStatus() {
        List<HdmiDeviceInfo> deviceInfos = Tv()->GetDeviceInfoList(FALSE);
        ResetPowerStatus(deviceInfos);
        for (HdmiDeviceInfo info : deviceInfos) {
            final Int32 logicalAddress = info->GetLogicalAddress();
            SendCommand(HdmiCecMessageBuilder->BuildGiveDevicePowerStatus(GetSourceAddress(),
                    logicalAddress),
                    new SendMessageCallback() {
                        //@Override
                        CARAPI OnSendCompleted(Int32 error) {
                            // If fails to send <Give Device Power Status>,
                            // update power status into UNKNOWN.
                            if (error != Constants.SEND_RESULT_SUCCESS) {
                               UpdatePowerStatus(logicalAddress, POWER_STATUS_UNKNOWN, TRUE);
                            }
                        }
                    });
        }

        mState = STATE_WAIT_FOR_REPORT_POWER_STATUS;

        // Add both timers, monitoring and timeout.
        AddTimer(STATE_WAIT_FOR_NEXT_MONITORING, MONITIROING_INTERNAL_MS);
        AddTimer(STATE_WAIT_FOR_REPORT_POWER_STATUS, REPORT_POWER_STATUS_TIMEOUT_MS);
    }

    private void UpdatePowerStatus(Int32 logicalAddress, Int32 newStatus, Boolean remove) {
        Tv()->UpdateDevicePowerStatus(logicalAddress, newStatus);

        if (remove) {
            mPowerStatus->Delete(logicalAddress);
        }
    }
}
