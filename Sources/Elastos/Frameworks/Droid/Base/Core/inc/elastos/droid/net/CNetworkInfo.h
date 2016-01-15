
#ifndef __ELASTOS_DROID_NET_CNETWORKINFO_H__
#define __ELASTOS_DROID_NET_CNETWORKINFO_H__

#include "_Elastos_Droid_Net_CNetworkInfo.h"
#include "elastos/droid/net/NetworkInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Describes the status of a network interface.
 * <p>Use {@link ConnectivityManager#getActiveNetworkInfo()} to get an instance that represents
 * the current network connection.
 */
CarClass(CNetworkInfo)
    , public NetworkInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKINFO_H__
