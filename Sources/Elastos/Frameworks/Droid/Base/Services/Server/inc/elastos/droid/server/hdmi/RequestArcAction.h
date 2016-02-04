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
 * Base feature action class for &lt;Request ARC Initiation&gt;/&lt;Request ARC Termination&gt;.
 */
abstract class RequestArcAction extends HdmiCecFeatureAction {
    private static const String TAG = "RequestArcAction";

    // State in which waits for ARC response.
    protected static const Int32 STATE_WATING_FOR_REQUEST_ARC_REQUEST_RESPONSE = 1;

    // Logical address of AV Receiver.
    protected final Int32 mAvrAddress;

    /**
     * @Constructor
     *
     * @param source {@link HdmiCecLocalDevice} instance
     * @param avrAddress address of AV receiver. It should be AUDIO_SYSTEM type
     * @throw IllegalArugmentException if device type of sourceAddress and avrAddress
     *                      is invalid
     */
    RequestArcAction(HdmiCecLocalDevice source, Int32 avrAddress) {
        Super(source);
        HdmiUtils->VerifyAddressType(GetSourceAddress(), HdmiDeviceInfo.DEVICE_TV);
        HdmiUtils->VerifyAddressType(avrAddress, HdmiDeviceInfo.DEVICE_AUDIO_SYSTEM);
        mAvrAddress = avrAddress;
    }

    //@Override
    Boolean ProcessCommand(HdmiCecMessage cmd) {
        if (mState != STATE_WATING_FOR_REQUEST_ARC_REQUEST_RESPONSE
                || !HdmiUtils->CheckCommandSource(cmd, mAvrAddress, TAG)) {
            return FALSE;
        }
        Int32 opcode = cmd->GetOpcode();
        switch (opcode) {
            // Handles only <Feature Abort> here and, both <Initiate ARC> and <Terminate ARC>
            // are handled in HdmiControlService itself because both can be
            // received without <Request ARC Initiation> or <Request ARC Termination>.
            case Constants.MESSAGE_FEATURE_ABORT:
                Int32 originalOpcode = cmd->GetParams()[0] & 0xFF;
                if (originalOpcode == Constants.MESSAGE_REQUEST_ARC_INITIATION
                        || originalOpcode == Constants.MESSAGE_REQUEST_ARC_TERMINATION) {
                    DisableArcTransmission();
                    Finish();
                    return TRUE;
                } else {
                    return FALSE;
                }
        }
        return FALSE;
    }

    protected final void DisableArcTransmission() {
        // Start Set ARC Transmission State action.
        SetArcTransmissionStateAction action = new SetArcTransmissionStateAction(LocalDevice(),
                mAvrAddress, FALSE);
        AddAndStartAction(action);
    }

    //@Override
    final void HandleTimerEvent(Int32 state) {
        if (mState != state || state != STATE_WATING_FOR_REQUEST_ARC_REQUEST_RESPONSE) {
            return;
        }
        HdmiLogger->Debug("[T]RequestArcAction.");
        DisableArcTransmission();
        Finish();
    }
}
