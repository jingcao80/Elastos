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

#ifndef __ELASTOS_DROID_NET_WIFI_CWIFIINFO_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFIINFO_H__

#include <Elastos.CoreLibrary.Net.h>
#include "_Elastos_Droid_Wifi_CWifiInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>
#include "elastos/core/Object.h"

using Elastos::Utility::Etl::HashMap;
using Elastos::Net::IInetAddress;
using Elastos::Droid::Net::INetworkUtils;
using Elastos::Droid::Net::NetworkInfoDetailedState;

namespace Elastos {
namespace Droid {
namespace Wifi {

/**
 * Describes the state of any Wifi connection that is active or
 * is in the process of being set up.
 */
CarClass(CWifiInfo)
    , public Object
    , public IWifiInfo
    , public IParcelable
{
private:
    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWifiInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Boolean fake);

    /**
     * Copy constructor
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IWifiInfo* source);

    CARAPI GetTxBad(
        /* [out] */ Int64* result);

    CARAPI SetTxBad(
        /* [in] */ Int64 txBad);

    CARAPI GetTxRetries(
        /* [out] */ Int64* result);

    CARAPI SetTxRetries(
        /* [in] */ Int64 txRetries);

    CARAPI GetTxSuccess(
        /* [out] */ Int64* result);

    CARAPI SetTxSuccess(
        /* [in] */ Int64 txSuccess);

    CARAPI GetRxSuccess(
        /* [out] */ Int64* result);

    CARAPI SetRxSuccess(
        /* [in] */ Int64 rxSuccess);

    CARAPI GetTxBadRate(
        /* [out] */ Double* result);

    CARAPI SetTxBadRate(
        /* [in] */ Double txBadRate);

    CARAPI GetTxRetriesRate(
        /* [out] */ Double* result);

    CARAPI SetTxRetriesRate(
        /* [in] */ Double txRetriesRate);

    CARAPI GetTxSuccessRate(
        /* [out] */ Double* result);

    CARAPI SetTxSuccessRate(
        /* [in] */ Double txSuccessRate);

    CARAPI GetRxSuccessRate(
        /* [out] */ Double* result);

    CARAPI SetRxSuccessRate(
        /* [in] */ Double rxSuccessRate);

    CARAPI GetBadRssiCount(
        /* [out] */ Int32* result);

    CARAPI SetBadRssiCount(
        /* [in] */ Int32 badRssiCount);

    CARAPI GetLinkStuckCount(
        /* [out] */ Int32* result);

    CARAPI SetLinkStuckCount(
        /* [in] */ Int32 linkStuckCount);

    CARAPI GetLowRssiCount(
        /* [out] */ Int32* result);

    CARAPI SetLowRssiCount(
        /* [in] */ Int32 lowRssiCount);

    CARAPI GetScore(
        /* [out] */ Int32* result);

    CARAPI SetScore(
        /* [in] */ Int32 score);

    CARAPI UpdatePacketRates(
        /* [in] */ IWifiLinkLayerStats* stats);

    CARAPI UpdatePacketRates(
        /* [in] */ Int64 txPackets,
        /* [in] */ Int64 rxPackets);

    CARAPI Reset();

    CARAPI GetFrequency(
        /* [out] */ Int32* result);

    CARAPI SetFrequency(
        /* [in] */ Int32 frequency);

    CARAPI Is24GHz(
        /* [out] */ Boolean* result);

    CARAPI Is5GHz(
        /* [out] */ Boolean* result);

    CARAPI GetSSID(
        /* [out] */ String* ssid);

    /** @hide */
    CARAPI GetWifiSsid(
        /* [out] */ IWifiSsid** ssid);

    /**
     * Return the basic service set identifier (BSSID) of the current access point.
     * The BSSID may be {@code NULL} if there is no network currently connected.
     * @return the BSSID, in the form of a six-byte MAC address: {@code XX:XX:XX:XX:XX:XX}
     */
    CARAPI GetBSSID(
        /* [out] */ String* bssid);

    /**
     * Returns the received signal strength indicator of the current 802.11
     * network.
     * <p><strong>This is not normalized, but should be!</strong></p>
     * @return the RSSI, in the range ??? to ???
     */
    CARAPI GetRssi(
        /* [out] */ Int32* rssi);

    /**
     * Returns the current link speed in {@link #LINK_SPEED_UNITS}.
     * @return the link speed.
     * @see #LINK_SPEED_UNITS
     */
    CARAPI GetLinkSpeed(
        /* [out] */ Int32* speed);

    CARAPI GetMacAddress(
        /* [out] */ String* addr);

    /** {@hide} */
    CARAPI SetMeteredHint(
        /* [in] */ Boolean meteredHint);

    /** {@hide} */
    CARAPI GetMeteredHint(
        /* [out] */ Boolean* hint);

    /**
     * Each configured network has a unique small integer ID, used to identify
     * the network when performing operations on the supplicant. This method
     * returns the ID for the currently connected network.
     * @return the network ID, or -1 if there is no currently connected network
     */
    CARAPI GetNetworkId(
        /* [out] */ Int32* id);

