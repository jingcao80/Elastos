#ifndef __ELASTOS_DROID_SERVER_LOCATION_CFUSED_LOCATION_HARDWARE_SERVICE_H__
#define __ELASTOS_DROID_SERVER_LOCATION_CFUSED_LOCATION_HARDWARE_SERVICE_H__

#include "_Elastos_Droid_Server_Location_CFusedLocationHardwareService.h"
#include "elastos/droid/server/location/FlpHardwareProvider.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

CarClass(CFusedLocationHardwareService)
    , public FlpHardwareProvider::FusedLocationHardwareService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Location
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_LOCATION_CFUSED_LOCATION_HARDWARE_SERVICE_H__