#include "elastos/droid/services/securitybridge/api/PackageManagerMonitor.h"

namespace Elastos {
namespace Droid {
namespace Services {
namespace SecurityBridge {
namespace Api {

PackageManagerMonitor::PackageManagerMonitor()
{}

PackageManagerMonitor::~PackageManagerMonitor()
{}

ECode PackageManagerMonitor::constructor()
{
    return NOERROR;
}

CAR_INTERFACE_IMPL(PackageManagerMonitor, Object, IPackageManagerMonitor)

ECode PackageManagerMonitor::ApproveAppInstallRequest(
    /* [in] */ const String& apkFilePath,
    /* [in] */ const String& originalAPKFilePath,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

} // namespace Api
} // namespace SecurityBridge
} // namespace Services
} // namespace Droid
} // namespace Elastos