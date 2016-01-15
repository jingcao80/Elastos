
#ifndef __ELASTOS_DROID_LOCATION_CGPSMEASUREMENTLISTENERTRANSPORT_H__
#define __ELASTOS_DROID_LOCATION_CGPSMEASUREMENTLISTENERTRANSPORT_H__

#include "_Elastos_Droid_Location_CGpsMeasurementListenerTransport.h"
#include "elastos/droid/location/GpsMeasurementListenerTransport.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CGpsMeasurementListenerTransport)
    , public GpsMeasurementListenerTransport
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_CGPSMEASUREMENTLISTENERTRANSPORT_H__
