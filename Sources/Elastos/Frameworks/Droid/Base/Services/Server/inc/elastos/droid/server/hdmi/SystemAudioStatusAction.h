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

using Elastos::Droid::annotation.Nullable;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::Server::Ihdmi.HdmiControlService.SendMessageCallback;

/**
 * Action to update audio status (volume or mute) of audio amplifier
 */
final class SystemAudioStatusAction extends HdmiCecFeatureAction {
    private static const String TAG = "SystemAudioStatusAction";

    // State that waits for <ReportAudioStatus>.
    private static const Int32 STATE_WAIT_FOR_REPORT_AUDIO_STATUS = 1;

    private final Int32 mAvrAddress;
    @Nullable private final IHdmiControlCallback mCallback;

    SystemAudioStatusAction(HdmiCecLocalDevice source, Int32 avrAddress,
            IHdmiControlCallback callback) {
        Super(source);
        mAvrAddress = avrAddress;
        mCallback = callback;
    }

    //@Override
    Boolean Start() {
        mState = STATE_WAIT_FOR_REPORT_AUDIO_STATUS;
        AddTimer(mState, HdmiConfig.TIMEOUT_MS);
        SendGiveAudioStatus();
        return TRUE;
    }

    private void SendGiveAudioStatus() {
        SendCommand(HdmiCecMessageBuilder->BuildGiveAudioStatus(GetSourceAddress(), mAvrAddress),
                new SendMessageCallback() {
            //@Override
            CARAPI OnSendCompleted(Int32 error) {
                if (error != Constants.SEND_RESULT_SUCCESS) {
                    HandleSendGiveAudioStatusFailure();
                }
            }
        });
    }

    private void HandleSendGiveAudioStatusFailure() {
        // Inform to all application that the audio status (volumn, mute) of
        // the audio amplifier is unknown.
        Tv()->SetAudioStatus(FALSE, Constants.UNKNOWN_VOLUME);

        Int32 uiCommand = Tv()->IsSystemAudioActivated()
                ? HdmiCecKeycode.CEC_KEYCODE_RESTORE_VOLUME_FUNCTION  // SystemAudioMode: ON
                : HdmiCecKeycode.CEC_KEYCODE_MUTE_FUNCTION;           // SystemAudioMode: OFF
        SendUserControlPressedAndReleased(mAvrAddress, uiCommand);

        // Still return SUCCESS to callback.
        FinishWithCallback(HdmiControlManager.RESULT_SUCCESS);
    }

    //@Override
    Boolean ProcessCommand(HdmiCecMessage cmd) {
        if (mState != STATE_WAIT_FOR_REPORT_AUDIO_STATUS) {
            return FALSE;
        }

        switch (cmd->GetOpcode()) {
            case Constants.MESSAGE_REPORT_AUDIO_STATUS:
                HandleReportAudioStatus(cmd);
                return TRUE;
        }

        return FALSE;
    }

    private void HandleReportAudioStatus(HdmiCecMessage cmd) {
        Byte[] params = cmd->GetParams();
        Boolean mute = (params[0] & 0x80) == 0x80;
        Int32 volume = params[0] & 0x7F;
        Tv()->SetAudioStatus(mute, volume);

        if (!(Tv()->IsSystemAudioActivated() ^ mute)) {
            // Toggle AVR's mute status to match with the system audio status.
            SendUserControlPressedAndReleased(mAvrAddress, HdmiCecKeycode.CEC_KEYCODE_MUTE);
        }
        FinishWithCallback(HdmiControlManager.RESULT_SUCCESS);
    }

    private void FinishWithCallback(Int32 returnCode) {
        if (mCallback != NULL) {
            try {
                mCallback->OnComplete(returnCode);
            } catch (RemoteException e) {
                Slogger::E(TAG, "Failed to invoke callback.", e);
            }
        }
        Finish();
    }

    //@Override
    void HandleTimerEvent(Int32 state) {
        if (mState != state) {
            return;
        }

        HandleSendGiveAudioStatusFailure();
    }
}
