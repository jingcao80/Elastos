
#include "elastos/droid/server/wifi/WifiService.h"
#include "elastos/droid/server/wifi/CWifiServiceImpl.h"
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

WifiService::WifiService()
{
}

ECode WifiService::constructor(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);
    AutoPtr<IIWifiManager> wifiManager;
    CWifiServiceImpl::New(context, (IIWifiManager**)&wifiManager);
    mImpl = (WifiServiceImpl*)wifiManager.Get();
    return NOERROR;
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
