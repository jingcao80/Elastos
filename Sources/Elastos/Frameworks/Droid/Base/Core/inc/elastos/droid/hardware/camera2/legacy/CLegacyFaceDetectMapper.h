
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CLEGACYFACEDETECTMAPPER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CLEGACYFACEDETECTMAPPER_H__

#include "_Elastos_Droid_Hardware_Camera2_Legacy_CLegacyFaceDetectMapper.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyFaceDetectMapper.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CarClass(CLegacyFaceDetectMapper)
    , public LegacyFaceDetectMapper
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CLEGACYFACEDETECTMAPPER_H__
