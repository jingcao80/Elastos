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
using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::internal.util.Preconditions;
using Elastos::Droid::Server::Ihdmi.HdmiControlService.DevicePollingCallback;

using Elastos::IO::IUnsupportedEncodingException;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

/**
 * Feature action that handles device discovery sequences.
 * Device discovery is launched when TV device is woken from "Standby" state
 * or enabled "Control for Hdmi" from disabled state.
 *
 * <p>Device discovery goes through the following steps.
 * <ol>
 *   <li>Poll all non-local devices by sending &lt;Polling Message&gt;
 *   <li>Gather "Physical address" and "device type" of all acknowledged devices
 *   <li>Gather "OSD (display) name" of all acknowledge devices
 *   <li>Gather "Vendor id" of all acknowledge devices
 * </ol>
 */
final class DeviceDiscoveryAction extends HdmiCecFeatureAction {
    private static const String TAG = "DeviceDiscoveryAction";

    // State in which the action is waiting for device polling.
    private static const Int32 STATE_WAITING_FOR_DEVICE_POLLING = 1;
    // State in which the action is waiting for gathering physical address of non-local devices.
    private static const Int32 STATE_WAITING_FOR_PHYSICAL_ADDRESS = 2;
    // State in which the action is waiting for gathering osd name of non-local devices.
    private static const Int32 STATE_WAITING_FOR_OSD_NAME = 3;
    // State in which the action is waiting for gathering vendor id of non-local devices.
    private static const Int32 STATE_WAITING_FOR_VENDOR_ID = 4;

    /**
     * Interface used to report result of device discovery.
     */
    interface DeviceDiscoveryCallback {
        /**
         * Called when device discovery is done.
         *
         * @param deviceInfos a list of all non-local devices. It can be empty list.
         */
        void OnDeviceDiscoveryDone(List<HdmiDeviceInfo> deviceInfos);
    }

    // An internal container used to keep track of device information during
    // this action.
    private static const class DeviceInfo {
        private final Int32 mLogicalAddress;

        private Int32 mPhysicalAddress = Constants.INVALID_PHYSICAL_ADDRESS;
        private Int32 mPortId = Constants.INVALID_PORT_ID;
        private Int32 mVendorId = Constants.UNKNOWN_VENDOR_ID;
        private String mDisplayName = "";
        private Int32 mDeviceType = HdmiDeviceInfo.DEVICE_INACTIVE;

        private DeviceInfo(Int32 logicalAddress) {
            mLogicalAddress = logicalAddress;
        }

        private HdmiDeviceInfo ToHdmiDeviceInfo() {
            return new HdmiDeviceInfo(mLogicalAddress, mPhysicalAddress, mPortId, mDeviceType,
                    mVendorId, mDisplayName);
        }
    }

    private final ArrayList<DeviceInfo> mDevices = new ArrayList<>();
    private final DeviceDiscoveryCallback mCallback;
    private Int32 mProcessedDeviceCount = 0;

    /**
     * Constructor.
     *
     * @param source an instance of {@link HdmiCecLocalDevice}.
     */
    DeviceDiscoveryAction(HdmiCecLocalDevice source, DeviceDiscoveryCallback callback) {
        Super(source);
        mCallback = Preconditions->CheckNotNull(callback);
    }

    //@Override
    Boolean Start() {
        mDevices->Clear();
        mState = STATE_WAITING_FOR_DEVICE_POLLING;

        PollDevices(new DevicePollingCallback() {
            //@Override
            CARAPI OnPollingFinished(List<Integer> ackedAddress) {
                if (ackedAddress->IsEmpty()) {
                    Slogger::V(TAG, "No device is detected.");
                    WrapUpAndFinish();
                    return;
                }

                Slogger::V(TAG, "Device detected: " + ackedAddress);
                AllocateDevices(ackedAddress);
                StartPhysicalAddressStage();
            }
        }, Constants.POLL_ITERATION_REVERSE_ORDER
            | Constants.POLL_STRATEGY_REMOTES_DEVICES, HdmiConfig.DEVICE_POLLING_RETRY);
        return TRUE;
    }

    private void AllocateDevices(List<Integer> addresses) {
        for (Integer i : addresses) {
            DeviceInfo info = new DeviceInfo(i);
            mDevices->Add(info);
        }
    }

    private void StartPhysicalAddressStage() {
        Slogger::V(TAG, "Start [Physical Address Stage]:" + mDevices->Size());
        mProcessedDeviceCount = 0;
        mState = STATE_WAITING_FOR_PHYSICAL_ADDRESS;

        CheckAndProceedStage();
    }

