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

#ifndef __ELASTOS_DROID_NET_WIFI_CWIFISCANNERHOTLISTSETTINGS_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFISCANNERHOTLISTSETTINGS_H__

#include "_Elastos_Droid_Wifi_CWifiScannerHotlistSettings.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiScannerHotlistSettings)
    , public Object
    , public IParcelable
    , public IWifiScannerHotlistSettings
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI GetBssidInfos(
        /* [out, callee] */ ArrayOf<IWifiScannerBssidInfo*>** result);

    CARAPI SetBssidInfos(
        /* [in] */ ArrayOf<IWifiScannerBssidInfo*>* bssidInfos);

    CARAPI GetApLostThreshold(
        /* [out] */ Int32* result);

    CARAPI SetApLostThreshold(
        /* [in] */ Int32 apLostThreshold);

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
    // public static final Creator<HotlistSettings> CREATOR =
    //         new Creator<HotlistSettings>() {
    //             public HotlistSettings createFromParcel(Parcel in) {
    //                 HotlistSettings settings = new HotlistSettings();
    //                 settings.apLostThreshold = in.readInt();
    //                 int n = in.readInt();
    //                 settings.bssidInfos = new BssidInfo[n];
    //                 for (int i = 0; i < n; i++) {
    //                     BssidInfo info = new BssidInfo();
    //                     info.bssid = in.readString();
    //                     info.low = in.readInt();
    //                     info.high = in.readInt();
    //                     info.frequencyHint = in.readInt();
    //                     settings.bssidInfos[i] = info;
    //                 }
    //                 return settings;
    //             }

    //             public HotlistSettings[] newArray(int size) {
    //                 return new HotlistSettings[size];
    //             }
    //         };

private:
    AutoPtr< ArrayOf<IWifiScannerBssidInfo*> > mBssidInfos;
    Int32 mApLostThreshold;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFISCANNERHOTLISTSETTINGS_H__
