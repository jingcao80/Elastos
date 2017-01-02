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

#ifndef __ELASTOS_DROID_NET_WIFI_CRSSIPACKETCOUNTINFO_H__
#define __ELASTOS_DROID_NET_WIFI_CRSSIPACKETCOUNTINFO_H__

#include "_Elastos_Droid_Wifi_CRssiPacketCountInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CRssiPacketCountInfo)
    , public Object
    , public IRssiPacketCountInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParcel* parcel);

    CARAPI GetRssi(
        /* [out] */ Int32* rssi);

    CARAPI SetRssi(
        /* [in] */ Int32 rssi);

    CARAPI GetTxgood(
        /* [out] */ Int32* txgood);

    CARAPI SetTxgood(
        /* [in] */ Int32 txgood);

    CARAPI GetTxbad(
        /* [out] */ Int32* txbad);

    CARAPI SetTxbad(
        /* [in] */ Int32 txbad);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetRxgood(
        /* [out] */ Int32* result);

    CARAPI SetRxgood(
        /* [in] */ Int32 rxgood);

private:
    Int32 mRssi;
    Int32 mTxgood;
    Int32 mTxbad;
    Int32 mRxgood;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_CRSSIPACKETCOUNTINFO_H__
