#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFISCANNINGSERVICE_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFISCANNINGSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/server/wifi/WifiScanningServiceImpl.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Server::SystemService;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

class WifiScanningService
    : public SystemService
{
public:
    WifiScanningService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnBootPhase(
        /* [in] */ Int32 phase);

public:
    AutoPtr<WifiScanningServiceImpl> mImpl;

private:
    static const String TAG;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFISCANNINGSERVICE_H__
