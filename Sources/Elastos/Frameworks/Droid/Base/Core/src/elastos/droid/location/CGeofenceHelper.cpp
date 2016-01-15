#include "elastos/droid/location/CGeofenceHelper.h"
#include "elastos/droid/location/Geofence.h"

namespace Elastos {
namespace Droid {
namespace Location {

CAR_INTERFACE_IMPL(CGeofenceHelper, Singleton, IGeofenceHelper)

CAR_SINGLETON_IMPL(CGeofenceHelper)

ECode CGeofenceHelper::CreateCircle(
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Float radius,
    /* [out] */ IGeofence** geofence)
{
    VALIDATE_NOT_NULL(geofence)
    return Geofence::CreateCircle(latitude, longitude, radius, geofence);
}

} // namespace Location
} // namespace Droid
} // namespace Elastos