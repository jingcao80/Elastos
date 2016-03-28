
#ifndef __ELASTOS_DROID_LOCATION_CGEOFENCEPARAMS_H__
#define __ELASTOS_DROID_LOCATION_CGEOFENCEPARAMS_H__

#include "_Elastos_Droid_Location_CGeoFenceParams.h"
#include "elastos/droid/location/GeoFenceParams.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CGeoFenceParams)
    , public GeoFenceParams
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_CGEOFENCEPARAMS_H__
