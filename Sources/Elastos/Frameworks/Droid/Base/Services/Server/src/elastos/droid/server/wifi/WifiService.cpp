
#include "elastos/droid/server/wifi/WifiService.h"

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
    // ==================before translated======================
    // super(context);
    // mImpl = new WifiServiceImpl(context);
}

ECode WifiService::OnStart()
{
    // ==================before translated======================
    // Log.i(TAG, "Registering " + Context.WIFI_SERVICE);
    // publishBinderService(Context.WIFI_SERVICE, mImpl);
    assert(0);
    return NOERROR;
}

ECode WifiService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    // ==================before translated======================
    // if (phase == SystemService.PHASE_SYSTEM_SERVICES_READY) {
    //     mImpl.checkAndStartWifi();
    // }
    assert(0);
    return NOERROR;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos


