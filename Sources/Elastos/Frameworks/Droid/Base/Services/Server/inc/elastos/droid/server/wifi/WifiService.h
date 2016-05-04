#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFISERVICE_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFISERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/server/wifi/WifiServiceImpl.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Server::SystemService;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

class WifiService
    : public SystemService
{
public:
    WifiService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnBootPhase(
        /* [in] */ Int32 phase);

public:
    AutoPtr<WifiServiceImpl> mImpl;

private:
    static const String TAG;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFISERVICE_H__

