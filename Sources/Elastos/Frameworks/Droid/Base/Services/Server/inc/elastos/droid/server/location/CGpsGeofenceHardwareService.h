#ifndef __ELASTOS_DROID_SERVER_LOCATION_CMYGPSGEOFENCEBINDER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_CMYGPSGEOFENCEBINDER_H__

#include "_Elastos_Droid_Server_Location_CGpsGeofenceHardwareService.h"
#include "elastos/droid/server/location/GpsLocationProvider.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

CarClass(CGpsGeofenceHardwareService)
    , public GpsLocationProvider::GpsGeofenceHardwareService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Location
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_LOCATION_CMYGPSGEOFENCEBINDER_H__