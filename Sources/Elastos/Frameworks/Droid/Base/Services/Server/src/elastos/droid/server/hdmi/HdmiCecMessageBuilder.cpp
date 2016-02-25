
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include <elastos/utility/Arrays.h>

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
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        byte opcode = body[0];
        byte params[] = Arrays.copyOfRange(body, 1, body.length);
        return new HdmiCecMessage(src, dest, opcode, params);

#endif
}

ECode HdmiCecMessageBuilder::BuildFeatureAbortCommand(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ Int32 originalOpcode,
    /* [in] */ Int32 reason,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        byte[] params = new byte[] {
                (byte) (originalOpcode & 0xFF),
                (byte) (reason & 0xFF),
        };
        return buildCommand(src, dest, Constants::MESSAGE_FEATURE_ABORT, params);

#endif
}

ECode HdmiCecMessageBuilder::BuildGivePhysicalAddress(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_GIVE_PHYSICAL_ADDRESS);

#endif
}

ECode HdmiCecMessageBuilder::BuildGiveOsdNameCommand(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_GIVE_OSD_NAME);

#endif
}

ECode HdmiCecMessageBuilder::BuildGiveDeviceVendorIdCommand(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_GIVE_DEVICE_VENDOR_ID);

#endif
}

ECode HdmiCecMessageBuilder::BuildSetMenuLanguageCommand(
    /* [in] */ Int32 src,
    /* [in] */ const String& language,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (language.length() != 3) {
            return NULL;
        }
        // Hdmi CEC uses lower-cased ISO 639-2 (3 letters code).
        String normalized = language.toLowerCase();
        byte[] params = new byte[] {
                (byte) (normalized.charAt(0) & 0xFF),
                (byte) (normalized.charAt(1) & 0xFF),
                (byte) (normalized.charAt(2) & 0xFF),
        };
        // <Set Menu Language> is broadcast message.
        return buildCommand(src, Constants::ADDR_BROADCAST,
                Constants::MESSAGE_SET_MENU_LANGUAGE, params);

#endif
}

ECode HdmiCecMessageBuilder::BuildSetOsdNameCommand(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ const String& name,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int length = Math.min(name.length(), OSD_NAME_MAX_LENGTH);
        byte[] params;
        try {
            params = name.substring(0, length).getBytes("US-ASCII");
        } catch (UnsupportedEncodingException e) {
            return NULL;
        }
        return buildCommand(src, dest, Constants::MESSAGE_SET_OSD_NAME, params);

#endif
}

ECode HdmiCecMessageBuilder::BuildReportPhysicalAddressCommand(
    /* [in] */ Int32 src,
    /* [in] */ Int32 address,
    /* [in] */ Int32 deviceType,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        byte[] params = new byte[] {
                // Two bytes for physical address
                (byte) ((address >> 8) & 0xFF),
                (byte) (address & 0xFF),
                // One byte device type
                (byte) (deviceType & 0xFF)
        };
        // <Report Physical Address> is broadcast message.
        return buildCommand(src, Constants::ADDR_BROADCAST,
                Constants::MESSAGE_REPORT_PHYSICAL_ADDRESS, params);

#endif
}

ECode HdmiCecMessageBuilder::BuildDeviceVendorIdCommand(
    /* [in] */ Int32 src,
    /* [in] */ Int32 vendorId,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        byte[] params = new byte[] {
                (byte) ((vendorId >> 16) & 0xFF),
                (byte) ((vendorId >> 8) & 0xFF),
                (byte) (vendorId & 0xFF)
        };
        // <Device Vendor Id> is broadcast message.
        return buildCommand(src, Constants::ADDR_BROADCAST,
                Constants::MESSAGE_DEVICE_VENDOR_ID, params);

#endif
}

ECode HdmiCecMessageBuilder::BuildCecVersion(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ Int32 version,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        byte[] params = new byte[] {
                (byte) (version & 0xFF)
        };
        return buildCommand(src, dest, Constants::MESSAGE_CEC_VERSION, params);

#endif
}

ECode HdmiCecMessageBuilder::BuildRequestArcInitiation(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_REQUEST_ARC_INITIATION);

#endif
}

ECode HdmiCecMessageBuilder::BuildRequestArcTermination(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_REQUEST_ARC_TERMINATION);

#endif
}

ECode HdmiCecMessageBuilder::BuildReportArcInitiated(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_REPORT_ARC_INITIATED);

#endif
}

ECode HdmiCecMessageBuilder::BuildReportArcTerminated(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_REPORT_ARC_TERMINATED);

#endif
}

ECode HdmiCecMessageBuilder::BuildTextViewOn(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_TEXT_VIEW_ON);

#endif
}

ECode HdmiCecMessageBuilder::BuildActiveSource(
    /* [in] */ Int32 src,
    /* [in] */ Int32 physicalAddress,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, Constants::ADDR_BROADCAST, Constants::MESSAGE_ACTIVE_SOURCE,
                physicalAddressToParam(physicalAddress));

#endif
}

ECode HdmiCecMessageBuilder::BuildInactiveSource(
    /* [in] */ Int32 src,
    /* [in] */ Int32 physicalAddress,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, Constants::ADDR_TV,
                Constants::MESSAGE_INACTIVE_SOURCE, physicalAddressToParam(physicalAddress));

#endif
}

ECode HdmiCecMessageBuilder::BuildSetStreamPath(
    /* [in] */ Int32 src,
    /* [in] */ Int32 streamPath,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, Constants::ADDR_BROADCAST,
                Constants::MESSAGE_SET_STREAM_PATH, physicalAddressToParam(streamPath));

#endif
}

