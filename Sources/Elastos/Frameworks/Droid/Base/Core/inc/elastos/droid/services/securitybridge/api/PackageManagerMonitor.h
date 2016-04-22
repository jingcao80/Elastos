#ifndef _ELASTOS_DROID_SERVICES_SECURITYBRIDGE_API_PACKAGEMANAGERMONITOR_H__
#define _ELASTOS_DROID_SERVICES_SECURITYBRIDGE_API_PACKAGEMANAGERMONITOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.SecurityBridge.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Services {
namespace SecurityBridge {
namespace Api {

class PackageManagerMonitor
    : public Object
    , public IPackageManagerMonitor
{
public:
    PackageManagerMonitor();

    virtual ~PackageManagerMonitor();

    CARAPI constructor();

    CAR_INTERFACE_DECL()

    CARAPI ApproveAppInstallRequest(
        /* [in] */ const String& apkFilePath,
        /* [in] */ const String& originalAPKFilePath,
        /* [out] */ Boolean* result);
};

} // namespace Api
} // namespace SecurityBridge
} // namespace Services
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_SERVICES_SECURITYBRIDGE_API_PACKAGEMANAGERMONITOR_H__