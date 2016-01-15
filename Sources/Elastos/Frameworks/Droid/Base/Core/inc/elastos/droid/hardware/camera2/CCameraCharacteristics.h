
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_CGEOMAGNETICFIELD_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_CGEOMAGNETICFIELD_H__

#include "_Elastos_Droid_Hardware_Camera2_CCameraCharacteristics.h"
#include "elastos/droid/hardware/camera2/CameraCharacteristics.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

CarClass(CCameraCharacteristics)
    , public CameraCharacteristics
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_CGEOMAGNETICFIELD_H__
