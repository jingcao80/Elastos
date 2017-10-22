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

#ifndef __ELASTOS_DROID_NET_WIFI_CWIFILINKLAYERSTATS_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFILINKLAYERSTATS_H__

#include "_Elastos_Droid_Wifi_CWifiLinkLayerStats.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiLinkLayerStats)
    , public Object
    , public IWifiLinkLayerStats
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /** {@hide} */
    CARAPI GetStatus(
        /* [out] */ Int32* result);

    CARAPI SetStatus(
        /* [in] */ Int32 status);

    /** {@hide} */
    CARAPI GetSSID(
        /* [out] */ String* result);

    CARAPI SetSSID(
        /* [in] */ const String& SSID);

    /** {@hide} */
    CARAPI GetBSSID(
        /* [out] */ String* result);

    CARAPI SetBSSID(
        /* [in] */ const String& BSSID);

    /** {@hide} */
    CARAPI GetBeacon_rx(
        /* [out] */ Int32* result);

    CARAPI SetBeacon_rx(
        /* [in] */ Int32 beacon_rx);

    /** {@hide} */
    CARAPI GetRssi_mgmt(
        /* [out] */ Int32* result);

    CARAPI SetRssi_mgmt(
        /* [in] */ Int32 rssi_mgmt);

    /* WME Best Effort Access Category (receive mpdu, transmit mpdu, lost mpdu, number of retries)*/
    CARAPI GetRxmpdu_be(
        /* [out] */ Int64* result);

    CARAPI SetRxmpdu_be(
        /* [in] */ Int64 rxmpdu_be);

    /** {@hide} */
    CARAPI GetTxmpdu_be(
        /* [out] */ Int64* result);

    CARAPI SetTxmpdu_be(
        /* [in] */ Int64 txmpdu_be);

    /** {@hide} */
    CARAPI GetLostmpdu_be(
        /* [out] */ Int64* result);

    CARAPI SetLostmpdu_be(
        /* [in] */ Int64 lostmpdu_be);

    /** {@hide} */
    CARAPI GetRetries_be(
        /* [out] */ Int64* result);

    CARAPI SetRetries_be(
        /* [in] */ Int64 retries_be);

    /* WME Background Access Category (receive mpdu, transmit mpdu, lost mpdu, number of retries) */
    CARAPI GetRxmpdu_bk(
        /* [out] */ Int64* result);

    CARAPI SetRxmpdu_bk(
        /* [in] */ Int64 rxmpdu_bk);

    /** {@hide} */
    CARAPI GetTxmpdu_bk(
        /* [out] */ Int64* result);

    CARAPI SetTxmpdu_bk(
        /* [in] */ Int64 txmpdu_bk);

    /** {@hide} */
    CARAPI GetLostmpdu_bk(
        /* [out] */ Int64* result);

    CARAPI SetLostmpdu_bk(
        /* [in] */ Int64 lostmpdu_bk);

    /** {@hide} */
    CARAPI GetRetries_bk(
        /* [out] */ Int64* result);

    CARAPI SetRetries_bk(
        /* [in] */ Int64 retries_bk);

    /* WME Video Access Category (receive mpdu, transmit mpdu, lost mpdu, number of retries) */
    CARAPI GetRxmpdu_vi(
        /* [out] */ Int64* result);

    CARAPI SetRxmpdu_vi(
        /* [in] */ Int64 rxmpdu_vi);

    /** {@hide} */
    CARAPI GetTxmpdu_vi(
        /* [out] */ Int64* result);

    CARAPI SetTxmpdu_vi(
        /* [in] */ Int64 txmpdu_vi);

    /** {@hide} */
    CARAPI GetLostmpdu_vi(
        /* [out] */ Int64* result);

    CARAPI SetLostmpdu_vi(
        /* [in] */ Int64 lostmpdu_vi);

    /** {@hide} */
    CARAPI GetRetries_vi(
        /* [out] */ Int64* result);

    CARAPI SetRetries_vi(
        /* [in] */ Int64 retries_vi);

    /* WME Voice Access Category (receive mpdu, transmit mpdu, lost mpdu, number of retries) */
    CARAPI GetRxmpdu_vo(
        /* [out] */ Int64* result);

    CARAPI SetRxmpdu_vo(
        /* [in] */ Int64 rxmpdu_vo);

    /** {@hide} */
    CARAPI GetTxmpdu_vo(
        /* [out] */ Int64* result);

    CARAPI SetTxmpdu_vo(
        /* [in] */ Int64 txmpdu_vo);

    /** {@hide} */
    CARAPI GetLostmpdu_vo(
        /* [out] */ Int64* result);

    CARAPI SetLostmpdu_vo(
        /* [in] */ Int64 lostmpdu_vo);

    /** {@hide} */
    CARAPI GetRetries_vo(
        /* [out] */ Int64* result);

    CARAPI SetRetries_vo(
        /* [in] */ Int64 retries_vo);

    /** {@hide} */
    CARAPI GetOn_time(
        /* [out] */ Int32* result);

    CARAPI SetOn_time(
        /* [in] */ Int32 on_time);

    /** {@hide} */
    CARAPI GetTx_time(
        /* [out] */ Int32* result);

    CARAPI SetTx_time(
        /* [in] */ Int32 tx_time);

    /** {@hide} */
    CARAPI GetRx_time(
        /* [out] */ Int32* result);

    CARAPI SetRx_time(
        /* [in] */ Int32 rx_time);

    /** {@hide} */
    CARAPI GetOn_time_scan(
        /* [out] */ Int32* result);

    CARAPI SetOn_time_scan(
        /* [in] */ Int32 on_time_scan);

    /** {@hide} */
    CARAPI GetPrintableSsid(
        /* [out] */ String* result);

    //@Override
    /** {@hide} */
    CARAPI ToString(
        /* [out] */ String* str);

    /** Implement the Parcelable interface {@hide} */
    CARAPI DescribeContents(
        /* [out] */ Int32* contents);

    /** Implement the Parcelable interface {@hide} */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest,
        /* [in] */ Int32 flags);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    /** Implement the Parcelable interface {@hide} */
    // public static final Creator<WifiLinkLayerStats> CREATOR =
    //     new Creator<WifiLinkLayerStats>() {
    //         public WifiLinkLayerStats createFromParcel(Parcel in) {
    //             WifiLinkLayerStats stats = new WifiLinkLayerStats();
    //             stats.SSID = in.readString();
    //             stats.BSSID = in.readString();
    //             stats.on_time = in.readInt();
    //             stats.tx_time = in.readInt();
    //             stats.rx_time = in.readInt();
    //             stats.on_time_scan = in.readInt();
    //             return stats;
    //         };
    //         public WifiLinkLayerStats[] newArray(int size) {
    //             return new WifiLinkLayerStats[size];
    //         }

    //     };

