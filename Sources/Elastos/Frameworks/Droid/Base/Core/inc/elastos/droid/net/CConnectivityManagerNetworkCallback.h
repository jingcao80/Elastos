
#ifndef __ELASTOS_DROID_NET_CCONNECTIVITYMANAGERNETWORKCALLBACK_H__
#define __ELASTOS_DROID_NET_CCONNECTIVITYMANAGERNETWORKCALLBACK_H__

#include "_Elastos_Droid_Net_CConnectivityManagerNetworkCallback.h"
#include "elastos/droid/net/CConnectivityManager.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Base class for NetworkRequest callbacks.  Used for notifications about network
 * changes.  Should be extended by applications wanting notifications.
 */
// inner class of ConnectivityManager
CarClass(CConnectivityManagerNetworkCallback)
    , public CConnectivityManager::ConnectivityManagerNetworkCallback
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CCONNECTIVITYMANAGERNETWORKCALLBACK_H__
