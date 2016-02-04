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
using Elastos::Droid::Utility::ISparseArray;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IList;

/**
 * Various utilities to handle HDMI CEC messages.
 */
final class HdmiUtils {

    private static const Int32[] ADDRESS_TO_TYPE = {
        HdmiDeviceInfo.DEVICE_TV,  // ADDR_TV
        HdmiDeviceInfo.DEVICE_RECORDER,  // ADDR_RECORDER_1
        HdmiDeviceInfo.DEVICE_RECORDER,  // ADDR_RECORDER_2
        HdmiDeviceInfo.DEVICE_TUNER,  // ADDR_TUNER_1
        HdmiDeviceInfo.DEVICE_PLAYBACK,  // ADDR_PLAYBACK_1
        HdmiDeviceInfo.DEVICE_AUDIO_SYSTEM,  // ADDR_AUDIO_SYSTEM
        HdmiDeviceInfo.DEVICE_TUNER,  // ADDR_TUNER_2
        HdmiDeviceInfo.DEVICE_TUNER,  // ADDR_TUNER_3
        HdmiDeviceInfo.DEVICE_PLAYBACK,  // ADDR_PLAYBACK_2
        HdmiDeviceInfo.DEVICE_RECORDER,  // ADDR_RECORDER_3
        HdmiDeviceInfo.DEVICE_TUNER,  // ADDR_TUNER_4
        HdmiDeviceInfo.DEVICE_PLAYBACK,  // ADDR_PLAYBACK_3
        HdmiDeviceInfo.DEVICE_RESERVED,
        HdmiDeviceInfo.DEVICE_RESERVED,
        HdmiDeviceInfo.DEVICE_TV,  // ADDR_SPECIFIC_USE
    };

    private static const String[] DEFAULT_NAMES = {
        "TV",
        "Recorder_1",
        "Recorder_2",
        "Tuner_1",
        "Playback_1",
        "AudioSystem",
        "Tuner_2",
        "Tuner_3",
        "Playback_2",
        "Recorder_3",
        "Tuner_4",
        "Playback_3",
        "Reserved_1",
        "Reserved_2",
        "Secondary_TV",
    };

    private HdmiUtils() { /* cannot be instantiated */ }

    /**
     * Check if the given logical address is valid. A logical address is valid
     * if it is one allocated for an actual device which allows communication
     * with other logical devices.
     *
     * @param address logical address
     * @return TRUE if the given address is valid
     */
    static Boolean IsValidAddress(Int32 address) {
        return (Constants.ADDR_TV <= address && address <= Constants.ADDR_SPECIFIC_USE);
    }

    /**
     * Return the device type for the given logical address.
     *
     * @param address logical address
     * @return device type for the given logical address; DEVICE_INACTIVE
     *         if the address is not valid.
     */
    static Int32 GetTypeFromAddress(Int32 address) {
        if (IsValidAddress(address)) {
            return ADDRESS_TO_TYPE[address];
        }
        return HdmiDeviceInfo.DEVICE_INACTIVE;
    }

    /**
     * Return the default device name for a logical address. This is the name
     * by which the logical device is known to others until a name is
     * set explicitly using HdmiCecService.setOsdName.
     *
     * @param address logical address
     * @return default device name; empty string if the address is not valid
     */
    static String GetDefaultDeviceName(Int32 address) {
        if (IsValidAddress(address)) {
            return DEFAULT_NAMES[address];
        }
        return "";
    }

    /**
     * Verify if the given address is for the given device type.  If not it will throw
     * {@link IllegalArgumentException}.
     *
     * @param logicalAddress the logical address to verify
     * @param deviceType the device type to check
     * @throw IllegalArgumentException
     */
    static void VerifyAddressType(Int32 logicalAddress, Int32 deviceType) {
        Int32 actualDeviceType = GetTypeFromAddress(logicalAddress);
        if (actualDeviceType != deviceType) {
            throw new IllegalArgumentException("Device type missmatch:[Expected:" + deviceType
                    + ", Actual:" + actualDeviceType);
        }
    }

    /**
     * Check if the given CEC message come from the given address.
     *
     * @param cmd the CEC message to check
     * @param expectedAddress the expected source address of the given message
     * @param tag the tag of caller module (for log message)
     * @return TRUE if the CEC message comes from the given address
     */
    static Boolean CheckCommandSource(HdmiCecMessage cmd, Int32 expectedAddress, String tag) {
        Int32 src = cmd->GetSource();
        if (src != expectedAddress) {
            Slogger::W(tag, "Invalid source [Expected:" + expectedAddress + ", Actual:" + src + "]");
            return FALSE;
        }
        return TRUE;
    }

    /**
     * Parse the parameter block of CEC message as [System Audio Status].
     *
     * @param cmd the CEC message to parse
     * @return TRUE if the given parameter has [ON] value
     */
    static Boolean ParseCommandParamSystemAudioStatus(HdmiCecMessage cmd) {
        return cmd->GetParams()[0] == Constants.SYSTEM_AUDIO_STATUS_ON;
    }

