
#ifndef __ELASTOS_DROID_NET_CLOCALSOCKET_H__
#define __ELASTOS_DROID_NET_CLOCALSOCKET_H__

#include "_Elastos_Droid_Net_CLocalSocket.h"
#include "elastos/droid/net/LocalSocket.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Creates a (non-server) socket in the UNIX-domain namespace. The interface
 * here is not entirely unlike that of java.net.Socket
 */
CarClass(CLocalSocket)
    , public LocalSocket
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CLOCALSOCKET_H__
