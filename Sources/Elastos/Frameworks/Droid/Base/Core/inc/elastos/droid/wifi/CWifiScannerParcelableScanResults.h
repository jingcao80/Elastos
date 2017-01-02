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

#ifndef __ELASTOS_DROID_NET_WIFI_CWIFISCANNERPARCELABLESCANRESULTS_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFISCANNERPARCELABLESCANRESULTS_H__

#include "_Elastos_Droid_Wifi_CWifiScannerParcelableScanResults.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiScannerParcelableScanResults)
    , public Object
    , public IParcelable
    , public IWifiScannerParcelableScanResults
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ArrayOf<IScanResult*>* results);

    CARAPI SetResults(
        /* [in] */ ArrayOf<IScanResult*>* results);

    CARAPI GetResults(
        /* [out, callee] */ ArrayOf<IScanResult*>** result);

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
    // public static final Creator<ParcelableScanResults> CREATOR =
    //         new Creator<ParcelableScanResults>() {
    //             public ParcelableScanResults createFromParcel(Parcel in) {
    //                 int n = in.readInt();
    //                 ScanResult results[] = new ScanResult[n];
    //                 for (int i = 0; i < n; i++) {
    //                     results[i] = ScanResult.CREATOR.createFromParcel(in);
    //                 }
    //                 return new ParcelableScanResults(results);
    //             }

    //             public ParcelableScanResults[] newArray(int size) {
    //                 return new ParcelableScanResults[size];
    //             }
    //         };

private:
    AutoPtr< ArrayOf<IScanResult*> > mResults;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFISCANNERPARCELABLESCANRESULTS_H__
