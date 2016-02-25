
#ifndef __ELASTOS_DROID_SERVER_HDMI_CONSTANTS_H__
#define __ELASTOS_DROID_SERVER_HDMI_CONSTANTS_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Defines constants related to HDMI-CEC protocol internal implementation.
 * If a constant will be used in the public api, it should be located in
 * {@link android.hardware.hdmi.HdmiControlManager}.
 */
class Constants
{
public:
    /** Logical address for TV */
    static const Int32 ADDR_TV;

    /** Logical address for recorder 1 */
    static const Int32 ADDR_RECORDER_1;

    /** Logical address for recorder 2 */
    static const Int32 ADDR_RECORDER_2;

    /** Logical address for tuner 1 */
    static const Int32 ADDR_TUNER_1;

    /** Logical address for playback 1 */
    static const Int32 ADDR_PLAYBACK_1;

    /** Logical address for audio system */
    static const Int32 ADDR_AUDIO_SYSTEM;

    /** Logical address for tuner 2 */
    static const Int32 ADDR_TUNER_2;

    /** Logical address for tuner 3 */
    static const Int32 ADDR_TUNER_3;

    /** Logical address for playback 2 */
    static const Int32 ADDR_PLAYBACK_2;

    /** Logical address for recorder 3 */
    static const Int32 ADDR_RECORDER_3;

    /** Logical address for tuner 4 */
    static const Int32 ADDR_TUNER_4;

    /** Logical address for playback 3 */
    static const Int32 ADDR_PLAYBACK_3;

    /** Logical address reserved for future usage */
    static const Int32 ADDR_RESERVED_1;

    /** Logical address reserved for future usage */
    static const Int32 ADDR_RESERVED_2;

    /** Logical address for TV other than the one assigned with {@link #ADDR_TV} */
    static const Int32 ADDR_SPECIFIC_USE;

    /** Logical address for devices to which address cannot be allocated */
    static const Int32 ADDR_UNREGISTERED;

    /** Logical address used in the destination address field for broadcast messages */
    static const Int32 ADDR_BROADCAST;

    /** Logical address used to indicate it is not initialized or invalid. */
    static const Int32 ADDR_INVALID;

    /** Logical address used to indicate the source comes from internal device. */
    static const Int32 ADDR_INTERNAL;

    static const Int32 MESSAGE_FEATURE_ABORT;

    static const Int32 MESSAGE_IMAGE_VIEW_ON;

    static const Int32 MESSAGE_TUNER_STEP_INCREMENT;

    static const Int32 MESSAGE_TUNER_STEP_DECREMENT;

    static const Int32 MESSAGE_TUNER_DEVICE_STATUS;

    static const Int32 MESSAGE_GIVE_TUNER_DEVICE_STATUS;

    static const Int32 MESSAGE_RECORD_ON;

    static const Int32 MESSAGE_RECORD_STATUS;

    static const Int32 MESSAGE_RECORD_OFF;

    static const Int32 MESSAGE_TEXT_VIEW_ON;

    static const Int32 MESSAGE_RECORD_TV_SCREEN;

    static const Int32 MESSAGE_GIVE_DECK_STATUS;

    static const Int32 MESSAGE_DECK_STATUS;

    static const Int32 MESSAGE_SET_MENU_LANGUAGE;

    static const Int32 MESSAGE_CLEAR_ANALOG_TIMER;

    static const Int32 MESSAGE_SET_ANALOG_TIMER;

    static const Int32 MESSAGE_TIMER_STATUS;

    static const Int32 MESSAGE_STANDBY;

    static const Int32 MESSAGE_PLAY;

    static const Int32 MESSAGE_DECK_CONTROL;

    static const Int32 MESSAGE_TIMER_CLEARED_STATUS;

    static const Int32 MESSAGE_USER_CONTROL_PRESSED;

    static const Int32 MESSAGE_USER_CONTROL_RELEASED;

