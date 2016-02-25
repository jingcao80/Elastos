
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_CMETHODNAMEINVOKER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_CMETHODNAMEINVOKER_H__

#include "_Elastos_Droid_Hardware_Camera2_Dispatch_CMethodNameInvoker.h"
#include "elastos/droid/hardware/camera2/dispatch/MethodNameInvoker.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

CarClass(CMethodNameInvoker)
    , public MethodNameInvoker
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_CMETHODNAMEINVOKER_H__
