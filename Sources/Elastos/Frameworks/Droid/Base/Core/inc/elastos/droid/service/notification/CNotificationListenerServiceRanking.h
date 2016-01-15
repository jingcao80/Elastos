#ifndef __ELASTOS_DROID_SERVICE_NOTIFICATION_CNOTIFICATIONLISTENERERVICERANKING_H__
#define __ELASTOS_DROID_SERVICE_NOTIFICATION_CNOTIFICATIONLISTENERERVICERANKING_H__

#include "_Elastos_Droid_Service_Notification_CNotificationListenerServiceRanking.h"
#include "elastos/droid/service/notification/NotificationListenerService.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

CarClass(CNotificationListenerServiceRanking)
    , public NotificationListenerService::Ranking
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_NOTIFICATION_CNOTIFICATIONLISTENERERVICERANKING_H__
