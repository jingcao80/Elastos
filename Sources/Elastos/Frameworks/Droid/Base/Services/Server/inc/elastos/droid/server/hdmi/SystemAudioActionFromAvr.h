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
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;

/**
 * Feature action that handles System Audio initiated by AVR devices.
 */
// Seq #33
final class SystemAudioActionFromAvr extends SystemAudioAction {
    /**
     * Constructor
     *
     * @param source {@link HdmiCecLocalDevice} instance
     * @param avrAddress logical address of AVR device
     * @param targetStatus Whether to enable the system audio mode or not
     * @param callback callback interface to be notified when it's done
     * @throw IllegalArugmentException if device type of tvAddress and avrAddress is invalid
     */
    SystemAudioActionFromAvr(HdmiCecLocalDevice source, Int32 avrAddress,
            Boolean targetStatus, IHdmiControlCallback callback) {
        Super(source, avrAddress, targetStatus, callback);
        HdmiUtils->VerifyAddressType(GetSourceAddress(), HdmiDeviceInfo.DEVICE_TV);
    }

    //@Override
    Boolean Start() {
        RemoveSystemAudioActionInProgress();
        HandleSystemAudioActionFromAvr();
        return TRUE;
    }

    private void HandleSystemAudioActionFromAvr() {
        if (mTargetAudioStatus == Tv()->IsSystemAudioActivated()) {
            FinishWithCallback(HdmiControlManager.RESULT_SUCCESS);
            return;
        }
        if (Tv()->IsProhibitMode()) {
            SendCommand(HdmiCecMessageBuilder->BuildFeatureAbortCommand(
                    GetSourceAddress(), mAvrLogicalAddress,
                    Constants.MESSAGE_SET_SYSTEM_AUDIO_MODE, Constants.ABORT_REFUSED));
            mTargetAudioStatus = FALSE;
            SendSystemAudioModeRequest();
            return;
        }

        RemoveAction(SystemAudioAutoInitiationAction.class);

        if (mTargetAudioStatus) {
            SetSystemAudioMode(TRUE);
            StartAudioStatusAction();
        } else {
            SetSystemAudioMode(FALSE);
            FinishWithCallback(HdmiControlManager.RESULT_SUCCESS);
        }
    }
}
