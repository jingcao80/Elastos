
#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_CNOTIFICATIONINTRUSIVENESSEXTRACTOR_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_CNOTIFICATIONINTRUSIVENESSEXTRACTOR_H__

#include "_Elastos_Droid_Server_Notification_CNotificationIntrusivenessExtractor.h"
#include "elastos/droid/server/notification/NotificationIntrusivenessExtractor.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

/**
 * This {@link com.android.server.notification.NotificationSignalExtractor} noticies noisy
 * notifications and marks them to get a temporary ranking bump.
 */
CarClass(CNotificationIntrusivenessExtractor)
    , public NotificationIntrusivenessExtractor
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_NOTIFICATION_CNOTIFICATIONINTRUSIVENESSEXTRACTOR_H__
