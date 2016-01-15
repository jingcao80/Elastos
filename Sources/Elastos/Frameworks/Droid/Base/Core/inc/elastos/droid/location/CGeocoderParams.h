
#ifndef __ELASTOS_DROID_LOCATION_CGEOCODERPARAMS_H__
#define __ELASTOS_DROID_LOCATION_CGEOCODERPARAMS_H__

#include "_Elastos_Droid_Location_CGeocoderParams.h"
#include "elastos/droid/location/GeocoderParams.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CGeocoderParams)
    , public GeocoderParams
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_CGEOCODERPARAMS_H__
