#include "elastos/droid/server/am/CActivityContainer.h"
#include "elastos/droid/server/am/CBatteryStatsService.h"
#include "elastos/droid/server/am/CVirtualActivityContainer.h"
#include "elastos/droid/server/am/CActivityManagerBootCompletedReceiver.h"
#include "elastos/droid/server/am/CActivityManagerSwitchUserReceiver.h"
#include "elastos/droid/server/am/CActivityManagerNeedStartReceiver.h"
#include "elastos/droid/server/am/CActivityManagerPreBootCompletedReceiver.h"
#include "elastos/droid/server/am/CActivityManagerSystemReadyReceiver.h"
#include "elastos/droid/server/am/CActivityManagerShutdownReceiver.h"
#include "elastos/droid/server/am/CActivityManagerStoppingReceiver.h"
#include "elastos/droid/server/am/CActivityManagerDispatchUserSwitchCallback.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_OBJECT_IMPL(CBatteryStatsService)
CAR_OBJECT_IMPL(CActivityContainer)
CAR_OBJECT_IMPL(CVirtualActivityContainer)
CAR_OBJECT_IMPL(CActivityManagerBootCompletedReceiver)
CAR_OBJECT_IMPL(CActivityManagerSwitchUserReceiver)
CAR_OBJECT_IMPL(CActivityManagerNeedStartReceiver)
CAR_OBJECT_IMPL(CActivityManagerPreBootCompletedReceiver)
CAR_OBJECT_IMPL(CActivityManagerSystemReadyReceiver)
CAR_OBJECT_IMPL(CActivityManagerShutdownReceiver)
CAR_OBJECT_IMPL(CActivityManagerStoppingReceiver)
CAR_OBJECT_IMPL(CActivityManagerDispatchUserSwitchCallback)


} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
