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

/**
 * Defines constants related to HDMI-CEC protocol internal implementation.
 * If a constant will be used in the public api, it should be located in
 * {@link android.hardware.hdmi.HdmiControlManager}.
 */
final class Constants {

    /** Logical address for TV */
    public static const Int32 ADDR_TV = 0;

    /** Logical address for recorder 1 */
    public static const Int32 ADDR_RECORDER_1 = 1;

    /** Logical address for recorder 2 */
    public static const Int32 ADDR_RECORDER_2 = 2;

    /** Logical address for tuner 1 */
    public static const Int32 ADDR_TUNER_1 = 3;

    /** Logical address for playback 1 */
    public static const Int32 ADDR_PLAYBACK_1 = 4;

    /** Logical address for audio system */
    public static const Int32 ADDR_AUDIO_SYSTEM = 5;

    /** Logical address for tuner 2 */
    public static const Int32 ADDR_TUNER_2 = 6;

    /** Logical address for tuner 3 */
    public static const Int32 ADDR_TUNER_3 = 7;

    /** Logical address for playback 2 */
    public static const Int32 ADDR_PLAYBACK_2 = 8;

    /** Logical address for recorder 3 */
    public static const Int32 ADDR_RECORDER_3 = 9;

    /** Logical address for tuner 4 */
    public static const Int32 ADDR_TUNER_4 = 10;

    /** Logical address for playback 3 */
    public static const Int32 ADDR_PLAYBACK_3 = 11;

    /** Logical address reserved for future usage */
    public static const Int32 ADDR_RESERVED_1 = 12;

    /** Logical address reserved for future usage */
    public static const Int32 ADDR_RESERVED_2 = 13;

    /** Logical address for TV other than the one assigned with {@link #ADDR_TV} */
    public static const Int32 ADDR_SPECIFIC_USE = 14;

    /** Logical address for devices to which address cannot be allocated */
    public static const Int32 ADDR_UNREGISTERED = 15;

    /** Logical address used in the destination address field for broadcast messages */
    public static const Int32 ADDR_BROADCAST = 15;

    /** Logical address used to indicate it is not initialized or invalid. */
    public static const Int32 ADDR_INVALID = -1;

    /** Logical address used to indicate the source comes from internal device. */
    public static const Int32 ADDR_INTERNAL = HdmiDeviceInfo.ADDR_INTERNAL;