    static const Int32 MESSAGE_GIVE_OSD_NAME;

    static const Int32 MESSAGE_SET_OSD_NAME;

    static const Int32 MESSAGE_SET_OSD_STRING;

    static const Int32 MESSAGE_SET_TIMER_PROGRAM_TITLE;

    static const Int32 MESSAGE_SYSTEM_AUDIO_MODE_REQUEST;

    static const Int32 MESSAGE_GIVE_AUDIO_STATUS;

    static const Int32 MESSAGE_SET_SYSTEM_AUDIO_MODE;

    static const Int32 MESSAGE_REPORT_AUDIO_STATUS;

    static const Int32 MESSAGE_GIVE_SYSTEM_AUDIO_MODE_STATUS;

    static const Int32 MESSAGE_SYSTEM_AUDIO_MODE_STATUS;

    static const Int32 MESSAGE_ROUTING_CHANGE;

    static const Int32 MESSAGE_ROUTING_INFORMATION;

    static const Int32 MESSAGE_ACTIVE_SOURCE;

    static const Int32 MESSAGE_GIVE_PHYSICAL_ADDRESS;

    static const Int32 MESSAGE_REPORT_PHYSICAL_ADDRESS;

    static const Int32 MESSAGE_REQUEST_ACTIVE_SOURCE;

    static const Int32 MESSAGE_SET_STREAM_PATH;

    static const Int32 MESSAGE_DEVICE_VENDOR_ID;

    static const Int32 MESSAGE_VENDOR_COMMAND;

    static const Int32 MESSAGE_VENDOR_REMOTE_BUTTON_DOWN;

    static const Int32 MESSAGE_VENDOR_REMOTE_BUTTON_UP;

    static const Int32 MESSAGE_GIVE_DEVICE_VENDOR_ID;

    static const Int32 MESSAGE_MENU_REQUEST;

    static const Int32 MESSAGE_MENU_STATUS;

    static const Int32 MESSAGE_GIVE_DEVICE_POWER_STATUS;

    static const Int32 MESSAGE_REPORT_POWER_STATUS;

    static const Int32 MESSAGE_GET_MENU_LANGUAGE;

    static const Int32 MESSAGE_SELECT_ANALOG_SERVICE;

    static const Int32 MESSAGE_SELECT_DIGITAL_SERVICE;

    static const Int32 MESSAGE_SET_DIGITAL_TIMER;

    static const Int32 MESSAGE_CLEAR_DIGITAL_TIMER;

    static const Int32 MESSAGE_SET_AUDIO_RATE;

    static const Int32 MESSAGE_INACTIVE_SOURCE;

    static const Int32 MESSAGE_CEC_VERSION;

    static const Int32 MESSAGE_GET_CEC_VERSION;

    static const Int32 MESSAGE_VENDOR_COMMAND_WITH_ID;

    static const Int32 MESSAGE_CLEAR_EXTERNAL_TIMER;

    static const Int32 MESSAGE_SET_EXTERNAL_TIMER;

    static const Int32 MESSAGE_REPORT_SHORT_AUDIO_DESCRIPTOR;

    static const Int32 MESSAGE_REQUEST_SHORT_AUDIO_DESCRIPTOR;

    static const Int32 MESSAGE_INITIATE_ARC;

    static const Int32 MESSAGE_REPORT_ARC_INITIATED;

    static const Int32 MESSAGE_REPORT_ARC_TERMINATED;

    static const Int32 MESSAGE_REQUEST_ARC_INITIATION;

    static const Int32 MESSAGE_REQUEST_ARC_TERMINATION;

    static const Int32 MESSAGE_TERMINATE_ARC;

    static const Int32 MESSAGE_CDC_MESSAGE;

    static const Int32 MESSAGE_ABORT;

    static const Int32 UNKNOWN_VENDOR_ID;

    static const Int32 sTRUE;

