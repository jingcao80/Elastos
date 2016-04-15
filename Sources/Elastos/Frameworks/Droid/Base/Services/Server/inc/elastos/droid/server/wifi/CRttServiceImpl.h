#ifndef __ELASTOS_DROID_SERVER_WIFI_CRTTSERVICEIMPL_H__
#define __ELASTOS_DROID_SERVER_WIFI_CRTTSERVICEIMPL_H__

#include "_Elastos_Droid_Server_Wifi_CRttServiceImpl.h"
#include "elastos/droid/server/wifi/RttService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

CarClass(CRttServiceImpl)
    , public RttService::RttServiceImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_CRTTSERVICEIMPL_H__
