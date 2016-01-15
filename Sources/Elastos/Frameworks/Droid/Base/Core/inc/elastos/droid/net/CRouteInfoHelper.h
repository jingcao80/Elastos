
#ifndef __ELASTOS_DROID_NET_CROUTEINFOHELPER_H__
#define __ELASTOS_DROID_NET_CROUTEINFOHELPER_H__

#include "_Elastos_Droid_Net_CRouteInfoHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Net::IInetAddress;
using Elastos::Utility::ICollection;

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
CarClass(CRouteInfoHelper)
    , public Singleton
    , public IRouteInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * @hide
     */
    CARAPI MakeHostRoute(
        /* [in] */ IInetAddress* host,
        /* [in] */ const String& iface,
        /* [out] */ IRouteInfo** result);

    /**
     * @hide
     */
    CARAPI MakeHostRoute(
        /* [in] */ IInetAddress* host,
        /* [in] */ IInetAddress* gateway,
        /* [in] */ const String& iface,
        /* [out] */ IRouteInfo** result);

    /**
    * Find the route from a Collection of routes that best matches a given address.
    * May return null if no routes are applicable.
    * @param routes a Collection of RouteInfos to chose from
    * @param dest the InetAddress your trying to get to
    * @return the RouteInfo from the Collection that best fits the given address
    */
    //To-do: confirm the use of Collection<> (that is originally used here) in Elastos and replace List<>
    CARAPI SelectBestRoute(
        /* [in] */ ICollection* routes,
        /* [in] */ IInetAddress* dest,
        /* [out] */ IRouteInfo** info);
};

} //namespace Net
} //namespace Droid
} //namespace Elastos

#endif // __ELASTOS_DROID_NET_CROUTEINFOHELPER_H__
