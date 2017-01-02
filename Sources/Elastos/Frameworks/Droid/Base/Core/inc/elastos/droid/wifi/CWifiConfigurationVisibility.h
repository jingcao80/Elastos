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

#ifndef __ELASTOS_DROID_NET_WIFI_CWIFICONFIGURATIONVISIBILITY_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFICONFIGURATIONVISIBILITY_H__

#include "_Elastos_Droid_Wifi_CWifiConfigurationVisibility.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiConfigurationVisibility)
    , public Object
    , public IWifiConfigurationVisibility
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWifiConfigurationVisibility();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IWifiConfigurationVisibility* source);

    CARAPI GetRssi5(
        /* [out] */ Int32* result);

    CARAPI SetRssi5(
        /* [in] */ Int32 rssi5);

    // strongest 5GHz RSSI
    CARAPI GetRssi24(
        /* [out] */ Int32* result);

    CARAPI SetRssi24(
        /* [in] */ Int32 rssi24);

    // strongest 2.4GHz RSSI
    CARAPI GetNum5(
        /* [out] */ Int32* result);

    CARAPI SetNum5(
        /* [in] */ Int32 num5);

    // number of BSSIDs on 5GHz
    CARAPI GetNum24(
        /* [out] */ Int32* result);

    CARAPI SetNum24(
        /* [in] */ Int32 num24);

    // number of BSSIDs on 2.4GHz
    CARAPI GetAge5(
        /* [out] */ Int64* result);

    CARAPI SetAge5(
        /* [in] */ Int64 age5);

    // timestamp of the strongest 5GHz BSSID (last time it was seen)
    CARAPI GetAge24(
        /* [out] */ Int64* result);

    CARAPI SetAge24(
        /* [in] */ Int64 age24);

    // timestamp of the strongest 2.4GHz BSSID (last time it was seen)
    CARAPI GetBSSID24(
        /* [out] */ String* result);

    CARAPI SetBSSID24(
        /* [in] */ const String& BSSID24);

    CARAPI GetBSSID5(
        /* [out] */ String* result);

    CARAPI SetBSSID5(
        /* [in] */ const String& BSSID5);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    Int32 mRssi5;   // strongest 5GHz RSSI
    Int32 mRssi24;  // strongest 2.4GHz RSSI
    Int32 mNum5;    // number of BSSIDs on 5GHz
    Int32 mNum24;   // number of BSSIDs on 2.4GHz
    Int64 mAge5;   // timestamp of the strongest 5GHz BSSID (last time it was seen)
    Int64 mAge24;  // timestamp of the strongest 2.4GHz BSSID (last time it was seen)
    String mBSSID24;
    String mBSSID5;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFICONFIGURATIONVISIBILITY_H__