
#ifndef __ELASTOS_DROID_LOCATION_CLOCATIONREQUEST_H__
#define __ELASTOS_DROID_LOCATION_CLOCATIONREQUEST_H__

#include "_Elastos_Droid_Location_CLocationRequest.h"
#include "elastos/droid/location/LocationRequest.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CLocationRequest)
    , public LocationRequest
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_CLOCATIONREQUEST_H__
