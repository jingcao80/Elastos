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
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::internal.util.IndentingPrintWriter;
using Elastos::Droid::Server::Ihdmi.HdmiAnnotations.ServiceThreadOnly;

/**
 * Represent a logical device of type Playback residing in Android system.
 */
final class HdmiCecLocalDevicePlayback extends HdmiCecLocalDevice {
    private static const String TAG = "HdmiCecLocalDevicePlayback";

    private Boolean mIsActiveSource = FALSE;

    HdmiCecLocalDevicePlayback(HdmiControlService service) {
        Super(service, HdmiDeviceInfo.DEVICE_PLAYBACK);
    }

    //@Override
    void Init() {
        super->Init();
        mIsActiveSource = FALSE;
    }

    //@Override
    @ServiceThreadOnly
    protected void OnAddressAllocated(Int32 logicalAddress, Int32 reason) {
        AssertRunOnServiceThread();
        mService->SendCecCommand(HdmiCecMessageBuilder->BuildReportPhysicalAddressCommand(
                mAddress, mService->GetPhysicalAddress(), mDeviceType));
        StartQueuedActions();
    }

    //@Override
    @ServiceThreadOnly
    protected Int32 GetPreferredAddress() {
        AssertRunOnServiceThread();
        return SystemProperties->GetInt(Constants.PROPERTY_PREFERRED_ADDRESS_PLAYBACK,
                Constants.ADDR_UNREGISTERED);
    }

    //@Override
    @ServiceThreadOnly
    protected void SetPreferredAddress(Int32 addr) {
        AssertRunOnServiceThread();
        SystemProperties->Set(Constants.PROPERTY_PREFERRED_ADDRESS_PLAYBACK,
                String->ValueOf(addr));
    }

    @ServiceThreadOnly
    void OneTouchPlay(IHdmiControlCallback callback) {
        AssertRunOnServiceThread();
        if (HasAction(OneTouchPlayAction.class)) {
            Slogger::W(TAG, "oneTouchPlay already in progress");
            InvokeCallback(callback, HdmiControlManager.RESULT_ALREADY_IN_PROGRESS);
            return;
        }

        // TODO: Consider the case of multiple TV sets. For now we always direct the command
        //       to the primary one.
        OneTouchPlayAction action = OneTouchPlayAction->Create(this, Constants.ADDR_TV,
                callback);
        if (action == NULL) {
            Slogger::W(TAG, "Cannot initiate oneTouchPlay");
            InvokeCallback(callback, HdmiControlManager.RESULT_EXCEPTION);
            return;
        }
        AddAndStartAction(action);
    }

    @ServiceThreadOnly
    void QueryDisplayStatus(IHdmiControlCallback callback) {
        AssertRunOnServiceThread();
        if (HasAction(DevicePowerStatusAction.class)) {
            Slogger::W(TAG, "queryDisplayStatus already in progress");
            InvokeCallback(callback, HdmiControlManager.RESULT_ALREADY_IN_PROGRESS);
            return;
        }
        DevicePowerStatusAction action = DevicePowerStatusAction->Create(this,
                Constants.ADDR_TV, callback);
        if (action == NULL) {
            Slogger::W(TAG, "Cannot initiate queryDisplayStatus");
            InvokeCallback(callback, HdmiControlManager.RESULT_EXCEPTION);
            return;
        }
        AddAndStartAction(action);
    }

    @ServiceThreadOnly
    private void InvokeCallback(IHdmiControlCallback callback, Int32 result) {
        AssertRunOnServiceThread();
        try {
            callback->OnComplete(result);
        } catch (RemoteException e) {
            Slogger::E(TAG, "Invoking callback failed:" + e);
        }
    }

    //@Override
    @ServiceThreadOnly
    void OnHotplug(Int32 portId, Boolean connected) {
        AssertRunOnServiceThread();
        mCecMessageCache->FlushAll();
        // We'll not clear mIsActiveSource on the hotplug event to pass CETC 11.2.2-2 ~ 3.
        if (connected && mService->IsPowerStandbyOrTransient()) {
            mService->WakeUp();
        }
    }

    @ServiceThreadOnly
    void MarkActiveSource() {
        AssertRunOnServiceThread();
        mIsActiveSource = TRUE;
    }

    //@Override
    @ServiceThreadOnly
    protected Boolean HandleActiveSource(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        Int32 physicalAddress = HdmiUtils->TwoBytesToInt(message->GetParams());
        MayResetActiveSource(physicalAddress);
        return TRUE;  // Broadcast message.
    }

    private void MayResetActiveSource(Int32 physicalAddress) {
        if (physicalAddress != mService->GetPhysicalAddress()) {
            mIsActiveSource = FALSE;
        }
    }

    //@Override
    @ServiceThreadOnly
    protected Boolean HandleSetStreamPath(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        Int32 physicalAddress = HdmiUtils->TwoBytesToInt(message->GetParams());
        MaySetActiveSource(physicalAddress);
        MaySendActiveSource();
        WakeUpIfActiveSource();
        return TRUE;  // Broadcast message.
    }

    // Samsung model, we tested, sends <RoutingChange> and <RequestActiveSource> consecutively,
    // Then if there is no <ActiveSource> response, it will change the input to
    // the internal source.  To handle this, we'll set ActiveSource aggressively.
    //@Override
    @ServiceThreadOnly
    protected Boolean HandleRoutingChange(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        Int32 newPath = HdmiUtils->TwoBytesToInt(message->GetParams(), 2);
        MaySetActiveSource(newPath);
        return TRUE;  // Broadcast message.
    }

    //@Override
    @ServiceThreadOnly
    protected Boolean HandleRoutingInformation(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        Int32 physicalAddress = HdmiUtils->TwoBytesToInt(message->GetParams());
        MaySetActiveSource(physicalAddress);
        return TRUE;  // Broadcast message.
    }

    private void MaySetActiveSource(Int32 physicalAddress) {
        if (physicalAddress == mService->GetPhysicalAddress()) {
            mIsActiveSource = TRUE;
        } else {
            mIsActiveSource = FALSE;
        }
    }

    private void WakeUpIfActiveSource() {
        if (mIsActiveSource && mService->IsPowerStandbyOrTransient()) {
            mService->WakeUp();
        }
    }

    private void MaySendActiveSource() {
        if (mIsActiveSource) {
            mService->SendCecCommand(HdmiCecMessageBuilder->BuildActiveSource(
                    mAddress, mService->GetPhysicalAddress()));
        }
    }

    //@Override
    @ServiceThreadOnly
    protected Boolean HandleRequestActiveSource(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        MaySendActiveSource();
        return TRUE;  // Broadcast message.
    }

    //@Override
    @ServiceThreadOnly
    protected void DisableDevice(Boolean initiatedByCec, PendingActionClearedCallback callback) {
        super->DisableDevice(initiatedByCec, callback);

        AssertRunOnServiceThread();
        if (!initiatedByCec && mIsActiveSource) {
            mService->SendCecCommand(HdmiCecMessageBuilder->BuildInactiveSource(
                    mAddress, mService->GetPhysicalAddress()));
        }
        mIsActiveSource = FALSE;
        CheckIfPendingActionsCleared();
    }

    //@Override
    protected void Dump(final IndentingPrintWriter pw) {
        super->Dump(pw);
        pw->Println("mIsActiveSource: " + mIsActiveSource);
    }
}