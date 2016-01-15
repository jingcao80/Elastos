
#ifndef __ELASTOS_DROID_NET_CROUTEINFO_H__
#define __ELASTOS_DROID_NET_CROUTEINFO_H__

#include "_Elastos_Droid_Net_CRouteInfo.h"
#include "elastos/droid/net/RouteInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Represents a network route.
 * <p>
 * This is used both to describe static network configuration and live network
 * configuration information.
 *
 * A route contains three pieces of information:
 * <ul>
 * <li>a destination {@link IpPrefix} specifying the network destinations covered by this route.
 *     If this is {@code null} it indicates a default route of the address family (IPv4 or IPv6)
 *     implied by the gateway IP address.
 * <li>a gateway {@link InetAddress} indicating the next hop to use.  If this is {@code null} it
 *     indicates a directly-connected route.
 * <li>an interface (which may be unspecified).
 * </ul>
 * Either the destination or the gateway may be {@code null}, but not both.  If the
 * destination and gateway are both specified, they must be of the same address family
 * (IPv4 or IPv6).
 */
CarClass(CRouteInfo)
    , public RouteInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CROUTEINFO_H__
