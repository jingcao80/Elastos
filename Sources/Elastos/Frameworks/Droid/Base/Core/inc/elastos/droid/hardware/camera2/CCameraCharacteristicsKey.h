
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_CCAMERACHARACTERISTICSKEY_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_CCAMERACHARACTERISTICSKEY_H__

#include "_Elastos_Droid_Hardware_Camera2_CCameraCharacteristicsKey.h"
#include "elastos/droid/hardware/camera2/CameraCharacteristics.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

CarClass(CCameraCharacteristicsKey)
    , public CameraCharacteristics::Key
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_CCAMERACHARACTERISTICSKEY_H__
