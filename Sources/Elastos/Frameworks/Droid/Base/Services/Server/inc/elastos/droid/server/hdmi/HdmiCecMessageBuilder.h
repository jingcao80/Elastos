//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SERVER_HDMI_HDMICECMESSAGEBUILDER_H__
#define __ELASTOS_DROID_SERVER_HDMI_HDMICECMESSAGEBUILDER_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * A helper class to build {@link HdmiCecMessage} from various cec commands.
 */
class HdmiCecMessageBuilder
    : public Object
{
public:
    /**
     * Build {@link HdmiCecMessage} from raw data.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param body body of message. It includes opcode.
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI Of(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ ArrayOf<Byte>* body,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Feature Abort&gt; command. &lt;Feature Abort&gt; consists of
     * 1 byte original opcode and 1 byte reason fields with basic fields.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param originalOpcode original opcode causing feature abort
     * @param reason reason of feature abort
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildFeatureAbortCommand(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ Int32 originalOpcode,
        /* [in] */ Int32 reason,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Give Physical Address&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildGivePhysicalAddress(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Give Osd Name&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildGiveOsdNameCommand(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Give Vendor Id Command&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildGiveDeviceVendorIdCommand(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Set Menu Language &gt; command.
     *
     * <p>This is a broadcast message sent to all devices on the bus.
     *
     * @param src source address of command
     * @param language 3-letter ISO639-2 based language code
     * @return newly created {@link HdmiCecMessage} if language is valid.
     *         Otherwise, return null
     */
    static CARAPI BuildSetMenuLanguageCommand(
        /* [in] */ Int32 src,
        /* [in] */ const String& language,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Set Osd Name &gt; command.
     *
     * @param src source address of command
     * @param name display (OSD) name of device
     * @return newly created {@link HdmiCecMessage} if valid name. Otherwise,
     *         return null
     */
    static CARAPI BuildSetOsdNameCommand(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ const String& name,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Report Physical Address&gt; command. It has two bytes physical
     * address and one byte device type as parameter.
     *
     * <p>This is a broadcast message sent to all devices on the bus.
     *
     * @param src source address of command
     * @param address physical address of device
     * @param deviceType type of device
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildReportPhysicalAddressCommand(
        /* [in] */ Int32 src,
        /* [in] */ Int32 address,
        /* [in] */ Int32 deviceType,
        /* [out] */ IHdmiCecMessage** result);

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
    static CARAPI BuildDeviceVendorIdCommand(
        /* [in] */ Int32 src,
        /* [in] */ Int32 vendorId,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Device Vendor Id&gt; command. It has one byte cec version as parameter.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param version version of cec. Use 0x04 for "Version 1.3a" and 0x05 for
     *                "Version 1.4 or 1.4a or 1.4b
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildCecVersion(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ Int32 version,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Request Arc Initiation&gt;
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildRequestArcInitiation(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Request Arc Termination&gt;
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildRequestArcTermination(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Report Arc Initiated&gt;
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildReportArcInitiated(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Report Arc Terminated&gt;
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildReportArcTerminated(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Text View On&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildTextViewOn(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Active Source&gt; command.
     *
     * @param src source address of command
     * @param physicalAddress physical address of the device to become active
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildActiveSource(
        /* [in] */ Int32 src,
        /* [in] */ Int32 physicalAddress,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Inactive Source&gt; command.
     *
     * @param src source address of command
     * @param physicalAddress physical address of the device to become inactive
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildInactiveSource(
        /* [in] */ Int32 src,
        /* [in] */ Int32 physicalAddress,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Set Stream Path&gt; command.
     *
     * <p>This is a broadcast message sent to all devices on the bus.
     *
     * @param src source address of command
     * @param streamPath physical address of the device to start streaming
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildSetStreamPath(
        /* [in] */ Int32 src,
        /* [in] */ Int32 streamPath,
        /* [out] */ IHdmiCecMessage** result);

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
    static CARAPI BuildRoutingChange(
        /* [in] */ Int32 src,
        /* [in] */ Int32 oldPath,
        /* [in] */ Int32 newPath,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Give Device Power Status&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildGiveDevicePowerStatus(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Report Power Status&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param powerStatus power status of the device
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildReportPowerStatus(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ Int32 powerStatus,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Report Menu Status&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param menuStatus menu status of the device
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildReportMenuStatus(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ Int32 menuStatus,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;System Audio Mode Request&gt; command.
     *
     * @param src source address of command
     * @param avr destination address of command, it should be AVR
     * @param avrPhysicalAddress physical address of AVR
     * @param enableSystemAudio whether to enable System Audio Mode or not
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildSystemAudioModeRequest(
        /* [in] */ Int32 src,
        /* [in] */ Int32 avr,
        /* [in] */ Int32 avrPhysicalAddress,
        /* [in] */ Boolean enableSystemAudio,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Give Audio Status&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildGiveAudioStatus(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;User Control Pressed&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param uiCommand keycode that user pressed
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildUserControlPressed(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ Int32 uiCommand,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;User Control Pressed&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param commandParam uiCommand and the additional parameter
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildUserControlPressed(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ ArrayOf<Byte>* commandParam,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;User Control Released&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildUserControlReleased(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Give System Audio Mode Status&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildGiveSystemAudioModeStatus(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Standby&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildStandby(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Vendor Command&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param params vendor-specific parameters
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildVendorCommand(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ ArrayOf<Byte>* params,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Vendor Command With ID&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param vendorId vendor ID
     * @param operands vendor-specific parameters
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildVendorCommandWithId(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ Int32 vendorId,
        /* [in] */ ArrayOf<Byte>* operands,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Record On&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param params parameter of command
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildRecordOn(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ ArrayOf<Byte>* params,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Record Off&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildRecordOff(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Set Digital Timer&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param params byte array of timing information and digital service information to be recorded
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildSetDigitalTimer(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ ArrayOf<Byte>* params,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Set Analogue Timer&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param params byte array of timing information and analog service information to be recorded
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildSetAnalogueTimer(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ ArrayOf<Byte>* params,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Set External Timer&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param params byte array of timing information and external source information to be recorded
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildSetExternalTimer(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ ArrayOf<Byte>* params,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Clear Digital Timer&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param params byte array of timing information and digital service information to be cleared
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildClearDigitalTimer(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ ArrayOf<Byte>* params,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Clear Analog Timer&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param params byte array of timing information and analog service information to be cleared
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildClearAnalogueTimer(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ ArrayOf<Byte>* params,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build &lt;Clear Digital Timer&gt; command.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param params byte array of timing information and external source information to be cleared
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildClearExternalTimer(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ ArrayOf<Byte>* params,
        /* [out] */ IHdmiCecMessage** result);

private:
    HdmiCecMessageBuilder();

    /**
     * Build a {@link HdmiCecMessage} without extra parameter.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param opcode opcode for a message
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildCommand(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ Int32 opcode,
        /* [out] */ IHdmiCecMessage** result);

    /**
     * Build a {@link HdmiCecMessage} with given values.
     *
     * @param src source address of command
     * @param dest destination address of command
     * @param opcode opcode for a message
     * @param params extra parameters for command
     * @return newly created {@link HdmiCecMessage}
     */
    static CARAPI BuildCommand(
        /* [in] */ Int32 src,
        /* [in] */ Int32 dest,
        /* [in] */ Int32 opcode,
        /* [in] */ ArrayOf<Byte>* params,
        /* [out] */ IHdmiCecMessage** result);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) PhysicalAddressToParam(
        /* [in] */ Int32 physicalAddress);

    static const Int32 OSD_NAME_MAX_LENGTH;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_HDMICECMESSAGEBUILDER_H__
