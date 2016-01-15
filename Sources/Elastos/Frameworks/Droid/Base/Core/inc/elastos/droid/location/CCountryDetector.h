
#ifndef __ELASTOS_DROID_LOCATION_CCOUNTRYDETECTOR_H__
#define __ELASTOS_DROID_LOCATION_CCOUNTRYDETECTOR_H__

#include "_Elastos_Droid_Location_CCountryDetector.h"
#include "elastos/droid/location/CountryDetector.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CCountryDetector)
    , public CountryDetector
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_CCOUNTRYDETECTOR_H__
