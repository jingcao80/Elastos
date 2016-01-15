
#ifndef __ELASTOS_DROID_LOCATION_CGPSMEASUREMENT_H__
#define __ELASTOS_DROID_LOCATION_CGPSMEASUREMENT_H__

#include "_Elastos_Droid_Location_CGpsMeasurement.h"
#include "elastos/droid/location/GpsMeasurement.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CGpsMeasurement)
    , public GpsMeasurement
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_CGPSMEASUREMENT_H__
