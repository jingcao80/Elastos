
#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include <elastos/core/Math.h>
#include <elastos/utility/Arrays.h>
#include "elastos/droid/server/hdmi/HdmiCecMessage.h"

using Elastos::Core::Math;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const Int32 HdmiCecMessageBuilder::OSD_NAME_MAX_LENGTH = 13;

HdmiCecMessageBuilder::HdmiCecMessageBuilder()
{}

ECode HdmiCecMessageBuilder::Of(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* body,
    /* [out] */ IHdmiCecMessage** result)
{
    VALIDATE_NOT_NULL(result)

    Byte opcode = (*body)[0];
    AutoPtr<ArrayOf<Byte> > params;
    Arrays::CopyOfRange(body, 1, body->GetLength(), (ArrayOf<Byte>**)&params);
    *result = new HdmiCecMessage();
    ((HdmiCecMessage*)(*result))->constructor(src, dest, opcode, params);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecMessageBuilder::BuildFeatureAbortCommand(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ Int32 originalOpcode,
    /* [in] */ Int32 reason,
    /* [out] */ IHdmiCecMessage** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > params = ArrayOf<Byte>::Alloc(2);
    (*params)[0] = (Byte) (originalOpcode & 0xFF);
    (*params)[1] = (Byte) (reason & 0xFF);
    return BuildCommand(src, dest, Constants::MESSAGE_FEATURE_ABORT, params, result);
}

ECode HdmiCecMessageBuilder::BuildGivePhysicalAddress(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_GIVE_PHYSICAL_ADDRESS, result);
}

ECode HdmiCecMessageBuilder::BuildGiveOsdNameCommand(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_GIVE_OSD_NAME, result);
}

ECode HdmiCecMessageBuilder::BuildGiveDeviceVendorIdCommand(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_GIVE_DEVICE_VENDOR_ID, result);
}

ECode HdmiCecMessageBuilder::BuildSetMenuLanguageCommand(
    /* [in] */ Int32 src,
    /* [in] */ const String& language,
    /* [out] */ IHdmiCecMessage** result)
{
    VALIDATE_NOT_NULL(result)

    if (language.GetLength() != 3) {
        *result = NULL;
        return NOERROR;
    }
    // Hdmi CEC uses lower-cased ISO 639-2 (3 letters code).
    String normalized = language.ToLowerCase();
    AutoPtr<ArrayOf<Byte> > params = ArrayOf<Byte>::Alloc(3);
    (*params)[0] = (Byte) (normalized.GetChar(0) & 0xFF);
    (*params)[1] = (Byte) (normalized.GetChar(1) & 0xFF);
    (*params)[2] = (Byte) (normalized.GetChar(2) & 0xFF);
    // <Set Menu Language> is broadcast message.
    return BuildCommand(src, Constants::ADDR_BROADCAST,
            Constants::MESSAGE_SET_MENU_LANGUAGE, params, result);
}

ECode HdmiCecMessageBuilder::BuildSetOsdNameCommand(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ const String& name,
    /* [out] */ IHdmiCecMessage** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 length = Elastos::Core::Math::Min(name.GetLength(), OSD_NAME_MAX_LENGTH);
    AutoPtr<ArrayOf<Byte> > params;
    // try {
    params = name.Substring(0, length).GetBytes(/* "US-ASCII" */);
    // } catch (UnsupportedEncodingException e) {
        // *result = NULL;
        // return NOERROR;
    // }
    return BuildCommand(src, dest, Constants::MESSAGE_SET_OSD_NAME, params, result);
}

ECode HdmiCecMessageBuilder::BuildReportPhysicalAddressCommand(
    /* [in] */ Int32 src,
    /* [in] */ Int32 address,
    /* [in] */ Int32 deviceType,
    /* [out] */ IHdmiCecMessage** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > params = ArrayOf<Byte>::Alloc(3);
    // Two bytes for physical address
    (*params)[0] = (Byte) ((address >> 8) & 0xFF);
    (*params)[1] = (Byte) (address & 0xFF);
    // One byte device type
    (*params)[2] = (Byte) (deviceType & 0xFF);
    // <Report Physical Address> is broadcast message.
    return BuildCommand(src, Constants::ADDR_BROADCAST,
            Constants::MESSAGE_REPORT_PHYSICAL_ADDRESS, params, result);
}

