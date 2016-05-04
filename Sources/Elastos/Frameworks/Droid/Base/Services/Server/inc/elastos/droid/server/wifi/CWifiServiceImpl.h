#ifndef __ELASTOS_DROID_SERVER_WIFI_CWIFISERVICEIMPL_H__
#define __ELASTOS_DROID_SERVER_WIFI_CWIFISERVICEIMPL_H__

#include "_Elastos_Droid_Server_Wifi_CWifiServiceImpl.h"
#include "elastos/droid/server/wifi/WifiServiceImpl.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

CarClass(CWifiServiceImpl)
    , public WifiServiceImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_CWIFISERVICEIMPL_H__
