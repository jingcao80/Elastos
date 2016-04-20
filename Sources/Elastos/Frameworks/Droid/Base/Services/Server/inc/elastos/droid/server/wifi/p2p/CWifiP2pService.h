#ifndef __ELASTOS_DROID_SERVER_WIFI_P2P_CWIFIP2PSERVICE_H__
#define __ELASTOS_DROID_SERVER_WIFI_P2P_CWIFIP2PSERVICE_H__

#include "_Elastos_Droid_Server_Wifi_P2p_CWifiP2pService.h"
#include "elastos/droid/server/wifi/p2p/WifiP2pServiceImpl.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {
namespace P2p {

CarClass(CWifiP2pService)
    , public WifiP2pServiceImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace P2p
} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_P2P_CWIFIP2PSERVICE_H__
