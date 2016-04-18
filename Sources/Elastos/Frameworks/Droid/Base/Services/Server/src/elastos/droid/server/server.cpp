#include <elastos/droid/server/CAlarmManagerBinderService.h>
#include <elastos/droid/server/CBatteryBinderService.h>
#include <elastos/droid/server/CBluetoothManagerServiceBluetoothCallback.h>
#include <elastos/droid/server/CCommonTimeManagementServiceIfaceObserver.h>
#include <elastos/droid/server/CEntropyMixer.h>
#include <elastos/droid/server/CPersistentDataBlockService.h>
#include <elastos/droid/server/CPersistentDataBlockBinderService.h>
#include <elastos/droid/server/CProfileManagerService.h>
#include <elastos/droid/server/CUiModeManagerBinderService.h>
#include <elastos/droid/server/CUiModeManagerService.h>

namespace Elastos {
namespace Droid {
namespace Server {

CAR_OBJECT_IMPL(CAlarmManagerBinderService)
CAR_OBJECT_IMPL(CBatteryBinderService)
CAR_OBJECT_IMPL(CBluetoothManagerServiceBluetoothCallback)
CAR_OBJECT_IMPL(CCommonTimeManagementServiceIfaceObserver)
CAR_OBJECT_IMPL(CEntropyMixer)
CAR_OBJECT_IMPL(CPersistentDataBlockService)
CAR_OBJECT_IMPL(CPersistentDataBlockBinderService)
CAR_OBJECT_IMPL(CUiModeManagerService)
CAR_OBJECT_IMPL(CUiModeManagerBinderService)
CAR_OBJECT_IMPL(CProfileManagerService)

} // namespace Server
} // namepsace Droid
} // namespace Elastos
