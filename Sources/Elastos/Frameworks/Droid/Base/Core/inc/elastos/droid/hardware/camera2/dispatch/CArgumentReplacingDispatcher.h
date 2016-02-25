
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_CARGUMENTREPLACINGDISPATCHER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_CARGUMENTREPLACINGDISPATCHER_H__

#include "_Elastos_Droid_Hardware_Camera2_Dispatch_CArgumentReplacingDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/ArgumentReplacingDispatcher.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

CarClass(CArgumentReplacingDispatcher)
    , public ArgumentReplacingDispatcher
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_CARGUMENTREPLACINGDISPATCHER_H__
