
#ifndef __ELASTOS_DROID_LOCATION_CGPSNAVIGATIONMESSAGELISTENERTRANSPORT_H__
#define __ELASTOS_DROID_LOCATION_CGPSNAVIGATIONMESSAGELISTENERTRANSPORT_H__

#include "_Elastos_Droid_Location_CGpsNavigationMessageListenerTransport.h"
#include "elastos/droid/location/GpsNavigationMessageListenerTransport.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CGpsNavigationMessageListenerTransport)
    , public GpsNavigationMessageListenerTransport
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_CGPSNAVIGATIONMESSAGELISTENERTRANSPORT_H__
