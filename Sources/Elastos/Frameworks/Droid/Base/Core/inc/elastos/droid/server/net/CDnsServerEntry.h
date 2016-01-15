#ifndef __ELASTOS_DROID_SERVER_NET_CDNSSERVERENTRY_H__
#define __ELASTOS_DROID_SERVER_NET_CDNSSERVERENTRY_H__

#include "_Elastos_Droid_Server_Net_CDnsServerEntry.h"
#include "elastos/droid/server/net/DnsServerEntry.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

CarClass(CDnsServerEntry)
    , public DnsServerEntry
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_NET_CDNSSERVERENTRY_H__