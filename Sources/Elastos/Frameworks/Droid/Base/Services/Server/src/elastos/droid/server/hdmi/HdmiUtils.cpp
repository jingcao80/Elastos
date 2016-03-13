
#include "elastos/droid/server/hdmi/HdmiUtils.h"
#include "elastos/droid/server/hdmi/Constants.h"
#include <Elastos.Droid.Hardware.h>
#include <Elastos.CoreLibrary.Utility.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const AutoPtr<ArrayOf<Int32> > HdmiUtils::ADDRESS_TO_TYPE = InitADDRESS_TO_TYPE();

const AutoPtr<ArrayOf<String> > HdmiUtils::DEFAULT_NAMES = InitDEFAULT_NAMES();

HdmiUtils::HdmiUtils()
{
    /* cannot be instantiated */
}

Boolean HdmiUtils::IsValidAddress(
    /* [in] */ Int32 address)
{
    return (Constants::ADDR_TV <= address && address <= Constants::ADDR_SPECIFIC_USE);
}

Int32 HdmiUtils::GetTypeFromAddress(
    /* [in] */ Int32 address)
{
    if (IsValidAddress(address)) {
        return (*ADDRESS_TO_TYPE)[address];
    }
    return IHdmiDeviceInfo::DEVICE_INACTIVE;
}

String HdmiUtils::GetDefaultDeviceName(
    /* [in] */ Int32 address)
{
    if (IsValidAddress(address)) {
        return (*DEFAULT_NAMES)[address];
    }
    return String("");
}

ECode HdmiUtils::VerifyAddressType(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 deviceType)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Int32 actualDeviceType = GetTypeFromAddress(logicalAddress);
        if (actualDeviceType != deviceType) {
            Logger::E(TAG, "Device type missmatch:[Expected:" + deviceType + ", Actual:" + actualDeviceType);
            return E_IllegalArgumentException;
        }
#endif
}

Boolean HdmiUtils::CheckCommandSource(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [in] */ Int32 expectedAddress,
    /* [in] */ const String& tag)
{
    return FALSE;
#if 0 // TODO: Translate codes below
        Int32 srcAddr;
        cmd->GetSource(&srcAddr);
        Int32 src = srcAddr;
        if (src != expectedAddress) {
            Slogger::W(tag, "Invalid source [Expected:" + expectedAddress + ", Actual:" + src + "]");
            return FALSE;
        }
        return = TRUE;
#endif
}

Boolean HdmiUtils::ParseCommandParamSystemAudioStatus(
    /* [in] */ IHdmiCecMessage* cmd)
{
    return FALSE;
#if 0 // TODO: Translate codes below
        AutoPtr<ArrayOf<Byte> > params;
        cmd->GetParams((ArrayOf<Byte>**)&params);
        return (*params)[0] == Constants::SYSTEM_AUDIO_STATUS_ON;
#endif
}

AutoPtr<IList> HdmiUtils::AsImmutableList(
    /* [in] */ ArrayOf<Int32>* is)
{
    AutoPtr<IList> rev;
#if 0 // TODO: Translate codes below
        AutoPtr<IArrayList> list;
        CArrayList::New(is->GetLength(), (IArrayList**)&list);
        for (Int32 type : is) {
            list->Add(type);
        }
        return Collections->UnmodifiableList(list);
#endif
    return rev;
}

Int32 HdmiUtils::TwoBytesToInt32(
    /* [in] */ ArrayOf<Byte>* data)
{
    return 0;
#if 0 // TODO: Translate codes below
        return (((*data)[offset] & 0xFF) << 8) | ((*data)[offset + 1] & 0xFF);
#endif
}

Int32 HdmiUtils::TwoBytesToInt32(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset)
{
    return 0;
#if 0 // TODO: Translate codes below
        return (((*data)[0] & 0xFF) << 16) | (((*data)[1] & 0xFF) << 8) | ((*data)[2] & 0xFF);
#endif
}

Int32 HdmiUtils::ThreeBytesToInt32(
    /* [in] */ ArrayOf<Byte>* data)
{
    return 0;
#if 0 // TODO: Translate codes below
        return (((*data)[0] & 0xFF) << 16) | (((*data)[1] & 0xFF) << 8) | ((*data)[2] & 0xFF);

#endif
}

AutoPtr<IList> HdmiUtils::SparseArrayToList(
    /* [in] */ ISparseArray* array)
{
    AutoPtr<IList> rev;
#if 0 // TODO: Translate codes below
        AutoPtr<IArrayList> list;
        CArrayList::New((IArrayList**)&list);
        Int32 size;
        array->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            list->Add(array->ValueAt(i));
        }
        return list;
#endif
    return rev;
}

