
#include "elastos/droid/server/wifi/WifiService.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//                             WifiService
//=====================================================================
const String WifiService::TAG("WifiService");

WifiService::WifiService(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);
    mImpl = new WifiServiceImpl();
    mImpl->constructor(context);
}

ECode WifiService::OnStart()
{
    Logger::I(TAG, "Registering %s", IContext::WIFI_SERVICE.string());
    PublishBinderService(IContext::WIFI_SERVICE, mImpl);
    return NOERROR;
}

ECode WifiService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == ISystemService::PHASE_SYSTEM_SERVICES_READY) {
         mImpl->CheckAndStartWifi();
    }
    return NOERROR;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