ECode HdmiCecMessageBuilder::BuildDeviceVendorIdCommand(
    /* [in] */ Int32 src,
    /* [in] */ Int32 vendorId,
    /* [out] */ IHdmiCecMessage** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > params = ArrayOf<Byte>::Alloc(3);
    (*params)[0] = (Byte) ((vendorId >> 16) & 0xFF);
    (*params)[1] = (Byte) ((vendorId >> 8) & 0xFF);
    (*params)[2] = (Byte) (vendorId & 0xFF);
    // <Device Vendor Id> is broadcast message.
    return BuildCommand(src, Constants::ADDR_BROADCAST,
            Constants::MESSAGE_DEVICE_VENDOR_ID, params, result);
}

ECode HdmiCecMessageBuilder::BuildCecVersion(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ Int32 version,
    /* [out] */ IHdmiCecMessage** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > params = ArrayOf<Byte>::Alloc(1);
    (*params)[0] = (Byte) (version & 0xFF);
    return BuildCommand(src, dest, Constants::MESSAGE_CEC_VERSION, params, result);
}

ECode HdmiCecMessageBuilder::BuildRequestArcInitiation(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_REQUEST_ARC_INITIATION, result);
}

ECode HdmiCecMessageBuilder::BuildRequestArcTermination(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_REQUEST_ARC_TERMINATION, result);
}

ECode HdmiCecMessageBuilder::BuildReportArcInitiated(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_REPORT_ARC_INITIATED, result);
}

ECode HdmiCecMessageBuilder::BuildReportArcTerminated(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_REPORT_ARC_TERMINATED, result);
}

ECode HdmiCecMessageBuilder::BuildTextViewOn(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_TEXT_VIEW_ON, result);
}

ECode HdmiCecMessageBuilder::BuildActiveSource(
    /* [in] */ Int32 src,
    /* [in] */ Int32 physicalAddress,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, Constants::ADDR_BROADCAST, Constants::MESSAGE_ACTIVE_SOURCE,
            PhysicalAddressToParam(physicalAddress), result);
}

ECode HdmiCecMessageBuilder::BuildInactiveSource(
    /* [in] */ Int32 src,
    /* [in] */ Int32 physicalAddress,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, Constants::ADDR_TV,
            Constants::MESSAGE_INACTIVE_SOURCE, PhysicalAddressToParam(physicalAddress), result);
}

ECode HdmiCecMessageBuilder::BuildSetStreamPath(
    /* [in] */ Int32 src,
    /* [in] */ Int32 streamPath,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, Constants::ADDR_BROADCAST,
            Constants::MESSAGE_SET_STREAM_PATH, PhysicalAddressToParam(streamPath), result);
}

ECode HdmiCecMessageBuilder::BuildRoutingChange(
    /* [in] */ Int32 src,
    /* [in] */ Int32 oldPath,
    /* [in] */ Int32 newPath,
    /* [out] */ IHdmiCecMessage** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > param = ArrayOf<Byte>::Alloc(2);
    (*param)[0] = (Byte) ((oldPath >> 8) & 0xFF);
    (*param)[1] = (Byte) (oldPath & 0xFF);
    (*param)[2] = (Byte) ((newPath >> 8) & 0xFF);
    (*param)[3] =  (Byte) (newPath & 0xFF);
    return BuildCommand(src, Constants::ADDR_BROADCAST, Constants::MESSAGE_ROUTING_CHANGE,
            param, result);
}

ECode HdmiCecMessageBuilder::BuildGiveDevicePowerStatus(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_GIVE_DEVICE_POWER_STATUS, result);
}

ECode HdmiCecMessageBuilder::BuildReportPowerStatus(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ Int32 powerStatus,
    /* [out] */ IHdmiCecMessage** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > param = ArrayOf<Byte>::Alloc(1);
    (*param)[0] = (Byte) (powerStatus & 0xFF);
    return BuildCommand(src, dest, Constants::MESSAGE_REPORT_POWER_STATUS, param, result);
}

ECode HdmiCecMessageBuilder::BuildReportMenuStatus(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ Int32 menuStatus,
    /* [out] */ IHdmiCecMessage** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > param = ArrayOf<Byte>::Alloc(1);
    (*param)[0] = (Byte) (menuStatus & 0xFF);
    return BuildCommand(src, dest, Constants::MESSAGE_MENU_STATUS, param, result);
}

ECode HdmiCecMessageBuilder::BuildSystemAudioModeRequest(
    /* [in] */ Int32 src,
    /* [in] */ Int32 avr,
    /* [in] */ Int32 avrPhysicalAddress,
    /* [in] */ Boolean enableSystemAudio,
    /* [out] */ IHdmiCecMessage** result)
{
    VALIDATE_NOT_NULL(result)

    if (enableSystemAudio) {
        return BuildCommand(src, avr, Constants::MESSAGE_SYSTEM_AUDIO_MODE_REQUEST,
                PhysicalAddressToParam(avrPhysicalAddress), result);
    } else {
        return BuildCommand(src, avr, Constants::MESSAGE_SYSTEM_AUDIO_MODE_REQUEST, result);
    }
    return NOERROR;
}

