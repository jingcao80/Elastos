#ifndef __ELASTOS_DROID_SERVER_LOCATION_CMYNETINITIATEDLISTENER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_CMYNETINITIATEDLISTENER_H__

#include "_Elastos_Droid_Server_Location_CNetInitiatedListener.h"
#include "elastos/droid/server/location/GpsLocationProvider.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

CarClass(CNetInitiatedListener)
    , public GpsLocationProvider::NetInitiatedListener
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Location
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_LOCATION_CMYNETINITIATEDLISTENER_H__