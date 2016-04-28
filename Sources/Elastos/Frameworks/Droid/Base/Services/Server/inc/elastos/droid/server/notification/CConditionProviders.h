
#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_CCONDITIONPROVIDERS_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_CCONDITIONPROVIDERS_H__

#include "_Elastos_Droid_Server_Notification_CConditionProviders.h"
#include "elastos/droid/server/notification/ConditionProviders.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

CarClass(CConditionProviders)
    , public ConditionProviders
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_NOTIFICATION_CCONDITIONPROVIDERS_H__
