
#ifndef __ELASTOS_DROID_NET_ROUTEINFO_H__
#define __ELASTOS_DROID_NET_ROUTEINFO_H__

#include <Elastos.CoreLibrary.Net.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

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
class RouteInfo
    : public Object
    , public IParcelable
    , public IRouteInfo
{
public:
    CAR_INTERFACE_DECL()

    RouteInfo();

    /**
     * Constructs a RouteInfo object.
     *
     * If destination is null, then gateway must be specified and the
     * constructed route is either the IPv4 default route <code>0.0.0.0</code>
     * if the gateway is an instance of {@link Inet4Address}, or the IPv6 default
     * route <code>::/0</code> if gateway is an instance of
     * {@link Inet6Address}.
     * <p>
     * destination and gateway may not both be null.
     *
     * @param destination the destination prefix
     * @param gateway the IP address to route packets through
     * @param iface the interface name to send packets on
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IIpPrefix* destination,
        /* [in] */ IInetAddress* gateway,
        /* [in] */ const String& iface,
        /* [in] */ Int32 type);

    /**
     *  @hide
     */
    CARAPI constructor(
        /* [in] */ IIpPrefix* destination,
        /* [in] */ IInetAddress* gateway,
        /* [in] */ const String& iface);

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ ILinkAddress* destination,
        /* [in] */ IInetAddress* gateway,
        /* [in] */ const String& iface);

    /**
     * Constructs a {@code RouteInfo} object.
     *
     * If destination is null, then gateway must be specified and the
     * constructed route is either the IPv4 default route <code>0.0.0.0</code>
     * if the gateway is an instance of {@link Inet4Address}, or the IPv6 default
     * route <code>::/0</code> if gateway is an instance of {@link Inet6Address}.
     * <p>
     * Destination and gateway may not both be null.
     *
     * @param destination the destination address and prefix in an {@link IpPrefix}
     * @param gateway the {@link InetAddress} to route packets through
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IIpPrefix* destination,
        /* [in] */ IInetAddress* gateway);

    /**
     * @hide
     *
     * TODO: Remove this.
     */
    CARAPI constructor(
        /* [in] */ ILinkAddress* destination,
        /* [in] */ IInetAddress* gateway);

    /**
     * Constructs a default {@code RouteInfo} object.
     *
     * @param gateway the {@link InetAddress} to route packets through
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IInetAddress* gateway);

    /**
     * Constructs a {@code RouteInfo} object representing a direct connected subnet.
     *
     * @param destination the {@link IpPrefix} describing the address and prefix
     *                    length of the subnet.
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IIpPrefix* destination);

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ ILinkAddress* destination);

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IIpPrefix* destination,
        /* [in] */ Int32 type);

    /**
     * @hide
     */
    static CARAPI MakeHostRoute(
        /* [in] */ IInetAddress* host,
        /* [in] */ const String& iface,
        /* [out] */ IRouteInfo** result);

    /**
     * @hide
     */
    static CARAPI MakeHostRoute(
        /* [in] */ IInetAddress* host,
        /* [in] */ IInetAddress* gateway,
        /* [in] */ const String& iface,
        /* [out] */ IRouteInfo** result);

    /**
     * Retrieves the destination address and prefix length in the form of an {@link IpPrefix}.
     *
     * @return {@link IpPrefix} specifying the destination.  This is never {@code null}.
     */
    CARAPI GetDestination(
        /* [out] */ IIpPrefix** result);

    /**
     * TODO: Convert callers to use IpPrefix and then remove.
     * @hide
     */
    CARAPI GetDestinationLinkAddress(
        /* [out] */ ILinkAddress** result);

    /**
     * Retrieves the gateway or next hop {@link InetAddress} for this route.
     *
     * @return {@link InetAddress} specifying the gateway or next hop.  This may be
     *                             {@code null} for a directly-connected route."
     */
    CARAPI GetGateway(
        /* [out] */ IInetAddress** result);

    /**
     * Retrieves the interface used for this route if specified, else {@code null}.
     *
     * @return The name of the interface used for this route.
     */
    CARAPI GetInterface(
        /* [out] */ String* result);

    /**
     * Retrieves the type of this route.
     *
     * @return The type of this route; one of the {@code RTN_xxx} constants defined in this class.
     *
     * @hide
     */
    CARAPI GetType(
        /* [out] */ Int32* result);

    /**
     * Indicates if this route is a default route (ie, has no destination specified).
     *
     * @return {@code true} if the destination has a prefix length of 0.
     */
    CARAPI IsDefaultRoute(
        /* [out] */ Boolean* result);

    /**
     * Indicates if this route is an IPv4 default route.
     * @hide
     */
    CARAPI IsIPv4Default(
        /* [out] */ Boolean* result);

    /**
     * Indicates if this route is an IPv6 default route.
     * @hide
     */
    CARAPI IsIPv6Default(
        /* [out] */ Boolean* result);

    /**
     * Indicates if this route is a host route (ie, matches only a single host address).
     *
     * @return {@code true} if the destination has a prefix length of 32 or 128 for IPv4 or IPv6,
     * respectively.
     * @hide
     */
    CARAPI IsHostRoute(
        /* [out] */ Boolean* result);

    /**
     * Indicates if this route has a next hop ({@code true}) or is directly-connected
     * ({@code false}).
     *
     * @return {@code true} if a gateway is specified
     * @hide
     */
    CARAPI HasGateway(
        /* [out] */ Boolean* result);

    /**
     * Determines whether the destination and prefix of this route includes the specified
     * address.
     *
     * @param destination A {@link InetAddress} to test to see if it would match this route.
     * @return {@code true} if the destination and prefix length cover the given address.
     */
    CARAPI Matches(
        /* [in] */ IInetAddress* destination,
        /* [out] */ Boolean* result);

    /**
     * Find the route from a Collection of routes that best matches a given address.
     * May return null if no routes are applicable.
     * @param routes a Collection of RouteInfos to chose from
     * @param dest the InetAddress your trying to get to
     * @return the RouteInfo from the Collection that best fits the given address
     *
     * @hide
     */
    static CARAPI SelectBestRoute(
        /* [in] */ ICollection* routes,
        /* [in] */ IInetAddress* dest,
        /* [out] */ IRouteInfo** result);

    /**
     * Returns a human-readable description of this object.
     */
    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Compares this RouteInfo object against the specified object and indicates if they are equal.
     * @return {@code true} if the objects are equal, {@code false} otherwise.
     */
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    /**
     *  Returns a hashcode for this <code>RouteInfo</code> object.
     */
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    /**
     * Implement the Parcelable interface.
     */
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    CARAPI_(Boolean) IsHost();

private:
    /**
     * The IP destination address for this route.
     */
    AutoPtr<IIpPrefix> mDestination;

    /**
     * The gateway address for this route.
     */
    AutoPtr<IInetAddress> mGateway;

    /**
     * The interface for this route.
     */
    String mInterface;

    /**
     * The type of this route; one of the RTN_xxx constants above.
     */
    Int32 mType;

    // Derived data members.
    // TODO: remove these.
    Boolean mIsHost;
    Boolean mHasGateway;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_ROUTEINFO_H__
