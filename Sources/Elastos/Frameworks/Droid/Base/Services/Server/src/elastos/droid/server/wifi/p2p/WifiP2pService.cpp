
#include "elastos/droid/server/wifi/p2p/WifiP2pService.h"

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
    // ==================before translated======================
    // super(context);
    // mImpl = new WifiP2pServiceImpl(context);
}

ECode WifiP2pService::OnStart()
{
    // ==================before translated======================
    // Log.i(TAG, "Registering " + Context.WIFI_P2P_SERVICE);
    // publishBinderService(Context.WIFI_P2P_SERVICE, mImpl);
    assert(0);
    return NOERROR;
}

ECode WifiP2pService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    // ==================before translated======================
    // if (phase == SystemService.PHASE_SYSTEM_SERVICES_READY) {
    //     mImpl.connectivityServiceReady();
    // }
    assert(0);
    return NOERROR;
}

} // namespace P2p
} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos


