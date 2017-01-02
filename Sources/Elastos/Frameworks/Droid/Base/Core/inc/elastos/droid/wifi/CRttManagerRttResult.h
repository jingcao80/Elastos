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

#ifndef __ELASTOS_DROID_NET_WIFI_CRTTMANAGERRTTRESULT_H__
#define __ELASTOS_DROID_NET_WIFI_CRTTMANAGERRTTRESULT_H__

#include "_Elastos_Droid_Wifi_CRttManagerRttResult.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CRttManagerRttResult)
    , public Object
    , public IRttManagerRttResult
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI GetBssid(
        /* [out] */ String* result);

    CARAPI SetBssid(
        /* [in] */ const String& bssid);

    CARAPI GetStatus(
        /* [out] */ Int32* result);

    CARAPI SetStatus(
        /* [in] */ Int32 status);

    CARAPI GetRequestType(
        /* [out] */ Int32* result);

    CARAPI SetRequestType(
        /* [in] */ Int32 requestType);

    CARAPI GetTs(
        /* [out] */ Int64* result);

    CARAPI SetTs(
        /* [in] */ Int64 ts);

    CARAPI GetRssi(
        /* [out] */ Int32* result);

    CARAPI SetRssi(
        /* [in] */ Int32 rssi);

    CARAPI GetRssi_spread(
        /* [out] */ Int32* result);

    CARAPI SetRssi_spread(
        /* [in] */ Int32 rssi_spread);

    CARAPI GetTx_rate(
        /* [out] */ Int32* result);

    CARAPI SetTx_rate(
        /* [in] */ Int32 tx_rate);

    CARAPI GetRtt_ns(
        /* [out] */ Int64* result);

    CARAPI SetRtt_ns(
        /* [in] */ Int64 rtt_ns);

    CARAPI GetRtt_sd_ns(
        /* [out] */ Int64* result);

    CARAPI SetRtt_sd_ns(
        /* [in] */ Int64 rtt_sd_ns);

    CARAPI GetRtt_spread_ns(
        /* [out] */ Int64* result);

    CARAPI SetRtt_spread_ns(
        /* [in] */ Int64 rtt_spread_ns);

    CARAPI GetDistance_cm(
        /* [out] */ Int32* result);

    CARAPI SetDistance_cm(
        /* [in] */ Int32 distance_cm);

    CARAPI GetDistance_sd_cm(
        /* [out] */ Int32* result);

    CARAPI SetDistance_sd_cm(
        /* [in] */ Int32 distance_sd_cm);

    CARAPI GetDistance_spread_cm(
        /* [out] */ Int32* result);

    CARAPI SetDistance_spread_cm(
        /* [in] */ Int32 distance_spread_cm);

private:
    /** mac address of the device being ranged */
    String mBssid;

    /** status of the request */
    Int32 mStatus;

    /** type of the request used */
    Int32 mRequestType;

    /** timestamp of completion, in microsecond since boot */
    Int64 mTs;

    /** average RSSI observed */
    Int32 mRssi;

    /** RSSI spread (i.e. max - min) */
    Int32 mRssiSpread;

    /** average transmit rate */
    Int32 mTxRate;

    /** average round trip time in nano second */
    Int64 mRttNs;

    /** standard deviation observed in round trip time */
    Int64 mRttSdNs;

    /** spread (i.e. max - min) round trip time */
    Int64 mRttSpreadNs;

    /** average distance in centimeter, computed based on rtt_ns */
    Int32 mDistanceCm;

    /** standard deviation observed in distance */
    Int32 mDistanceSdCm;

    /** spread (i.e. max - min) distance */
    Int32 mDistanceSpreadCm;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CRTTMANAGERRTTRESULT_H__
