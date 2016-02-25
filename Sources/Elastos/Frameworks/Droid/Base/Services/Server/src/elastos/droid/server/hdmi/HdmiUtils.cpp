
#include "elastos/droid/server/hdmi/HdmiUtils.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const AutoPtr<ArrayOf<Int32> > HdmiUtils::ADDRESS_TO_TYPE = InitADDRESS_TO_TYPE();

const AutoPtr<ArrayOf<String> > HdmiUtils::DEFAULT_NAMES = InitDEFAULT_NAMES();

HdmiUtils::HdmiUtils()
{
#if 0 // TODO: Translate codes below
 /* cannot be instantiated */
#endif
}

ECode HdmiUtils::IsValidAddress(
    /* [in] */ Int32 address,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return (Constants::ADDR_TV <= address && address <= Constants::ADDR_SPECIFIC_USE);
#endif
}

ECode HdmiUtils::GetTypeFromAddress(
    /* [in] */ Int32 address,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (IsValidAddress(address)) {
            return ADDRESS_TO_TYPE[address];
        }
        return HdmiDeviceInfo.DEVICE_INACTIVE;
#endif
}

ECode HdmiUtils::GetDefaultDeviceName(
    /* [in] */ Int32 address,
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (IsValidAddress(address)) {
            return DEFAULT_NAMES[address];
        }
        return "";
#endif
}

ECode HdmiUtils::VerifyAddressType(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 deviceType)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Int32 actualDeviceType = GetTypeFromAddress(logicalAddress);
        if (actualDeviceType != deviceType) {
            throw new IllegalArgumentException("Device type missmatch:[Expected:" + deviceType
                    + ", Actual:" + actualDeviceType);
        }
#endif
}

ECode HdmiUtils::CheckCommandSource(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [in] */ Int32 expectedAddress,
    /* [in] */ const String& tag,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Int32 src = cmd->GetSource();
        if (src != expectedAddress) {
            Slogger::W(tag, "Invalid source [Expected:" + expectedAddress + ", Actual:" + src + "]");
            return FALSE;
        }
        return TRUE;
#endif
}

ECode HdmiUtils::ParseCommandParamSystemAudioStatus(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return cmd->GetParams()[0] == Constants::SYSTEM_AUDIO_STATUS_ON;
#endif
}

ECode HdmiUtils::AsImmutableList(
    /* [in] */ ArrayOf<Int32>* is,
    /* [out] */ IList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ArrayList<Integer> list = new ArrayList<>(is.length);
        for (Int32 type : is) {
            list->Add(type);
        }
        return Collections->UnmodifiableList(list);
#endif
}

ECode HdmiUtils::TwoBytesToInt(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return ((data[offset] & 0xFF) << 8) | (data[offset + 1] & 0xFF);
#endif
}

ECode HdmiUtils::TwoBytesToInt(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return ((data[0] & 0xFF) << 16) | ((data[1] & 0xFF) << 8) | (data[2] & 0xFF);
#endif
}

ECode HdmiUtils::ThreeBytesToInt(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return ((data[0] & 0xFF) << 16) | ((data[1] & 0xFF) << 8) | (data[2] & 0xFF);

#endif
}

ECode HdmiUtils::SparseArrayToList(
    /* [in] */ ISparseArray* array,
    /* [out] */ IList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ArrayList<T> list = new ArrayList<>();
        for (Int32 i = 0; i < array->Size(); ++i) {
            list->Add(array->ValueAt(i));
        }
        return list;
#endif
}

ECode HdmiUtils::MergeToUnmodifiableList(
    /* [in] */ IList* a,
    /* [in] */ IList* b,
    /* [out] */ IList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (a->IsEmpty() && b->IsEmpty()) {
            return Collections->EmptyList();
        }
        if (a->IsEmpty()) {
            return Collections->UnmodifiableList(b);
        }
        if (b->IsEmpty()) {
            return Collections->UnmodifiableList(a);
        }
        List<T> newList = new ArrayList<>();
        newList->AddAll(a);
        newList->AddAll(b);
        return Collections->UnmodifiableList(newList);
#endif
}

ECode HdmiUtils::IsAffectingActiveRoutingPath(
    /* [in] */ Int32 activePath,
    /* [in] */ Int32 newPath,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
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
            return TRUE;  // Top path always affects the active path
        }
        return IsInActiveRoutingPath(activePath, newPath);
#endif
}

ECode HdmiUtils::IsInActiveRoutingPath(
    /* [in] */ Int32 activePath,
    /* [in] */ Int32 newPath,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
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
                return FALSE;
            }
        }
        return TRUE;
#endif
}

ECode HdmiUtils::CloneHdmiDeviceInfo(
    /* [in] */ IHdmiDeviceInfo* info,
    /* [in] */ Int32 newPowerStatus,
    /* [out] */ IHdmiDeviceInfo** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return new HdmiDeviceInfo(info->GetLogicalAddress(),
                info->GetPhysicalAddress(), info->GetPortId(), info->GetDeviceType(),
                info->GetVendorId(), info->GetDisplayName(), newPowerStatus);
#endif
}

AutoPtr<ArrayOf<Int32> > HdmiUtils::InitADDRESS_TO_TYPE()
{
    AutoPtr<ArrayOf<Int32> > rev;
#if 0 // TODO: Translate codes below
    private static final int[] ADDRESS_TO_TYPE = {
        HdmiDeviceInfo.DEVICE_TV,  // ADDR_TV
        HdmiDeviceInfo.DEVICE_RECORDER,  // ADDR_RECORDER_1
        HdmiDeviceInfo.DEVICE_RECORDER,  // ADDR_RECORDER_2
        HdmiDeviceInfo.DEVICE_TUNER,  // ADDR_TUNER_1
        HdmiDeviceInfo.DEVICE_PLAYBACK,  // ADDR_PLAYBACK_1
        HdmiDeviceInfo.DEVICE_AUDIO_SYSTEM,  // ADDR_AUDIO_SYSTEM
        HdmiDeviceInfo.DEVICE_TUNER,  // ADDR_TUNER_2
        HdmiDeviceInfo.DEVICE_TUNER,  // ADDR_TUNER_3
        HdmiDeviceInfo.DEVICE_PLAYBACK,  // ADDR_PLAYBACK_2
        HdmiDeviceInfo.DEVICE_RECORDER,  // ADDR_RECORDER_3
        HdmiDeviceInfo.DEVICE_TUNER,  // ADDR_TUNER_4
        HdmiDeviceInfo.DEVICE_PLAYBACK,  // ADDR_PLAYBACK_3
        HdmiDeviceInfo.DEVICE_RESERVED,
        HdmiDeviceInfo.DEVICE_RESERVED,
        HdmiDeviceInfo.DEVICE_TV,  // ADDR_SPECIFIC_USE
    }
#endif
    return rev;
}

AutoPtr<ArrayOf<String> > HdmiUtils::InitDEFAULT_NAMES()
{
    AutoPtr<ArrayOf<String> > rev;
#if 0 // TODO: Translate codes below
    private static final String[] DEFAULT_NAMES = {
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
