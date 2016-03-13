#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_CNOTIFICATION_MANAGER_SERVICE_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_CNOTIFICATION_MANAGER_SERVICE_H__

#include "_Elastos_Droid_Server_Notification_CNotificationManagerService.h"
#include "elastos/droid/server/notification/NotificationManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

CarClass(CNotificationManagerService)
    , public NotificationManagerService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_NOTIFICATION_CNOTIFICATION_MANAGER_SERVICE_H__
