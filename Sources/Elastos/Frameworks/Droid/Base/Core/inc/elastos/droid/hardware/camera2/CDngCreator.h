
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_CDNGCREATOR_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_CDNGCREATOR_H__

#include "_Elastos_Droid_Hardware_Camera2_CDngCreator.h"
#include "elastos/droid/hardware/camera2/DngCreator.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

CarClass(CDngCreator)
    , public DngCreator
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_CDNGCREATOR_H__
