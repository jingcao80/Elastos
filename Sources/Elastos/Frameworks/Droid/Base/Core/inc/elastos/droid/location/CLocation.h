
#ifndef __ELASTOS_DROID_LOCATION_CLOCATION_H__
#define __ELASTOS_DROID_LOCATION_CLOCATION_H__

#include "elastos/droid/location/Location.h"
#include "_Elastos_Droid_Location_CLocation.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CLocation)
    , public Location
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Location
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_CLOCATION_H__
