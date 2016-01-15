
#ifndef __ELASTOS_DROID_NET_CNETWORK_H__
#define __ELASTOS_DROID_NET_CNETWORK_H__

#include "_Elastos_Droid_Net_CNetwork.h"
#include "elastos/droid/net/Network.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Identifies a {@code Network}.  This is supplied to applications via
 * {@link ConnectivityManager.NetworkCallback} in response to the active
 * {@link ConnectivityManager#requestNetwork} or passive
 * {@link ConnectivityManager#registerNetworkCallback} calls.
 * It is used to direct traffic to the given {@code Network}, either on a {@link Socket} basis
 * through a targeted {@link SocketFactory} or process-wide via
 * {@link ConnectivityManager#setProcessDefaultNetwork}.
 */
CarClass(CNetwork)
    , public Network
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORK_H__
