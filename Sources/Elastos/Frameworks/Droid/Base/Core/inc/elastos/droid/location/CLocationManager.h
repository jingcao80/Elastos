#ifndef __ELASTOS_DROID_LOCATION_CLOCATIONMANAGER_H__
#define __ELASTOS_DROID_LOCATION_CLOCATIONMANAGER_H__

#include "_Elastos_Droid_Location_CLocationManager.h"
#include "elastos/droid/location/LocationManager.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CLocationManager)
    , public LocationManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Location
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_CLOCATIONMANAGER_H__
