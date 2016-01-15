
#ifndef __ELASTOS_DROID_HARDWARE_CSENSOREVENT_H__
#define __ELASTOS_DROID_HARDWARE_CSENSOREVENT_H__

#include "_Elastos_Droid_Hardware_CSensorEvent.h"
#include "elastos/droid/hardware/SensorEvent.h"

namespace Elastos {
namespace Droid {
namespace Hardware {

CarClass(CSensorEvent)
    , public SensorEvent
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_CSENSOREVENT_H__
