
#include "elastos/droid/server/wifi/WifiScanningService.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

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
    SystemService::constructor(context);
    Logger::I(TAG, "Creating %s", IContext::WIFI_SCANNING_SERVICE.string());
}

ECode WifiScanningService::OnStart()
{
    mImpl = new WifiScanningServiceImpl();
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    mImpl->constructor(context);

    Logger::I(TAG, "Starting %s", IContext::WIFI_SCANNING_SERVICE.string());
    PublishBinderService(IContext::WIFI_SCANNING_SERVICE, mImpl);
    return NOERROR;
}

ECode WifiScanningService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == ISystemService::PHASE_SYSTEM_SERVICES_READY) {
        Logger::I(TAG, "Registering %s", IContext::WIFI_SCANNING_SERVICE.string());
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        if (mImpl == NULL) {
            mImpl = new WifiScanningServiceImpl();
            mImpl->constructor(context);
        }
        mImpl->StartService(context);
    }
    return NOERROR;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
