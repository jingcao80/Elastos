
#ifndef __ELASTOS_DROID_LOCATION_CLOCATIONMANAGERLISTENERTRANSPORT_H__
#define __ELASTOS_DROID_LOCATION_CLOCATIONMANAGERLISTENERTRANSPORT_H__

#include "_Elastos_Droid_Location_CLocationManagerListenerTransport.h"
#include "elastos/droid/location/LocationManager.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CLocationManagerListenerTransport)
    , public LocationManager::ListenerTransport
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Location
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_CLOCATIONMANAGERLISTENERTRANSPORT_H__