ECode HdmiCecMessageBuilder::BuildGiveAudioStatus(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_GIVE_AUDIO_STATUS, result);
}

ECode HdmiCecMessageBuilder::BuildUserControlPressed(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ Int32 uiCommand,
    /* [out] */ IHdmiCecMessage** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > params = ArrayOf<Byte>::Alloc(1);
    (*params)[0] = (Byte) (uiCommand & 0xFF);
    return BuildUserControlPressed(src, dest, params, result);
}

ECode HdmiCecMessageBuilder::BuildUserControlPressed(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* commandParam,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_USER_CONTROL_PRESSED, commandParam, result);
}

ECode HdmiCecMessageBuilder::BuildUserControlReleased(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_USER_CONTROL_RELEASED, result);
}

ECode HdmiCecMessageBuilder::BuildGiveSystemAudioModeStatus(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_GIVE_SYSTEM_AUDIO_MODE_STATUS, result);
}

ECode HdmiCecMessageBuilder::BuildStandby(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_STANDBY, result);
}

ECode HdmiCecMessageBuilder::BuildVendorCommand(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_VENDOR_COMMAND, params, result);
}

ECode HdmiCecMessageBuilder::BuildVendorCommandWithId(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ Int32 vendorId,
    /* [in] */ ArrayOf<Byte>* operands,
    /* [out] */ IHdmiCecMessage** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > params = ArrayOf<Byte>::Alloc(operands->GetLength() + 3);  // parameter plus len(vendorId)
    (*params)[0] = (Byte) ((vendorId >> 16) & 0xFF);
    (*params)[1] = (Byte) ((vendorId >> 8) & 0xFF);
    (*params)[2] = (Byte) (vendorId & 0xFF);
    params->Copy(3, operands, 0, operands->GetLength());
    return BuildCommand(src, dest, Constants::MESSAGE_VENDOR_COMMAND_WITH_ID, params, result);
}

ECode HdmiCecMessageBuilder::BuildRecordOn(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_RECORD_ON, params, result);
}

ECode HdmiCecMessageBuilder::BuildRecordOff(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_RECORD_OFF, result);
}

ECode HdmiCecMessageBuilder::BuildSetDigitalTimer(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_SET_DIGITAL_TIMER, params, result);
}

ECode HdmiCecMessageBuilder::BuildSetAnalogueTimer(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_SET_ANALOG_TIMER, params, result);
}

ECode HdmiCecMessageBuilder::BuildSetExternalTimer(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_SET_EXTERNAL_TIMER, params, result);
}

ECode HdmiCecMessageBuilder::BuildClearDigitalTimer(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_CLEAR_DIGITAL_TIMER, params, result);
}

ECode HdmiCecMessageBuilder::BuildClearAnalogueTimer(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_CLEAR_ANALOG_TIMER, params, result);
}

ECode HdmiCecMessageBuilder::BuildClearExternalTimer(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out] */ IHdmiCecMessage** result)
{
    return BuildCommand(src, dest, Constants::MESSAGE_CLEAR_EXTERNAL_TIMER, params, result);
}

ECode HdmiCecMessageBuilder::BuildCommand(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ Int32 opcode,
    /* [out] */ IHdmiCecMessage** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > emptyParam;
    HdmiCecMessage::GetEMPTY_PARAM((ArrayOf<Byte>**)&emptyParam);
    *result = new HdmiCecMessage();
    ((HdmiCecMessage*)(*result))->constructor(src, dest, opcode, emptyParam);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecMessageBuilder::BuildCommand(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ Int32 opcode,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out] */ IHdmiCecMessage** result)
{
    VALIDATE_NOT_NULL(result)

    *result = new HdmiCecMessage();
    ((HdmiCecMessage*)(*result))->constructor(src, dest, opcode, params);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<ArrayOf<Byte> > HdmiCecMessageBuilder::PhysicalAddressToParam(
    /* [in] */ Int32 physicalAddress)
{
    AutoPtr<ArrayOf<Byte> > rev = ArrayOf<Byte>::Alloc(2);
    (*rev)[0] = (Byte) ((physicalAddress >> 8) & 0xFF);
    (*rev)[1] = (Byte) (physicalAddress & 0xFF);
    return rev;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