    private Boolean VerifyValidLogicalAddress(Int32 address) {
        return address >= Constants.ADDR_TV && address < Constants.ADDR_UNREGISTERED;
    }

    private void QueryPhysicalAddress(Int32 address) {
        if (!VerifyValidLogicalAddress(address)) {
            CheckAndProceedStage();
            return;
        }

        mActionTimer->ClearTimerMessage();

        // Check cache first and send request if not exist.
        if (MayProcessMessageIfCached(address, Constants.MESSAGE_REPORT_PHYSICAL_ADDRESS)) {
            return;
        }
        SendCommand(HdmiCecMessageBuilder->BuildGivePhysicalAddress(GetSourceAddress(), address));
        AddTimer(mState, HdmiConfig.TIMEOUT_MS);
    }

    private void StartOsdNameStage() {
        Slogger::V(TAG, "Start [Osd Name Stage]:" + mDevices->Size());
        mProcessedDeviceCount = 0;
        mState = STATE_WAITING_FOR_OSD_NAME;

        CheckAndProceedStage();
    }

    private void QueryOsdName(Int32 address) {
        if (!VerifyValidLogicalAddress(address)) {
            CheckAndProceedStage();
            return;
        }

        mActionTimer->ClearTimerMessage();

        if (MayProcessMessageIfCached(address, Constants.MESSAGE_SET_OSD_NAME)) {
            return;
        }
        SendCommand(HdmiCecMessageBuilder->BuildGiveOsdNameCommand(GetSourceAddress(), address));
        AddTimer(mState, HdmiConfig.TIMEOUT_MS);
    }

    private void StartVendorIdStage() {
        Slogger::V(TAG, "Start [Vendor Id Stage]:" + mDevices->Size());

        mProcessedDeviceCount = 0;
        mState = STATE_WAITING_FOR_VENDOR_ID;

        CheckAndProceedStage();
    }

    private void QueryVendorId(Int32 address) {
        if (!VerifyValidLogicalAddress(address)) {
            CheckAndProceedStage();
            return;
        }

        mActionTimer->ClearTimerMessage();

        if (MayProcessMessageIfCached(address, Constants.MESSAGE_DEVICE_VENDOR_ID)) {
            return;
        }
        SendCommand(
                HdmiCecMessageBuilder->BuildGiveDeviceVendorIdCommand(GetSourceAddress(), address));
        AddTimer(mState, HdmiConfig.TIMEOUT_MS);
    }

    private Boolean MayProcessMessageIfCached(Int32 address, Int32 opcode) {
        HdmiCecMessage message = GetCecMessageCache()->GetMessage(address, opcode);
        if (message != NULL) {
            ProcessCommand(message);
            return TRUE;
        }
        return FALSE;
    }

    //@Override
    Boolean ProcessCommand(HdmiCecMessage cmd) {
        switch (mState) {
            case STATE_WAITING_FOR_PHYSICAL_ADDRESS:
                if (cmd->GetOpcode() == Constants.MESSAGE_REPORT_PHYSICAL_ADDRESS) {
                    HandleReportPhysicalAddress(cmd);
                    return TRUE;
                }
                return FALSE;
            case STATE_WAITING_FOR_OSD_NAME:
                if (cmd->GetOpcode() == Constants.MESSAGE_SET_OSD_NAME) {
                    HandleSetOsdName(cmd);
                    return TRUE;
                }
                return FALSE;
            case STATE_WAITING_FOR_VENDOR_ID:
                if (cmd->GetOpcode() == Constants.MESSAGE_DEVICE_VENDOR_ID) {
                    HandleVendorId(cmd);
                    return TRUE;
                }
                return FALSE;
            case STATE_WAITING_FOR_DEVICE_POLLING:
                // Fall through.
            default:
                return FALSE;
        }
    }

    private void HandleReportPhysicalAddress(HdmiCecMessage cmd) {
        Preconditions->CheckState(mProcessedDeviceCount < mDevices->Size());

        DeviceInfo current = mDevices->Get(mProcessedDeviceCount);
        if (current.mLogicalAddress != cmd->GetSource()) {
            Slogger::W(TAG, "Unmatched address[expected:" + current.mLogicalAddress + ", actual:" +
                    cmd->GetSource());
            return;
        }

        Byte params[] = cmd->GetParams();
        current.mPhysicalAddress = HdmiUtils->TwoBytesToInt(params);
        current.mPortId = GetPortId(current.mPhysicalAddress);
        current.mDeviceType = params[2] & 0xFF;

        Tv()->UpdateCecSwitchInfo(current.mLogicalAddress, current.mDeviceType,
                    current.mPhysicalAddress);

        IncreaseProcessedDeviceCount();
        CheckAndProceedStage();
    }