    static const Int32 MESSAGE_FEATURE_ABORT = 0x00;
    static const Int32 MESSAGE_IMAGE_VIEW_ON = 0x04;
    static const Int32 MESSAGE_TUNER_STEP_INCREMENT = 0x05;
    static const Int32 MESSAGE_TUNER_STEP_DECREMENT = 0x06;
    static const Int32 MESSAGE_TUNER_DEVICE_STATUS = 0x07;
    static const Int32 MESSAGE_GIVE_TUNER_DEVICE_STATUS = 0x08;
    static const Int32 MESSAGE_RECORD_ON = 0x09;
    static const Int32 MESSAGE_RECORD_STATUS = 0x0A;
    static const Int32 MESSAGE_RECORD_OFF = 0x0B;
    static const Int32 MESSAGE_TEXT_VIEW_ON = 0x0D;
    static const Int32 MESSAGE_RECORD_TV_SCREEN = 0x0F;
    static const Int32 MESSAGE_GIVE_DECK_STATUS = 0x1A;
    static const Int32 MESSAGE_DECK_STATUS = 0x1B;
    static const Int32 MESSAGE_SET_MENU_LANGUAGE = 0x32;
    static const Int32 MESSAGE_CLEAR_ANALOG_TIMER = 0x33;
    static const Int32 MESSAGE_SET_ANALOG_TIMER = 0x34;
    static const Int32 MESSAGE_TIMER_STATUS = 0x35;
    static const Int32 MESSAGE_STANDBY = 0x36;
    static const Int32 MESSAGE_PLAY = 0x41;
    static const Int32 MESSAGE_DECK_CONTROL = 0x42;
    static const Int32 MESSAGE_TIMER_CLEARED_STATUS = 0x043;
    static const Int32 MESSAGE_USER_CONTROL_PRESSED = 0x44;
    static const Int32 MESSAGE_USER_CONTROL_RELEASED = 0x45;
    static const Int32 MESSAGE_GIVE_OSD_NAME = 0x46;
    static const Int32 MESSAGE_SET_OSD_NAME = 0x47;
    static const Int32 MESSAGE_SET_OSD_STRING = 0x64;
    static const Int32 MESSAGE_SET_TIMER_PROGRAM_TITLE = 0x67;
    static const Int32 MESSAGE_SYSTEM_AUDIO_MODE_REQUEST = 0x70;
    static const Int32 MESSAGE_GIVE_AUDIO_STATUS = 0x71;
    static const Int32 MESSAGE_SET_SYSTEM_AUDIO_MODE = 0x72;
    static const Int32 MESSAGE_REPORT_AUDIO_STATUS = 0x7A;
    static const Int32 MESSAGE_GIVE_SYSTEM_AUDIO_MODE_STATUS = 0x7D;
    static const Int32 MESSAGE_SYSTEM_AUDIO_MODE_STATUS = 0x7E;
    static const Int32 MESSAGE_ROUTING_CHANGE = 0x80;
    static const Int32 MESSAGE_ROUTING_INFORMATION = 0x81;
    static const Int32 MESSAGE_ACTIVE_SOURCE = 0x82;
    static const Int32 MESSAGE_GIVE_PHYSICAL_ADDRESS = 0x83;
    static const Int32 MESSAGE_REPORT_PHYSICAL_ADDRESS = 0x84;
    static const Int32 MESSAGE_REQUEST_ACTIVE_SOURCE = 0x85;
    static const Int32 MESSAGE_SET_STREAM_PATH = 0x86;
    static const Int32 MESSAGE_DEVICE_VENDOR_ID = 0x87;
    static const Int32 MESSAGE_VENDOR_COMMAND = 0x89;
    static const Int32 MESSAGE_VENDOR_REMOTE_BUTTON_DOWN = 0x8A;
    static const Int32 MESSAGE_VENDOR_REMOTE_BUTTON_UP = 0x8B;
    static const Int32 MESSAGE_GIVE_DEVICE_VENDOR_ID = 0x8C;
    static const Int32 MESSAGE_MENU_REQUEST = 0x8D;
    static const Int32 MESSAGE_MENU_STATUS = 0x8E;
    static const Int32 MESSAGE_GIVE_DEVICE_POWER_STATUS = 0x8F;
    static const Int32 MESSAGE_REPORT_POWER_STATUS = 0x90;
    static const Int32 MESSAGE_GET_MENU_LANGUAGE = 0x91;
    static const Int32 MESSAGE_SELECT_ANALOG_SERVICE = 0x92;
    static const Int32 MESSAGE_SELECT_DIGITAL_SERVICE = 0x93;
    static const Int32 MESSAGE_SET_DIGITAL_TIMER = 0x97;
    static const Int32 MESSAGE_CLEAR_DIGITAL_TIMER = 0x99;
    static const Int32 MESSAGE_SET_AUDIO_RATE = 0x9A;
    static const Int32 MESSAGE_INACTIVE_SOURCE = 0x9D;
    static const Int32 MESSAGE_CEC_VERSION = 0x9E;
    static const Int32 MESSAGE_GET_CEC_VERSION = 0x9F;
    static const Int32 MESSAGE_VENDOR_COMMAND_WITH_ID = 0xA0;
    static const Int32 MESSAGE_CLEAR_EXTERNAL_TIMER = 0xA1;
    static const Int32 MESSAGE_SET_EXTERNAL_TIMER = 0xA2;
    static const Int32 MESSAGE_REPORT_SHORT_AUDIO_DESCRIPTOR = 0xA3;
    static const Int32 MESSAGE_REQUEST_SHORT_AUDIO_DESCRIPTOR = 0xA4;
    static const Int32 MESSAGE_INITIATE_ARC = 0xC0;
    static const Int32 MESSAGE_REPORT_ARC_INITIATED = 0xC1;
    static const Int32 MESSAGE_REPORT_ARC_TERMINATED = 0xC2;
    static const Int32 MESSAGE_REQUEST_ARC_INITIATION = 0xC3;
    static const Int32 MESSAGE_REQUEST_ARC_TERMINATION = 0xC4;
    static const Int32 MESSAGE_TERMINATE_ARC = 0xC5;
    static const Int32 MESSAGE_CDC_MESSAGE = 0xF8;
    static const Int32 MESSAGE_ABORT = 0xFF;

    static const Int32 UNKNOWN_VENDOR_ID = 0xFFFFFF;

    static const Int32 TRUE = 1;
    static const Int32 FALSE = 0;

    // Internal abort error code. It's the same as success.
    static const Int32 ABORT_NO_ERROR = -1;
    // Constants related to operands of HDMI CEC commands.
    // Refer to CEC Table 29 in HDMI Spec v1.4b.
    // [Abort Reason]
    static const Int32 ABORT_UNRECOGNIZED_OPCODE = 0;
    static const Int32 ABORT_NOT_IN_CORRECT_MODE = 1;
    static const Int32 ABORT_CANNOT_PROVIDE_SOURCE = 2;
    static const Int32 ABORT_INVALID_OPERAND = 3;
    static const Int32 ABORT_REFUSED = 4;
    static const Int32 ABORT_UNABLE_TO_DETERMINE = 5;

