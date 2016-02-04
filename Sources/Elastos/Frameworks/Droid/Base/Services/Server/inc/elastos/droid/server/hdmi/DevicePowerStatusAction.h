package com.android.server.hdmi;

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

using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiPlaybackClient;
using Elastos::Droid::Hardware::Hdmi::IHdmiPlaybackClient.DisplayStatusCallback;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Utility::ISlog;

/**
 * Feature action that queries the power status of other device. This action is initiated via
 * {@link HdmiPlaybackClient#QueryDisplayStatus(DisplayStatusCallback)} from the Android system
 * working as playback device to get the power status of TV device.
 * <p>
 * Package-private, accessed by {@link HdmiControlService} only.
 */
final class DevicePowerStatusAction extends HdmiCecFeatureAction {
    private static const String TAG = "DevicePowerStatusAction";

    // State in which the action is waiting for <Report Power Status>.
    private static const Int32 STATE_WAITING_FOR_REPORT_POWER_STATUS = 1;

    private final Int32 mTargetAddress;
    private final IHdmiControlCallback mCallback;

    static DevicePowerStatusAction Create(HdmiCecLocalDevice source,
            Int32 targetAddress, IHdmiControlCallback callback) {
        if (source == NULL || callback == NULL) {
            Slogger::E(TAG, "Wrong arguments");
            return NULL;
        }
        return new DevicePowerStatusAction(source, targetAddress, callback);
    }

    private DevicePowerStatusAction(HdmiCecLocalDevice localDevice,
            Int32 targetAddress, IHdmiControlCallback callback) {
        Super(localDevice);
        mTargetAddress = targetAddress;
        mCallback = callback;
    }

    //@Override
    Boolean Start() {
        QueryDevicePowerStatus();
        mState = STATE_WAITING_FOR_REPORT_POWER_STATUS;
        AddTimer(mState, HdmiConfig.TIMEOUT_MS);
        return TRUE;
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
            InvokeCallback(status);
            Finish();
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
            // Got no response from TV. Report status 'unknown'.
            InvokeCallback(HdmiControlManager.POWER_STATUS_UNKNOWN);
            Finish();
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
