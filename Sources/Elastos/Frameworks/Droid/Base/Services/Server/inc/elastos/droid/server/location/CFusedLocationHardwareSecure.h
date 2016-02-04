#ifndef __ELASTOS_DROID_SERVER_LOCATION_CFUSEDLOCATIONHARDWARESECURE_H__
#define __ELASTOS_DROID_SERVER_LOCATION_CFUSEDLOCATIONHARDWARESECURE_H__

#include "_Elastos_Droid_Server_Location_CFusedLocationHardwareSecure.h"
#include "elastos/droid/server/location/FusedLocationHardwareSecure.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

CarClass(CFusedLocationHardwareSecure)
    , public FusedLocationHardwareSecure
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Location
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_LOCATION_CFUSEDLOCATIONHARDWARESECURE_H__