    /**
     * Convert integer array to list of {@link Integer}.
     *
     * <p>The result is immutable.
     *
     * @param is integer array
     * @return {@link List} instance containing the elements in the given array
     */
    static List<Integer> AsImmutableList(final Int32[] is) {
        ArrayList<Integer> list = new ArrayList<>(is.length);
        for (Int32 type : is) {
            list->Add(type);
        }
        return Collections->UnmodifiableList(list);
    }

    /**
     * Assemble two bytes into single integer value.
     *
     * @param data to be assembled
     * @return assembled value
     */
    static Int32 TwoBytesToInt(Byte[] data) {
        return ((data[0] & 0xFF) << 8) | (data[1] & 0xFF);
    }

    /**
     * Assemble two bytes into single integer value.
     *
     * @param data to be assembled
     * @param offset offset to the data to convert in the array
     * @return assembled value
     */
    static Int32 TwoBytesToInt(Byte[] data, Int32 offset) {
        return ((data[offset] & 0xFF) << 8) | (data[offset + 1] & 0xFF);
    }

    /**
     * Assemble three bytes into single integer value.
     *
     * @param data to be assembled
     * @return assembled value
     */
    static Int32 ThreeBytesToInt(Byte[] data) {
        return ((data[0] & 0xFF) << 16) | ((data[1] & 0xFF) << 8) | (data[2] & 0xFF);
    }

    static <T> List<T> SparseArrayToList(SparseArray<T> array) {
        ArrayList<T> list = new ArrayList<>();
        for (Int32 i = 0; i < array->Size(); ++i) {
            list->Add(array->ValueAt(i));
        }
        return list;
    }

    static <T> List<T> MergeToUnmodifiableList(List<T> a, List<T> b) {
        if (a->IsEmpty() && b->IsEmpty()) {
            return Collections->EmptyList();
        }
        if (a->IsEmpty()) {
            return Collections->UnmodifiableList(b);
        }
        if (b->IsEmpty()) {
            return Collections->UnmodifiableList(a);
        }
        List<T> newList = new ArrayList<>();
        newList->AddAll(a);
        newList->AddAll(b);
        return Collections->UnmodifiableList(newList);
    }

    /**
     * See if the new path is affecting the active path.
     *
     * @param activePath current active path
     * @param newPath new path
     * @return TRUE if the new path changes the current active path
     */
    static Boolean IsAffectingActiveRoutingPath(Int32 activePath, Int32 newPath) {
        // The new path affects the current active path if the parent of the new path
        // is an ancestor of the active path.
        // (1.1.0.0, 2.0.0.0) -> TRUE, new path alters the parent
        // (1.1.0.0, 1.2.0.0) -> TRUE, new path is a sibling
        // (1.1.0.0, 1.2.1.0) -> FALSE, new path is a descendant of a sibling
        // (1.0.0.0, 3.2.0.0) -> FALSE, in a completely different path

        // Get the parent of the new path by clearing the least significant
        // non-zero nibble.
        for (Int32 i = 0; i <= 12; i += 4) {
            Int32 nibble = (newPath >> i) & 0xF;
            if (nibble != 0) {
                Int32 mask = 0xFFF0 << i;
                newPath &= mask;
                break;
            }
        }
        if (newPath == 0x0000) {
            return TRUE;  // Top path always affects the active path
        }
        return IsInActiveRoutingPath(activePath, newPath);
    }

    /**
     * See if the new path is in the active path.
     *
     * @param activePath current active path
     * @param newPath new path
     * @return TRUE if the new path in the active routing path
     */
    static Boolean IsInActiveRoutingPath(Int32 activePath, Int32 newPath) {
        // Check each nibble of the currently active path and the new path till the position
        // where the active nibble is not zero. For (activePath, newPath),
        // (1.1.0.0, 1.0.0.0) -> TRUE, new path is a parent
        // (1.2.1.0, 1.2.1.2) -> TRUE, new path is a descendant
        // (1.1.0.0, 1.2.0.0) -> FALSE, new path is a sibling
        // (1.0.0.0, 2.0.0.0) -> FALSE, in a completely different path
        for (Int32 i = 12; i >= 0; i -= 4) {
            Int32 nibbleActive = (activePath >> i) & 0xF;
            if (nibbleActive == 0) {
                break;
            }
            Int32 nibbleNew = (newPath >> i) & 0xF;
            if (nibbleNew == 0) {
                break;
            }
            if (nibbleActive != nibbleNew) {
                return FALSE;
            }
        }
        return TRUE;
    }

    /**
     * Clone {@link HdmiDeviceInfo} with new power status.
     */
    static HdmiDeviceInfo CloneHdmiDeviceInfo(HdmiDeviceInfo info, Int32 newPowerStatus) {
        return new HdmiDeviceInfo(info->GetLogicalAddress(),
                info->GetPhysicalAddress(), info->GetPortId(), info->GetDeviceType(),
                info->GetVendorId(), info->GetDisplayName(), newPowerStatus);
    }

}
