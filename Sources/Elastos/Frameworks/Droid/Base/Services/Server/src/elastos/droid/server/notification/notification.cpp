
#include "elastos/droid/server/notification/CConditionProviders.h"
#include "elastos/droid/server/notification/CNotificationIntrusivenessExtractor.h"
#include "elastos/droid/server/notification/CNotificationListeners.h"
#include "elastos/droid/server/notification/CNotificationManagerBinderService.h"
#include "elastos/droid/server/notification/CNotificationManagerService.h"
#include "elastos/droid/server/notification/CPackagePriorityExtractor.h"
#include "elastos/droid/server/notification/CPackageVisibilityExtractor.h"
#include "elastos/droid/server/notification/CStatusBarNotificationHolder.h"
#include "elastos/droid/server/notification/CValidateNotificationPeople.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

CAR_OBJECT_IMPL(CConditionProviders)
CAR_OBJECT_IMPL(CNotificationIntrusivenessExtractor)
CAR_OBJECT_IMPL(CNotificationListeners)
CAR_OBJECT_IMPL(CNotificationManagerBinderService);
CAR_OBJECT_IMPL(CNotificationManagerService);
CAR_OBJECT_IMPL(CPackagePriorityExtractor)
CAR_OBJECT_IMPL(CPackageVisibilityExtractor)
CAR_OBJECT_IMPL(CStatusBarNotificationHolder);
CAR_OBJECT_IMPL(CValidateNotificationPeople)

} // Notification
} // Server
} // Droid
} // Elastos
