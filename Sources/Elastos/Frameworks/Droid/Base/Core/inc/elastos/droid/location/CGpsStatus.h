
#ifndef __ELASTOS_DROID_LOCATION_CGPSSTATUS_H__
#define __ELASTOS_DROID_LOCATION_CGPSSTATUS_H__

#include "_Elastos_Droid_Location_CGpsStatus.h"
#include "elastos/droid/location/GpsStatus.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CGpsStatus)
    , public GpsStatus
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_CGPSSTATUS_H__
