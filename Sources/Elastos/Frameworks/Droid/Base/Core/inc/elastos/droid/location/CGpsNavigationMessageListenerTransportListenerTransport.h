
#ifndef __ELASTOS_DROID_LOCATION_CGPSNAVIGATIONMESSAGELISTENERTRANSPORTLISTENERTRANSPORT_H__
#define __ELASTOS_DROID_LOCATION_CGPSNAVIGATIONMESSAGELISTENERTRANSPORTLISTENERTRANSPORT_H__

#include "_Elastos_Droid_Location_CGpsNavigationMessageListenerTransportListenerTransport.h"
#include "elastos/droid/location/GpsNavigationMessageListenerTransport.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CGpsNavigationMessageListenerTransportListenerTransport)
    , public GpsNavigationMessageListenerTransport::ListenerTransport
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_CGPSNAVIGATIONMESSAGELISTENERTRANSPORTLISTENERTRANSPORT_H__
