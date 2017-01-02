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

#ifndef __ELASTOS_DROID_LOCATION_GEOFENCE_H__
#define __ELASTOS_DROID_LOCATION_GEOFENCE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Location.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Location {

/**
 * Represents a geographical boundary, also known as a geofence.
 *
 * <p>Currently only circular geofences are supported and they do not support altitude changes.
 *
 * @hide
 */
class Geofence
    : public Object
    , public IGeofence
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    Geofence();

    CARAPI constructor();

    static CARAPI CreateCircle(
        /* [in] */ Double latitude,
        /* [in] */ Double longitude,
        /* [in] */ Float radius,
        /* [out] */ IGeofence** geofence);

    /** @hide */
    CARAPI GetType(
        /* [out] */ Int32* type);

    /** @hide */
    CARAPI GetLatitude(
        /* [out] */ Double* latitude);

    /** @hide */
    CARAPI GetLongitude(
        /* [out] */ Double* longitude);

    /** @hide */
    CARAPI GetRadius(
        /* [out] */ Float* radius);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    //@Override
    CARAPI ToString(
        /* [out] */ String* strOut);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* rst);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

private:
    Geofence(
        /* [in] */ Double latitude,
        /* [in] */ Double longitude,
        /* [in] */ Float radius);

    static CARAPI CheckRadius(
        /* [in] */ Float radius);

    static CARAPI CheckLatLong(
        /* [in] */ Double latitude,
        /* [in] */ Double longitude);

    static CARAPI CheckType(
        /* [in] */ Int32 type);

    static CARAPI TypeToString(
        /* [in] */ Int32 type,
        /* [out] */ String* result);

private:
    const static String TAG;
    Int32 mType;
    Double mLatitude;
    Double mLongitude;
    Float mRadius;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_GEOFENCE_H__
