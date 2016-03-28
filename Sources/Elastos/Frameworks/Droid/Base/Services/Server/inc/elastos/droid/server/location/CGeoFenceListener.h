#ifndef __ELASTOS_DROID_SERVER_LOCATION_CGEOFENCELISTENER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_CGEOFENCELISTENER_H__

#include "_Elastos_Droid_Server_Location_CGeoFenceListener.h"
#include "elastos/droid/server/location/GeoFencerProxy.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

CarClass(CGeoFenceListener)
    , public GeoFencerProxy::GeoFenceListener
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Location
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_LOCATION_CGEOFENCELISTENER_H__