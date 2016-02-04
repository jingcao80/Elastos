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

using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;
using Elastos::Droid::Hardware::Hdmi::IHdmiPlaybackClient.OneTouchPlayCallback;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Utility::ISlog;

/**
 * Feature action that performs one touch play against TV/Display device. This action is initiated
 * via {@link android.hardware.hdmi.HdmiPlaybackClient#OneTouchPlay(OneTouchPlayCallback)} from the
 * Android system working as playback device to turn on the TV, and switch the input.
 * <p>
 * Package-private, accessed by {@link HdmiControlService} only.
 */
final class OneTouchPlayAction extends HdmiCecFeatureAction {
    private static const String TAG = "OneTouchPlayAction";

    // State in which the action is waiting for <Report Power Status>. In normal situation
    // source device can simply send <Text|Image View On> and <Active Source> in succession
    // since the standard requires that the TV/Display should buffer the <Active Source>
    // if the TV is brought of out standby state.
    //
    // But there are TV's that fail to buffer the <Active Source> while getting out of
    // standby mode, and do not accept the command until their power status becomes 'ON'.
    // For a workaround, we send <Give Device Power Status> commands periodically to make sure
    // the device switches its status to 'ON'. Then we send additional <Active Source>.
    private static const Int32 STATE_WAITING_FOR_REPORT_POWER_STATUS = 1;

    // The maximum number of times we send <Give Device Power Status> before we give up.
    // We wait up to RESPONSE_TIMEOUT_MS * LOOP_COUNTER_MAX = 20 seconds.
    private static const Int32 LOOP_COUNTER_MAX = 10;

    private final Int32 mTargetAddress;
    private final IHdmiControlCallback mCallback;

    private Int32 mPowerStatusCounter = 0;

    // Factory method. Ensures arguments are valid.
    static OneTouchPlayAction Create(HdmiCecLocalDevicePlayback source,
            Int32 targetAddress, IHdmiControlCallback callback) {
        if (source == NULL || callback == NULL) {
            Slogger::E(TAG, "Wrong arguments");
            return NULL;
        }
        return new OneTouchPlayAction(source, targetAddress,
                callback);
    }

    private OneTouchPlayAction(HdmiCecLocalDevice localDevice, Int32 targetAddress,
            IHdmiControlCallback callback) {
        Super(localDevice);
        mTargetAddress = targetAddress;
        mCallback = callback;
    }

    //@Override
    Boolean Start() {
        SendCommand(HdmiCecMessageBuilder->BuildTextViewOn(GetSourceAddress(), mTargetAddress));
        BroadcastActiveSource();
        QueryDevicePowerStatus();
        mState = STATE_WAITING_FOR_REPORT_POWER_STATUS;
        AddTimer(mState, HdmiConfig.TIMEOUT_MS);
        return TRUE;
    }

    private void BroadcastActiveSource() {
        SendCommand(HdmiCecMessageBuilder->BuildActiveSource(GetSourceAddress(), GetSourcePath()));
        // Because only playback device can create this action, it's safe to cast.
        Playback()->MarkActiveSource();
    }

    private void QueryDevicePowerStatus() {
        SendCommand(HdmiCecMessageBuilder->BuildGiveDevicePowerStatus(GetSourceAddress(),
                mTargetAddress));
    }

    //@Override
    Boolean ProcessCommand(HdmiCecMessage cmd) {
        if (mState != STATE_WAITING_FOR_REPORT_POWER_STATUS) {
            return FALSE;
        }
        if (cmd->GetOpcode() == Constants.MESSAGE_REPORT_POWER_STATUS) {
            Int32 status = cmd->GetParams()[0];
            if (status == HdmiControlManager.POWER_STATUS_ON) {
                BroadcastActiveSource();
                InvokeCallback(HdmiControlManager.RESULT_SUCCESS);
                Finish();
            }
            return TRUE;
        }
        return FALSE;
    }

    //@Override
    void HandleTimerEvent(Int32 state) {
        if (mState != state) {
            return;
        }
        if (state == STATE_WAITING_FOR_REPORT_POWER_STATUS) {
            if (mPowerStatusCounter++ < LOOP_COUNTER_MAX) {
                QueryDevicePowerStatus();
                AddTimer(mState, HdmiConfig.TIMEOUT_MS);
            } else {
                // Couldn't wake up the TV for whatever reason. Report failure.
                InvokeCallback(HdmiControlManager.RESULT_TIMEOUT);
                Finish();
            }
        }
    }

    private void InvokeCallback(Int32 result) {
        try {
            mCallback->OnComplete(result);
        } catch (RemoteException e) {
            Slogger::E(TAG, "Callback failed:" + e);
        }
    }
}
