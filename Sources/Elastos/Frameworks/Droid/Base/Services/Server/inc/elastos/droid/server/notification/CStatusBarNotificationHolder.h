#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_CSTATUSBARNOTIFICATIONHOLDER_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_CSTATUSBARNOTIFICATIONHOLDER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_Notification_CStatusBarNotificationHolder.h"
#include "elastos/droid/server/notification/NotificationManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

CarClass(CStatusBarNotificationHolder)
    , public NotificationManagerService::StatusBarNotificationHolder
{
public:
    CAR_OBJECT_DECL();
};

} // Notification
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_NOTIFICATION_CSTATUSBARNOTIFICATIONHOLDER_H__
