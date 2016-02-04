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

using Elastos::IO::IUnsupportedEncodingException;
using Elastos::Utility::IArrays;

/**
 * A helper class to build {@link HdmiCecMessage} from various cec commands.
 */
public class HdmiCecMessageBuilder {
    private static const Int32 OSD_NAME_MAX_LENGTH = 13;

    private HdmiCecMessageBuilder() {}

    /**
     * Build {@link HdmiCecMessage} from raw data.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param body body of message. It includes opcode.
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage Of(Int32 src, Int32 dest, Byte[] body) {
        Byte opcode = body[0];
        Byte params[] = Arrays->CopyOfRange(body, 1, body.length);
        return new HdmiCecMessage(src, dest, opcode, params);
    }

    /**
     * Build &lt;Feature Abort&gt; command. &lt;Feature Abort&gt; consists of
     * 1 Byte original opcode and 1 Byte reason fields with basic fields.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param originalOpcode original opcode causing feature abort
     * @param reason reason of feature abort
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildFeatureAbortCommand(Int32 src, Int32 dest, Int32 originalOpcode,
            Int32 reason) {
        Byte[] params = new Byte[] {
                (Byte) (originalOpcode & 0xFF),
                (Byte) (reason & 0xFF),
        };
        return BuildCommand(src, dest, Constants.MESSAGE_FEATURE_ABORT, params);
    }

    /**
     * Build &lt;Give Physical Address&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildGivePhysicalAddress(Int32 src, Int32 dest) {
        return BuildCommand(src, dest, Constants.MESSAGE_GIVE_PHYSICAL_ADDRESS);
    }

    /**
     * Build &lt;Give Osd Name&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildGiveOsdNameCommand(Int32 src, Int32 dest) {
        return BuildCommand(src, dest, Constants.MESSAGE_GIVE_OSD_NAME);
    }

    /**
     * Build &lt;Give Vendor Id Command&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildGiveDeviceVendorIdCommand(Int32 src, Int32 dest) {
        return BuildCommand(src, dest, Constants.MESSAGE_GIVE_DEVICE_VENDOR_ID);
    }

    /**
     * Build &lt;Set Menu Language &gt; command.
     *
     * <p>This is a broadcast message sent to all devices on the bus.
     *
     * @param src source address of command
     * @param language 3-letter ISO639-2 based language code
     * @return newly created {@link HdmiCecMessage} if language is valid.
     *         Otherwise, return NULL
     */
    static HdmiCecMessage BuildSetMenuLanguageCommand(Int32 src, String language) {
        if (language->Length() != 3) {
            return NULL;
        }
        // Hdmi CEC uses lower-cased ISO 639-2 (3 letters code).
        String normalized = language->ToLowerCase();
        Byte[] params = new Byte[] {
                (Byte) (normalized->CharAt(0) & 0xFF),
                (Byte) (normalized->CharAt(1) & 0xFF),
                (Byte) (normalized->CharAt(2) & 0xFF),
        };
        // <Set Menu Language> is broadcast message.
        return BuildCommand(src, Constants.ADDR_BROADCAST,
                Constants.MESSAGE_SET_MENU_LANGUAGE, params);
    }

    /**
     * Build &lt;Set Osd Name &gt; command.
     *
     * @param src source address of command
     * @param name display (OSD) name of device
     * @return newly created {@link HdmiCecMessage} if valid name. Otherwise,
     *         return NULL
     */
    static HdmiCecMessage BuildSetOsdNameCommand(Int32 src, Int32 dest, String name) {
        Int32 length = Math->Min(name->Length(), OSD_NAME_MAX_LENGTH);
        Byte[] params;
        try {
            params = name->Substring(0, length).GetBytes("US-ASCII");
        } catch (UnsupportedEncodingException e) {
            return NULL;
        }
        return BuildCommand(src, dest, Constants.MESSAGE_SET_OSD_NAME, params);
    }

