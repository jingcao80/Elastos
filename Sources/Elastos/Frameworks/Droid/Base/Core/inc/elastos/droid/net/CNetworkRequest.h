
#ifndef __ELASTOS_DROID_NET_CNETWORKREQUEST_H__
#define __ELASTOS_DROID_NET_CNETWORKREQUEST_H__

#include "_Elastos_Droid_Net_CNetworkRequest.h"
#include "elastos/droid/net/NetworkRequest.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Defines a request for a network, made through {@link NetworkRequest.Builder} and used
 * to request a network via {@link ConnectivityManager#requestNetwork} or listen for changes
 * via {@link ConnectivityManager#registerNetworkCallback}.
 */
CarClass(CNetworkRequest)
    , public NetworkRequest
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKREQUEST_H__
