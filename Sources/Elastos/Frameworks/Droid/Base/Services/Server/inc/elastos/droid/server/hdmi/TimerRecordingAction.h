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

import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION;
import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_RESULT_EXTRA_FAIL_TO_RECORD_SELECTED_SOURCE;
import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_TYPE_ANALOGUE;
import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_TYPE_DIGITAL;
import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_TYPE_EXTERNAL;

using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::Server::Ihdmi.HdmiControlService.SendMessageCallback;

using Elastos::Utility::IArrays;

/**
 * Feature action that performs timer recording.
 */
public class TimerRecordingAction extends HdmiCecFeatureAction {
    private static const String TAG = "TimerRecordingAction";

    // Timer out for waiting <Timer Status> 120s.
    private static const Int32 TIMER_STATUS_TIMEOUT_MS = 120000;

    // State that waits for <Timer Status> once sending <Set XXX Timer>
    private static const Int32 STATE_WAITING_FOR_TIMER_STATUS = 1;

    private final Int32 mRecorderAddress;
    private final Int32 mSourceType;
    private final Byte[] mRecordSource;

    TimerRecordingAction(HdmiCecLocalDevice source, Int32 recorderAddress, Int32 sourceType,
            Byte[] recordSource) {
        Super(source);
        mRecorderAddress = recorderAddress;
        mSourceType = sourceType;
        mRecordSource = recordSource;
    }

    //@Override
    Boolean Start() {
        SendTimerMessage();
        return TRUE;
    }

    private void SendTimerMessage() {
        HdmiCecMessage message = NULL;
        switch (mSourceType) {
            case TIMER_RECORDING_TYPE_DIGITAL:
                message = HdmiCecMessageBuilder->BuildSetDigitalTimer(GetSourceAddress(),
                        mRecorderAddress, mRecordSource);
                break;
            case TIMER_RECORDING_TYPE_ANALOGUE:
                message = HdmiCecMessageBuilder->BuildSetAnalogueTimer(GetSourceAddress(),
                        mRecorderAddress, mRecordSource);
                break;
            case TIMER_RECORDING_TYPE_EXTERNAL:
                message = HdmiCecMessageBuilder->BuildSetExternalTimer(GetSourceAddress(),
                        mRecorderAddress, mRecordSource);
                break;
            default:
                Tv()->AnnounceTimerRecordingResult(
                        TIMER_RECORDING_RESULT_EXTRA_FAIL_TO_RECORD_SELECTED_SOURCE);
                Finish();
                return;
        }
        SendCommand(message, new SendMessageCallback() {
            //@Override
            CARAPI OnSendCompleted(Int32 error) {
                if (error != Constants.SEND_RESULT_SUCCESS) {
                    Tv()->AnnounceTimerRecordingResult(
                            TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION);
                    Finish();
                    return;
                }
                mState = STATE_WAITING_FOR_TIMER_STATUS;
                AddTimer(mState, TIMER_STATUS_TIMEOUT_MS);
            }
        });
    }

    //@Override
    Boolean ProcessCommand(HdmiCecMessage cmd) {
        if (mState != STATE_WAITING_FOR_TIMER_STATUS) {
            return FALSE;
        }

        if (cmd->GetSource() != mRecorderAddress) {
            return FALSE;
        }

        switch (cmd->GetOpcode()) {
            case Constants.MESSAGE_TIMER_STATUS:
                return HandleTimerStatus(cmd);
            case Constants.MESSAGE_FEATURE_ABORT:
                return HandleFeatureAbort(cmd);
        }
        return FALSE;
    }

    private Boolean HandleTimerStatus(HdmiCecMessage cmd) {
        Byte[] timerStatusData = cmd->GetParams();
        // [Timer Status Data] should be one or three bytes.
        if (timerStatusData.length == 1 || timerStatusData.length == 3) {
            Tv()->AnnounceTimerRecordingResult(BytesToInt(timerStatusData));
            Slogger::I(TAG, "Received [Timer Status Data]:" + Arrays->ToString(timerStatusData));
        } else {
            Slogger::W(TAG, "Invalid [Timer Status Data]:" + Arrays->ToString(timerStatusData));
        }

        // Unlike one touch record, finish timer record when <Timer Status> is received.
        Finish();
        return TRUE;
    }

    private Boolean HandleFeatureAbort(HdmiCecMessage cmd) {
        Byte[] params = cmd->GetParams();
        Int32 messageType = params[0] & 0xFF;
        switch (messageType) {
            case Constants.MESSAGE_SET_DIGITAL_TIMER: // fall through
            case Constants.MESSAGE_SET_ANALOG_TIMER: // fall through
            case Constants.MESSAGE_SET_EXTERNAL_TIMER: // fall through
                break;
            default:
                return FALSE;
        }
        Int32 reason = params[1] & 0xFF;
        Slogger::I(TAG, "[Feature Abort] for " + messageType + " reason:" + reason);
        Tv()->AnnounceTimerRecordingResult(TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION);
        Finish();
        return TRUE;
    }

    // Convert Byte array to Int32.
    private static Int32 BytesToInt(Byte[] data) {
        if (data.length > 4) {
            throw new IllegalArgumentException("Invalid data size:" + Arrays->ToString(data));
        }
        Int32 result = 0;
        for (Int32 i = 0; i < data.length; ++i) {
            Int32 shift = (3 - i) * 8;
            result |= ((data[i] & 0xFF) << shift);
        }
        return result;
    }

    //@Override
    void HandleTimerEvent(Int32 state) {
        if (mState != state) {
            Slogger::W(TAG, "Timeout in invalid state:[Expected:" + mState + ", Actual:" + state + "]");
            return;
        }

        Tv()->AnnounceTimerRecordingResult(TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION);
        Finish();
    }
}
