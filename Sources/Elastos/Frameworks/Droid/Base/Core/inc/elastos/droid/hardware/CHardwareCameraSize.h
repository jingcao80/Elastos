
#ifndef __ELASTOS_DROID_HARDWARE_CHARDWARECAMERASIZE_H__
#define __ELASTOS_DROID_HARDWARE_CHARDWARECAMERASIZE_H__

#include "_Elastos_Droid_Hardware_CHardwareCameraSize.h"
#include "elastos/droid/hardware/HardwareCamera.h"

namespace Elastos {
namespace Droid {
namespace Hardware {

CarClass(CHardwareCameraSize)
    , public HardwareCamera::Size
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CHARDWARECAMERASIZE_H__
