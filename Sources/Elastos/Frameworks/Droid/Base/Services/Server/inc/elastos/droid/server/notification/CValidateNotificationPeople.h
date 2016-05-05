
#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_CVALIDATENOTIFICATIONPEOPLE_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_CVALIDATENOTIFICATIONPEOPLE_H__

#include "_Elastos_Droid_Server_Notification_CValidateNotificationPeople.h"
#include "elastos/droid/server/notification/ValidateNotificationPeople.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

/**
 * This {@link NotificationSignalExtractor} attempts to validate
 * people references. Also elevates the priority of real people.
 *
 * {@hide}
 */
CarClass(CValidateNotificationPeople)
    , public ValidateNotificationPeople
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_NOTIFICATION_CVALIDATENOTIFICATIONPEOPLE_H__
