#ifndef __ELASTOS_DROID_SERVICE_NOTIFICATION_CCONDITION_H__
#define __ELASTOS_DROID_SERVICE_NOTIFICATION_CCONDITION_H__

#include "_Elastos_Droid_Service_Notification_CCondition.h"
#include "elastos/droid/service/notification/Condition.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

CarClass(CCondition)
    , public Condition
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_NOTIFICATION_CONDITION_H__
