
#ifndef __ELASTOS_DROID_NET_CLOCALSOCKETADDRESS_H__
#define __ELASTOS_DROID_NET_CLOCALSOCKETADDRESS_H__

#include "_Elastos_Droid_Net_CLocalSocketAddress.h"
#include "elastos/droid/net/LocalSocketAddress.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A UNIX-domain (AF_LOCAL) socket address. For use with
 * android.net.LocalSocket and android.net.LocalServerSocket.
 *
 * On the Android system, these names refer to names in the Linux
 * abstract (non-filesystem) UNIX domain namespace.
 */
CarClass(CLocalSocketAddress)
    , public LocalSocketAddress
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CLOCALSOCKETADDRESS_H__
