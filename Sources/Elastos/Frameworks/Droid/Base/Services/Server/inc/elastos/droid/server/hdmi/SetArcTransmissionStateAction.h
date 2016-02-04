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
using Elastos::Droid::Utility::ISlog;

/**
 * Feature action that handles enabling/disabling of ARC transmission channel.
 * Once TV gets &lt;Initiate ARC&gt;, TV sends &lt;Report ARC Initiated&gt; to AV Receiver.
 * If it fails or it gets &lt;Terminate ARC&gt;, TV just disables ARC.
 */
final class SetArcTransmissionStateAction extends HdmiCecFeatureAction {
    private static const String TAG = "SetArcTransmissionStateAction";

    // State in which the action sent <Rerpot Arc Initiated> and
    // is waiting for time out. If it receives <Feature Abort> within timeout
    // ARC should be disabled.
    private static const Int32 STATE_WAITING_TIMEOUT = 1;

    private final Boolean mEnabled;
    private final Int32 mAvrAddress;

    /**
     * @Constructor
     *
     * @param source {@link HdmiCecLocalDevice} instance
     * @param enabled whether to enable ARC Transmission channel
     */
    SetArcTransmissionStateAction(HdmiCecLocalDevice source, Int32 avrAddress,
            Boolean enabled) {
        Super(source);
        HdmiUtils->VerifyAddressType(GetSourceAddress(), HdmiDeviceInfo.DEVICE_TV);
        HdmiUtils->VerifyAddressType(avrAddress, HdmiDeviceInfo.DEVICE_AUDIO_SYSTEM);
        mAvrAddress = avrAddress;
        mEnabled = enabled;
    }

    //@Override
    Boolean Start() {
        if (mEnabled) {
            // Enable ARC status immediately after sending <Report Arc Initiated>.
            // If AVR responds with <Feature Abort>, disable ARC status again.
            // This is different from spec that says that turns ARC status to
            // "Enabled" if <Report ARC Initiated> is acknowledged and no
            // <Feature Abort> is received.
            // But implemented this way to save the time having to wait for
            // <Feature Abort>.
            SetArcStatus(TRUE);
            // If succeeds to send <Report ARC Initiated>, wait general timeout
            // to check whether there is no <Feature Abort> for <Report ARC Initiated>.
            mState = STATE_WAITING_TIMEOUT;
            AddTimer(mState, HdmiConfig.TIMEOUT_MS);
            SendReportArcInitiated();
        } else {
            SetArcStatus(FALSE);
            Finish();
        }
        return TRUE;
    }

    private void SendReportArcInitiated() {
        HdmiCecMessage command =
                HdmiCecMessageBuilder->BuildReportArcInitiated(GetSourceAddress(), mAvrAddress);
        SendCommand(command, new HdmiControlService->SendMessageCallback() {
            //@Override
            CARAPI OnSendCompleted(Int32 error) {
                if (error != Constants.SEND_RESULT_SUCCESS) {
                    // If fails to send <Report ARC Initiated>, disable ARC and
                    // send <Report ARC Terminated> directly.
                    SetArcStatus(FALSE);
                    HdmiLogger->Debug("Failed to send <Report Arc Initiated>.");
                    Finish();
                }
            }
        });
    }

    private void SetArcStatus(Boolean enabled) {
        Boolean wasEnabled = Tv()->SetArcStatus(enabled);
        Slogger::I(TAG, "Change arc status [old:" + wasEnabled + ", new:" + enabled + "]");

        // If enabled before and set to "disabled" and send <Report Arc Terminated> to
        // av reciever.
        if (!enabled && wasEnabled) {
            SendCommand(HdmiCecMessageBuilder->BuildReportArcTerminated(GetSourceAddress(),
                    mAvrAddress));
        }
    }

    //@Override
    Boolean ProcessCommand(HdmiCecMessage cmd) {
        if (mState != STATE_WAITING_TIMEOUT) {
            return FALSE;
        }

        Int32 opcode = cmd->GetOpcode();
        if (opcode == Constants.MESSAGE_FEATURE_ABORT) {
            Int32 originalOpcode = cmd->GetParams()[0] & 0xFF;
            if (originalOpcode == Constants.MESSAGE_REPORT_ARC_INITIATED) {
                HdmiLogger->Debug("Feature aborted for <Report Arc Initiated>");
                SetArcStatus(FALSE);
                Finish();
                return TRUE;
            }
        }
        return FALSE;
    }

    //@Override
    void HandleTimerEvent(Int32 state) {
        if (mState != state || mState != STATE_WAITING_TIMEOUT) {
            return;
        }
        // Expire timeout for <Feature Abort>.
        Finish();
    }
}
