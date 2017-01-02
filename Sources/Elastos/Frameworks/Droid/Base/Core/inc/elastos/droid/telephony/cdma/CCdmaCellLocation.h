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

#ifndef __ELASTOS_DROID_TELEPHONY_CDMA_CCDMACELLLOCATION_H__
#define __ELASTOS_DROID_TELEPHONY_CDMA_CCDMACELLLOCATION_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Telephony_Cdma_CCdmaCellLocation.h"
#include "elastos/droid/telephony/CellLocation.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Cdma {

CarClass(CCdmaCellLocation)
    , public CellLocation
    , public ICdmaCellLocation
{
public:
    CCdmaCellLocation();

    virtual ~CCdmaCellLocation();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IBundle* bundle);

    CARAPI FillInNotifierBundle(
        /* [in] */ IBundle* bundle);

    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    CARAPI GetBaseStationId(
        /* [out] */ Int32* baseStationId);

    CARAPI GetBaseStationLatitude(
        /* [out] */ Int32* baseStationLatitude);

    CARAPI GetBaseStationLongitude(
        /* [out] */ Int32* baseStationLongitude);

    CARAPI GetSystemId(
        /* [out] */ Int32* systemid);

    CARAPI GetNetworkId(
        /* [out] */ Int32* networkid);

    CARAPI SetStateInvalid();

    CARAPI SetCellLocationData(
        /* [in] */ Int32 baseStationId,
        /* [in] */ Int32 baseStationLatitude,
        /* [in] */ Int32 baseStationLongitude);

    CARAPI SetCellLocationData(
        /* [in] */ Int32 baseStationId,
        /* [in] */ Int32 baseStationLatitude,
        /* [in] */ Int32 baseStationLongitude,
        /* [in] */ Int32 systemId,
        /* [in] */ Int32 networkId);

    static CARAPI ConvertQuartSecToDecDegrees(
        /* [in] */ Int32 quartSec,
        /* [out] */ Double* val);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

private:

    static CARAPI_(Boolean) EqualsHandlesNulls(
        /* [in] */ Int32 a,
        /* [in] */ Int32 b);

    Int32 mBaseStationId;

    /**
     * Latitude is a decimal number as specified in 3GPP2 C.S0005-A v6.0.
     * It is represented in units of 0.25 seconds and ranges from -1296000
     * to 1296000, both values inclusive (corresponding to a range of -90
     * to +90 degrees). Integer.MAX_VALUE is considered invalid value.
     */
    Int32 mBaseStationLatitude;

    /**
     * Longitude is a decimal number as specified in 3GPP2 C.S0005-A v6.0.
     * It is represented in units of 0.25 seconds and ranges from -2592000
     * to 2592000, both values inclusive (corresponding to a range of -180
     * to +180 degrees). Integer.MAX_VALUE is considered invalid value.
     */
    Int32 mBaseStationLongitude;

    Int32 mSystemId;
    Int32 mNetworkId;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CDMA_CCDMACELLLOCATION_H__
