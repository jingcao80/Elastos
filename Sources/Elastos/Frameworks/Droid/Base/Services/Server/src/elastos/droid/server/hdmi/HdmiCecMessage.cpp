
#include "elastos/droid/server/hdmi/HdmiCecMessage.h"
#include <elastos/utility/Arrays.h>

// import libcore.util.EmptyArray;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CAR_INTERFACE_IMPL(HdmiCecMessage, Object, IHdmiCecMessage)

HdmiCecMessage::HdmiCecMessage()
    : mSource(0)
    , mDestination(0)
    , mOpcode(0)
{}

ECode HdmiCecMessage::constructor(
    /* [in] */ Int32 source,
    /* [in] */ Int32 destination,
    /* [in] */ Int32 opcode,
    /* [in] */ ArrayOf<Byte>* params)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mSource = source;
        mDestination = destination;
        mOpcode = opcode & 0xFF;
        mParams = Arrays.copyOf(params, params.length);

#endif
}

ECode HdmiCecMessage::GetSource(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mSource;

#endif
}

ECode HdmiCecMessage::GetDestination(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mDestination;

#endif
}

ECode HdmiCecMessage::GetOpcode(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mOpcode;

#endif
}

ECode HdmiCecMessage::GetParams(
    /* [out, callee] */ ArrayOf<Byte>* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mParams;

#endif
}

ECode HdmiCecMessage::ToString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        StringBuffer s = new StringBuffer();
        s.append(String.format("<%s> src: %d, dst: %d",
                opcodeToString(mOpcode), mSource, mDestination));
        if (mParams.length > 0) {
            s.append(", params:");
            for (byte data : mParams) {
                s.append(String.format(" %02X", data));
            }
        }
        return s.toString();

#endif
}

