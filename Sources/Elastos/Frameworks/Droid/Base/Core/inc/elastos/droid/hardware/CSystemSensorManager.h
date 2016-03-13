#ifndef __ELASTOS_DROID_HARDWARE_CSYSTEM_SENSORMANAGER_H__
#define __ELASTOS_DROID_HARDWARE_CSYSTEM_SENSORMANAGER_H__

#include "_Elastos_Droid_Hardware_CSystemSensorManager.h"
#include "elastos/droid/hardware/SystemSensorManager.h"

namespace Elastos {
namespace Droid {
namespace Hardware {

CarClass(CSystemSensorManager)
    , public SystemSensorManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_HARDWARE_CSYSTEM_SENSORMANAGER_H__