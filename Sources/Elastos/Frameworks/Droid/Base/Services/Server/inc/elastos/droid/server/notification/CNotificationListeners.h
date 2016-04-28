
#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_CNOTIFICATIONLISTENERS_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_CNOTIFICATIONLISTENERS_H__

#include "_Elastos_Droid_Server_Notification_CNotificationListeners.h"
#include "elastos/droid/server/notification/NotificationManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

CarClass(CNotificationListeners)
    , public NotificationManagerService::NotificationListeners
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_NOTIFICATION_CNOTIFICATIONLISTENERS_H__