    static const Int32 sFALSE;

    // Internal abort error code. It's the same as success.
    static const Int32 ABORT_NO_ERROR;

    // [Abort Reason]
    static const Int32 ABORT_UNRECOGNIZED_OPCODE;

    static const Int32 ABORT_NOT_IN_CORRECT_MODE;

    static const Int32 ABORT_CANNOT_PROVIDE_SOURCE;

    static const Int32 ABORT_INVALID_OPERAND;

    static const Int32 ABORT_REFUSED;

    static const Int32 ABORT_UNABLE_TO_DETERMINE;

    // [Audio Status]
    static const Int32 SYSTEM_AUDIO_STATUS_OFF;

    static const Int32 SYSTEM_AUDIO_STATUS_ON;

    // [Menu State]
    static const Int32 MENU_STATE_ACTIVATED;

    static const Int32 MENU_STATE_DEACTIVATED;

    // When &'d with the path 1.2.2.0 (0x1220), for instance, gives 1.0.0.0.
    static const Int32 ROUTING_PATH_TOP_MASK;

    static const Int32 ROUTING_PATH_TOP_SHIFT;

    static const Int32 INVALID_PORT_ID;

    static const Int32 INVALID_PHYSICAL_ADDRESS;

    // Send result codes. It should be consistent with hdmi_cec.h's send_message error code.
    static const Int32 SEND_RESULT_SUCCESS;

    static const Int32 SEND_RESULT_NAK;

    static const Int32 SEND_RESULT_BUSY;

    static const Int32 SEND_RESULT_FAILURE;

    // Should use "OR(|) operation of POLL_STRATEGY_XXX and POLL_ITERATION_XXX.
    static const Int32 POLL_STRATEGY_MASK;

    // first and second bit.
    static const Int32 POLL_STRATEGY_REMOTES_DEVICES;

    static const Int32 POLL_STRATEGY_SYSTEM_AUDIO;

    static const Int32 POLL_ITERATION_STRATEGY_MASK;

    // first and second bit.
    static const Int32 POLL_ITERATION_IN_ORDER;

    static const Int32 POLL_ITERATION_REVERSE_ORDER;

    static const Int32 UNKNOWN_VOLUME;

    //       in config.xml to allow customization.
    static const Int32 IRT_MS;

    static const String PROPERTY_PREFERRED_ADDRESS_PLAYBACK;

    static const String PROPERTY_PREFERRED_ADDRESS_TV;

    //     values which denotes the device type in HDMI Spec 1.4.
    static const String PROPERTY_DEVICE_TYPE;

    static const Int32 RECORDING_TYPE_DIGITAL_RF;

    static const Int32 RECORDING_TYPE_ANALOGUE_RF;

    static const Int32 RECORDING_TYPE_EXTERNAL_PHYSICAL_ADDRESS;

    static const Int32 RECORDING_TYPE_OWN_SOURCE;

    // If set to disabled, TV won't turn on automatically.
    static const Int32 OPTION_CEC_AUTO_WAKEUP;

    // If set to disabled, all CEC commands are discarded.
    static const Int32 OPTION_CEC_ENABLE;

    // If enabled, it take the control back.
    static const Int32 OPTION_CEC_SERVICE_CONTROL;

    // If set to disabled, TV doesn't send <Standby> to other devices.
    static const Int32 OPTION_CEC_AUTO_DEVICE_OFF;

    // If set to disabled, TV does not switch ports when mobile device is connected.
    static const Int32 OPTION_MHL_INPUT_SWITCHING;

    // If set to enabled, TV disables power charging for mobile device.
    static const Int32 OPTION_MHL_POWER_CHARGE;

    // If set to disabled, all MHL commands are discarded.
    static const Int32 OPTION_MHL_ENABLE;

    static const Int32 DISABLED;

    static const Int32 ENABLED;

private:
    Constants();
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_CONSTANTS_H__
