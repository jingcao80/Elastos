
#ifndef __ELASTOS_APPS_PACKAGEINSTALLER_CPACKAGEDELETEOBSERVER_H__
#define __ELASTOS_APPS_PACKAGEINSTALLER_CPACKAGEDELETEOBSERVER_H__

#include "_Elastos_Droid_PackageInstaller_CPackageDeleteObserver.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/packageinstaller/CUninstallAppProgress.h"

namespace Elastos {
namespace Droid {
namespace PackageInstaller {

CarClass(CPackageDeleteObserver)
    , public CUninstallAppProgress::PackageDeleteObserver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace PackageInstaller
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_APPS_PACKAGEINSTALLER_CPACKAGEDELETEOBSERVER_H__

