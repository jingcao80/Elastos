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

#ifndef __ELASTOS_DROID_NET_WIFI_CRTTMANAGERPARCELABLERTTPARAMS_H__
#define __ELASTOS_DROID_NET_WIFI_CRTTMANAGERPARCELABLERTTPARAMS_H__

#include "_Elastos_Droid_Wifi_CRttManagerParcelableRttParams.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CRttManagerParcelableRttParams)
    , public Object
    , public IParcelable
    , public IRttManagerParcelableRttParams
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ArrayOf<IRttManagerRttParams*>* params);

    CARAPI GetParams(
        /* [out, callee] */ ArrayOf<IRttManagerRttParams*>** result);

    CARAPI SetParams(
        /* [in] */ ArrayOf<IRttManagerRttParams*>* params);

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
    // public static final Creator<ParcelableRttParams> CREATOR =
    //         new Creator<ParcelableRttParams>() {
    //             public ParcelableRttParams createFromParcel(Parcel in) {

    //                 int num = in.readInt();

    //                 if (num == 0) {
    //                     return new ParcelableRttParams(null);
    //                 }

    //                 RttParams params[] = new RttParams[num];
    //                 for (int i = 0; i < num; i++) {
    //                     params[i] = new RttParams();
    //                     params[i].deviceType = in.readInt();
    //                     params[i].requestType = in.readInt();
    //                     params[i].bssid = in.readString();
    //                     params[i].frequency = in.readInt();
    //                     params[i].channelWidth = in.readInt();
    //                     params[i].num_samples = in.readInt();
    //                     params[i].num_retries = in.readInt();

    //                 }

    //                 ParcelableRttParams parcelableParams = new ParcelableRttParams(params);
    //                 return parcelableParams;
    //             }

    //             public ParcelableRttParams[] newArray(int size) {
    //                 return new ParcelableRttParams[size];
    //             }
    //         };

private:
    AutoPtr< ArrayOf<IRttManagerRttParams*> > mParams;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CRTTMANAGERPARCELABLERTTPARAMS_H__
