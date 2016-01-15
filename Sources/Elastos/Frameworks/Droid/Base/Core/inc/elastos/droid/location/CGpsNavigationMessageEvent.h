
#ifndef __ELASTOS_DROID_LOCATION_CGPSNAVIGATIONMESSAGEEVENT_H__
#define __ELASTOS_DROID_LOCATION_CGPSNAVIGATIONMESSAGEEVENT_H__

#include "_Elastos_Droid_Location_CGpsNavigationMessageEvent.h"
#include "elastos/droid/location/GpsNavigationMessageEvent.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CGpsNavigationMessageEvent)
    , public GpsNavigationMessageEvent
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_CGPSNAVIGATIONMESSAGEEVENT_H__
