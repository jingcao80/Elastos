
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CLEGACYFOCUSSTATEMAPPER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CLEGACYFOCUSSTATEMAPPER_H__

#include "_Elastos_Droid_Hardware_Camera2_Legacy_CLegacyFocusStateMapper.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyFocusStateMapper.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CarClass(CLegacyFocusStateMapper)
    , public LegacyFocusStateMapper
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CLEGACYFOCUSSTATEMAPPER_H__
