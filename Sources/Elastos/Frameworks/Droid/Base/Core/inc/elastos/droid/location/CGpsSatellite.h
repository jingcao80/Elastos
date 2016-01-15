
#ifndef __ELASTOS_DROID_LOCATION_CGPSATELLITE_H__
#define __ELASTOS_DROID_LOCATION_CGPSATELLITE_H__

#include "_Elastos_Droid_Location_CGpsSatellite.h"
#include "elastos/droid/location/GpsSatellite.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CGpsSatellite)
    , public GpsSatellite
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_CGPSATELLITE_H__
