
#ifndef __ELASTOS_DROID_LOCATION_CLOCATIONMANAGERGPSSTATUSLISTENERTRANSPORT_H__
#define __ELASTOS_DROID_LOCATION_CLOCATIONMANAGERGPSSTATUSLISTENERTRANSPORT_H__

#include "_Elastos_Droid_Location_CLocationManagerGpsStatusListenerTransport.h"
#include "elastos/droid/location/LocationManager.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CLocationManagerGpsStatusListenerTransport)
    , public LocationManager::GpsStatusListenerTransport
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Location
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_CLOCATIONMANAGERGPSSTATUSLISTENERTRANSPORT_H__
