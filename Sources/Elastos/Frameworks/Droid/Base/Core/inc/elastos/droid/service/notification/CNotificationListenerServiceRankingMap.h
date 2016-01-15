#ifndef __ELASTOS_DROID_SERVICE_NOTIFICATION_CNOTIFICATIONLISTENERERVICERANKINGMAP_H__
#define __ELASTOS_DROID_SERVICE_NOTIFICATION_CNOTIFICATIONLISTENERERVICERANKINGMAP_H__

#include "_Elastos_Droid_Service_Notification_CNotificationListenerServiceRankingMap.h"
#include "elastos/droid/service/notification/NotificationListenerService.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

CarClass(CNotificationListenerServiceRankingMap)
    , public NotificationListenerService::RankingMap
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_NOTIFICATION_CNOTIFICATIONLISTENERERVICERANKINGMAP_H__
