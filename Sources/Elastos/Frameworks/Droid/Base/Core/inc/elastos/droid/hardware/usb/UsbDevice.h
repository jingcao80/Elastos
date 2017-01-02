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

#ifndef __ELASTOS_DROID_HARDWARE_USB_USBDEVICE_H__
#define __ELASTOS_DROID_HARDWARE_USB_USBDEVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

/**
 * This class represents a USB device attached to the android device with the android device
 * acting as the USB host.
 * Each device contains one or more {@link UsbInterface}s, each of which contains a number of
 * {@link UsbEndpoint}s (the channels via which data is transmitted over USB).
 *
 * <p> This class contains information (along with {@link UsbInterface} and {@link UsbEndpoint})
 * that describes the capabilities of the USB device.
 * To communicate with the device, you open a {@link UsbDeviceConnection} for the device
 * and use {@link UsbRequest} to send and receive data on an endpoint.
 * {@link UsbDeviceConnection#controlTransfer} is used for control requests on endpoint zero.
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For more information about communicating with USB hardware, read the
 * <a href="{@docRoot}guide/topics/usb/index.html">USB</a> developer guide.</p>
 * </div>
 */
class UsbDevice
    : public Object
    , public IUsbDevice
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * UsbDevice should only be instantiated by UsbService implementation
     * @hide
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ Int32 vendorId,
        /* [in] */ Int32 productId,
        /* [in] */ Int32 cls,
        /* [in] */ Int32 subCls,
        /* [in] */ Int32 protocol,
        /* [in] */ const String& manufacturerName,
        /* [in] */ const String& productName,
        /* [in] */ const String& serialNumber);

    /**
     * Returns the name of the device.
     * In the standard implementation, this is the path of the device file
     * for the device in the usbfs file system.
     *
     * @return the device name
     */
    CARAPI GetDeviceName(
        /* [out] */ String* name);

    /**
     * Returns the manufacturer name of the device.
     *
     * @return the manufacturer name
     */
    CARAPI GetManufacturerName(
        /* [out] */ String* name);

    /**
     * Returns the product name of the device.
     *
     * @return the product name
     */
    CARAPI GetProductName(
        /* [out] */ String* name);

    /**
     * Returns the serial number of the device.
     *
     * @return the serial number name
     */
    CARAPI GetSerialNumber(
        /* [out] */ String* name);

    /**
     * Returns a unique integer ID for the device.
     * This is a convenience for clients that want to use an integer to represent
     * the device, rather than the device name.
     * IDs are not persistent across USB disconnects.
     *
     * @return the device ID
     */
    CARAPI GetDeviceId(
        /* [out] */ Int32* id);

    /**
     * Returns a vendor ID for the device.
     *
     * @return the device vendor ID
     */
    CARAPI GetVendorId(
        /* [out] */ Int32* id);

    /**
     * Returns a product ID for the device.
     *
     * @return the device product ID
     */
    CARAPI GetProductId(
        /* [out] */ Int32* id);

    /**
     * Returns the devices's class field.
     * Some useful constants for USB device classes can be found in {@link UsbConstants}.
     *
     * @return the devices's class
     */
    CARAPI GetDeviceClass(
        /* [out] */ Int32* cls);

    /**
     * Returns the device's subclass field.
     *
     * @return the device's subclass
     */
    CARAPI GetDeviceSubclass(
        /* [out] */ Int32* cls);

    /**
     * Returns the device's protocol field.
     *
     * @return the device's protocol
     */
    CARAPI GetDeviceProtocol(
        /* [out] */ Int32* protocol);

    /**
     * Returns the number of {@link UsbConfiguration}s this device contains.
     *
     * @return the number of configurations
     */
    CARAPI GetConfigurationCount(
        /* [out] */ Int32* count);

    /**
     * Returns the {@link UsbConfiguration} at the given index.
     *
     * @return the configuration
     */
    CARAPI GetConfiguration(
        /* [in] */ Int32 index,
        /* [out] */ IUsbConfiguration** config);

    /**
     * Returns the number of {@link UsbInterface}s this device contains.
     * For devices with multiple configurations, you will probably want to use
     * {@link UsbConfiguration#getInterfaceCount} instead.
     *
     * @return the number of interfaces
     */
    CARAPI GetInterfaceCount(
        /* [out] */ Int32* count);

    /**
     * Returns the {@link UsbInterface} at the given index.
     * For devices with multiple configurations, you will probably want to use
     * {@link UsbConfiguration#getInterface} instead.
     *
     * @return the interface
     */
    CARAPI GetInterface(
        /* [in] */ Int32 index,
        /* [out] */ IUsbInterface** usbInterface);

    /**
     * Only used by UsbService implementation
     * @hide
     */
    CARAPI SetConfigurations(
        /* [in] */ ArrayOf<IParcelable*>* configuration);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* value);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [out] */ IParcel* dest);

    static CARAPI_(Int32) NativeGetDeviceId(
        /* [in] */ const String& name);

    static CARAPI_(String) NativeGetDeviceName(
        /* [in] */ Int32 id);

    // public static final Parcelable.Creator<UsbDevice> CREATOR =
    //     new Parcelable.Creator<UsbDevice>() {
    //     public UsbDevice createFromParcel(Parcel in) {
    //         String name = in.readString();
    //         int vendorId = in.readInt();
    //         int productId = in.readInt();
    //         int clasz = in.readInt();
    //         int subClass = in.readInt();
    //         int protocol = in.readInt();
    //         String manufacturerName = in.readString();
    //         String productName = in.readString();
    //         String serialNumber = in.readString();
    //         Parcelable[] configurations = in.readParcelableArray(UsbInterface.class.getClassLoader());
    //         UsbDevice device = new UsbDevice(name, vendorId, productId, clasz, subClass, protocol,
    //                              manufacturerName, productName, serialNumber);
    //         device.setConfigurations(configurations);
    //         return device;
    //     }

    //     public UsbDevice[] newArray(int size) {
    //         return new UsbDevice[size];
    //     }
    // };

private:
    CARAPI_(AutoPtr< ArrayOf<IUsbInterface*> >) GetInterfaceList();

private:
    String mName;
    String mManufacturerName;
    String mProductName;
    String mSerialNumber;
    Int32 mVendorId;
    Int32 mProductId;
    Int32 mClass;
    Int32 mSubclass;
    Int32 mProtocol;
    AutoPtr< ArrayOf<IParcelable*> > mConfigurations;

    // list of all interfaces on the device
    AutoPtr< ArrayOf<IUsbInterface*> > mInterfaces;
};

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_USB_USBDEVICE_H__
