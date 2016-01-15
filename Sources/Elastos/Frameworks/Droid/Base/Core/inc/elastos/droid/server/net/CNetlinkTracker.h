#ifndef __ELASTOS_DROID_SERVER_NET_CNETLINKTRACKER_H__
#define __ELASTOS_DROID_SERVER_NET_CNETLINKTRACKER_H__

#include "_Elastos_Droid_Server_Net_CNetlinkTracker.h"
#include "elastos/droid/server/net/NetlinkTracker.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

CarClass(CNetlinkTracker)
    , public NetlinkTracker
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_NET_CNETLINKTRACKER_H__