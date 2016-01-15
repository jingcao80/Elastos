#ifndef __ELASTOS_DROID_SERVER_NET_CDNSSERVERRESPOSITORY_H__
#define __ELASTOS_DROID_SERVER_NET_CDNSSERVERRESPOSITORY_H__

#include "_Elastos_Droid_Server_Net_CDnsServerRepository.h"
#include "elastos/droid/server/net/DnsServerRepository.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

CarClass(CDnsServerRepository)
    , public DnsServerRepository
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_NET_CDNSSERVERRESPOSITORY_H__