
#ifndef __ELASTOS_DROID_LOCATION_CGPSMEASUREMENTEVENT_H__
#define __ELASTOS_DROID_LOCATION_CGPSMEASUREMENTEVENT_H__

#include "_Elastos_Droid_Location_CGpsMeasurementsEvent.h"
#include "elastos/droid/location/GpsMeasurementsEvent.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CGpsMeasurementsEvent)
    , public GpsMeasurementsEvent
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_CGPSMEASUREMENTEVENT_H__
