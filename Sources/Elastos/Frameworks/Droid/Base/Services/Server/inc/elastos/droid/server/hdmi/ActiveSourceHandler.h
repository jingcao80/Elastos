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
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::Server::Ihdmi.HdmiCecLocalDevice.ActiveSource;

/**
 * Handles CEC command &lt;Active Source&gt;.
 * <p>
 * Used by feature actions that need to handle the command in their flow. Only for TV
 * local device.
 */
final class ActiveSourceHandler {
    private static const String TAG = "ActiveSourceHandler";

    private final HdmiCecLocalDeviceTv mSource;
    private final HdmiControlService mService;
    @Nullable
    private final IHdmiControlCallback mCallback;

    static ActiveSourceHandler Create(HdmiCecLocalDeviceTv source, IHdmiControlCallback callback) {
        if (source == NULL) {
            Slogger::E(TAG, "Wrong arguments");
            return NULL;
        }
        return new ActiveSourceHandler(source, callback);
    }

    private ActiveSourceHandler(HdmiCecLocalDeviceTv source, IHdmiControlCallback callback) {
        mSource = source;
        mService = mSource->GetService();
        mCallback = callback;
    }

    /**
     * Handles the incoming active source command.
     *
     * @param newActive new active source information
     */
    void Process(ActiveSource newActive) {
        // Seq #17
        HdmiCecLocalDeviceTv tv = mSource;
        ActiveSource activeSource = tv->GetActiveSource();
        if (activeSource->Equals(newActive)) {
            InvokeCallback(HdmiControlManager.RESULT_SUCCESS);
            return;
        }
        HdmiDeviceInfo device = mService->GetDeviceInfo(newActive.logicalAddress);
        if (device == NULL) {
            tv->StartNewDeviceAction(newActive);
        }

        if (!tv->IsProhibitMode()) {
            tv->UpdateActiveSource(newActive);
            Boolean notifyInputChange = (mCallback == NULL);
            tv->UpdateActiveInput(newActive.physicalAddress, notifyInputChange);
            InvokeCallback(HdmiControlManager.RESULT_SUCCESS);
        } else {
            // TV is in a mode that should keep its current source/input from
            // being changed for its operation. Reclaim the active source
            // or switch the port back to the one used for the current mode.
            ActiveSource current = tv->GetActiveSource();
            if (current.logicalAddress == GetSourceAddress()) {
                HdmiCecMessage activeSourceCommand = HdmiCecMessageBuilder->BuildActiveSource(
                        current.logicalAddress, current.physicalAddress);
                mService->SendCecCommand(activeSourceCommand);
                tv->UpdateActiveSource(current);
                InvokeCallback(HdmiControlManager.RESULT_SUCCESS);
            } else {
                HdmiCecMessage routingChange = HdmiCecMessageBuilder->BuildRoutingChange(
                        GetSourceAddress(), newActive.physicalAddress, current.physicalAddress);
                mService->SendCecCommand(routingChange);
                tv->AddAndStartAction(
                        new RoutingControlAction(tv, current.physicalAddress, TRUE, mCallback));
            }
        }
    }

    private final Int32 GetSourceAddress() {
        return mSource->GetDeviceInfo()->GetLogicalAddress();
    }

    private void InvokeCallback(Int32 result) {
        if (mCallback == NULL) {
            return;
        }
        try {
            mCallback->OnComplete(result);
        } catch (RemoteException e) {
            Slogger::E(TAG, "Callback failed:" + e);
        }
    }
}
