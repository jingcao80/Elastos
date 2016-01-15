#ifndef __ELASTOS_DROID_SERVICE_NOTIFICATION_CNOTIFICATIONRANKINGUPDATE_H__
#define __ELASTOS_DROID_SERVICE_NOTIFICATION_CNOTIFICATIONRANKINGUPDATE_H__

#include "_Elastos_Droid_Service_Notification_CNotificationRankingUpdate.h"
#include "elastos/droid/service/notification/NotificationRankingUpdate.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

CarClass(CNotificationRankingUpdate)
    , public NotificationRankingUpdate
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_NOTIFICATION_CNOTIFICATIONRANKINGUPDATE_H__
