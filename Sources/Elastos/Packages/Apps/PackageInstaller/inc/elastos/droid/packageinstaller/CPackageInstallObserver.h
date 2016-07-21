
#ifndef __ELASTOS_APPS_PACKAGEINSTALLER_CPACKAGEINSTALLOBSERVER_H__
#define __ELASTOS_APPS_PACKAGEINSTALLER_CPACKAGEINSTALLOBSERVER_H__

#include "_Elastos_Droid_PackageInstaller_CPackageInstallObserver.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/packageinstaller/CInstallAppProgress.h"

namespace Elastos {
namespace Droid {
namespace PackageInstaller {

CarClass(CPackageInstallObserver)
    , public CInstallAppProgress::PackageInstallObserver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace PackageInstaller
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_APPS_PACKAGEINSTALLER_CPACKAGEINSTALLOBSERVER_H__

