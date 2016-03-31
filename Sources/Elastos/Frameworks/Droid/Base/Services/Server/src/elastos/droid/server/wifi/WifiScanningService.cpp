
#include "elastos/droid/server/wifi/WifiScanningService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//                         WifiScanningService
//=====================================================================
const String WifiScanningService::TAG("WifiScanningService");

WifiScanningService::WifiScanningService(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // super(context);
    // Log.i(TAG, "Creating " + Context.WIFI_SCANNING_SERVICE);
}

ECode WifiScanningService::OnStart()
{
    // ==================before translated======================
    // mImpl = new WifiScanningServiceImpl(getContext());
    //
    // Log.i(TAG, "Starting " + Context.WIFI_SCANNING_SERVICE);
    // publishBinderService(Context.WIFI_SCANNING_SERVICE, mImpl);
    assert(0);
    return NOERROR;
}

ECode WifiScanningService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    // ==================before translated======================
    // if (phase == SystemService.PHASE_SYSTEM_SERVICES_READY) {
    //     Log.i(TAG, "Registering " + Context.WIFI_SCANNING_SERVICE);
    //     if (mImpl == null) {
    //         mImpl = new WifiScanningServiceImpl(getContext());
    //     }
    //     mImpl.startService(getContext());
    // }
    assert(0);
    return NOERROR;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos


