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

#ifndef __ELASTOS_DROID_HARDWARE_DISPLAY_WIFIDISPLAY_H__
#define __ELASTOS_DROID_HARDWARE_DISPLAY_WIFIDISPLAY_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

/**
 * Describes the properties of a Wifi display.
 * <p>
 * This object is immutable.
 * </p>
 *
 * @hide
 */
class WifiDisplay
    : public Object
    , public IWifiDisplay
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    WifiDisplay();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& deviceAddress,
        /* [in] */ const String& deviceName,
        /* [in] */ const String& deviceAlias,
        /* [in] */ Boolean available,
        /* [in] */ Boolean canConnect,
        /* [in] */ Boolean remembered);

    /**
     * Gets the MAC address of the Wifi display device.
     */
    CARAPI GetDeviceAddress(
        /* [out] */ String* deviceAddress);

    /**
     * Gets the name of the Wifi display device.
     */
    CARAPI GetDeviceName(
        /* [out] */ String* deviceName);

    /**
     * Gets the user-specified alias of the Wifi display device, or null if none.
     * <p>
     * The alias should be used in the UI whenever available.  It is the value
     * provided by the user when renaming the device.
     * </p>
     */
    CARAPI GetDeviceAlias(
        /* [out] */ String* deviceAlias);

    /**
     * Returns true if device is available, false otherwise.
     */
    CARAPI IsAvailable(
        /* [out] */ Boolean* result);

    /**
     * Returns true if device can be connected to (not in use), false otherwise.
     */
    CARAPI CanConnect(
        /* [out] */ Boolean* result);

    /**
     * Returns true if device has been remembered, false otherwise.
     */
    CARAPI IsRemembered(
        /* [out] */ Boolean* result);

    /**
     * Gets the name to show in the UI.
     * Uses the device alias if available, otherwise uses the device name.
     */
    CARAPI GetFriendlyDisplayName(
        /* [out] */ String* name);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* res);

    /**
     * Returns true if the two displays have the same identity (address, name and alias).
     * This method does not compare the current status of the displays.
     */
    CARAPI Equals(
        /* [in] */ IWifiDisplay* other,
        /* [out] */ Boolean* res);

    CARAPI GetHashCode(
        /* [out] */ Int32* code);

    CARAPI HasSameAddress(
        /* [in] */ IWifiDisplay* other,
        /* [out] */ Boolean* res);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    // public static final Creator<WifiDisplay> CREATOR = new Creator<WifiDisplay>() {
    //     public WifiDisplay createFromParcel(Parcel in) {
    //         String deviceAddress = in.readString();
    //         String deviceName = in.readString();
    //         String deviceAlias = in.readString();
    //         return new WifiDisplay(deviceAddress, deviceName, deviceAlias);
    //     }

    //     public WifiDisplay[] newArray(int size) {
    //         return size == 0 ? EMPTY_ARRAY : new WifiDisplay[size];
    //     }
    // };

    CARAPI ToString(
        /* [out] */ String* info);

public:
    static const AutoPtr<ArrayOf<IWifiDisplay*> > EMPTY_ARRAY;

private:
    String mDeviceAddress;
    String mDeviceName;
    String mDeviceAlias;
    Boolean mIsAvailable;
    Boolean mCanConnect;
    Boolean mIsRemembered;
};

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_DISPLAY_WIFIDISPLAY_H__
