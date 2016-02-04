
#include "elastos/droid/server/location/CFusedLocationHardwareService.h"
#include "elastos/droid/server/location/CFusedGeofenceHardwareService.h"
#include "elastos/droid/server/location/CFusedLocationHardwareSecure.h"
#include "elastos/droid/server/location/CGpsStatusProviderService.h"
#include "elastos/droid/server/location/CGpsGeofenceHardwareService.h"
#include "elastos/droid/server/location/CNetInitiatedListener.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

CAR_OBJECT_IMPL(CFusedLocationHardwareService)
CAR_OBJECT_IMPL(CFusedGeofenceHardwareService)
CAR_OBJECT_IMPL(CFusedLocationHardwareSecure)
CAR_OBJECT_IMPL(CGpsStatusProviderService)
CAR_OBJECT_IMPL(CGpsGeofenceHardwareService)
CAR_OBJECT_IMPL(CNetInitiatedListener)

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos