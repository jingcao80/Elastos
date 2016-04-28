
#include "elastos/droid/server/notification/CConditionProviders.h"
#include "elastos/droid/server/notification/CNotificationManagerBinderService.h"
#include "elastos/droid/server/notification/CNotificationManagerService.h"
#include "elastos/droid/server/notification/CStatusBarNotificationHolder.h"
#include "elastos/droid/server/notification/CNotificationListeners.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

CAR_OBJECT_IMPL(CConditionProviders)
CAR_OBJECT_IMPL(CNotificationManagerBinderService);
CAR_OBJECT_IMPL(CNotificationManagerService);
CAR_OBJECT_IMPL(CNotificationListeners)
CAR_OBJECT_IMPL(CStatusBarNotificationHolder);

} // Notification
} // Server
} // Droid
} // Elastos