    private Int32 GetPortId(Int32 physicalAddress) {
        return Tv()->GetPortId(physicalAddress);
    }

    private void HandleSetOsdName(HdmiCecMessage cmd) {
        Preconditions->CheckState(mProcessedDeviceCount < mDevices->Size());

        DeviceInfo current = mDevices->Get(mProcessedDeviceCount);
        if (current.mLogicalAddress != cmd->GetSource()) {
            Slogger::W(TAG, "Unmatched address[expected:" + current.mLogicalAddress + ", actual:" +
                    cmd->GetSource());
            return;
        }

        String displayName = NULL;
        try {
            displayName = new String(cmd->GetParams(), "US-ASCII");
        } catch (UnsupportedEncodingException e) {
            Slogger::W(TAG, "Failed to decode display name: " + cmd->ToString());
            // If failed to get display name, use the default name of device.
            displayName = HdmiUtils->GetDefaultDeviceName(current.mLogicalAddress);
        }
        current.mDisplayName = displayName;
        IncreaseProcessedDeviceCount();
        CheckAndProceedStage();
    }

    private void HandleVendorId(HdmiCecMessage cmd) {
        Preconditions->CheckState(mProcessedDeviceCount < mDevices->Size());

        DeviceInfo current = mDevices->Get(mProcessedDeviceCount);
        if (current.mLogicalAddress != cmd->GetSource()) {
            Slogger::W(TAG, "Unmatched address[expected:" + current.mLogicalAddress + ", actual:" +
                    cmd->GetSource());
            return;
        }

        Byte[] params = cmd->GetParams();
        Int32 vendorId = HdmiUtils->ThreeBytesToInt(params);
        current.mVendorId = vendorId;
        IncreaseProcessedDeviceCount();
        CheckAndProceedStage();
    }

    private void IncreaseProcessedDeviceCount() {
        mProcessedDeviceCount++;
    }

    private void RemoveDevice(Int32 index) {
        mDevices->Remove(index);
    }

    private void WrapUpAndFinish() {
        Slogger::V(TAG, "---------Wrap up Device Discovery:[" + mDevices->Size() + "]---------");
        ArrayList<HdmiDeviceInfo> result = new ArrayList<>();
        for (DeviceInfo info : mDevices) {
            HdmiDeviceInfo cecDeviceInfo = info->ToHdmiDeviceInfo();
            Slogger::V(TAG, " DeviceInfo: " + cecDeviceInfo);
            result->Add(cecDeviceInfo);
        }
        Slogger::V(TAG, "--------------------------------------------");
        mCallback->OnDeviceDiscoveryDone(result);
        Finish();
    }

    private void CheckAndProceedStage() {
        if (mDevices->IsEmpty()) {
            WrapUpAndFinish();
            return;
        }

        // If finished current stage, move on to next stage.
        if (mProcessedDeviceCount == mDevices->Size()) {
            mProcessedDeviceCount = 0;
            switch (mState) {
                case STATE_WAITING_FOR_PHYSICAL_ADDRESS:
                    StartOsdNameStage();
                    return;
                case STATE_WAITING_FOR_OSD_NAME:
                    StartVendorIdStage();
                    return;
                case STATE_WAITING_FOR_VENDOR_ID:
                    WrapUpAndFinish();
                    return;
                default:
                    return;
            }
        } else {
            Int32 address = mDevices->Get(mProcessedDeviceCount).mLogicalAddress;
            switch (mState) {
                case STATE_WAITING_FOR_PHYSICAL_ADDRESS:
                    QueryPhysicalAddress(address);
                    return;
                case STATE_WAITING_FOR_OSD_NAME:
                    QueryOsdName(address);
                    return;
                case STATE_WAITING_FOR_VENDOR_ID:
                    QueryVendorId(address);
                default:
                    return;
            }
        }
    }

    //@Override
    void HandleTimerEvent(Int32 state) {
        if (mState == STATE_NONE || mState != state) {
            return;
        }

        Slogger::V(TAG, "Timeout[State=" + mState + ", Processed=" + mProcessedDeviceCount);
        RemoveDevice(mProcessedDeviceCount);
        CheckAndProceedStage();
    }
}
