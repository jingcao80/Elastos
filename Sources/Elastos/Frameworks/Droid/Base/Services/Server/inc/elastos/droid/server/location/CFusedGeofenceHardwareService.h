#ifndef __ELASTOS_DROID_SERVER_LOCATION_CMYGEOFENCEHARDWARESERVICE_H__
#define __ELASTOS_DROID_SERVER_LOCATION_CMYGEOFENCEHARDWARESERVICE_H__

#include "_Elastos_Droid_Server_Location_CFusedGeofenceHardwareService.h"
#include "elastos/droid/server/location/FlpHardwareProvider.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

CarClass(CFusedGeofenceHardwareService)
    , public FlpHardwareProvider::FusedGeofenceHardwareService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Location
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_LOCATION_CMYGEOFENCEHARDWARESERVICE_H__