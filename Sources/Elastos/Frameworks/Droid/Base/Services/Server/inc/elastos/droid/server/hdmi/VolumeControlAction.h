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

import static com.android.server.hdmi.Constants.IRT_MS;
import static com.android.server.hdmi.Constants.MESSAGE_FEATURE_ABORT;
import static com.android.server.hdmi.Constants.MESSAGE_REPORT_AUDIO_STATUS;
import static com.android.server.hdmi.Constants.MESSAGE_USER_CONTROL_PRESSED;

using Elastos::Droid::Media::IAudioManager;

/**
 * Feature action that transmits volume change to Audio Receiver.
 * <p>
 * This action is created when a user pressed volume up/down. However, Android only provides a
 * listener for delta of some volume change instead of individual key event. Also it's hard to know
 * Audio Receiver's number of volume steps for a single volume control key. Because of this, it
 * sends key-down event until IRT timeout happens, and it will send key-up event if no additional
 * volume change happens; otherwise, it will send again key-down as press and hold feature does.
 */
final class VolumeControlAction extends HdmiCecFeatureAction {
    private static const String TAG = "VolumeControlAction";

    // State that wait for next volume press.
    private static const Int32 STATE_WAIT_FOR_NEXT_VOLUME_PRESS = 1;
    private static const Int32 MAX_VOLUME = 100;

    private static const Int32 UNKNOWN_AVR_VOLUME = -1;

    private final Int32 mAvrAddress;
    private Boolean mIsVolumeUp;
    private Int64 mLastKeyUpdateTime;
    private Int32 mLastAvrVolume;
    private Boolean mSentKeyPressed;

    /**
     * Scale a custom volume value to cec volume scale.
     *
     * @param volume volume value in custom scale
     * @param scale scale of volume (max volume)
     * @return a volume scaled to cec volume range
     */
    public static Int32 ScaleToCecVolume(Int32 volume, Int32 scale) {
        return (volume * MAX_VOLUME) / scale;
    }

    /**
     * Scale a cec volume which is in range of 0 to 100 to custom volume level.
     *
     * @param cecVolume volume value in cec volume scale. It should be in a range of [0-100]
     * @param scale scale of custom volume (max volume)
     * @return a volume scaled to custom volume range
     */
    public static Int32 ScaleToCustomVolume(Int32 cecVolume, Int32 scale) {
        return (cecVolume * scale) / MAX_VOLUME;
    }

    VolumeControlAction(HdmiCecLocalDevice source, Int32 avrAddress, Boolean isVolumeUp) {
        Super(source);
        mAvrAddress = avrAddress;
        mIsVolumeUp = isVolumeUp;
        mLastAvrVolume = UNKNOWN_AVR_VOLUME;
        mSentKeyPressed = FALSE;

        UpdateLastKeyUpdateTime();
    }

    private void UpdateLastKeyUpdateTime() {
        mLastKeyUpdateTime = System->CurrentTimeMillis();
    }

    //@Override
    Boolean Start() {
        mState = STATE_WAIT_FOR_NEXT_VOLUME_PRESS;
        SendVolumeKeyPressed();
        ResetTimer();
        return TRUE;
    }

    private void SendVolumeKeyPressed() {
        SendCommand(HdmiCecMessageBuilder->BuildUserControlPressed(GetSourceAddress(), mAvrAddress,
                mIsVolumeUp ? HdmiCecKeycode.CEC_KEYCODE_VOLUME_UP
                        : HdmiCecKeycode.CEC_KEYCODE_VOLUME_DOWN));
        mSentKeyPressed = TRUE;
    }

    private void ResetTimer() {
        mActionTimer->ClearTimerMessage();
        AddTimer(STATE_WAIT_FOR_NEXT_VOLUME_PRESS, IRT_MS);
    }

    void HandleVolumeChange(Boolean isVolumeUp) {
        if (mIsVolumeUp != isVolumeUp) {
            HdmiLogger->Debug("Volume Key Status Changed[old:%b new:%b]", mIsVolumeUp, isVolumeUp);
            SendVolumeKeyReleased();
            mIsVolumeUp = isVolumeUp;
        }
        UpdateLastKeyUpdateTime();
    }

    private void SendVolumeKeyReleased() {
        SendCommand(HdmiCecMessageBuilder->BuildUserControlReleased(
                GetSourceAddress(), mAvrAddress));
        mSentKeyPressed = FALSE;
    }

    //@Override
    Boolean ProcessCommand(HdmiCecMessage cmd) {
        if (mState != STATE_WAIT_FOR_NEXT_VOLUME_PRESS || cmd->GetSource() != mAvrAddress) {
            return FALSE;
        }

        switch (cmd->GetOpcode()) {
            case MESSAGE_REPORT_AUDIO_STATUS:
                return HandleReportAudioStatus(cmd);
            case MESSAGE_FEATURE_ABORT:
                return HandleFeatureAbort(cmd);
            default:
                return FALSE;
        }
    }

    private Boolean HandleReportAudioStatus(HdmiCecMessage cmd) {
        Byte params[] = cmd->GetParams();
        Boolean mute = (params[0] & 0x80) == 0x80;
        Int32 volume = params[0] & 0x7F;
        mLastAvrVolume = volume;
        if (ShouldUpdateAudioVolume(mute)) {
            HdmiLogger->Debug("Force volume change[mute:%b, volume=%d]", mute, volume);
            Tv()->SetAudioStatus(mute, volume);
        }
        return TRUE;
    }

    private Boolean ShouldUpdateAudioVolume(Boolean mute) {
        // Do nothing if in mute.
        if (mute) {
            return TRUE;
        }

        // Update audio status if current volume position is edge of volume bar,
        // i.e max or min volume.
        AudioManager audioManager = Tv()->GetService().GetAudioManager();
        Int32 currentVolume = audioManager->GetStreamVolume(AudioManager.STREAM_MUSIC);
        if (mIsVolumeUp) {
            Int32 maxVolume = audioManager->GetStreamMaxVolume(AudioManager.STREAM_MUSIC);
            return currentVolume == maxVolume;
        } else {
            return currentVolume == 0;
        }
    }

    private Boolean HandleFeatureAbort(HdmiCecMessage cmd) {
        Int32 originalOpcode = cmd->GetParams()[0] & 0xFF;
        // Since it sends <User Control Released> only when it finishes this action,
        // it takes care of <User Control Pressed> only here.
        if (originalOpcode == MESSAGE_USER_CONTROL_PRESSED) {
            Finish();
            return TRUE;
        }
        return FALSE;
    }

    //@Override
    protected void Clear() {
        super->Clear();
        if (mSentKeyPressed) {
            SendVolumeKeyReleased();
        }
        if (mLastAvrVolume != UNKNOWN_AVR_VOLUME) {
            Tv()->SetAudioStatus(FALSE, mLastAvrVolume);
            mLastAvrVolume = UNKNOWN_AVR_VOLUME;
        }
    }

    //@Override
    void HandleTimerEvent(Int32 state) {
        if (state != STATE_WAIT_FOR_NEXT_VOLUME_PRESS) {
            return;
        }

        if (System->CurrentTimeMillis() - mLastKeyUpdateTime >= IRT_MS) {
            Finish();
        } else {
            SendVolumeKeyPressed();
            ResetTimer();
        }
    }
}
