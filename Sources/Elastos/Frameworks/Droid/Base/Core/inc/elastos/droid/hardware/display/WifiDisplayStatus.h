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

#ifndef __ELASTOS_DROID_HARDWARE_DISPLAY_WIFIDISPLAYSTATUS_H__
#define __ELASTOS_DROID_HARDWARE_DISPLAY_WIFIDISPLAYSTATUS_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

/**
 * Describes the current global state of Wifi display connectivity, including the
 * currently connected display and all available or remembered displays.
 * <p>
 * This object is immutable.
 * </p>
 *
 * @hide
 */
class WifiDisplayStatus
    : public Object
    , public IWifiDisplayStatus
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    WifiDisplayStatus();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 featureState,
        /* [in] */ Int32 scanState,
        /* [in] */ Int32 activeDisplayState,
        /* [in] */ IWifiDisplay* activeDisplay,
        /* [in] */ ArrayOf<IWifiDisplay*>* displays,
        /* [in] */ IWifiDisplaySessionInfo* sessionInfo);

    /**
     * Returns the state of the Wifi display feature on this device.
     * <p>
     * The value of this property reflects whether the device supports the Wifi display,
     * whether it has been enabled by the user and whether the prerequisites for
     * connecting to displays have been met.
     * </p>
     */
    CARAPI GetFeatureState(
        /* [out] */ Int32* state);

    /**
     * Returns the current state of the Wifi display scan.
     *
     * @return One of: {@link #SCAN_STATE_NOT_SCANNING} or {@link #SCAN_STATE_SCANNING}.
     */
    CARAPI GetScanState(
        /* [out] */ Int32* state);

    /**
     * Get the state of the currently active display.
     *
     * @return One of: {@link #DISPLAY_STATE_NOT_CONNECTED}, {@link #DISPLAY_STATE_CONNECTING},
     * or {@link #DISPLAY_STATE_CONNECTED}.
     */
    CARAPI GetActiveDisplayState(
        /* [out] */ Int32* state);

    /**
     * Gets the Wifi display that is currently active.  It may be connecting or
     * connected.
     */
    CARAPI GetActiveDisplay(
        /* [out] */ IWifiDisplay** display);

    /**
     * Gets the list of Wifi displays, returns a combined list of all available
     * Wifi displays as reported by the most recent scan, and all remembered
     * Wifi displays (not necessarily available at the time).
     */
    CARAPI GetDisplays(
        /* [out] */ ArrayOf<IWifiDisplay*>** displays);

    /**
     * Gets the Wifi display session info (required for certification only)
     */
    CARAPI GetSessionInfo(
        /* [out] */ IWifiDisplaySessionInfo** result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* info);

    // public static final Creator<WifiDisplayStatus> CREATOR = new Creator<WifiDisplayStatus>() {
    //     public WifiDisplayStatus createFromParcel(Parcel in) {
    //         int featureState = in.readInt();
    //         int scanState = in.readInt();
    //         int activeDisplayState= in.readInt();

    //         WifiDisplay activeDisplay = null;
    //         if (in.readInt() != 0) {
    //             activeDisplay = WifiDisplay.CREATOR.createFromParcel(in);
    //         }

    //         WifiDisplay[] displays = WifiDisplay.CREATOR.newArray(in.readInt());
    //         for (int i = 0; i < displays.length; i++) {
    //             displays[i] = WifiDisplay.CREATOR.createFromParcel(in);
    //         }

    //         WifiDisplaySessionInfo sessionInfo =
    //                 WifiDisplaySessionInfo.CREATOR.createFromParcel(in);

    //         return new WifiDisplayStatus(featureState, scanState, activeDisplayState,
    //                 activeDisplay, displays, sessionInfo);
    //     }

    //     public WifiDisplayStatus[] newArray(int size) {
    //         return new WifiDisplayStatus[size];
    //     }
    // };

private:
    Int32 mFeatureState;
    Int32 mScanState;
    Int32 mActiveDisplayState;
    AutoPtr<IWifiDisplay> mActiveDisplay;
    AutoPtr<ArrayOf<IWifiDisplay*> > mDisplays;

    /** Session info needed for Miracast Certification */
    AutoPtr<IWifiDisplaySessionInfo> mSessionInfo;
};

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_DISPLAY_WIFIDISPLAYSTATUS_H__
