
#ifndef __ELASTOS_DROID_NET_CLOCALSERVERSOCKET_H__
#define __ELASTOS_DROID_NET_CLOCALSERVERSOCKET_H__

#include "_Elastos_Droid_Net_CLocalServerSocket.h"
#include "elastos/droid/net/LocalServerSocket.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * non-standard class for creating inbound UNIX-domain socket
 * on the Android platform, this is created in the Linux non-filesystem
 * namespace.
 *
 * On simulator platforms, this may be created in a temporary directory on
 * the filesystem
 */
CarClass(CLocalServerSocket)
    , public LocalServerSocket
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CLOCALSERVERSOCKET_H__
