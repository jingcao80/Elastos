#ifndef __ELASTOS_DROID_CONTENT_PM_CREGISTERED_SERVICES_CACHE_SERVICE_INFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CREGISTERED_SERVICES_CACHE_SERVICE_INFO_H__

#include "_Elastos_Droid_Content_Pm_CRegisteredServicesCacheServiceInfo.h"
#include "elastos/droid/content/pm/RegisteredServicesCache.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CRegisteredServicesCacheServiceInfo)
    , public RegisteredServicesCache::ServiceInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CREGISTERED_SERVICES_CACHE_SERVICE_INFO_H__
