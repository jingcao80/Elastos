
#ifndef __ELASTOS_DROID_HARDWARE_HDMI_HDMIDEVICEINFO_H__
#define __ELASTOS_DROID_HARDWARE_HDMI_HDMIDEVICEINFO_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

class HdmiDeviceInfo
    : public Object
    , public IHdmiDeviceInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    HdmiDeviceInfo();

    virtual ~HdmiDeviceInfo() {}

    CARAPI constructor();

    /**
     * Constructor. Used to initialize the instance for CEC device.
     *
     * @param logicalAddress logical address of HDMI-CEC device
     * @param physicalAddress physical address of HDMI-CEC device
     * @param portId HDMI port ID (1 for HDMI1)
     * @param deviceType type of device
     * @param vendorId vendor id of device. Used for vendor specific command.
     * @param displayName name of device
     * @param powerStatus device power status
     * @hide
     */
    CARAPI constructor(
        /* [in] */ Int32 logicalAddress,
        /* [in] */ Int32 physicalAddress,
        /* [in] */ Int32 portId,
        /* [in] */ Int32 deviceType,
        /* [in] */ Int32 vendorId,
        /* [in] */ const String& displayName,
        /* [in] */ Int32 powerStatus);

    /**
     * Constructor. Used to initialize the instance for CEC device.
     *
     * @param logicalAddress logical address of HDMI-CEC device
     * @param physicalAddress physical address of HDMI-CEC device
     * @param portId HDMI port ID (1 for HDMI1)
     * @param deviceType type of device
     * @param vendorId vendor id of device. Used for vendor specific command.
     * @param displayName name of device
     * @hide
     */
    CARAPI constructor(
        /* [in] */ Int32 logicalAddress,
        /* [in] */ Int32 physicalAddress,
        /* [in] */ Int32 portId,
        /* [in] */ Int32 deviceType,
        /* [in] */ Int32 vendorId,
        /* [in] */ const String& displayName);

    /**
     * Constructor. Used to initialize the instance for device representing hardware port.
     *
     * @param physicalAddress physical address of the port
     * @param portId HDMI port ID (1 for HDMI1)
     * @hide
     */
    CARAPI constructor(
        /* [in] */ Int32 physicalAddress,
        /* [in] */ Int32 portId);

    /**
     * Constructor. Used to initialize the instance for MHL device.
     *
     * @param physicalAddress physical address of HDMI device
     * @param portId portId HDMI port ID (1 for HDMI1)
     * @param adopterId adopter id of MHL
     * @param deviceId device id of MHL
     * @hide
     */
    CARAPI constructor(
        /* [in] */ Int32 physicalAddress,
        /* [in] */ Int32 portId,
        /* [in] */ Int32 adopterId,
        /* [in] */ Int32 deviceId);

    /**
     * Return the id of the device.
     */
    CARAPI GetId(
        /* [out] */ Int32* id);

    /**
     * Return the id to be used for CEC device.
     *
     * @param address logical address of CEC device
     * @return id for CEC device
     */
    static CARAPI IdForCecDevice(
        /* [in] */ Int32 address,
        /* [out] */ Int32* device);

    /**
     * Return the id to be used for MHL device.
     *
     * @param portId port which the MHL device is connected to
     * @return id for MHL device
     */
    static CARAPI IdForMhlDevice(
        /* [in] */ Int32 portId,
        /* [out] */ Int32* device);

    /**
     * Return the id to be used for hardware port.
     *
     * @param portId port id
     * @return id for hardware port
     */
    static CARAPI IdForHardware(
        /* [in] */ Int32 portId,
        /* [out] */ Int32* hardware);

    /**
     * Return the CEC logical address of the device.
     */
    CARAPI GetLogicalAddress(
        /* [out] */ Int32* address);

    /**
     * Return the physical address of the device.
     */
    CARAPI GetPhysicalAddress(
        /* [out] */ Int32* address);

    /**
     * Return the port ID.
     */
    CARAPI GetPortId(
        /* [out] */ Int32* id);

    /**
     * Return CEC type of the device. For more details, refer constants between {@link #DEVICE_TV}
     * and {@link #DEVICE_INACTIVE}.
     */
    CARAPI GetDeviceType(
        /* [out] */ Int32* type);

    /**
     * Return device's power status. It should be one of the following values.
     * <ul>
     * <li>{@link HdmiControlManager#POWER_STATUS_ON}
     * <li>{@link HdmiControlManager#POWER_STATUS_STANDBY}
     * <li>{@link HdmiControlManager#POWER_STATUS_TRANSIENT_TO_ON}
     * <li>{@link HdmiControlManager#POWER_STATUS_TRANSIENT_TO_STANDBY}
     * <li>{@link HdmiControlManager#POWER_STATUS_UNKNOWN}
     * </ul>
     */
    CARAPI GetDevicePowerStatus(
        /* [out] */ Int32* status);

    /**
     * Return MHL device id. Return -1 for non-MHL device.
     */
    CARAPI GetDeviceId(
        /* [out] */ Int32* id);

    /**
     * Return MHL adopter id. Return -1 for non-MHL device.
     */
    CARAPI GetAdopterId(
        /* [out] */ Int32* id);

    /**
     * Return {@code true} if the device is of a type that can be an input source.
     */
    CARAPI IsSourceType(
        /* [out] */ Boolean* result);

    /**
     * Return {@code true} if the device represents an HDMI-CEC device. {@code false} if the device
     * is either MHL or other device.
     */
    CARAPI IsCecDevice(
        /* [out] */ Boolean* result);

    /**
     * Return {@code true} if the device represents an MHL device. {@code false} if the device is
     * either CEC or other device.
     */
    CARAPI IsMhlDevice(
        /* [out] */ Boolean* result);

    /**
     * Return display (OSD) name of the device.
     */
    CARAPI GetDisplayName(
        /* [out] */ String* name);

    /**
     * Return vendor id of the device. Vendor id is used to distinguish devices built by other
     * manufactures. This is required for vendor-specific command on CEC standard.
     */
    CARAPI GetVendorId(
        /* [out] */ Int32* id);

    /**
     * Serialize this object into a {@link Parcel}.
     *
     * @param dest The Parcel in which the object should be written.
     * @param flags Additional flags about how the object should be written. May be 0 or
     *            {@link Parcelable#PARCELABLE_WRITE_RETURN_VALUE}.
     */
    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    //@Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    /**
     * A helper class to deserialize {@link HdmiDeviceInfo} for a parcel.
     */
    // public static final Parcelable.Creator<HdmiDeviceInfo> CREATOR =
    //         new Parcelable.Creator<HdmiDeviceInfo>() {
    //             @Override
    //             public HdmiDeviceInfo createFromParcel(Parcel source) {
    //                 int hdmiDeviceType = source.readInt();
    //                 int physicalAddress = source.readInt();
    //                 int portId = source.readInt();

    //                 switch (hdmiDeviceType) {
    //                     case HDMI_DEVICE_TYPE_CEC:
    //                         int logicalAddress = source.readInt();
    //                         int deviceType = source.readInt();
    //                         int vendorId = source.readInt();
    //                         int powerStatus = source.readInt();
    //                         String displayName = source.readString();
    //                         return new HdmiDeviceInfo(logicalAddress, physicalAddress, portId,
    //                                 deviceType, vendorId, displayName, powerStatus);
    //                     case HDMI_DEVICE_TYPE_MHL:
    //                         int deviceId = source.readInt();
    //                         int adopterId = source.readInt();
    //                         return new HdmiDeviceInfo(physicalAddress, portId, adopterId, deviceId);
    //                     case HDMI_DEVICE_TYPE_HARDWARE:
    //                         return new HdmiDeviceInfo(physicalAddress, portId);
    //                     default:
    //                         return null;
    //                 }
    //             }

    //             @Override
    //             public HdmiDeviceInfo[] newArray(int size) {
    //                 return new HdmiDeviceInfo[size];
    //             }
    //         };

private:
    static const Int32 HDMI_DEVICE_TYPE_CEC;
    static const Int32 HDMI_DEVICE_TYPE_MHL;
    static const Int32 HDMI_DEVICE_TYPE_HARDWARE;

    // Offset used for id value. MHL devices, for instance, will be assigned the value from
    // ID_OFFSET_MHL.
    static const Int32 ID_OFFSET_CEC;
    static const Int32 ID_OFFSET_MHL;
    static const Int32 ID_OFFSET_HARDWARE;

    // Common parameters for all device.
    Int32 mId;
    Int32 mHdmiDeviceType;
    Int32 mPhysicalAddress;
    Int32 mPortId;

    // CEC only parameters.
    Int32 mLogicalAddress;
    Int32 mDeviceType;
    Int32 mVendorId;
    String mDisplayName;
    Int32 mDevicePowerStatus;

    // MHL only parameters.
    Int32 mDeviceId;
    Int32 mAdopterId;
};

} //Hdmi
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_HDMI_HDMIDEVICEINFO_H__