AutoPtr<IList> HdmiUtils::MergeToUnmodifiableList(
    /* [in] */ IList* a,
    /* [in] */ IList* b)
{
    AutoPtr<IList> rev;
#if 0 // TODO: Translate codes below
        Boolean aIsEmpty;
        a->IsEmpty(&aIsEmpty);
        Boolean bIsEmpty;
        b->IsEmpty(&bIsEmpty);
        if (aIsEmpty && bIsEmpty) {
            return Collections->EmptyList();
        }
        if (aIsEmpty) {
            return Collections->UnmodifiableList(b);
        }
        if (bIsEmpty) {
            return Collections->UnmodifiableList(a);
        }
        AutoPtr<IList> newList;
        CArrayList::New((IList**)&newList);
        newList->AddAll(a);
        newList->AddAll(b);
        return Collections->UnmodifiableList(newList);
#endif
    return rev;
}

Boolean HdmiUtils::IsAffectingActiveRoutingPath(
    /* [in] */ Int32 activePath,
    /* [in] */ Int32 newPath)
{
    return FALSE;
#if 0 // TODO: Translate codes below
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
            *result = TRUE;
            return NOERROR;  // Top path always affects the active path
        }
        return IsInActiveRoutingPath(activePath, newPath);
#endif
}

Boolean HdmiUtils::IsInActiveRoutingPath(
    /* [in] */ Int32 activePath,
    /* [in] */ Int32 newPath)
{
    return FALSE;
#if 0 // TODO: Translate codes below
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
                *result = FALSE;
                return NOERROR;
            }
        }
        *result = TRUE;
        return NOERROR;
#endif
}

AutoPtr<IHdmiDeviceInfo> HdmiUtils::CloneHdmiDeviceInfo(
    /* [in] */ IHdmiDeviceInfo* info,
    /* [in] */ Int32 newPowerStatus)
{
    AutoPtr<IHdmiDeviceInfo> rev;
#if 0 // TODO: Translate codes below
        Int32 logicalAddr;
        info->GetLogicalAddress(&logicalAddr);
        Int32 physicalAddr;
        info->GetPhysicalAddress(&physicalAddr);
        String displayName;
        info->GetDisplayName(&displayName);
        Int32 portId;
        info->GetPortId(&portId);
        Int32 deviceType;
        info->GetDeviceType(&deviceType);
        Int32 vendorId;
        info->GetVendorId(&vendorId);
        return new HdmiDeviceInfo(logicalAddr,
                physicalAddr, portId, deviceType,
                vendorId, displayName, newPowerStatus);
#endif
    return rev;
}

AutoPtr<ArrayOf<Int32> > HdmiUtils::InitADDRESS_TO_TYPE()
{
    AutoPtr<ArrayOf<Int32> > rev;
#if 0 // TODO: Translate codes below
    private static int[] ADDRESS_TO_TYPE = {
        IHdmiDeviceInfo::DEVICE_TV,  // ADDR_TV
        IHdmiDeviceInfo::DEVICE_RECORDER,  // ADDR_RECORDER_1
        IHdmiDeviceInfo::DEVICE_RECORDER,  // ADDR_RECORDER_2
        IHdmiDeviceInfo::DEVICE_TUNER,  // ADDR_TUNER_1
        IHdmiDeviceInfo::DEVICE_PLAYBACK,  // ADDR_PLAYBACK_1
        IHdmiDeviceInfo::DEVICE_AUDIO_SYSTEM,  // ADDR_AUDIO_SYSTEM
        IHdmiDeviceInfo::DEVICE_TUNER,  // ADDR_TUNER_2
        IHdmiDeviceInfo::DEVICE_TUNER,  // ADDR_TUNER_3
        IHdmiDeviceInfo::DEVICE_PLAYBACK,  // ADDR_PLAYBACK_2
        IHdmiDeviceInfo::DEVICE_RECORDER,  // ADDR_RECORDER_3
        IHdmiDeviceInfo::DEVICE_TUNER,  // ADDR_TUNER_4
        IHdmiDeviceInfo::DEVICE_PLAYBACK,  // ADDR_PLAYBACK_3
        IHdmiDeviceInfo::DEVICE_RESERVED,
        IHdmiDeviceInfo::DEVICE_RESERVED,
        IHdmiDeviceInfo::DEVICE_TV,  // ADDR_SPECIFIC_USE
    }
#endif
    return rev;
}

AutoPtr<ArrayOf<String> > HdmiUtils::InitDEFAULT_NAMES()
{
    AutoPtr<ArrayOf<String> > rev;
#if 0 // TODO: Translate codes below
    private static AutoPtr<ArrayOf<String> > DEFAULT_NAMES = {
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
    }
#endif
    return rev;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
