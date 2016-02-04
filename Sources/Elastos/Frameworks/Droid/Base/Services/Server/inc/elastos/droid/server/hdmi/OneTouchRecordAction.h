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

import static android.hardware.hdmi.HdmiControlManager.ONE_TOUCH_RECORD_CHECK_RECORDER_CONNECTION;
import static android.hardware.hdmi.HdmiControlManager.ONE_TOUCH_RECORD_RECORDING_ANALOGUE_SERVICE;
import static android.hardware.hdmi.HdmiControlManager.ONE_TOUCH_RECORD_RECORDING_CURRENTLY_SELECTED_SOURCE;
import static android.hardware.hdmi.HdmiControlManager.ONE_TOUCH_RECORD_RECORDING_DIGITAL_SERVICE;
import static android.hardware.hdmi.HdmiControlManager.ONE_TOUCH_RECORD_RECORDING_EXTERNAL_INPUT;

using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::Server::Ihdmi.HdmiControlService.SendMessageCallback;

/**
 * Feature action that performs one touch record.
 */
public class OneTouchRecordAction extends HdmiCecFeatureAction {
    private static const String TAG = "OneTouchRecordAction";

    // Timer out for waiting <Record Status> 120s
    private static const Int32 RECORD_STATUS_TIMEOUT_MS = 120000;

    // State that waits for <Record Status> once sending <Record On>
    private static const Int32 STATE_WAITING_FOR_RECORD_STATUS = 1;
    // State that describes recording in progress.
    private static const Int32 STATE_RECORDING_IN_PROGRESS = 2;

    private final Int32 mRecorderAddress;
    private final Byte[] mRecordSource;

    OneTouchRecordAction(HdmiCecLocalDevice source, Int32 recorderAddress, Byte[] recordSource) {
        Super(source);
        mRecorderAddress = recorderAddress;
        mRecordSource = recordSource;
    }

    //@Override
    Boolean Start() {
        SendRecordOn();
        return TRUE;
    }

    private void SendRecordOn() {
        SendCommand(HdmiCecMessageBuilder->BuildRecordOn(GetSourceAddress(), mRecorderAddress,
                mRecordSource),
                new SendMessageCallback() {
                //@Override
                    CARAPI OnSendCompleted(Int32 error) {
                        // if failed to send <Record On>, display error message and finish action.
                        if (error != Constants.SEND_RESULT_SUCCESS) {
                            Tv()->AnnounceOneTouchRecordResult(
                                    ONE_TOUCH_RECORD_CHECK_RECORDER_CONNECTION);
                            Finish();
                            return;
                        }

                        mState = STATE_WAITING_FOR_RECORD_STATUS;
                        AddTimer(mState, RECORD_STATUS_TIMEOUT_MS);
                    }
                });
    }

    //@Override
    Boolean ProcessCommand(HdmiCecMessage cmd) {
        if (mState != STATE_WAITING_FOR_RECORD_STATUS) {
            return FALSE;
        }

        switch (cmd->GetOpcode()) {
            case Constants.MESSAGE_RECORD_STATUS:
                return HandleRecordStatus(cmd);

        }
        return FALSE;
    }

    private Boolean HandleRecordStatus(HdmiCecMessage cmd) {
        // Only handle message coming from original recorder.
        if (cmd->GetSource() != mRecorderAddress) {
            return FALSE;
        }

        Int32 recordStatus = cmd->GetParams()[0];
        Tv()->AnnounceOneTouchRecordResult(recordStatus);
        Slogger::I(TAG, "Got record status:" + recordStatus + " from " + cmd->GetSource());

        // If recording started successfully, change state and keep this action until <Record Off>
        // received. Otherwise, finish action.
        switch (recordStatus) {
            case ONE_TOUCH_RECORD_RECORDING_CURRENTLY_SELECTED_SOURCE:
            case ONE_TOUCH_RECORD_RECORDING_DIGITAL_SERVICE:
            case ONE_TOUCH_RECORD_RECORDING_ANALOGUE_SERVICE:
            case ONE_TOUCH_RECORD_RECORDING_EXTERNAL_INPUT:
                mState = STATE_RECORDING_IN_PROGRESS;
                mActionTimer->ClearTimerMessage();
                break;
            default:
                Finish();
                break;
        }
        return TRUE;
    }

    //@Override
    void HandleTimerEvent(Int32 state) {
        if (mState != state) {
            Slogger::W(TAG, "Timeout in invalid state:[Expected:" + mState + ", Actual:" + state + "]");
            return;
        }

        Tv()->AnnounceOneTouchRecordResult(ONE_TOUCH_RECORD_CHECK_RECORDER_CONNECTION);
        Finish();
    }

    Int32 GetRecorderAddress() {
        return mRecorderAddress;
    }
}
