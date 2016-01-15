#ifndef __ELASTOS_DROID_SERVER_NET_CBASENETWORKOBSERVER_H__
#define __ELASTOS_DROID_SERVER_NET_CBASENETWORKOBSERVER_H__

#include "_Elastos_Droid_Server_Net_CBaseNetworkObserver.h"
#include "elastos/droid/server/net/BaseNetworkObserver.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

CarClass(CBaseNetworkObserver)
    , public BaseNetworkObserver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_NET_CBASENETWORKOBSERVER_H__