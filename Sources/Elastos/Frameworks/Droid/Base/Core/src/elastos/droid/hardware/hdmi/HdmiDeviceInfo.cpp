
#include "elastos/droid/hardware/hdmi/HdmiDeviceInfo.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

const Int32 HdmiDeviceInfo::HDMI_DEVICE_TYPE_CEC = 0;
const Int32 HdmiDeviceInfo::HDMI_DEVICE_TYPE_MHL = 1;
const Int32 HdmiDeviceInfo::HDMI_DEVICE_TYPE_HARDWARE = 2;

// Offset used for id value. MHL devices, for instance, will be assigned the value from
// ID_OFFSET_MHL.
const Int32 HdmiDeviceInfo::ID_OFFSET_CEC = 0x0;
const Int32 HdmiDeviceInfo::ID_OFFSET_MHL = 0x80;
const Int32 HdmiDeviceInfo::ID_OFFSET_HARDWARE = 0xC0;

CAR_INTERFACE_IMPL_2(HdmiDeviceInfo, Object, IHdmiDeviceInfo, IParcelable)

HdmiDeviceInfo::HdmiDeviceInfo()
    : mId(0)
    , mHdmiDeviceType(0)
    , mPhysicalAddress(0)
    , mPortId(0)
    , mLogicalAddress(0)
    , mDeviceType(0)
    , mVendorId(0)
    , mDisplayName(NULL)
    , mDevicePowerStatus(0)
    , mDeviceId(0)
    , mAdopterId(0)
{
}

ECode HdmiDeviceInfo::constructor()
{
    return NOERROR;
}

ECode HdmiDeviceInfo::constructor(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 physicalAddress,
    /* [in] */ Int32 portId,
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 vendorId,
    /* [in] */ const String& displayName,
    /* [in] */ Int32 powerStatus)
{
    mHdmiDeviceType = HDMI_DEVICE_TYPE_CEC;
    mPhysicalAddress = physicalAddress;
    mPortId = portId;

    IdForCecDevice(logicalAddress, &mId);
    mLogicalAddress = logicalAddress;
    mDeviceType = deviceType;
    mVendorId = vendorId;
    mDevicePowerStatus = powerStatus;
    mDisplayName = displayName;

    mDeviceId = -1;
    mAdopterId = -1;

    return NOERROR;
}

ECode HdmiDeviceInfo::constructor(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 physicalAddress,
    /* [in] */ Int32 portId,
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 vendorId,
    /* [in] */ const String& displayName)
{
    return constructor(logicalAddress, physicalAddress, portId, deviceType,
            vendorId, displayName, IHdmiControlManager::POWER_STATUS_UNKNOWN);
}

ECode HdmiDeviceInfo::constructor(
    /* [in] */ Int32 physicalAddress,
    /* [in] */ Int32 portId)
{
    mHdmiDeviceType = HDMI_DEVICE_TYPE_HARDWARE;
    mPhysicalAddress = physicalAddress;
    mPortId = portId;

    IdForHardware(portId, &mId);
    mLogicalAddress = -1;
    mDeviceType = IHdmiDeviceInfo::DEVICE_RESERVED;
    mVendorId = 0;
    mDevicePowerStatus = IHdmiControlManager::POWER_STATUS_UNKNOWN;
    StringBuilder sb;
    sb += "HDMI";
    sb += portId;
    mDisplayName = sb.ToString();

    mDeviceId = -1;
    mAdopterId = -1;

    return NOERROR;
}

ECode HdmiDeviceInfo::constructor(
    /* [in] */ Int32 physicalAddress,
    /* [in] */ Int32 portId,
    /* [in] */ Int32 adopterId,
    /* [in] */ Int32 deviceId)
{
    mHdmiDeviceType = HDMI_DEVICE_TYPE_MHL;
    mPhysicalAddress = physicalAddress;
    mPortId = portId;

    IdForMhlDevice(portId, &mId);
    mLogicalAddress = -1;
    mDeviceType = IHdmiDeviceInfo::DEVICE_RESERVED;
    mVendorId = 0;
    mDevicePowerStatus = IHdmiControlManager::POWER_STATUS_UNKNOWN;
    mDisplayName = String("Mobile");

    mDeviceId = adopterId;
    mAdopterId = deviceId;

    return NOERROR;
}

ECode HdmiDeviceInfo::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)

    *id = mId;
    return NOERROR;
}

ECode HdmiDeviceInfo::IdForCecDevice(
    /* [in] */ Int32 address,
    /* [out] */ Int32* device)
{
    VALIDATE_NOT_NULL(device)

    // The id is generated based on the logical address.
    *device = ID_OFFSET_CEC + address;
    return NOERROR;
}

ECode HdmiDeviceInfo::IdForMhlDevice(
    /* [in] */ Int32 portId,
    /* [out] */ Int32* device)
{
    VALIDATE_NOT_NULL(device)

    // The id is generated based on the port id since there can be only one MHL device per port.
    *device = ID_OFFSET_MHL + portId;
    return NOERROR;
}

ECode HdmiDeviceInfo::IdForHardware(
    /* [in] */ Int32 portId,
    /* [out] */ Int32* hardware)
{
    VALIDATE_NOT_NULL(hardware)

    *hardware = ID_OFFSET_HARDWARE + portId;
    return NOERROR;
}

ECode HdmiDeviceInfo::GetLogicalAddress(
    /* [out] */ Int32* address)
{
    VALIDATE_NOT_NULL(address)

    *address = mLogicalAddress;
    return NOERROR;
}

