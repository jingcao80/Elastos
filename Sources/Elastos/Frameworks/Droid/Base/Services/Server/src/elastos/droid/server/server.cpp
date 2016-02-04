
#include <elastos/droid/server/CAlarmManagerBinderService.h>
#include <elastos/droid/server/CBatteryBinderService.h>
#include <elastos/droid/server/CBatteryServiceBatteryListener.h>

#include <elastos/droid/server/CBluetoothManagerServiceBluetoothCallback.h>
#include <elastos/droid/server/CCommonTimeManagementServiceIfaceObserver.h>
#include <elastos/droid/server/CPersistentDataBlockBinderService.h>
#include <elastos/droid/server/CUiModeManagerBinderService.h>

namespace Elastos {
namespace Droid {
namespace Server {

CAR_OBJECT_IMPL(CAlarmManagerBinderService)
CAR_OBJECT_IMPL(CBatteryBinderService)
CAR_OBJECT_IMPL(CBatteryServiceBatteryListener)
CAR_OBJECT_IMPL(CBluetoothManagerServiceBluetoothCallback)
CAR_OBJECT_IMPL(CCommonTimeManagementServiceIfaceObserver)
CAR_OBJECT_IMPL(CPersistentDataBlockBinderService)
CAR_OBJECT_IMPL(CUiModeManagerBinderService)

} // namespace Server
} // namepsace Droid
} // namespace Elastos
