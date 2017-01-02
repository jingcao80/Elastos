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

#ifndef __ELASTOS_DROID_NET_WIFI_CWIFISCANNERBSSIDINFO_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFISCANNERBSSIDINFO_H__

#include "_Elastos_Droid_Wifi_CWifiScannerBssidInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiScannerBssidInfo)
    , public Object
    , public IWifiScannerBssidInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI GetBssid(
        /* [out] */ String* result);

    CARAPI SetBssid(
        /* [in] */ const String& bssid);

    CARAPI GetLow(
        /* [out] */ Int32* result);

    CARAPI SetLow(
        /* [in] */ Int32 low);

    CARAPI GetHigh(
        /* [out] */ Int32* result);

    CARAPI SetHigh(
        /* [in] */ Int32 high);

    CARAPI GetFrequencyHint(
        /* [out] */ Int32* result);

    CARAPI SetFrequencyHint(
        /* [in] */ Int32 frequencyHint);

private:
    /** bssid of the access point; in XX:XX:XX:XX:XX:XX format */
    String mBssid;
    /** low signal strength threshold; more information at {@link ScanResult#level} */
    Int32 mLow;                                            /* minimum RSSI */
    /** high signal threshold; more information at {@link ScanResult#level} */
    Int32 mHigh;                                           /* maximum RSSI */
    /** channel frequency (in KHz) where you may find this BSSID */
    Int32 mFrequencyHint;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFISCANNERBSSIDINFO_H__
