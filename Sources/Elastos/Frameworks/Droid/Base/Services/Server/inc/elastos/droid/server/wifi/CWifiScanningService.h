#ifndef __ELASTOS_DROID_SERVER_WIFI_CWIFISCANNINGSERVICE_H__
#define __ELASTOS_DROID_SERVER_WIFI_CWIFISCANNINGSERVICE_H__

#include "_Elastos_Droid_Server_Wifi_CWifiScanningService.h"
#include "elastos/droid/server/wifi/WifiScanningServiceImpl.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

CarClass(CWifiScanningService)
    , public WifiScanningServiceImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_CWIFISCANNINGSERVICE_H__
