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

using Elastos::Droid::Server::Ihdmi.HdmiCecLocalDevice.ActiveSource;
using Elastos::IO::IUnsupportedEncodingException;

/**
 * Feature action that discovers the information of a newly found logical device.
 *
 * This action is created when receiving &lt;Report Physical Address&gt;, a CEC command a newly
 * connected HDMI-CEC device broadcasts to announce its advent. Additional commands are issued in
 * this action to gather more information on the device such as OSD name and device vendor ID.
 *
 * <p>The result is made in the form of {@link HdmiDeviceInfo} object, and passed to service
 * for the management through its life cycle.
 *
 * <p>Package-private, accessed by {@link HdmiControlService} only.
 */
final class NewDeviceAction extends HdmiCecFeatureAction {

    private static const String TAG = "NewDeviceAction";

    // State in which the action sent <Give OSD Name> and is waiting for <Set OSD Name>
    // that contains the name of the device for display on screen.
    static const Int32 STATE_WAITING_FOR_SET_OSD_NAME = 1;

    // State in which the action sent <Give Device Vendor ID> and is waiting for
    // <Device Vendor ID> that contains the vendor ID of the device.
    static const Int32 STATE_WAITING_FOR_DEVICE_VENDOR_ID = 2;

    private final Int32 mDeviceLogicalAddress;
    private final Int32 mDevicePhysicalAddress;

    private Int32 mVendorId;
    private String mDisplayName;

    /**
     * Constructor.
     *
     * @param source {@link HdmiCecLocalDevice} instance
     * @param deviceLogicalAddress logical address of the device in interest
     * @param devicePhysicalAddress physical address of the device in interest
     */
    NewDeviceAction(HdmiCecLocalDevice source, Int32 deviceLogicalAddress,
            Int32 devicePhysicalAddress) {
        Super(source);
        mDeviceLogicalAddress = deviceLogicalAddress;
        mDevicePhysicalAddress = devicePhysicalAddress;
        mVendorId = Constants.UNKNOWN_VENDOR_ID;
    }

    //@Override
    public Boolean Start() {
        mState = STATE_WAITING_FOR_SET_OSD_NAME;
        if (MayProcessCommandIfCached(mDeviceLogicalAddress, Constants.MESSAGE_SET_OSD_NAME)) {
            return TRUE;
        }

        SendCommand(HdmiCecMessageBuilder->BuildGiveOsdNameCommand(GetSourceAddress(),
                mDeviceLogicalAddress));
        AddTimer(mState, HdmiConfig.TIMEOUT_MS);
        return TRUE;
    }

    //@Override
    public Boolean ProcessCommand(HdmiCecMessage cmd) {
        // For the logical device in interest, we want two more pieces of information -
        // osd name and vendor id. They are requested in sequence. In case we don't
        // get the expected responses (either by timeout or by receiving <feature abort> command),
        // set them to a default osd name and unknown vendor id respectively.
        Int32 opcode = cmd->GetOpcode();
        Int32 src = cmd->GetSource();
        Byte[] params = cmd->GetParams();

        if (mDeviceLogicalAddress != src) {
            return FALSE;
        }

        if (mState == STATE_WAITING_FOR_SET_OSD_NAME) {
            if (opcode == Constants.MESSAGE_SET_OSD_NAME) {
                try {
                    mDisplayName = new String(params, "US-ASCII");
                } catch (UnsupportedEncodingException e) {
                    Slogger::E(TAG, "Failed to get OSD name: " + e->GetMessage());
                }
                RequestVendorId();
                return TRUE;
            } else if (opcode == Constants.MESSAGE_FEATURE_ABORT) {
                Int32 requestOpcode = params[0] & 0xFF;
                if (requestOpcode == Constants.MESSAGE_GIVE_OSD_NAME) {
                    RequestVendorId();
                    return TRUE;
                }
            }
        } else if (mState == STATE_WAITING_FOR_DEVICE_VENDOR_ID) {
            if (opcode == Constants.MESSAGE_DEVICE_VENDOR_ID) {
                mVendorId = HdmiUtils->ThreeBytesToInt(params);
                AddDeviceInfo();
                Finish();
                return TRUE;
            } else if (opcode == Constants.MESSAGE_FEATURE_ABORT) {
                Int32 requestOpcode = params[0] & 0xFF;
                if (requestOpcode == Constants.MESSAGE_GIVE_DEVICE_VENDOR_ID) {
                    AddDeviceInfo();
                    Finish();
                    return TRUE;
                }
            }
        }
        return FALSE;
    }

    private Boolean MayProcessCommandIfCached(Int32 destAddress, Int32 opcode) {
        HdmiCecMessage message = GetCecMessageCache()->GetMessage(destAddress, opcode);
        if (message != NULL) {
            return ProcessCommand(message);
        }
        return FALSE;
    }

    private void RequestVendorId() {
        // At first, transit to waiting status for <Device Vendor Id>.
        mState = STATE_WAITING_FOR_DEVICE_VENDOR_ID;
        // If the message is already in cache, process it.
        if (MayProcessCommandIfCached(mDeviceLogicalAddress,
                Constants.MESSAGE_DEVICE_VENDOR_ID)) {
            return;
        }
        SendCommand(HdmiCecMessageBuilder->BuildGiveDeviceVendorIdCommand(GetSourceAddress(),
                mDeviceLogicalAddress));
        AddTimer(mState, HdmiConfig.TIMEOUT_MS);
    }

    private void AddDeviceInfo() {
        if (mDisplayName == NULL) {
            mDisplayName = HdmiUtils->GetDefaultDeviceName(mDeviceLogicalAddress);
        }
        HdmiDeviceInfo deviceInfo = new HdmiDeviceInfo(
                mDeviceLogicalAddress, mDevicePhysicalAddress,
                Tv()->GetPortId(mDevicePhysicalAddress),
                HdmiUtils->GetTypeFromAddress(mDeviceLogicalAddress),
                mVendorId, mDisplayName);
        Tv()->AddCecDevice(deviceInfo);

        if (HdmiUtils->GetTypeFromAddress(mDeviceLogicalAddress)
                == HdmiDeviceInfo.DEVICE_AUDIO_SYSTEM) {
            Tv()->OnNewAvrAdded(deviceInfo);
        }
    }

    //@Override
    CARAPI HandleTimerEvent(Int32 state) {
        if (mState == STATE_NONE || mState != state) {
            return;
        }
        if (state == STATE_WAITING_FOR_SET_OSD_NAME) {
            // Osd name request timed out. Try vendor id
            RequestVendorId();
        } else if (state == STATE_WAITING_FOR_DEVICE_VENDOR_ID) {
            // vendor id timed out. Go ahead creating the device info what we've got so far.
            AddDeviceInfo();
            Finish();
        }
    }

    Boolean IsActionOf(ActiveSource activeSource) {
        return (mDeviceLogicalAddress == activeSource.logicalAddress)
                && (mDevicePhysicalAddress == activeSource.physicalAddress);
    }
}
