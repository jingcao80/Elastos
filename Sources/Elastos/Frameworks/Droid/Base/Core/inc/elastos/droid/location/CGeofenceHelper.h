#ifndef __ELASTOS_DROID_LOCATION_CGEOFENCEHELPER_H__
#define __ELASTOS_DROID_LOCATION_CGEOFENCEHELPER_H__

#include "_Elastos_Droid_Location_CGeofenceHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CGeofenceHelper)
    , public Singleton
    , public IGeofenceHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Create a circular geofence (on a flat, horizontal plane).
     *
     * @param latitude latitude in degrees
     * @param longitude longitude in degrees
     * @param radius radius in meters
     * @return a new geofence
     * @throws IllegalArgumentException if any parameters are out of range
     */
    CARAPI CreateCircle(
        /* [in] */ Double latitude,
        /* [in] */ Double longitude,
        /* [in] */ Float radius,
        /* [out] */ IGeofence** geofence);
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_CGEOFENCEHELPER_H__