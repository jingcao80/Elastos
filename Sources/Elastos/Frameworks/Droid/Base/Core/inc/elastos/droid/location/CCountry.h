
#ifndef __ELASTOS_DROID_LOCATION_CCOUNTRY_H__
#define __ELASTOS_DROID_LOCATION_CCOUNTRY_H__

#include "_Elastos_Droid_Location_CCountry.h"
#include "elastos/droid/location/Country.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CCountry)
    , public Country
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_CCOUNTRY_H__
