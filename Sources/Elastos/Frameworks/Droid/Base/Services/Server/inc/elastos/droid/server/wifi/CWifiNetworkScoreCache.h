#ifndef __ELASTOS_DROID_SERVER_WIFI_CWIFINETWORKSCORECACHE_H__
#define __ELASTOS_DROID_SERVER_WIFI_CWIFINETWORKSCORECACHE_H__

#include "_Elastos_Droid_Server_Wifi_CWifiNetworkScoreCache.h"
#include "elastos/droid/server/wifi/WifiNetworkScoreCache.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

CarClass(CWifiNetworkScoreCache)
    , public WifiNetworkScoreCache
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_CWIFINETWORKSCORECACHE_H__
