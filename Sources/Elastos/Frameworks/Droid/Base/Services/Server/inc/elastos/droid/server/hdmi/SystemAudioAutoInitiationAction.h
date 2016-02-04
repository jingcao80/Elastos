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

using Elastos::Droid::Server::Ihdmi.HdmiControlService.SendMessageCallback;

/**
 * Action to initiate system audio once AVR is detected on Device discovery action.
 */
// Seq #27
final class SystemAudioAutoInitiationAction extends HdmiCecFeatureAction {
    private final Int32 mAvrAddress;

    // State that waits for <System Audio Mode Status> once send
    // <Give System Audio Mode Status> to AV Receiver.
    private static const Int32 STATE_WAITING_FOR_SYSTEM_AUDIO_MODE_STATUS = 1;

    SystemAudioAutoInitiationAction(HdmiCecLocalDevice source, Int32 avrAddress) {
        Super(source);
        mAvrAddress = avrAddress;
    }

    //@Override
    Boolean Start() {
        mState = STATE_WAITING_FOR_SYSTEM_AUDIO_MODE_STATUS;

        AddTimer(mState, HdmiConfig.TIMEOUT_MS);
        SendGiveSystemAudioModeStatus();
        return TRUE;
    }

    private void SendGiveSystemAudioModeStatus() {
        SendCommand(HdmiCecMessageBuilder->BuildGiveSystemAudioModeStatus(GetSourceAddress(),
                mAvrAddress), new SendMessageCallback() {
            //@Override
            CARAPI OnSendCompleted(Int32 error) {
                if (error != Constants.SEND_RESULT_SUCCESS) {
                    Tv()->SetSystemAudioMode(FALSE, TRUE);
                    Finish();
                }
            }
        });
    }

    //@Override
    Boolean ProcessCommand(HdmiCecMessage cmd) {
        if (mState != STATE_WAITING_FOR_SYSTEM_AUDIO_MODE_STATUS) {
            return FALSE;
        }

        switch (cmd->GetOpcode()) {
            case Constants.MESSAGE_SYSTEM_AUDIO_MODE_STATUS:
                HandleSystemAudioModeStatusMessage();
                return TRUE;
            default:
                return FALSE;
        }
    }

    private void HandleSystemAudioModeStatusMessage() {
        // If the last setting is system audio, turn on system audio whatever AVR status is.
        if (Tv()->GetSystemAudioModeSetting()) {
            if (CanChangeSystemAudio()) {
                AddAndStartAction(new SystemAudioActionFromTv(Tv(), mAvrAddress, TRUE, NULL));
            }
        } else {
            // If the last setting is non-system audio, turn off system audio mode
            // and update system audio status (volume or mute).
            Tv()->SetSystemAudioMode(FALSE, TRUE);
            if (CanChangeSystemAudio()) {
                AddAndStartAction(new SystemAudioStatusAction(Tv(), mAvrAddress, NULL));
            }
        }
        Finish();
    }

    //@Override
    void HandleTimerEvent(Int32 state) {
        if (mState != state) {
            return;
        }

        switch (mState) {
            case STATE_WAITING_FOR_SYSTEM_AUDIO_MODE_STATUS:
                HandleSystemAudioModeStatusTimeout();
                break;
        }
    }

    private void HandleSystemAudioModeStatusTimeout() {
        if (Tv()->GetSystemAudioModeSetting()) {
            if (CanChangeSystemAudio()) {
                AddAndStartAction(new SystemAudioActionFromTv(Tv(), mAvrAddress, TRUE, NULL));
            }
        } else {
            Tv()->SetSystemAudioMode(FALSE, TRUE);
        }
        Finish();
    }

    private Boolean CanChangeSystemAudio() {
        return !(Tv()->HasAction(SystemAudioActionFromTv.class)
               || Tv()->HasAction(SystemAudioActionFromAvr.class));
    }
}
