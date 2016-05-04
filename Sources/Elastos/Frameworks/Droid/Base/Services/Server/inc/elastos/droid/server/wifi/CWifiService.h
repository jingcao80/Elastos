#ifndef __ELASTOS_DROID_SERVER_WIFI_CWIFISERVICE_H__
#define __ELASTOS_DROID_SERVER_WIFI_CWIFISERVICE_H__

#include "_Elastos_Droid_Server_Wifi_CWifiService.h"
#include "elastos/droid/server/wifi/WifiService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

CarClass(CWifiService)
    , public WifiService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_CWIFISERVICE_H__
