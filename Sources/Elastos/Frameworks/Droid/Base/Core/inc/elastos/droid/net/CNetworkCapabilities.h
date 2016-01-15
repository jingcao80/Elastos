
#ifndef __ELASTOS_DROID_NET_CNETWORKCAPABILITIES_H__
#define __ELASTOS_DROID_NET_CNETWORKCAPABILITIES_H__

#include "_Elastos_Droid_Net_CNetworkCapabilities.h"
#include "elastos/droid/net/NetworkCapabilities.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * This class represents the capabilities of a network.  This is used both to specify
 * needs to {@link ConnectivityManager} and when inspecting a network.
 *
 * Note that this replaces the old {@link ConnectivityManager#TYPE_MOBILE} method
 * of network selection.  Rather than indicate a need for Wi-Fi because an application
 * needs high bandwidth and risk obselence when a new, fast network appears (like LTE),
 * the application should specify it needs high bandwidth.  Similarly if an application
 * needs an unmetered network for a bulk transfer it can specify that rather than assuming
 * all cellular based connections are metered and all Wi-Fi based connections are not.
 */
CarClass(CNetworkCapabilities)
    , public NetworkCapabilities
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKCAPABILITIES_H__