    /**
     * Build &lt;Report Physical Address&gt; command. It has two bytes physical
     * address and one Byte device type as parameter.
     *
     * <p>This is a broadcast message sent to all devices on the bus.
     *
     * @param src source address of command
     * @param address physical address of device
     * @param deviceType type of device
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildReportPhysicalAddressCommand(Int32 src, Int32 address, Int32 deviceType) {
        Byte[] params = new Byte[] {
                // Two bytes for physical address
                (Byte) ((address >> 8) & 0xFF),
                (Byte) (address & 0xFF),
                // One Byte device type
                (Byte) (deviceType & 0xFF)
        };
        // <Report Physical Address> is broadcast message.
        return BuildCommand(src, Constants.ADDR_BROADCAST,
                Constants.MESSAGE_REPORT_PHYSICAL_ADDRESS, params);
    }

    /**
     * Build &lt;Device Vendor Id&gt; command. It has three bytes vendor id as
     * parameter.
     *
     * <p>This is a broadcast message sent to all devices on the bus.
     *
     * @param src source address of command
     * @param vendorId device's vendor id
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildDeviceVendorIdCommand(Int32 src, Int32 vendorId) {
        Byte[] params = new Byte[] {
                (Byte) ((vendorId >> 16) & 0xFF),
                (Byte) ((vendorId >> 8) & 0xFF),
                (Byte) (vendorId & 0xFF)
        };
        // <Device Vendor Id> is broadcast message.
        return BuildCommand(src, Constants.ADDR_BROADCAST,
                Constants.MESSAGE_DEVICE_VENDOR_ID, params);
    }

    /**
     * Build &lt;Device Vendor Id&gt; command. It has one Byte cec version as parameter.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param version version of cec. Use 0x04 for "Version 1.3a" and 0x05 for
     *                "Version 1.4 or 1.4a or 1.4b
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildCecVersion(Int32 src, Int32 dest, Int32 version) {
        Byte[] params = new Byte[] {
                (Byte) (version & 0xFF)
        };
        return BuildCommand(src, dest, Constants.MESSAGE_CEC_VERSION, params);
    }

    /**
     * Build &lt;Request Arc Initiation&gt;
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildRequestArcInitiation(Int32 src, Int32 dest) {
        return BuildCommand(src, dest, Constants.MESSAGE_REQUEST_ARC_INITIATION);
    }

    /**
     * Build &lt;Request Arc Termination&gt;
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildRequestArcTermination(Int32 src, Int32 dest) {
        return BuildCommand(src, dest, Constants.MESSAGE_REQUEST_ARC_TERMINATION);
    }

    /**
     * Build &lt;Report Arc Initiated&gt;
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildReportArcInitiated(Int32 src, Int32 dest) {
        return BuildCommand(src, dest, Constants.MESSAGE_REPORT_ARC_INITIATED);
    }

    /**
     * Build &lt;Report Arc Terminated&gt;
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildReportArcTerminated(Int32 src, Int32 dest) {
        return BuildCommand(src, dest, Constants.MESSAGE_REPORT_ARC_TERMINATED);
    }

    /**
     * Build &lt;Text View On&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildTextViewOn(Int32 src, Int32 dest) {
        return BuildCommand(src, dest, Constants.MESSAGE_TEXT_VIEW_ON);
    }

    /**
     * Build &lt;Active Source&gt; command.
     *
     * @param src source address of command
     * @param physicalAddress physical address of the device to become active
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildActiveSource(Int32 src, Int32 physicalAddress) {
        return BuildCommand(src, Constants.ADDR_BROADCAST, Constants.MESSAGE_ACTIVE_SOURCE,
                PhysicalAddressToParam(physicalAddress));
    }

    /**
     * Build &lt;Inactive Source&gt; command.
     *
     * @param src source address of command
     * @param physicalAddress physical address of the device to become inactive
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildInactiveSource(Int32 src, Int32 physicalAddress) {
        return BuildCommand(src, Constants.ADDR_TV,
                Constants.MESSAGE_INACTIVE_SOURCE, PhysicalAddressToParam(physicalAddress));
    }

    /**
     * Build &lt;Set Stream Path&gt; command.
     *
     * <p>This is a broadcast message sent to all devices on the bus.
     *
     * @param src source address of command
     * @param streamPath physical address of the device to start streaming
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildSetStreamPath(Int32 src, Int32 streamPath) {
        return BuildCommand(src, Constants.ADDR_BROADCAST,
                Constants.MESSAGE_SET_STREAM_PATH, PhysicalAddressToParam(streamPath));
    }

    /**
     * Build &lt;Routing Change&gt; command.
     *
     * <p>This is a broadcast message sent to all devices on the bus.
     *
     * @param src source address of command
     * @param oldPath physical address of the currently active routing path
     * @param newPath physical address of the new active routing path
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildRoutingChange(Int32 src, Int32 oldPath, Int32 newPath) {
        Byte[] param = new Byte[] {
            (Byte) ((oldPath >> 8) & 0xFF), (Byte) (oldPath & 0xFF),
            (Byte) ((newPath >> 8) & 0xFF), (Byte) (newPath & 0xFF)
        };
        return BuildCommand(src, Constants.ADDR_BROADCAST, Constants.MESSAGE_ROUTING_CHANGE,
                param);
    }

    /**
     * Build &lt;Give Device Power Status&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildGiveDevicePowerStatus(Int32 src, Int32 dest) {
        return BuildCommand(src, dest, Constants.MESSAGE_GIVE_DEVICE_POWER_STATUS);
    }

    /**
     * Build &lt;Report Power Status&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param powerStatus power status of the device
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildReportPowerStatus(Int32 src, Int32 dest, Int32 powerStatus) {
        Byte[] param = new Byte[] {
                (Byte) (powerStatus & 0xFF)
        };
        return BuildCommand(src, dest, Constants.MESSAGE_REPORT_POWER_STATUS, param);
    }

    /**
     * Build &lt;Report Menu Status&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param menuStatus menu status of the device
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildReportMenuStatus(Int32 src, Int32 dest, Int32 menuStatus) {
        Byte[] param = new Byte[] {
                (Byte) (menuStatus & 0xFF)
        };
        return BuildCommand(src, dest, Constants.MESSAGE_MENU_STATUS, param);
    }

    /**
     * Build &lt;System Audio Mode Request&gt; command.
     *
     * @param src source address of command
     * @param avr destination address of command, it should be AVR
     * @param avrPhysicalAddress physical address of AVR
     * @param enableSystemAudio whether to enable System Audio Mode or not
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildSystemAudioModeRequest(Int32 src, Int32 avr, Int32 avrPhysicalAddress,
            Boolean enableSystemAudio) {
        if (enableSystemAudio) {
            return BuildCommand(src, avr, Constants.MESSAGE_SYSTEM_AUDIO_MODE_REQUEST,
                    PhysicalAddressToParam(avrPhysicalAddress));
        } else {
            return BuildCommand(src, avr, Constants.MESSAGE_SYSTEM_AUDIO_MODE_REQUEST);
        }
    }

    /**
     * Build &lt;Give Audio Status&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildGiveAudioStatus(Int32 src, Int32 dest) {
        return BuildCommand(src, dest, Constants.MESSAGE_GIVE_AUDIO_STATUS);
    }

    /**
     * Build &lt;User Control Pressed&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param uiCommand keycode that user pressed
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildUserControlPressed(Int32 src, Int32 dest, Int32 uiCommand) {
        return BuildUserControlPressed(src, dest, new Byte[] { (Byte) (uiCommand & 0xFF) });
    }

    /**
     * Build &lt;User Control Pressed&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param commandParam uiCommand and the additional parameter
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildUserControlPressed(Int32 src, Int32 dest, Byte[] commandParam) {
        return BuildCommand(src, dest, Constants.MESSAGE_USER_CONTROL_PRESSED, commandParam);
    }

    /**
     * Build &lt;User Control Released&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildUserControlReleased(Int32 src, Int32 dest) {
        return BuildCommand(src, dest, Constants.MESSAGE_USER_CONTROL_RELEASED);
    }

    /**
     * Build &lt;Give System Audio Mode Status&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildGiveSystemAudioModeStatus(Int32 src, Int32 dest) {
        return BuildCommand(src, dest, Constants.MESSAGE_GIVE_SYSTEM_AUDIO_MODE_STATUS);
    }

    /**
     * Build &lt;Standby&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    public static HdmiCecMessage BuildStandby(Int32 src, Int32 dest) {
        return BuildCommand(src, dest, Constants.MESSAGE_STANDBY);
    }

    /**
     * Build &lt;Vendor Command&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param params vendor-specific parameters
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildVendorCommand(Int32 src, Int32 dest, Byte[] params) {
        return BuildCommand(src, dest, Constants.MESSAGE_VENDOR_COMMAND, params);
    }

    /**
     * Build &lt;Vendor Command With ID&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param vendorId vendor ID
     * @param operands vendor-specific parameters
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildVendorCommandWithId(Int32 src, Int32 dest, Int32 vendorId,
            Byte[] operands) {
        Byte[] params = new Byte[operands.length + 3];  // parameter plus Len(vendorId)
        params[0] = (Byte) ((vendorId >> 16) & 0xFF);
        params[1] = (Byte) ((vendorId >> 8) & 0xFF);
        params[2] = (Byte) (vendorId & 0xFF);
        System->Arraycopy(operands, 0, params, 3, operands.length);
        return BuildCommand(src, dest, Constants.MESSAGE_VENDOR_COMMAND_WITH_ID, params);
    }

    /**
     * Build &lt;Record On&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param params parameter of command
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildRecordOn(Int32 src, Int32 dest, Byte[] params) {
        return BuildCommand(src, dest, Constants.MESSAGE_RECORD_ON, params);
    }

    /**
     * Build &lt;Record Off&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildRecordOff(Int32 src, Int32 dest) {
        return BuildCommand(src, dest, Constants.MESSAGE_RECORD_OFF);
    }

    /**
     * Build &lt;Set Digital Timer&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param params Byte array of timing information and digital service information to be recorded
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildSetDigitalTimer(Int32 src, Int32 dest, Byte[] params) {
        return BuildCommand(src, dest, Constants.MESSAGE_SET_DIGITAL_TIMER, params);
    }

    /**
     * Build &lt;Set Analogue Timer&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param params Byte array of timing information and analog service information to be recorded
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildSetAnalogueTimer(Int32 src, Int32 dest, Byte[] params) {
        return BuildCommand(src, dest, Constants.MESSAGE_SET_ANALOG_TIMER, params);
    }

    /**
     * Build &lt;Set External Timer&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param params Byte array of timing information and external source information to be recorded
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildSetExternalTimer(Int32 src, Int32 dest, Byte[] params) {
        return BuildCommand(src, dest, Constants.MESSAGE_SET_EXTERNAL_TIMER, params);
    }

    /**
     * Build &lt;Clear Digital Timer&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param params Byte array of timing information and digital service information to be cleared
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildClearDigitalTimer(Int32 src, Int32 dest, Byte[] params) {
        return BuildCommand(src, dest, Constants.MESSAGE_CLEAR_DIGITAL_TIMER, params);
    }

    /**
     * Build &lt;Clear Analog Timer&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param params Byte array of timing information and analog service information to be cleared
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildClearAnalogueTimer(Int32 src, Int32 dest, Byte[] params) {
        return BuildCommand(src, dest, Constants.MESSAGE_CLEAR_ANALOG_TIMER, params);
    }

    /**
     * Build &lt;Clear Digital Timer&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param params Byte array of timing information and external source information to be cleared
     * @return newly created {@link HdmiCecMessage}
     */
    static HdmiCecMessage BuildClearExternalTimer(Int32 src, Int32 dest, Byte[] params) {
        return BuildCommand(src, dest, Constants.MESSAGE_CLEAR_EXTERNAL_TIMER, params);
    }

    /***** Please ADD new BuildXXX() methods above. ******/

    /**
     * Build a {@link HdmiCecMessage} without extra parameter.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param opcode opcode for a message
     * @return newly created {@link HdmiCecMessage}
     */
    private static HdmiCecMessage BuildCommand(Int32 src, Int32 dest, Int32 opcode) {
        return new HdmiCecMessage(src, dest, opcode, HdmiCecMessage.EMPTY_PARAM);
    }

    /**
     * Build a {@link HdmiCecMessage} with given values.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param opcode opcode for a message
     * @param params extra parameters for command
     * @return newly created {@link HdmiCecMessage}
     */
    private static HdmiCecMessage BuildCommand(Int32 src, Int32 dest, Int32 opcode, Byte[] params) {
        return new HdmiCecMessage(src, dest, opcode, params);
    }

    private static Byte[] PhysicalAddressToParam(Int32 physicalAddress) {
        return new Byte[] {
                (Byte) ((physicalAddress >> 8) & 0xFF),
                (Byte) (physicalAddress & 0xFF)
        };
    }
}