ECode HdmiCecMessage::OpcodeToString(
    /* [in] */ Int32 opcode,
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        switch (opcode) {
            case Constants::MESSAGE_FEATURE_ABORT:
                return "Feature Abort";
            case Constants::MESSAGE_IMAGE_VIEW_ON:
                return "Image View On";
            case Constants::MESSAGE_TUNER_STEP_INCREMENT:
                return "Tuner Step Increment";
            case Constants::MESSAGE_TUNER_STEP_DECREMENT:
                return "Tuner Step Decrement";
            case Constants::MESSAGE_TUNER_DEVICE_STATUS:
                return "Tuner Device Staus";
            case Constants::MESSAGE_GIVE_TUNER_DEVICE_STATUS:
                return "Give Tuner Device Status";
            case Constants::MESSAGE_RECORD_ON:
                return "Record On";
            case Constants::MESSAGE_RECORD_STATUS:
                return "Record Status";
            case Constants::MESSAGE_RECORD_OFF:
                return "Record Off";
            case Constants::MESSAGE_TEXT_VIEW_ON:
                return "Text View On";
            case Constants::MESSAGE_RECORD_TV_SCREEN:
                return "Record Tv Screen";
            case Constants::MESSAGE_GIVE_DECK_STATUS:
                return "Give Deck Status";
            case Constants::MESSAGE_DECK_STATUS:
                return "Deck Status";
            case Constants::MESSAGE_SET_MENU_LANGUAGE:
                return "Set Menu Language";
            case Constants::MESSAGE_CLEAR_ANALOG_TIMER:
                return "Clear Analog Timer";
            case Constants::MESSAGE_SET_ANALOG_TIMER:
                return "Set Analog Timer";
            case Constants::MESSAGE_TIMER_STATUS:
                return "Timer Status";
            case Constants::MESSAGE_STANDBY:
                return "Standby";
            case Constants::MESSAGE_PLAY:
                return "Play";
            case Constants::MESSAGE_DECK_CONTROL:
                return "Deck Control";
            case Constants::MESSAGE_TIMER_CLEARED_STATUS:
                return "Timer Cleared Status";
            case Constants::MESSAGE_USER_CONTROL_PRESSED:
                return "User Control Pressed";
            case Constants::MESSAGE_USER_CONTROL_RELEASED:
                return "User Control Release";
            case Constants::MESSAGE_GIVE_OSD_NAME:
                return "Give Osd Name";
            case Constants::MESSAGE_SET_OSD_NAME:
                return "Set Osd Name";
            case Constants::MESSAGE_SET_OSD_STRING:
                return "Set Osd String";
            case Constants::MESSAGE_SET_TIMER_PROGRAM_TITLE:
                return "Set Timer Program Title";
            case Constants::MESSAGE_SYSTEM_AUDIO_MODE_REQUEST:
                return "System Audio Mode Request";
            case Constants::MESSAGE_GIVE_AUDIO_STATUS:
                return "Give Audio Status";
            case Constants::MESSAGE_SET_SYSTEM_AUDIO_MODE:
                return "Set System Audio Mode";
            case Constants::MESSAGE_REPORT_AUDIO_STATUS:
                return "Report Audio Status";
            case Constants::MESSAGE_GIVE_SYSTEM_AUDIO_MODE_STATUS:
                return "Give System Audio Mode Status";
            case Constants::MESSAGE_SYSTEM_AUDIO_MODE_STATUS:
                return "System Audio Mode Status";
            case Constants::MESSAGE_ROUTING_CHANGE:
                return "Routing Change";
            case Constants::MESSAGE_ROUTING_INFORMATION:
                return "Routing Information";
            case Constants::MESSAGE_ACTIVE_SOURCE:
                return "Active Source";
            case Constants::MESSAGE_GIVE_PHYSICAL_ADDRESS:
                return "Give Physical Address";
            case Constants::MESSAGE_REPORT_PHYSICAL_ADDRESS:
                return "Report Physical Address";
            case Constants::MESSAGE_REQUEST_ACTIVE_SOURCE:
                return "Request Active Source";
            case Constants::MESSAGE_SET_STREAM_PATH:
                return "Set Stream Path";
            case Constants::MESSAGE_DEVICE_VENDOR_ID:
                return "Device Vendor Id";
            case Constants::MESSAGE_VENDOR_COMMAND:
                return "Vendor Commandn";
            case Constants::MESSAGE_VENDOR_REMOTE_BUTTON_DOWN:
                return "Vendor Remote Button Down";
            case Constants::MESSAGE_VENDOR_REMOTE_BUTTON_UP:
                return "Vendor Remote Button Up";
            case Constants::MESSAGE_GIVE_DEVICE_VENDOR_ID:
                return "Give Device Vendor Id";
            case Constants::MESSAGE_MENU_REQUEST:
                return "Menu REquest";
            case Constants::MESSAGE_MENU_STATUS:
                return "Menu Status";
            case Constants::MESSAGE_GIVE_DEVICE_POWER_STATUS:
                return "Give Device Power Status";
            case Constants::MESSAGE_REPORT_POWER_STATUS:
                return "Report Power Status";
            case Constants::MESSAGE_GET_MENU_LANGUAGE:
                return "Get Menu Language";
            case Constants::MESSAGE_SELECT_ANALOG_SERVICE:
                return "Select Analog Service";
            case Constants::MESSAGE_SELECT_DIGITAL_SERVICE:
                return "Select Digital Service";
            case Constants::MESSAGE_SET_DIGITAL_TIMER:
                return "Set Digital Timer";
            case Constants::MESSAGE_CLEAR_DIGITAL_TIMER:
                return "Clear Digital Timer";
            case Constants::MESSAGE_SET_AUDIO_RATE:
                return "Set Audio Rate";
            case Constants::MESSAGE_INACTIVE_SOURCE:
                return "InActive Source";
            case Constants::MESSAGE_CEC_VERSION:
                return "Cec Version";
            case Constants::MESSAGE_GET_CEC_VERSION:
                return "Get Cec Version";
            case Constants::MESSAGE_VENDOR_COMMAND_WITH_ID:
                return "Vendor Command With Id";
            case Constants::MESSAGE_CLEAR_EXTERNAL_TIMER:
                return "Clear External Timer";
            case Constants::MESSAGE_SET_EXTERNAL_TIMER:
                return "Set External Timer";
            case Constants::MESSAGE_REPORT_SHORT_AUDIO_DESCRIPTOR:
                return "Repot Short Audio Descriptor";
            case Constants::MESSAGE_REQUEST_SHORT_AUDIO_DESCRIPTOR:
                return "Request Short Audio Descriptor";
            case Constants::MESSAGE_INITIATE_ARC:
                return "Initiate ARC";
            case Constants::MESSAGE_REPORT_ARC_INITIATED:
                return "Report ARC Initiated";
            case Constants::MESSAGE_REPORT_ARC_TERMINATED:
                return "Report ARC Terminated";
            case Constants::MESSAGE_REQUEST_ARC_INITIATION:
                return "Request ARC Initiation";
            case Constants::MESSAGE_REQUEST_ARC_TERMINATION:
                return "Request ARC Termination";
            case Constants::MESSAGE_TERMINATE_ARC:
                return "Terminate ARC";
            case Constants::MESSAGE_CDC_MESSAGE:
                return "Cdc Message";
            case Constants::MESSAGE_ABORT:
                return "Abort";
            default:
                return String.format("Opcode: %02X", opcode);
        }

#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
