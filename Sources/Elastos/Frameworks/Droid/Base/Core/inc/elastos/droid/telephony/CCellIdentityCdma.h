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

#ifndef __ELASTOS_DROID_TELEPHONY_CCELLIDENTITYCDMA_H__
#define __ELASTOS_DROID_TELEPHONY_CCELLIDENTITYCDMA_H__

#include "_Elastos_Droid_Telephony_CCellIdentityCdma.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCellIdentityCdma)
    , public Object
    , public ICellIdentityCdma
    , public IParcelable
{
public:
    CCellIdentityCdma();

    virtual ~CCellIdentityCdma();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 nid,
        /* [in] */ Int32 sid,
        /* [in] */ Int32 bid,
        /* [in] */ Int32 lon,
        /* [in] */ Int32 lat);

    CARAPI constructor(
        /* [in] */ ICellIdentityCdma* cid);

    CARAPI Copy(
        /* [out] */ ICellIdentityCdma** cid);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetNetworkId(
        /* [out] */ Int32* networkid);

    CARAPI GetSystemId(
        /* [out] */ Int32* systemid);

    CARAPI GetBasestationId(
        /* [out] */ Int32* basestationid);

    CARAPI GetLongitude(
        /* [out] */ Int32* longitude);

    CARAPI GetLatitude(
        /* [out] */ Int32* latitude);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* res);

    CARAPI ToString(
        /* [out] */ String* str);

private:

    static const String TAG;
    static const Boolean DBG;

    // Network Id 0..65535
    Int32 mNetworkId;
    // CDMA System Id 0..32767
    Int32 mSystemId;
    // Base Station Id 0..65535
    Int32 mBasestationId;
    /**
     * Longitude is a decimal number as specified in 3GPP2 C.S0005-A v6.0.
     * It is represented in units of 0.25 seconds and ranges from -2592000
     * to 2592000, both values inclusive (corresponding to a range of -180
     * to +180 degrees).
     */
    Int32 mLongitude;
    /**
     * Latitude is a decimal number as specified in 3GPP2 C.S0005-A v6.0.
     * It is represented in units of 0.25 seconds and ranges from -1296000
     * to 1296000, both values inclusive (corresponding to a range of -90
     * to +90 degrees).
     */
    Int32 mLatitude;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CCELLIDENTITYCDMA_H__
