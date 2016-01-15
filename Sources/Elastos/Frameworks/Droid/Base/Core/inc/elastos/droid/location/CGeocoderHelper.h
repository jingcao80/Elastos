#ifndef __ELASTOS_DROID_LOCATION_CGEOCODERHELPER_H__
#define __ELASTOS_DROID_LOCATION_CGEOCODERHELPER_H__

#include "_Elastos_Droid_Location_CGeocoderHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CGeocoderHelper)
    , public Singleton
    , public IGeocoderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns true if the Geocoder methods getFromLocation and
     * getFromLocationName are implemented.  Lack of network
     * connectivity may still cause these methods to return null or
     * empty lists.
     */
    CARAPI IsPresent(
        /* [out] */ Boolean* isPresent);
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_CGEOCODERHELPER_H__