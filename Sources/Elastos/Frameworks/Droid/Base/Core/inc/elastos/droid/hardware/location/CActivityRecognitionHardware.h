
#ifndef __ELASTOS_DROID_HARDWARE_LOCATION_CACTIVITYRECOGNITIONHARDWARE_H__
#define __ELASTOS_DROID_HARDWARE_LOCATION_CACTIVITYRECOGNITIONHARDWARE_H__

#include "_Elastos_Droid_Hardware_Location_CActivityRecognitionHardware.h"
#include "elastos/droid/hardware/location/ActivityRecognitionHardware.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

CarClass(CActivityRecognitionHardware) , public ActivityRecognitionHardware
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Location
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_LOCATION_CACTIVITYRECOGNITIONHARDWARE_H__
