
#ifndef __ELASTOSX_NET_SSL_ELASTOS_NET_SSL_CHANDSHAKECOMPLETEDEVENT_H__
#define __ELASTOSX_NET_SSL_ELASTOS_NET_SSL_CHANDSHAKECOMPLETEDEVENT_H__

#include "_Elastosx_Net_Ssl_CHandshakeCompletedEvent.h"
#include "HandshakeCompletedEvent.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CarClass(CHandshakeCompletedEvent)
    , public HandshakeCompletedEvent
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_ELASTOS_NET_SSL_CHANDSHAKECOMPLETEDEVENT_H__
