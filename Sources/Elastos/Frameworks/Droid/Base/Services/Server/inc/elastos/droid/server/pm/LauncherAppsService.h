#ifndef __ELASTOS_DROID_SERVER_PM_LAUNCHERAPPSSERVICE_H__
#define __ELASTOS_DROID_SERVER_PM_LAUNCHERAPPSSERVICE_H__

#include "elastos/droid/server/SystemService.h"

using Elastos::Droid::Server::SystemService;
using Elastos::Droid::Content::Pm::IILauncherApps;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

/**
 * Service that manages requests and callbacks for launchers that support
 * managed profiles.
 */

class LauncherAppsService : public SystemService
{
public:
    LauncherAppsService(
        /* [in] */ IContext* context);

    CARAPI OnStart();

private:
    AutoPtr<IILauncherApps> mLauncherAppsImpl;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_LAUNCHERAPPSSERVICE_H__
