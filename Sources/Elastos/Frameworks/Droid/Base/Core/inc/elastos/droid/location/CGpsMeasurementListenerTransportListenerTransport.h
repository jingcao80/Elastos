
#ifndef __ELASTOS_DROID_LOCATION_CGPSMEASUREMENTLISTENERTRANSPORTLISTENERTRANSPORT_H__
#define __ELASTOS_DROID_LOCATION_CGPSMEASUREMENTLISTENERTRANSPORTLISTENERTRANSPORT_H__

#include "_Elastos_Droid_Location_CGpsMeasurementListenerTransportListenerTransport.h"
#include "elastos/droid/location/GpsMeasurementListenerTransport.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CGpsMeasurementListenerTransportListenerTransport)
    , public GpsMeasurementListenerTransport::ListenerTransport
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_CGPSMEASUREMENTLISTENERTRANSPORTLISTENERTRANSPORT_H__
