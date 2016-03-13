
#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/HdmiCecMessage.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/utility/Arrays.h>
#include <libcore/utility/EmptyArray.h>

using Elastos::Core::IByte;
using Elastos::Core::StringBuffer;
using Elastos::Utility::Arrays;
using Libcore::Utility::EmptyArray;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CAR_INTERFACE_IMPL(HdmiCecMessage, Object, IHdmiCecMessage)

AutoPtr<ArrayOf<Byte> > HdmiCecMessage::EMPTY_PARAM = EmptyArray::BYTE;

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
    mSource = source;
    mDestination = destination;
    mOpcode = opcode & 0xFF;
    Arrays::CopyOf(params, params->GetLength(), (ArrayOf<Byte>**)&mParams);
    return NOERROR;
}

ECode HdmiCecMessage::GetEMPTY_PARAM(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)

    *result = EMPTY_PARAM;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecMessage::GetSource(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mSource;
    return NOERROR;
}

ECode HdmiCecMessage::GetDestination(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mDestination;
    return NOERROR;
}

ECode HdmiCecMessage::GetOpcode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mOpcode;
    return NOERROR;
}

ECode HdmiCecMessage::GetParams(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mParams;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecMessage::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String s;
    String str;
    OpcodeToString(mOpcode, &str);
    s.AppendFormat("<%s> src: %d, dst: %d", str.string(), mSource, mDestination);
    if (mParams->GetLength() > 0) {
        s.Append(", params:");
        for(Int32 i = 0; i < mParams->GetLength(); ++i) {
            Byte data = (*mParams)[i];
            s.AppendFormat(" %02X", data);
        }
    }
    *result = s;
    return NOERROR;
}

