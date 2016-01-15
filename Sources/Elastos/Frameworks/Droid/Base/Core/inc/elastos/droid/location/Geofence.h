
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
