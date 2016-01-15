#ifndef __ELASTOS_DROID_SERVICE_NOTIFICATION_CZENMODECONFIG_H__
#define __ELASTOS_DROID_SERVICE_NOTIFICATION_CZENMODECONFIG_H__

#include "_Elastos_Droid_Service_Notification_CZenModeConfig.h"
#include "elastos/droid/service/notification/ZenModeConfig.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

CarClass(CZenModeConfig)
    , public ZenModeConfig
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_NOTIFICATION_CZENMODECONFIG_H__