ECode HdmiCecMessage::OpcodeToString(
    /* [in] */ Int32 opcode,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    if (opcode == Constants::MESSAGE_FEATURE_ABORT) {
        *result = String("Feature Abort");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_IMAGE_VIEW_ON) {
        *result = String("Image View On");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_TUNER_STEP_INCREMENT) {
        *result = String("Tuner Step Increment");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_TUNER_STEP_DECREMENT) {
        *result = String("Tuner Step Decrement");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_TUNER_DEVICE_STATUS) {
        *result = String("Tuner Device Staus");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_GIVE_TUNER_DEVICE_STATUS) {
        *result = String("Give Tuner Device Status");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_RECORD_ON) {
        *result = String("Record On");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_RECORD_STATUS) {
        *result = String("Record Status");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_RECORD_OFF) {
        *result = String("Record Off");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_TEXT_VIEW_ON) {
        *result = String("Text View On");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_RECORD_TV_SCREEN) {
        *result = String("Record Tv Screen");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_GIVE_DECK_STATUS) {
        *result = String("Give Deck Status");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_DECK_STATUS) {
        *result = String("Deck Status");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_SET_MENU_LANGUAGE) {
        *result = String("Set Menu Language");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_CLEAR_ANALOG_TIMER) {
        *result = String("Clear Analog Timer");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_SET_ANALOG_TIMER) {
        *result = String("Set Analog Timer");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_TIMER_STATUS) {
        *result = String("Timer Status");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_STANDBY) {
        *result = String("Standby");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_PLAY) {
        *result = String("Play");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_DECK_CONTROL) {
        *result = String("Deck Control");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_TIMER_CLEARED_STATUS) {
        *result = String("Timer Cleared Status");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_USER_CONTROL_PRESSED) {
        *result = String("User Control Pressed");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_USER_CONTROL_RELEASED) {
        *result = String("User Control Release");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_GIVE_OSD_NAME) {
        *result = String("Give Osd Name");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_SET_OSD_NAME) {
        *result = String("Set Osd Name");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_SET_OSD_STRING) {
        *result = String("Set Osd String");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_SET_TIMER_PROGRAM_TITLE) {
        *result = String("Set Timer Program Title");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_SYSTEM_AUDIO_MODE_REQUEST) {
        *result = String("System Audio Mode Request");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_GIVE_AUDIO_STATUS) {
        *result = String("Give Audio Status");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_SET_SYSTEM_AUDIO_MODE) {
        *result = String("Set System Audio Mode");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_REPORT_AUDIO_STATUS) {
        *result = String("Report Audio Status");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_GIVE_SYSTEM_AUDIO_MODE_STATUS) {
        *result = String("Give System Audio Mode Status");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_SYSTEM_AUDIO_MODE_STATUS) {
        *result = String("System Audio Mode Status");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_ROUTING_CHANGE) {
        *result = String("Routing Change");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_ROUTING_INFORMATION) {
        *result = String("Routing Information");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_ACTIVE_SOURCE) {
        *result = String("Active Source");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_GIVE_PHYSICAL_ADDRESS) {
        *result = String("Give Physical Address");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_REPORT_PHYSICAL_ADDRESS) {
        *result = String("Report Physical Address");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_REQUEST_ACTIVE_SOURCE) {
        *result = String("Request Active Source");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_SET_STREAM_PATH) {
        *result = String("Set Stream Path");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_DEVICE_VENDOR_ID) {
        *result = String("Device Vendor Id");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_VENDOR_COMMAND) {
        *result = String("Vendor Commandn");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_VENDOR_REMOTE_BUTTON_DOWN) {
        *result = String("Vendor Remote Button Down");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_VENDOR_REMOTE_BUTTON_UP) {
        *result = String("Vendor Remote Button Up");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_GIVE_DEVICE_VENDOR_ID) {
        *result = String("Give Device Vendor Id");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_MENU_REQUEST) {
        *result = String("Menu REquest");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_MENU_STATUS) {
        *result = String("Menu Status");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_GIVE_DEVICE_POWER_STATUS) {
        *result = String("Give Device Power Status");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_REPORT_POWER_STATUS) {
        *result = String("Report Power Status");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_GET_MENU_LANGUAGE) {
        *result = String("Get Menu Language");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_SELECT_ANALOG_SERVICE) {
        *result = String("Select Analog Service");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_SELECT_DIGITAL_SERVICE) {
        *result = String("Select Digital Service");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_SET_DIGITAL_TIMER) {
        *result = String("Set Digital Timer");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_CLEAR_DIGITAL_TIMER) {
        *result = String("Clear Digital Timer");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_SET_AUDIO_RATE) {
        *result = String("Set Audio Rate");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_INACTIVE_SOURCE) {
        *result = String("InActive Source");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_CEC_VERSION) {
        *result = String("Cec Version");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_GET_CEC_VERSION) {
        *result = String("Get Cec Version");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_VENDOR_COMMAND_WITH_ID) {
        *result = String("Vendor Command With Id");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_CLEAR_EXTERNAL_TIMER) {
        *result = String("Clear External Timer");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_SET_EXTERNAL_TIMER) {
        *result = String("Set External Timer");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_REPORT_SHORT_AUDIO_DESCRIPTOR) {
        *result = String("Repot Short Audio Descriptor");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_REQUEST_SHORT_AUDIO_DESCRIPTOR) {
        *result = String("Request Short Audio Descriptor");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_INITIATE_ARC) {
        *result = String("Initiate ARC");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_REPORT_ARC_INITIATED) {
        *result = String("Report ARC Initiated");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_REPORT_ARC_TERMINATED) {
        *result = String("Report ARC Terminated");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_REQUEST_ARC_INITIATION) {
        *result = String("Request ARC Initiation");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_REQUEST_ARC_TERMINATION) {
        *result = String("Request ARC Termination");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_TERMINATE_ARC) {
        *result = String("Terminate ARC");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_CDC_MESSAGE) {
        *result = String("Cdc Message");
        return NOERROR;
    }
    else if (opcode == Constants::MESSAGE_ABORT) {
        *result = String("Abort");
        return NOERROR;
    }
    else {
        *result = String();
        (*result).AppendFormat("Opcode: %02X", opcode);
        return NOERROR;
    }
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
