
#ifndef __ELASTOS_DROID_INTERNAL_LOCATION_CGPSNIRESPONSE_H__
#define __ELASTOS_DROID_INTERNAL_LOCATION_CGPSNIRESPONSE_H__

#include "_Elastos_Droid_Internal_Location_CGpsNiResponse.h"
#include "elastos/droid/internal/location/CGpsNetInitiatedHandler.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Location {

CarClass(CGpsNiResponse)
    , public CGpsNetInitiatedHandler::GpsNiResponse
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Location
} // namespace Internal
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_LOCATION_CGPSNIRESPONSE_H__
