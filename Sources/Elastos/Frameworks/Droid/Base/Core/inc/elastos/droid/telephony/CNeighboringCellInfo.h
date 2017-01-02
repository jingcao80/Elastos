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

#ifndef __ELASTOS_DROID_TELEPHONY_CNEIGHBORINGCELLINFO_H__
#define __ELASTOS_DROID_TELEPHONY_CNEIGHBORINGCELLINFO_H__

#include "_Elastos_Droid_Telephony_CNeighboringCellInfo.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CNeighboringCellInfo)
    , public Object
    , public INeighboringCellInfo
    , public IParcelable
{
public:
    CNeighboringCellInfo();

    virtual ~CNeighboringCellInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 rssi,
        /* [in] */ Int32 cid);

    CARAPI constructor(
        /* [in] */ Int32 rssi,
        /* [in] */ const String& location,
        /* [in] */ Int32 radioType);

    CARAPI GetRssi(
        /* [out] */ Int32* rssi);

    CARAPI GetLac(
        /* [out] */ Int32* lac);

    CARAPI GetCid(
        /* [out] */ Int32* cid);

    CARAPI GetPsc(
        /* [out] */ Int32* psc);

    CARAPI GetNetworkType(
        /* [out] */ Int32* networkType);

    CARAPI SetCid(
        /* [in] */ Int32 cid);

    CARAPI SetRssi(
        /* [in] */ Int32 rssi);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:

    /**
     * In GSM, mRssi is the Received RSSI;
     * In UMTS, mRssi is the Level index of CPICH Received Signal Code Power
     */
    Int32 mRssi;
    /**
     * CID in 16 bits format in GSM. Return UNKNOWN_CID in UMTS and CMDA.
     */
    Int32 mCid;
    /**
     * LAC in 16 bits format in GSM. Return UNKNOWN_CID in UMTS and CMDA.
     */
    Int32 mLac;
    /**
     * Primary Scrambling Code in 9 bits format in UMTS
     * Return UNKNOWN_CID in GSM and CMDA.
     */
    Int32 mPsc;
    /**
     * Radio network type, value is one of following
     * TelephonyManager.NETWORK_TYPE_XXXXXX.
     */
    Int32 mNetworkType;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CNEIGHBORINGCELLINFO_H__
