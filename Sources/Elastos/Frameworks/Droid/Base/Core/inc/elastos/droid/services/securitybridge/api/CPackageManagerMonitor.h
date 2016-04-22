#ifndef _ELASTOS_DROID_SERVICES_SECURITYBRIDGE_API_CPACKAGEMANAGERMONITOR_H__
#define _ELASTOS_DROID_SERVICES_SECURITYBRIDGE_API_CPACKAGEMANAGERMONITOR_H__

#include "_Elastos_Droid_Services_SecurityBridge_Api_CPackageManagerMonitor.h"
#include "elastos/droid/services/securitybridge/api/PackageManagerMonitor.h"

namespace Elastos {
namespace Droid {
namespace Services {
namespace SecurityBridge {
namespace Api {

CarClass(CPackageManagerMonitor), public PackageManagerMonitor
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Api
} // namespace SecurityBridge
} // namespace Services
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_SERVICES_SECURITYBRIDGE_API_CPACKAGEMANAGERMONITOR_H__