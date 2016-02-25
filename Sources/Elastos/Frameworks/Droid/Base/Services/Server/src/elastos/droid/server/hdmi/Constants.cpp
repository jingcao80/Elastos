
#include "elastos/droid/server/hdmi/Constants.h"
#include <Elastos.Droid.Hardware.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const Int32 Constants::ADDR_TV = 0;
const Int32 Constants::ADDR_RECORDER_1 = 1;
const Int32 Constants::ADDR_RECORDER_2 = 2;
const Int32 Constants::ADDR_TUNER_1 = 3;
const Int32 Constants::ADDR_PLAYBACK_1 = 4;
const Int32 Constants::ADDR_AUDIO_SYSTEM = 5;
const Int32 Constants::ADDR_TUNER_2 = 6;
const Int32 Constants::ADDR_TUNER_3 = 7;
const Int32 Constants::ADDR_PLAYBACK_2 = 8;
const Int32 Constants::ADDR_RECORDER_3 = 9;
const Int32 Constants::ADDR_TUNER_4 = 10;
const Int32 Constants::ADDR_PLAYBACK_3 = 11;
const Int32 Constants::ADDR_RESERVED_1 = 12;
const Int32 Constants::ADDR_RESERVED_2 = 13;
const Int32 Constants::ADDR_SPECIFIC_USE = 14;
const Int32 Constants::ADDR_UNREGISTERED = 15;
const Int32 Constants::ADDR_BROADCAST = 15;
const Int32 Constants::ADDR_INVALID = -1;
const Int32 Constants::ADDR_INTERNAL = IHdmiDeviceInfo::ADDR_INTERNAL;
const Int32 Constants::MESSAGE_FEATURE_ABORT = 0x00;
const Int32 Constants::MESSAGE_IMAGE_VIEW_ON = 0x04;
const Int32 Constants::MESSAGE_TUNER_STEP_INCREMENT = 0x05;
const Int32 Constants::MESSAGE_TUNER_STEP_DECREMENT = 0x06;
const Int32 Constants::MESSAGE_TUNER_DEVICE_STATUS = 0x07;
const Int32 Constants::MESSAGE_GIVE_TUNER_DEVICE_STATUS = 0x08;
const Int32 Constants::MESSAGE_RECORD_ON = 0x09;
const Int32 Constants::MESSAGE_RECORD_STATUS = 0x0A;
const Int32 Constants::MESSAGE_RECORD_OFF = 0x0B;
const Int32 Constants::MESSAGE_TEXT_VIEW_ON = 0x0D;
const Int32 Constants::MESSAGE_RECORD_TV_SCREEN = 0x0F;
const Int32 Constants::MESSAGE_GIVE_DECK_STATUS = 0x1A;
const Int32 Constants::MESSAGE_DECK_STATUS = 0x1B;
const Int32 Constants::MESSAGE_SET_MENU_LANGUAGE = 0x32;
const Int32 Constants::MESSAGE_CLEAR_ANALOG_TIMER = 0x33;
const Int32 Constants::MESSAGE_SET_ANALOG_TIMER = 0x34;
const Int32 Constants::MESSAGE_TIMER_STATUS = 0x35;
const Int32 Constants::MESSAGE_STANDBY = 0x36;
const Int32 Constants::MESSAGE_PLAY = 0x41;
const Int32 Constants::MESSAGE_DECK_CONTROL = 0x42;
const Int32 Constants::MESSAGE_TIMER_CLEARED_STATUS = 0x043;
const Int32 Constants::MESSAGE_USER_CONTROL_PRESSED = 0x44;
const Int32 Constants::MESSAGE_USER_CONTROL_RELEASED = 0x45;
const Int32 Constants::MESSAGE_GIVE_OSD_NAME = 0x46;
const Int32 Constants::MESSAGE_SET_OSD_NAME = 0x47;
const Int32 Constants::MESSAGE_SET_OSD_STRING = 0x64;
const Int32 Constants::MESSAGE_SET_TIMER_PROGRAM_TITLE = 0x67;
const Int32 Constants::MESSAGE_SYSTEM_AUDIO_MODE_REQUEST = 0x70;
const Int32 Constants::MESSAGE_GIVE_AUDIO_STATUS = 0x71;
const Int32 Constants::MESSAGE_SET_SYSTEM_AUDIO_MODE = 0x72;
const Int32 Constants::MESSAGE_REPORT_AUDIO_STATUS = 0x7A;
const Int32 Constants::MESSAGE_GIVE_SYSTEM_AUDIO_MODE_STATUS = 0x7D;
const Int32 Constants::MESSAGE_SYSTEM_AUDIO_MODE_STATUS = 0x7E;
const Int32 Constants::MESSAGE_ROUTING_CHANGE = 0x80;
const Int32 Constants::MESSAGE_ROUTING_INFORMATION = 0x81;
const Int32 Constants::MESSAGE_ACTIVE_SOURCE = 0x82;
const Int32 Constants::MESSAGE_GIVE_PHYSICAL_ADDRESS = 0x83;
const Int32 Constants::MESSAGE_REPORT_PHYSICAL_ADDRESS = 0x84;
const Int32 Constants::MESSAGE_REQUEST_ACTIVE_SOURCE = 0x85;
const Int32 Constants::MESSAGE_SET_STREAM_PATH = 0x86;
const Int32 Constants::MESSAGE_DEVICE_VENDOR_ID = 0x87;
const Int32 Constants::MESSAGE_VENDOR_COMMAND = 0x89;
const Int32 Constants::MESSAGE_VENDOR_REMOTE_BUTTON_DOWN = 0x8A;
const Int32 Constants::MESSAGE_VENDOR_REMOTE_BUTTON_UP = 0x8B;
const Int32 Constants::MESSAGE_GIVE_DEVICE_VENDOR_ID = 0x8C;
const Int32 Constants::MESSAGE_MENU_REQUEST = 0x8D;
const Int32 Constants::MESSAGE_MENU_STATUS = 0x8E;
const Int32 Constants::MESSAGE_GIVE_DEVICE_POWER_STATUS = 0x8F;
const Int32 Constants::MESSAGE_REPORT_POWER_STATUS = 0x90;
const Int32 Constants::MESSAGE_GET_MENU_LANGUAGE = 0x91;
const Int32 Constants::MESSAGE_SELECT_ANALOG_SERVICE = 0x92;
const Int32 Constants::MESSAGE_SELECT_DIGITAL_SERVICE = 0x93;
const Int32 Constants::MESSAGE_SET_DIGITAL_TIMER = 0x97;
const Int32 Constants::MESSAGE_CLEAR_DIGITAL_TIMER = 0x99;
const Int32 Constants::MESSAGE_SET_AUDIO_RATE = 0x9A;
const Int32 Constants::MESSAGE_INACTIVE_SOURCE = 0x9D;
const Int32 Constants::MESSAGE_CEC_VERSION = 0x9E;
const Int32 Constants::MESSAGE_GET_CEC_VERSION = 0x9F;
const Int32 Constants::MESSAGE_VENDOR_COMMAND_WITH_ID = 0xA0;
const Int32 Constants::MESSAGE_CLEAR_EXTERNAL_TIMER = 0xA1;
const Int32 Constants::MESSAGE_SET_EXTERNAL_TIMER = 0xA2;
const Int32 Constants::MESSAGE_REPORT_SHORT_AUDIO_DESCRIPTOR = 0xA3;
const Int32 Constants::MESSAGE_REQUEST_SHORT_AUDIO_DESCRIPTOR = 0xA4;
const Int32 Constants::MESSAGE_INITIATE_ARC = 0xC0;
const Int32 Constants::MESSAGE_REPORT_ARC_INITIATED = 0xC1;
const Int32 Constants::MESSAGE_REPORT_ARC_TERMINATED = 0xC2;
const Int32 Constants::MESSAGE_REQUEST_ARC_INITIATION = 0xC3;
const Int32 Constants::MESSAGE_REQUEST_ARC_TERMINATION = 0xC4;
const Int32 Constants::MESSAGE_TERMINATE_ARC = 0xC5;
const Int32 Constants::MESSAGE_CDC_MESSAGE = 0xF8;
const Int32 Constants::MESSAGE_ABORT = 0xFF;
const Int32 Constants::UNKNOWN_VENDOR_ID = 0xFFFFFF;
const Int32 Constants::sTRUE = 1;
const Int32 Constants::sFALSE = 0;
const Int32 Constants::ABORT_NO_ERROR = -1;
const Int32 Constants::ABORT_UNRECOGNIZED_OPCODE = 0;
const Int32 Constants::ABORT_NOT_IN_CORRECT_MODE = 1;
const Int32 Constants::ABORT_CANNOT_PROVIDE_SOURCE = 2;
const Int32 Constants::ABORT_INVALID_OPERAND = 3;
const Int32 Constants::ABORT_REFUSED = 4;
const Int32 Constants::ABORT_UNABLE_TO_DETERMINE = 5;
const Int32 Constants::SYSTEM_AUDIO_STATUS_OFF = 0;
const Int32 Constants::SYSTEM_AUDIO_STATUS_ON = 1;
const Int32 Constants::MENU_STATE_ACTIVATED = 0;
const Int32 Constants::MENU_STATE_DEACTIVATED = 1;
const Int32 Constants::ROUTING_PATH_TOP_MASK = 0xF000;
const Int32 Constants::ROUTING_PATH_TOP_SHIFT = 12;
const Int32 Constants::INVALID_PORT_ID = IHdmiDeviceInfo::PORT_INVALID;
const Int32 Constants::INVALID_PHYSICAL_ADDRESS = IHdmiDeviceInfo::PATH_INVALID;
const Int32 Constants::SEND_RESULT_SUCCESS = 0;
const Int32 Constants::SEND_RESULT_NAK = 1;
const Int32 Constants::SEND_RESULT_BUSY = 2;
const Int32 Constants::SEND_RESULT_FAILURE = 3;
const Int32 Constants::POLL_STRATEGY_MASK = 0x3;
const Int32 Constants::POLL_STRATEGY_REMOTES_DEVICES = 0x1;
const Int32 Constants::POLL_STRATEGY_SYSTEM_AUDIO = 0x2;
const Int32 Constants::POLL_ITERATION_STRATEGY_MASK = 0x30000;
const Int32 Constants::POLL_ITERATION_IN_ORDER = 0x10000;
const Int32 Constants::POLL_ITERATION_REVERSE_ORDER = 0x20000;
const Int32 Constants::UNKNOWN_VOLUME = -1;
const Int32 Constants::IRT_MS = 300;
const String Constants::PROPERTY_PREFERRED_ADDRESS_PLAYBACK("persist.sys.hdmi.addr.playback");
const String Constants::PROPERTY_PREFERRED_ADDRESS_TV("persist.sys.hdmi.addr.tv");
const String Constants::PROPERTY_DEVICE_TYPE("ro.hdmi.device_type");
const Int32 Constants::RECORDING_TYPE_DIGITAL_RF = 1;
const Int32 Constants::RECORDING_TYPE_ANALOGUE_RF = 2;
const Int32 Constants::RECORDING_TYPE_EXTERNAL_PHYSICAL_ADDRESS = 3;
const Int32 Constants::RECORDING_TYPE_OWN_SOURCE = 4;
const Int32 Constants::OPTION_CEC_AUTO_WAKEUP = 1;
const Int32 Constants::OPTION_CEC_ENABLE = 2;
const Int32 Constants::OPTION_CEC_SERVICE_CONTROL = 3;
const Int32 Constants::OPTION_CEC_AUTO_DEVICE_OFF = 4;
const Int32 Constants::OPTION_MHL_INPUT_SWITCHING = 101;
const Int32 Constants::OPTION_MHL_POWER_CHARGE = 102;
const Int32 Constants::OPTION_MHL_ENABLE = 103;
const Int32 Constants::DISABLED = 0;
const Int32 Constants::ENABLED = 1;

Constants::Constants()
{
 /* cannot be instantiated */
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
