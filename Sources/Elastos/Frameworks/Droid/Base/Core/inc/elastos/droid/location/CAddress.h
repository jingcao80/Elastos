
#ifndef __ELASTOS_DROID_LOCATION_CADDRESS_H__
#define __ELASTOS_DROID_LOCATION_CADDRESS_H__

#include "elastos/droid/location/Address.h"
#include "_Elastos_Droid_Location_CAddress.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CAddress)
    , public Address
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_CADDRESS_H__
