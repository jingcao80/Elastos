
#ifndef __ELASTOS_DROID_LOCATION_CFUSEDBATCHOPTIONS_H__
#define __ELASTOS_DROID_LOCATION_CFUSEDBATCHOPTIONS_H__

#include "_Elastos_Droid_Location_CFusedBatchOptions.h"
#include "elastos/droid/location/FusedBatchOptions.h"

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CFusedBatchOptions)
    , public FusedBatchOptions
{
public:
    CAR_OBJECT_DECL()
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_CFUSEDBATCHOPTIONS_H__
