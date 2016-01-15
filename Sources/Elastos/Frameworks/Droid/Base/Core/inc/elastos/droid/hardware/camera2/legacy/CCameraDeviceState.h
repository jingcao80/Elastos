
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CCAMERADEVICESTATE_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CCAMERADEVICESTATE_H__

#include "_Elastos_Droid_Hardware_Camera2_Legacy_CCameraDeviceState.h"
#include "elastos/droid/hardware/camera2/legacy/CameraDeviceState.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CarClass(CCameraDeviceState)
    , public CameraDeviceState
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CCAMERADEVICESTATE_H__
