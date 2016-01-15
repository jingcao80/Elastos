
#ifndef __ELASTOS_DROID_LOCATION_CGEOCODER_H__
#define __ELASTOS_DROID_LOCATION_CGEOCODER_H__

#include "_Elastos_Droid_Location_CGeocoder.h"
#include "elastos/droid/location/Geocoder.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CGeocoder)
    , public Geocoder
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_CGEOCODER_H__
