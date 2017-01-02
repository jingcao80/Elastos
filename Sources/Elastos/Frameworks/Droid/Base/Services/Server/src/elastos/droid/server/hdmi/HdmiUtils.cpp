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

#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/HdmiUtils.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <Elastos.Droid.Hardware.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Utility.h>

// using Elastos::Droid::Hardware::Hdmi::CHdmiDeviceInfo;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Utility::ICollection;

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
    Int32 actualDeviceType = GetTypeFromAddress(logicalAddress);
    if (actualDeviceType != deviceType) {
        Logger::E("HdmiUtils", "Device type missmatch:[Expected:%d, Actual:%d", deviceType, actualDeviceType);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

Boolean HdmiUtils::CheckCommandSource(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [in] */ Int32 expectedAddress,
    /* [in] */ const String& tag)
{
    Int32 srcAddr;
    cmd->GetSource(&srcAddr);
    Int32 src = srcAddr;
    if (src != expectedAddress) {
        Slogger::W(tag, "Invalid source [Expected:%d, Actual:%d]", expectedAddress, src);
        return FALSE;
    }
    return TRUE;
}

Boolean HdmiUtils::ParseCommandParamSystemAudioStatus(
    /* [in] */ IHdmiCecMessage* cmd)
{
    AutoPtr<ArrayOf<Byte> > params;
    cmd->GetParams((ArrayOf<Byte>**)&params);
    return (*params)[0] == Constants::SYSTEM_AUDIO_STATUS_ON;
}

AutoPtr<IList> HdmiUtils::AsImmutableList(
    /* [in] */ ArrayOf<Int32>* is)
{
    AutoPtr<IList> rev;
    AutoPtr<IArrayList> list;
    CArrayList::New(is->GetLength(), (IArrayList**)&list);
    for (Int32 i = 0; i < is->GetLength(); ++i) {
        Int32 type = (*is)[i];
        AutoPtr<IInteger32> i32Type;
        CInteger32::New(type, (IInteger32**)&i32Type);
        list->Add(i32Type);
    }
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    helper->UnmodifiableList(IList::Probe(list), (IList**)&rev);
    return rev;
}

Int32 HdmiUtils::TwoBytesToInt32(
    /* [in] */ ArrayOf<Byte>* data)
{
    return (((*data)[0] & 0xFF) << 8) | ((*data)[1] & 0xFF);
}

Int32 HdmiUtils::TwoBytesToInt32(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset)
{
    return (((*data)[offset] & 0xFF) << 8) | ((*data)[offset + 1] & 0xFF);
}

Int32 HdmiUtils::ThreeBytesToInt32(
    /* [in] */ ArrayOf<Byte>* data)
{
    return (((*data)[0] & 0xFF) << 16) | (((*data)[1] & 0xFF) << 8) | ((*data)[2] & 0xFF);
}

AutoPtr<IList> HdmiUtils::SparseArrayToList(
    /* [in] */ ISparseArray* array)
{
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    Int32 size;
    array->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        array->ValueAt(i, (IInterface**)&obj);
        list->Add(obj);
    }
    return IList::Probe(list);
}

AutoPtr<IList> HdmiUtils::MergeToUnmodifiableList(
    /* [in] */ IList* a,
    /* [in] */ IList* b)
{
    AutoPtr<IList> rev;
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    Boolean aIsEmpty;
    a->IsEmpty(&aIsEmpty);
    Boolean bIsEmpty;
    b->IsEmpty(&bIsEmpty);
    if (aIsEmpty && bIsEmpty) {
        helper->GetEmptyList((IList**)&rev);
        return rev;
    }
    if (aIsEmpty) {
        helper->UnmodifiableList(b, (IList**)&rev);
        return rev;
    }
    if (bIsEmpty) {
        helper->UnmodifiableList(a, (IList**)&rev);
        return rev;
    }
    AutoPtr<IList> newList;
    CArrayList::New((IList**)&newList);
    newList->AddAll(ICollection::Probe(a));
    newList->AddAll(ICollection::Probe(b));
    helper->UnmodifiableList(newList, (IList**)&rev);
    return rev;
}

Boolean HdmiUtils::IsAffectingActiveRoutingPath(
    /* [in] */ Int32 activePath,
    /* [in] */ Int32 newPath)
{
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
}

Boolean HdmiUtils::IsInActiveRoutingPath(
    /* [in] */ Int32 activePath,
    /* [in] */ Int32 newPath)
{
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
}

AutoPtr<IHdmiDeviceInfo> HdmiUtils::CloneHdmiDeviceInfo(
    /* [in] */ IHdmiDeviceInfo* info,
    /* [in] */ Int32 newPowerStatus)
{
    AutoPtr<IHdmiDeviceInfo> rev;
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
    // TODO: Waiting for CHdmiDeviceInfo
    assert(0);
    // CHdmiDeviceInfo::New(logicalAddr,
    //         physicalAddr, portId, deviceType,
    //         vendorId, displayName, newPowerStatus, (IHdmiDeviceInfo**)&rev);
    return rev;
}

AutoPtr<ArrayOf<Int32> > HdmiUtils::InitADDRESS_TO_TYPE()
{
    Int32 ADDRESS_TO_TYPE[] = {
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
    };

    Int32 size = ArraySize(ADDRESS_TO_TYPE);
    AutoPtr<ArrayOf<Int32> > rev = ArrayOf<Int32>::Alloc(size);
    rev->Copy(ADDRESS_TO_TYPE, size);
    return rev;
}

AutoPtr<ArrayOf<String> > HdmiUtils::InitDEFAULT_NAMES()
{
    String DEFAULT_NAMES[] = {
        String("TV"),
        String("Recorder_1"),
        String("Recorder_2"),
        String("Tuner_1"),
        String("Playback_1"),
        String("AudioSystem"),
        String("Tuner_2"),
        String("Tuner_3"),
        String("Playback_2"),
        String("Recorder_3"),
        String("Tuner_4"),
        String("Playback_3"),
        String("Reserved_1"),
        String("Reserved_2"),
        String("Secondary_TV"),
    };

    Int32 size = ArraySize(DEFAULT_NAMES);
    AutoPtr<ArrayOf<String> > rev = ArrayOf<String>::Alloc(size);
    rev->Copy(DEFAULT_NAMES, size);
    return rev;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