    /**
     * Return the detailed state of the supplicant's negotiation with an
     * access point, in the form of a {@link SupplicantState SupplicantState} object.
     * @return the current {@link SupplicantState SupplicantState}
     */
    CARAPI GetSupplicantState(
        /* [out] */ ISupplicantState** state);

    CARAPI GetIpAddress(
        /* [out] */ Int32* addr);

    /**
     * @return {@code true} if this network does not broadcast its SSID, so an
     * SSID-specific probe request must be used for scans.
     */
    CARAPI GetHiddenSSID(
        /* [out] */ Boolean* hide);

   /**
     * Map a supplicant state into a fine-grained network connectivity state.
     * @param suppState the supplicant state
     * @return the corresponding {@link DetailedState}
     */
    static CARAPI_(NetworkInfoDetailedState) GetDetailedStateOf(
        /* [in] */ SupplicantState suppState);

    /** {@hide} */
    static CARAPI RemoveDoubleQuotes(
        /* [in] */ const String& str,
        /* [out] */ String* quotes);

    CARAPI ToString(
        /* [out] */ String* str);

    /** Implement the Parcelable interface {@hide} */


    /** Implement the Parcelable interface {@hide} */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    // /** Implement the Parcelable interface {@hide} */
    // public static final Creator<WifiInfo> CREATOR =
    //     new Creator<WifiInfo>() {
    //         public WifiInfo createFromParcel(Parcel in) {
    //             WifiInfo info = new WifiInfo();
    //             info.setNetworkId(in.readInt());
    //             info.setRssi(in.readInt());
    //             info.setLinkSpeed(in.readInt());
    //             if (in.readByte() == 1) {
    //                 try {
    //                     info.setInetAddress(InetAddress.getByAddress(in.createByteArray()));
    //                 } catch (UnknownHostException e) {}
    //             }
    //             if (in.readInt() == 1) {
    //                 info.mWifiSsid = WifiSsid.CREATOR.createFromParcel(in);
    //             }
    //             info.mBSSID = in.readString();
    //             info.mMacAddress = in.readString();
    //             info.mMeteredHint = in.readInt() != 0;
    //             info.mSupplicantState = SupplicantState.CREATOR.createFromParcel(in);
    //             return info;
    //         }

    //         public WifiInfo[] newArray(Int32 size) {
    //             return new WifiInfo[size];
    //         }
    //     };

    CARAPI SetSSID(
        /* [in] */ IWifiSsid* wifiSsid);

    CARAPI SetBSSID(
        /* [in] */ const String& BSSID);

    CARAPI SetRssi(
        /* [in] */ Int32 rssi);

    CARAPI SetLinkSpeed(
        /* [in] */ Int32 linkSpeed);

    /**
     * Record the MAC address of the WLAN interface
     * @param macAddress the MAC address in {@code XX:XX:XX:XX:XX:XX} form
     */
    CARAPI SetMacAddress(
        /* [in] */ const String& macAddress);

    CARAPI SetNetworkId(
        /* [in] */ Int32 id);

    CARAPI SetSupplicantState(
        /* [in] */ ISupplicantState* state);

    CARAPI SetInetAddress(
        /* [in] */ IInetAddress* address);

    /**
     * Set the <code>SupplicantState</code> from the string name
     * of the state.
     * @param stateName the name of the state, as a <code>String</code> returned
     * in an event sent by {@code wpa_supplicant}.
     */
    CARAPI_(void) SetSupplicantState(
        /* [in] */ const String& stateName);

    static CARAPI_(SupplicantState) ValueOf(
        /* [in] */ const String& stateName);

private:
    static String TAG;

    /**
     * This is the map described in the Javadoc comment above. The positions
     * of the elements of the array must correspond to the ordinal values
     * of <code>DetailedState</code>.
     */
    // private static final EnumMap<SupplicantState, DetailedState> stateMap =
    //     new EnumMap<SupplicantState, DetailedState>(SupplicantState.class);
    static HashMap<SupplicantState, NetworkInfoDetailedState> sStateMap;

    //AutoPtr<ISupplicantStateHelper> mSupplicantState;
    SupplicantState mSupplicantState;
    String mBSSID;
    AutoPtr<IWifiSsid> mWifiSsid;
    Int32 mNetworkId;
    /** Received Signal Strength Indicator */
    Int32 mRssi;

    /** Link speed in Mbps */
    static String LINK_SPEED_UNITS;
    Int32 mLinkSpeed;

    Int32 mFrequency;
    AutoPtr<IInetAddress> mIpAddress;
    String mMacAddress;

    Int64 mTxBad;
    Int64 mTxRetries;
    Int64 mTxSuccess;
    Int64 mRxSuccess;
    Double mTxBadRate;
    Double mTxRetriesRate;
    Double mTxSuccessRate;
    Double mRxSuccessRate;
    Int32 mBadRssiCount;
    Int32 mLinkStuckCount;
    Int32 mLowRssiCount;
    Int32 mScore;

    /**
     * Flag indicating that AP has hinted that upstream connection is metered,
     * and sensitive to heavy data transfers.
     */
    Boolean mMeteredHint;
    static const StaticInitializer sInitializer;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_NET_WIFI_CWIFIINFO_H__
