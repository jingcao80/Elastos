
#ifndef __ELASTOS_DROID_LOCATION_CLOCATIONPROVIDER_H__
#define __ELASTOS_DROID_LOCATION_CLOCATIONPROVIDER_H__

#include "_Elastos_Droid_Location_CLocationProvider.h"
#include "elastos/droid/location/LocationProvider.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CLocationProvider)
    , public LocationProvider
{
public:
    CAR_OBJECT_DECL()

};

} // namespace Location
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_CLOCATIONPROVIDER_H__
