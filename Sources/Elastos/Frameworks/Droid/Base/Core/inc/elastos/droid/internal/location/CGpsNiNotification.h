
#ifndef __ELASTOS_DROID_INTERNAL_LOCATION_CGPSNINOTIFICATION_H__
#define __ELASTOS_DROID_INTERNAL_LOCATION_CGPSNINOTIFICATION_H__

#include "_Elastos_Droid_Internal_Location_CGpsNiNotification.h"
#include "elastos/droid/internal/location/CGpsNetInitiatedHandler.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Location {

CarClass(CGpsNiNotification)
    , public CGpsNetInitiatedHandler::GpsNiNotification
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Location
} // namespace Internal
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_LOCATION_CGPSNINOTIFICATION_H__