ECode HdmiCecMessageBuilder::BuildRoutingChange(
    /* [in] */ Int32 src,
    /* [in] */ Int32 oldPath,
    /* [in] */ Int32 newPath,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        byte[] param = new byte[] {
            (byte) ((oldPath >> 8) & 0xFF), (byte) (oldPath & 0xFF),
            (byte) ((newPath >> 8) & 0xFF), (byte) (newPath & 0xFF)
        };
        return buildCommand(src, Constants::ADDR_BROADCAST, Constants::MESSAGE_ROUTING_CHANGE,
                param);

#endif
}

ECode HdmiCecMessageBuilder::BuildGiveDevicePowerStatus(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_GIVE_DEVICE_POWER_STATUS);

#endif
}

ECode HdmiCecMessageBuilder::BuildReportPowerStatus(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ Int32 powerStatus,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        byte[] param = new byte[] {
                (byte) (powerStatus & 0xFF)
        };
        return buildCommand(src, dest, Constants::MESSAGE_REPORT_POWER_STATUS, param);

#endif
}

ECode HdmiCecMessageBuilder::BuildReportMenuStatus(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ Int32 menuStatus,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        byte[] param = new byte[] {
                (byte) (menuStatus & 0xFF)
        };
        return buildCommand(src, dest, Constants::MESSAGE_MENU_STATUS, param);

#endif
}

ECode HdmiCecMessageBuilder::BuildSystemAudioModeRequest(
    /* [in] */ Int32 src,
    /* [in] */ Int32 avr,
    /* [in] */ Int32 avrPhysicalAddress,
    /* [in] */ Boolean enableSystemAudio,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (enableSystemAudio) {
            return buildCommand(src, avr, Constants::MESSAGE_SYSTEM_AUDIO_MODE_REQUEST,
                    physicalAddressToParam(avrPhysicalAddress));
        } else {
            return buildCommand(src, avr, Constants::MESSAGE_SYSTEM_AUDIO_MODE_REQUEST);
        }

#endif
}

ECode HdmiCecMessageBuilder::BuildGiveAudioStatus(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_GIVE_AUDIO_STATUS);

#endif
}

ECode HdmiCecMessageBuilder::BuildUserControlPressed(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ Int32 uiCommand,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildUserControlPressed(src, dest, new byte[] { (byte) (uiCommand & 0xFF) });

#endif
}

ECode HdmiCecMessageBuilder::BuildUserControlPressed(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* commandParam,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_USER_CONTROL_PRESSED, commandParam);

#endif
}

ECode HdmiCecMessageBuilder::BuildUserControlReleased(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_USER_CONTROL_RELEASED);

#endif
}

ECode HdmiCecMessageBuilder::BuildGiveSystemAudioModeStatus(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_GIVE_SYSTEM_AUDIO_MODE_STATUS);

#endif
}

ECode HdmiCecMessageBuilder::BuildStandby(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_STANDBY);

#endif
}

ECode HdmiCecMessageBuilder::BuildVendorCommand(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_VENDOR_COMMAND, params);

#endif
}

ECode HdmiCecMessageBuilder::BuildVendorCommandWithId(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ Int32 vendorId,
    /* [in] */ ArrayOf<Byte>* operands,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        byte[] params = new byte[operands.length + 3];  // parameter plus len(vendorId)
        params[0] = (byte) ((vendorId >> 16) & 0xFF);
        params[1] = (byte) ((vendorId >> 8) & 0xFF);
        params[2] = (byte) (vendorId & 0xFF);
        System.arraycopy(operands, 0, params, 3, operands.length);
        return buildCommand(src, dest, Constants::MESSAGE_VENDOR_COMMAND_WITH_ID, params);

#endif
}

ECode HdmiCecMessageBuilder::BuildRecordOn(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_RECORD_ON, params);

#endif
}

ECode HdmiCecMessageBuilder::BuildRecordOff(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_RECORD_OFF);

#endif
}

ECode HdmiCecMessageBuilder::BuildSetDigitalTimer(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_SET_DIGITAL_TIMER, params);

#endif
}

ECode HdmiCecMessageBuilder::BuildSetAnalogueTimer(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_SET_ANALOG_TIMER, params);

#endif
}

ECode HdmiCecMessageBuilder::BuildSetExternalTimer(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_SET_EXTERNAL_TIMER, params);

#endif
}

ECode HdmiCecMessageBuilder::BuildClearDigitalTimer(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_CLEAR_DIGITAL_TIMER, params);

#endif
}

ECode HdmiCecMessageBuilder::BuildClearAnalogueTimer(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_CLEAR_ANALOG_TIMER, params);

#endif
}

ECode HdmiCecMessageBuilder::BuildClearExternalTimer(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return buildCommand(src, dest, Constants::MESSAGE_CLEAR_EXTERNAL_TIMER, params);

#endif
}

ECode HdmiCecMessageBuilder::BuildCommand(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ Int32 opcode,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return new HdmiCecMessage(src, dest, opcode, HdmiCecMessage.EMPTY_PARAM);

#endif
}

ECode HdmiCecMessageBuilder::BuildCommand(
    /* [in] */ Int32 src,
    /* [in] */ Int32 dest,
    /* [in] */ Int32 opcode,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return new HdmiCecMessage(src, dest, opcode, params);

#endif
}

AutoPtr<ArrayOf<Byte> > HdmiCecMessageBuilder::PhysicalAddressToParam(
    /* [in] */ Int32 physicalAddress)
{
    AutoPtr<ArrayOf<Byte> > rev;
#if 0 // TODO: Translate codes below
        return new byte[] {
                (byte) ((physicalAddress >> 8) & 0xFF),
                (byte) (physicalAddress & 0xFF)
        };

#endif
    return rev;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
