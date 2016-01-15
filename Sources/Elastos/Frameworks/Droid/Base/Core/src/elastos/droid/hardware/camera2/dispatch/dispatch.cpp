
#include "elastos/droid/hardware/camera2/dispatch/CNullDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/CInvokeDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/CHandlerDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/CArgumentReplacingDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/CDuckTypingDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/CBroadcastDispatcher.h"
#include "elastos/droid/hardware/camera2/dispatch/CMethodNameInvoker.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

CAR_OBJECT_IMPL(CNullDispatcher)
CAR_OBJECT_IMPL(CInvokeDispatcher)
CAR_OBJECT_IMPL(CHandlerDispatcher)
CAR_OBJECT_IMPL(CArgumentReplacingDispatcher)
CAR_OBJECT_IMPL(CDuckTypingDispatcher)
CAR_OBJECT_IMPL(CBroadcastDispatcher)
CAR_OBJECT_IMPL(CMethodNameInvoker)

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
