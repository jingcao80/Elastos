
#ifndef __ELASTOS_DROID_LOCATION_CGPSCLOCK_H__
#define __ELASTOS_DROID_LOCATION_CGPSCLOCK_H__

#include "_Elastos_Droid_Location_CGpsClock.h"
#include "elastos/droid/location/GpsClock.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CGpsClock)
    , public GpsClock
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_CGPSCLOCK_H__
