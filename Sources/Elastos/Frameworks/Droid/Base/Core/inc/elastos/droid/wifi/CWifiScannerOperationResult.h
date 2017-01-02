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

#ifndef __ELASTOS_DROID_NET_WIFI_CWIFISCANNEROPERATIONRESULT_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFISCANNEROPERATIONRESULT_H__

#include "_Elastos_Droid_Wifi_CWifiScannerOperationResult.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiScannerOperationResult)
    , public Object
    , public IParcelable
    , public IWifiScannerOperationResult
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 reason,
        /* [in] */ const String& description);

    CARAPI GetReason(
        /* [out] */ Int32* result);

    CARAPI SetReason(
        /* [in] */ Int32 reason);

    CARAPI GetDescription(
        /* [out] */ String* result);

    CARAPI SetDescription(
        /* [in] */ const String& description);

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
    // public static final Creator<OperationResult> CREATOR =
    //         new Creator<OperationResult>() {
    //             public OperationResult createFromParcel(Parcel in) {
    //                 int reason = in.readInt();
    //                 String description = in.readString();
    //                 return new OperationResult(reason, description);
    //             }

    //             public OperationResult[] newArray(int size) {
    //                 return new OperationResult[size];
    //             }
    //         };

private:
    Int32 mReason;
    String mDescription;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFISCANNEROPERATIONRESULT_H__
