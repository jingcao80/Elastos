
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_CBROADCASTDISPATCHER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_CBROADCASTDISPATCHER_H__

#include "_Elastos_Droid_Hardware_Camera2_Dispatch_CBroadcastDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/BroadcastDispatcher.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

CarClass(CBroadcastDispatcher)
    , public BroadcastDispatcher
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_CBROADCASTDISPATCHER_H__
