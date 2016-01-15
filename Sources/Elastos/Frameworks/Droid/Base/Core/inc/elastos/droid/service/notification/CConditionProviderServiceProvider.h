#ifndef __ELASTOS_DROID_SERVICE_NOTIFICATION_CCONDITIONPROVIDERSERVICEPROVIDER_H__
#define __ELASTOS_DROID_SERVICE_NOTIFICATION_CCONDITIONPROVIDERSERVICEPROVIDER_H__

#include "_Elastos_Droid_Service_Notification_CConditionProviderServiceProvider.h"
#include "elastos/droid/service/notification/ConditionProviderService.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

CarClass(CConditionProviderServiceProvider)
    , public ConditionProviderService::Provider
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_NOTIFICATION_CCONDITIONPROVIDERSERVICEPROVIDER_H__
