
#ifndef __ELASTOS_DROID_APP_CAPP_OPS_MANAGER_PACKAGE_OPS_H__
#define __ELASTOS_DROID_APP_CAPP_OPS_MANAGER_PACKAGE_OPS_H__

#include "_Elastos_Droid_App_CAppOpsManagerPackageOps.h"
#include "elastos/droid/app/AppOpsManager.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CAppOpsManagerPackageOps)
    , public AppOpsManager::PackageOps
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CAPP_OPS_MANAGER_PACKAGE_OPS_H__