ECode HdmiDeviceInfo::GetPhysicalAddress(
    /* [out] */ Int32* address)
{
    VALIDATE_NOT_NULL(address)

    *address = mPhysicalAddress;
    return NOERROR;
}

ECode HdmiDeviceInfo::GetPortId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)

    *id = mPortId;
    return NOERROR;
}

ECode HdmiDeviceInfo::GetDeviceType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)

    *type = mDeviceType;
    return NOERROR;
}

ECode HdmiDeviceInfo::GetDevicePowerStatus(
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status)

    *status = mDevicePowerStatus;
    return NOERROR;
}

ECode HdmiDeviceInfo::GetDeviceId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)

    *id = mDeviceId;
    return NOERROR;
}

ECode HdmiDeviceInfo::GetAdopterId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)

    *id = mAdopterId;
    return NOERROR;
}

ECode HdmiDeviceInfo::IsSourceType(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mDeviceType == IHdmiDeviceInfo::DEVICE_PLAYBACK
            || mDeviceType == IHdmiDeviceInfo::DEVICE_RECORDER
            || mDeviceType == IHdmiDeviceInfo::DEVICE_TUNER;
    return NOERROR;
}

ECode HdmiDeviceInfo::IsCecDevice(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mHdmiDeviceType == HDMI_DEVICE_TYPE_CEC;
    return NOERROR;
}

ECode HdmiDeviceInfo::IsMhlDevice(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mHdmiDeviceType == HDMI_DEVICE_TYPE_MHL;
    return NOERROR;
}

ECode HdmiDeviceInfo::GetDisplayName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    *name = mDisplayName;
    return NOERROR;
}

ECode HdmiDeviceInfo::GetVendorId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)

    *id = mVendorId;
    return NOERROR;
}

ECode HdmiDeviceInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mHdmiDeviceType);
    dest->WriteInt32(mPhysicalAddress);
    dest->WriteInt32(mPortId);
    switch (mHdmiDeviceType) {
        case HDMI_DEVICE_TYPE_CEC:
            dest->WriteInt32(mLogicalAddress);
            dest->WriteInt32(mDeviceType);
            dest->WriteInt32(mVendorId);
            dest->WriteInt32(mDevicePowerStatus);
            dest->WriteString(mDisplayName);
            break;
        case HDMI_DEVICE_TYPE_MHL:
            dest->WriteInt32(mDeviceId);
            dest->WriteInt32(mAdopterId);
            break;
        default:
            ;// no-op
    }
    return NOERROR;
}

ECode HdmiDeviceInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mHdmiDeviceType);
    source->ReadInt32(&mPhysicalAddress);
    source->ReadInt32(&mPortId);
    switch (mHdmiDeviceType) {
        case HDMI_DEVICE_TYPE_CEC:
            source->ReadInt32(&mLogicalAddress);
            source->ReadInt32(&mDeviceType);
            source->ReadInt32(&mVendorId);
            source->ReadInt32(&mDevicePowerStatus);
            source->ReadString(&mDisplayName);
            break;
        case HDMI_DEVICE_TYPE_MHL:
            source->ReadInt32(&mDeviceId);
            source->ReadInt32(&mAdopterId);
            break;
        default:
            ;// no-op
    }
    return NOERROR;
}

ECode HdmiDeviceInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder s;
    switch (mHdmiDeviceType) {
        case HDMI_DEVICE_TYPE_CEC:
            s += "CEC: ";
            s += "logical_address: ";
            s += mLogicalAddress; //String.format("0x%02X", mLogicalAddress);
            s += " ";
            s += "device_type: ";
            s += mDeviceType;
            s += " ";
            s += "vendor_id: ";
            s += mVendorId;
            s += " ";
            s += "display_name: ";
            s += mDisplayName;
            s += " ";
            s += "power_status: ";
            s += mDevicePowerStatus;
            s += " ";
            break;
        case HDMI_DEVICE_TYPE_MHL:
            s += "MHL: ";
            s += "device_id: ";
            s += mDeviceId; //String.format("0x%04X", mDeviceId);
            s += " ";
            s += "adopter_id: ";
            s += mAdopterId; //String.format("0x%04X", mAdopterId);
            s += " ";
            break;

        case HDMI_DEVICE_TYPE_HARDWARE:
            s += "Hardware: ";
            break;
        default:
            *str = String("");
            return NOERROR;
    }
    s += "physical_address: ";
    s += mPhysicalAddress; //String.format("0x%04X", mPhysicalAddress);
    s += " ";
    s += "port_id: ";
    s += mPortId;
    return s.ToString(str);
}

ECode HdmiDeviceInfo::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (IHdmiDeviceInfo::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    HdmiDeviceInfo* other = (HdmiDeviceInfo*)IHdmiDeviceInfo::Probe(obj);
    *result = mHdmiDeviceType == other->mHdmiDeviceType
            && mPhysicalAddress == other->mPhysicalAddress
            && mPortId == other->mPortId
            && mLogicalAddress == other->mLogicalAddress
            && mDeviceType == other->mDeviceType
            && mVendorId == other->mVendorId
            && mDevicePowerStatus == other->mDevicePowerStatus
            && mDisplayName.Equals(other->mDisplayName)
            && mDeviceId == other->mDeviceId
            && mAdopterId == other->mAdopterId;
    return NOERROR;
}

} // namespace Hdmi
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
