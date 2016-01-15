
#include "pm/LauncherAppsService.h"
#include "pm/CLauncherAppsImpl.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

LauncherAppsService::LauncherAppsService(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);
    CLauncherAppsImpl::New(context, (IILauncherApps**)&mLauncherAppsImpl);
}

ECode LauncherAppsService::OnStart()
{
    PublishBinderService(IContext::LAUNCHER_APPS_SERVICE, IBinder::Probe(mLauncherAppsImpl));
    return NOERROR;
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
