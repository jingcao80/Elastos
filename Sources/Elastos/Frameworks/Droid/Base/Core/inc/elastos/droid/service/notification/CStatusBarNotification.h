#ifndef __ELASTOS_DROID_SERVICE_NOTIFICATION_CSTATUSBARNOTIFICATION_H__
#define __ELASTOS_DROID_SERVICE_NOTIFICATION_CSTATUSBARNOTIFICATION_H__

#include "_Elastos_Droid_Service_Notification_CStatusBarNotification.h"
#include "elastos/droid/service/notification/StatusBarNotification.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

CarClass(CStatusBarNotification)
    , public StatusBarNotification
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_NOTIFICATION_CSTATUSBARNOTIFICATION_H__
