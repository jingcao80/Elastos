
#ifndef __ELASTOS_DROID_HARDWARE_CHARDWARECAMERA_H__
#define __ELASTOS_DROID_HARDWARE_CHARDWARECAMERA_H__

#include "_Elastos_Droid_Hardware_CHardwareCamera.h"
#include "elastos/droid/hardware/HardwareCamera.h"

namespace Elastos {
namespace Droid {
namespace Hardware {

CarClass(CHardwareCamera)
    , public HardwareCamera
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CHARDWARECAMERA_H__
