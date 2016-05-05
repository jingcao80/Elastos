
#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_CINOTIFICATIONLISTENERWRAPPER_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_CINOTIFICATIONLISTENERWRAPPER_H__

#include "_Elastos_Droid_Service_Notification_CINotificationListenerWrapper.h"
#include "elastos/droid/service/notification/NotificationListenerService.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

CarClass(CINotificationListenerWrapper)
    , public NotificationListenerService::INotificationListenerWrapper
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_NOTIFICATION_CINOTIFICATIONLISTENERWRAPPER_H__
