
#ifndef __ELASTOS_DROID_SERVER_PM_CLAUNCHER_APPS_SERVICE_H__
#define __ELASTOS_DROID_SERVER_PM_CLAUNCHER_APPS_SERVICE_H__

#include "_Elastos_Droid_Server_Pm_CLauncherAppsService.h"
#include "elastos/droid/server/pm/LauncherAppsService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

CarClass(CLauncherAppsService)
    , public LauncherAppsService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_CLAUNCHER_APPS_SERVICE_H__
