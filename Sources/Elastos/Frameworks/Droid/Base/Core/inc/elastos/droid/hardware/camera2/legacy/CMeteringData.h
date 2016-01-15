
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CMETERINGDATA_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CMETERINGDATA_H__

#include "_Elastos_Droid_Hardware_Camera2_Legacy_CMeteringData.h"
#include "elastos/droid/hardware/camera2/legacy/ParameterUtils.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CarClass(CMeteringData)
    , public ParameterUtils::MeteringData
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CMETERINGDATA_H__
