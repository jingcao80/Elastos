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

#ifndef __ELASTOS_DROID_HARDWARE_USB_USBINTERFACE_H__
#define __ELASTOS_DROID_HARDWARE_USB_USBINTERFACE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

/**
 * A class representing an interface on a {@link UsbDevice}.
 * USB devices can have one or more interfaces, each one providing a different
 * piece of functionality, separate from the other interfaces.
 * An interface will have one or more {@link UsbEndpoint}s, which are the
 * channels by which the host transfers data with the device.
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For more information about communicating with USB hardware, read the
 * <a href="{@docRoot}guide/topics/usb/index.html">USB</a> developer guide.</p>
 * </div>
 */
class UsbInterface
    : public Object
    , public IUsbInterface
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * UsbInterface should only be instantiated by UsbService implementation
     * @hide
     */
    CARAPI constructor(
        /* [in] */ Int32 id,
        /* [in] */ Int32 alternateSetting,
        /* [in] */ const String& name,
        /* [in] */ Int32 Class,
        /* [in] */ Int32 subClass,
        /* [in] */ Int32 protocol);

    /**
     * Returns the interface's bInterfaceNumber field.
     * This is an integer that along with the alternate setting uniquely identifies
     * the interface on the device.
     *
     * @return the interface's ID
     */
    CARAPI GetId(
        /* [out] */ Int32* id);

    /**
     * Returns the interface's bAlternateSetting field.
     * This is an integer that along with the ID uniquely identifies
     * the interface on the device.
     * {@link UsbDeviceConnection#setInterface} can be used to switch between
     * two interfaces with the same ID but different alternate setting.
     *
     * @return the interface's alternate setting
     */
    CARAPI GetAlternateSetting(
        /* [out] */ Int32* setting);

    /**
     * Returns the interface's name.
     *
     * @return the interface's name
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Returns the interface's class field.
     * Some useful constants for USB classes can be found in {@link UsbConstants}
     *
     * @return the interface's class
     */
    CARAPI GetInterfaceClass(
        /* [out] */ Int32* cls);

    /**
     * Returns the interface's subclass field.
     *
     * @return the interface's subclass
     */
    CARAPI GetInterfaceSubclass(
        /* [out] */ Int32* cls);

    /**
     * Returns the interface's protocol field.
     *
     * @return the interface's protocol
     */
    CARAPI GetInterfaceProtocol(
        /* [out] */ Int32* protocol);

    /**
     * Returns the number of {@link android.hardware.usb.UsbEndpoint}s this interface contains.
     *
     * @return the number of endpoints
     */
    CARAPI GetEndpointCount(
        /* [out] */ Int32* count);

    /**
     * Returns the {@link android.hardware.usb.UsbEndpoint} at the given index.
     *
     * @return the endpoint
     */
    CARAPI GetEndpoint(
        /* [in] */ Int32 index,
        /* [out] */ IUsbEndpoint** endpoint);

    /**
     * Only used by UsbService implementation
     * @hide
     */
    CARAPI SetEndpoints(
        /* [in] */ ArrayOf<IParcelable*>* endpoints);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [out] */ IParcel* dest);

    // public static final Parcelable.Creator<UsbInterface> CREATOR =
    //     new Parcelable.Creator<UsbInterface>() {
    //     public UsbInterface createFromParcel(Parcel in) {
    //         int id = in.readInt();
    //         int alternateSetting = in.readInt();
    //         String name = in.readString();
    //         int Class = in.readInt();
    //         int subClass = in.readInt();
    //         int protocol = in.readInt();
    //         Parcelable[] endpoints = in.readParcelableArray(UsbEndpoint.class.getClassLoader());
    //         UsbInterface intf = new UsbInterface(id, alternateSetting, name, Class, subClass, protocol);
    //         intf.setEndpoints(endpoints);
    //         return intf;
    //     }

    //     public UsbInterface[] newArray(int size) {
    //         return new UsbInterface[size];
    //     }
    // };

private:
    Int32 mId;
    Int32 mAlternateSetting;
    String mName;
    Int32 mClass;
    Int32 mSubclass;
    Int32 mProtocol;
    AutoPtr<ArrayOf<IParcelable*> > mEndpoints;
};

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_USB_USBINTERFACE_H__