private:
    static const String TAG;

    /**
     * The current status of this network configuration entry.
     * @see Status
     */
    /** {@hide} */
    Int32 mStatus;

    /**
     * The network's SSID. Can either be an ASCII string,
     * which must be enclosed in double quotation marks
     * (e.g., {@code "MyNetwork"}, or a string of
     * hex digits,which are not enclosed in quotes
     * (e.g., {@code 01a243f405}).
     */
    /** {@hide} */
    String mSSID;

    /**
     * When set. this is the BSSID the radio is currently associated with.
     * The value is a string in the format of an Ethernet MAC address, e.g.,
     * <code>XX:XX:XX:XX:XX:XX</code> where each <code>X</code> is a hex digit.
     */
    /** {@hide} */
    String mBSSID;

    /* number beacons received from our own AP */
    /** {@hide} */
    Int32 mBeaconRx;

    /* RSSI taken on management frames */
    /** {@hide} */
    Int32 mRssiMgmt;

    /* packets counters */
    /** {@hide} */
    /* WME Best Effort Access Category (receive mpdu, transmit mpdu, lost mpdu, number of retries)*/
    Int64 mRxmpduBe;

    /** {@hide} */
    Int64 mTxmpduBe;

    /** {@hide} */
    Int64 mLostmpduBe;

    /** {@hide} */
    Int64 mRetriesBe;

    /** {@hide} */
    /* WME Background Access Category (receive mpdu, transmit mpdu, lost mpdu, number of retries) */
    Int64 mRxmpduBk;

    /** {@hide} */
    Int64 mTxmpduBk;

    /** {@hide} */
    Int64 mLostmpduBk;

    /** {@hide} */
    Int64 mRetriesBk;

    /** {@hide} */
    /* WME Video Access Category (receive mpdu, transmit mpdu, lost mpdu, number of retries) */
    Int64 mRxmpduVi;

    /** {@hide} */
    Int64 mTxmpduVi;

    /** {@hide} */
    Int64 mLostmpduVi;

    /** {@hide} */
    Int64 mRetriesVi;

    /** {@hide} */
    /* WME Voice Access Category (receive mpdu, transmit mpdu, lost mpdu, number of retries) */
    Int64 mRxmpduVo;

    /** {@hide} */
    Int64 mTxmpduVo;

    /** {@hide} */
    Int64 mLostmpduVo;

    /** {@hide} */
    Int64 mRetriesVo;

    /** {@hide} */
    Int32 mOnTime;

    /** {@hide} */
    Int32 mTxTime;

    /** {@hide} */
    Int32 mRxTime;

    /** {@hide} */
    Int32 mOnTimeScan;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFILINKLAYERSTATS_H__
