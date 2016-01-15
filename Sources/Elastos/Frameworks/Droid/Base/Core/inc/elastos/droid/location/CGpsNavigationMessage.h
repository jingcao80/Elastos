
#ifndef __ELASTOS_DROID_LOCATION_CGPSNAVIGATIONMESSAGE_H__
#define __ELASTOS_DROID_LOCATION_CGPSNAVIGATIONMESSAGE_H__

#include "_Elastos_Droid_Location_CGpsNavigationMessage.h"
#include "elastos/droid/location/GpsNavigationMessage.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CGpsNavigationMessage)
    , public GpsNavigationMessage
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_CGPSNAVIGATIONMESSAGE_H__