    // [Audio Status]
    static const Int32 SYSTEM_AUDIO_STATUS_OFF = 0;
    static const Int32 SYSTEM_AUDIO_STATUS_ON = 1;

    // [Menu State]
    static const Int32 MENU_STATE_ACTIVATED = 0;
    static const Int32 MENU_STATE_DEACTIVATED = 1;

    // Bit mask used to get the routing path of the top level device.
    // When &'d with the path 1.2.2.0 (0x1220), for instance, gives 1.0.0.0.
    static const Int32 ROUTING_PATH_TOP_MASK = 0xF000;
    static const Int32 ROUTING_PATH_TOP_SHIFT = 12;

    static const Int32 INVALID_PORT_ID = HdmiDeviceInfo.PORT_INVALID;
    static const Int32 INVALID_PHYSICAL_ADDRESS = HdmiDeviceInfo.PATH_INVALID;

    // Send result codes. It should be consistent with hdmi_cec.h's send_message error code.
    static const Int32 SEND_RESULT_SUCCESS = 0;
    static const Int32 SEND_RESULT_NAK = 1;
    static const Int32 SEND_RESULT_BUSY = 2;
    static const Int32 SEND_RESULT_FAILURE = 3;

    // Strategy for device polling.
    // Should use "OR(|) operation of POLL_STRATEGY_XXX and POLL_ITERATION_XXX.
    static const Int32 POLL_STRATEGY_MASK = 0x3;  // first and second bit.
    static const Int32 POLL_STRATEGY_REMOTES_DEVICES = 0x1;
    static const Int32 POLL_STRATEGY_SYSTEM_AUDIO = 0x2;

    static const Int32 POLL_ITERATION_STRATEGY_MASK = 0x30000;  // first and second bit.
    static const Int32 POLL_ITERATION_IN_ORDER = 0x10000;
    static const Int32 POLL_ITERATION_REVERSE_ORDER = 0x20000;

    static const Int32 UNKNOWN_VOLUME = -1;

    // IRT(Initiator Repetition Time) in millisecond as recommended in the standard.
    // Outgoing UCP commands, when in 'Press and Hold' mode, should be this much apart
    // from the adjacent one so as not to place unnecessarily heavy load on the CEC line.
    // TODO: This value might need tweaking per product basis. Consider putting it
    //       in config.xml to allow customization.
    static const Int32 IRT_MS = 300;

    static const String PROPERTY_PREFERRED_ADDRESS_PLAYBACK = "persist.sys.hdmi.addr.playback";
    static const String PROPERTY_PREFERRED_ADDRESS_TV = "persist.sys.hdmi.addr.tv";

    // Property name for the local device configurations.
    // TODO(OEM): OEM should provide this property, and the value is the comma separated integer
    //     values which denotes the device type in HDMI Spec 1.4.
    static const String PROPERTY_DEVICE_TYPE = "ro.hdmi.device_type";

    static const Int32 RECORDING_TYPE_DIGITAL_RF = 1;
    static const Int32 RECORDING_TYPE_ANALOGUE_RF = 2;
    static const Int32 RECORDING_TYPE_EXTERNAL_PHYSICAL_ADDRESS = 3;
    static const Int32 RECORDING_TYPE_OWN_SOURCE = 4;

    // Definitions used for SetOption(). These should be in sync with the definition
    // in hardware/libhardware/include/hardware/{hdmi_cec.h,mhl.h}.

    // TV gets turned on by incoming <Text/Image View On>. enabled by default.
    // If set to disabled, TV won't turn on automatically.
    static const Int32 OPTION_CEC_AUTO_WAKEUP = 1;

    // If set to disabled, all CEC commands are discarded.
    static const Int32 OPTION_CEC_ENABLE = 2;

    // If set to disabled, system service yields control of CEC to sub-microcontroller.
    // If enabled, it take the control back.
    static const Int32 OPTION_CEC_SERVICE_CONTROL = 3;

    // Put other devices to standby when TV goes to standby. enabled by default.
    // If set to disabled, TV doesn't send <Standby> to other devices.
    static const Int32 OPTION_CEC_AUTO_DEVICE_OFF = 4;

    // If set to disabled, TV does not switch ports when mobile device is connected.
    static const Int32 OPTION_MHL_INPUT_SWITCHING = 101;

    // If set to enabled, TV disables power charging for mobile device.
    static const Int32 OPTION_MHL_POWER_CHARGE = 102;

    // If set to disabled, all MHL commands are discarded.
    static const Int32 OPTION_MHL_ENABLE = 103;

    static const Int32 DISABLED = 0;
    static const Int32 ENABLED = 1;

    private Constants() { /* cannot be instantiated */ }
}
