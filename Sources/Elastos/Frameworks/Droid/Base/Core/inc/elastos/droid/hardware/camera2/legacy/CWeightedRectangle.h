
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CWEIGHREDRECTANGLE_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CWEIGHREDRECTANGLE_H__

#include "_Elastos_Droid_Hardware_Camera2_Legacy_CWeightedRectangle.h"
#include "elastos/droid/hardware/camera2/legacy/ParameterUtils.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CarClass(CWeightedRectangle)
    , public ParameterUtils::WeightedRectangle
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CWEIGHREDRECTANGLE_H__
