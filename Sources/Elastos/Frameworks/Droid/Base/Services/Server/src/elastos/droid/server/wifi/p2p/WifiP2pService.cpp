#include "Elastos.Droid.Content.h"
#include "elastos/droid/server/wifi/p2p/WifiP2pService.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {
namespace P2p {

//=====================================================================
//                            WifiP2pService
//=====================================================================
const String WifiP2pService::TAG("WifiP2pService");

WifiP2pService::WifiP2pService(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);
    mImpl = new WifiP2pServiceImpl();
    mImpl->constructor(context);
}

ECode WifiP2pService::OnStart()
{
    Logger::I(TAG, "Registering %s", IContext::WIFI_P2P_SERVICE.string());
    PublishBinderService(IContext::WIFI_P2P_SERVICE, mImpl);
    return NOERROR;
}

ECode WifiP2pService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == ISystemService::PHASE_SYSTEM_SERVICES_READY) {
        mImpl->ConnectivityServiceReady();
    }
    return NOERROR;
}

} // namespace P2p
} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
