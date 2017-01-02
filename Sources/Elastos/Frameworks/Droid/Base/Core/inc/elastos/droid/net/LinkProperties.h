//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_NET_LINKPROPERTIES_H__
#define __ELASTOS_DROID_NET_LINKPROPERTIES_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Net::IInetAddress;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IHashTable;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Describes the properties of a network link.
 *
 * A link represents a connection to a network.
 * It may have multiple addresses and multiple gateways,
 * multiple dns servers but only one http proxy and one
 * network interface.
 *
 * Note that this is just a holder of data.  Modifying it
 * does not affect live networks.
 *
 */
class LinkProperties
    : public Object
    , public ILinkProperties
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    LinkProperties();

    /**
     * @hide
     */
    CARAPI constructor();

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ ILinkProperties* source);

    /**
     * Sets the interface name for this link.  All {@link RouteInfo} already set for this
     * will have their interface changed to match this new value.
     *
     * @param iface The name of the network interface used for this link.
     * @hide
     */
    CARAPI SetInterfaceName(
        /* [in] */ const String& iface);

    /**
     * Gets the interface name for this link.  May be {@code null} if not set.
     *
     * @return The interface name set for this link or {@code null}.
     */
    CARAPI GetInterfaceName(
        /* [out] */ String* result);

    /**
     * @hide
     */
    CARAPI GetAllInterfaceNames(
        /* [out] */ IList** result);

    /**
     * Returns all the addresses on this link.  We often think of a link having a single address,
     * however, particularly with Ipv6 several addresses are typical.  Note that the
     * {@code LinkProperties} actually contains {@link LinkAddress} objects which also include
     * prefix lengths for each address.  This is a simplified utility alternative to
     * {@link LinkProperties#getLinkAddresses}.
     *
     * @return An umodifiable {@link List} of {@link InetAddress} for this link.
     * @hide
     */
    CARAPI GetAddresses(
        /* [out] */ IList** result);

    /**
     * Returns all the addresses on this link and all the links stacked above it.
     * @hide
     */
    CARAPI GetAllAddresses(
        /* [out] */ IList** result);

    /**
     * Adds a {@link LinkAddress} to this {@code LinkProperties} if a {@link LinkAddress} of the
     * same address/prefix does not already exist.  If it does exist it is replaced.
     * @param address The {@code LinkAddress} to add.
     * @return true if {@code address} was added or updated, false otherwise.
     * @hide
     */
    CARAPI AddLinkAddress(
        /* [in] */ ILinkAddress* address,
        /* [out] */ Boolean* result);

    /**
     * Removes a {@link LinkAddress} from this {@code LinkProperties}.  Specifically, matches
     * and {@link LinkAddress} with the same address and prefix.
     *
     * @param toRemove A {@link LinkAddress} specifying the address to remove.
     * @return true if the address was removed, false if it did not exist.
     * @hide
     */
    CARAPI RemoveLinkAddress(
        /* [in] */ ILinkAddress* toRemove,
        /* [out] */ Boolean* result);

    /**
     * Returns all the {@link LinkAddress} on this link.  Typically a link will have
     * one IPv4 address and one or more IPv6 addresses.
     *
     * @return An unmodifiable {@link List} of {@link LinkAddress} for this link.
     */
    CARAPI GetLinkAddresses(
        /* [out] */ IList** result);

    /**
     * Returns all the addresses on this link and all the links stacked above it.
     * @hide
     */
    CARAPI GetAllLinkAddresses(
        /* [out] */ IList** result);

    /**
     * Replaces the {@link LinkAddress} in this {@code LinkProperties} with
     * the given {@link Collection} of {@link LinkAddress}.
     *
     * @param addresses The {@link Collection} of {@link LinkAddress} to set in this
     *                  object.
     * @hide
     */
    CARAPI SetLinkAddresses(
        /* [in] */ ICollection* addresses);

    /**
     * Adds the given {@link InetAddress} to the list of DNS servers, if not present.
     *
     * @param dnsServer The {@link InetAddress} to add to the list of DNS servers.
     * @return true if the DNS server was added, false if it was already present.
     * @hide
     */
    CARAPI AddDnsServer(
        /* [in] */ IInetAddress* dnsServer,
        /* [out] */ Boolean* result);

    /**
     * Replaces the DNS servers in this {@code LinkProperties} with
     * the given {@link Collection} of {@link InetAddress} objects.
     *
     * @param addresses The {@link Collection} of DNS servers to set in this object.
     * @hide
     */
    CARAPI SetDnsServers(
        /* [in] */ ICollection* dnsServers);

    /**
     * Returns all the {@link InetAddress} for DNS servers on this link.
     *
     * @return An umodifiable {@link List} of {@link InetAddress} for DNS servers on
     *         this link.
     */
    CARAPI GetDnsServers(
        /* [out] */ IList** result);

    /**
     * Sets the DNS domain search path used on this link.
     *
     * @param domains A {@link String} listing in priority order the comma separated
     *                domains to search when resolving host names on this link.
     * @hide
     */
    CARAPI SetDomains(
        /* [in] */ const String& domains);

    /**
     * Get the DNS domains search path set for this link.
     *
     * @return A {@link String} containing the comma separated domains to search when resolving
     *         host names on this link.
     */
    CARAPI GetDomains(
        /* [out] */ String* result);

    /**
     * Sets the Maximum Transmission Unit size to use on this link.  This should not be used
     * unless the system default (1500) is incorrect.  Values less than 68 or greater than
     * 10000 will be ignored.
     *
     * @param mtu The MTU to use for this link.
     * @hide
     */
    CARAPI SetMtu(
        /* [in] */ Int32 mtu);

    /**
     * Gets any non-default MTU size set for this link.  Note that if the default is being used
     * this will return 0.
     *
     * @return The mtu value set for this link.
     * @hide
     */
    CARAPI GetMtu(
        /* [out] */ Int32* result);

    /**
     * Sets the tcp buffers sizes to be used when this link is the system default.
     * Should be of the form "rmem_min,rmem_def,rmem_max,wmem_min,wmem_def,wmem_max".
     *
     * @param tcpBufferSizes The tcp buffers sizes to use.
     *
     * @hide
     */
    CARAPI SetTcpBufferSizes(
        /* [in] */ const String& tcpBufferSizes);

    /**
     * Gets the tcp buffer sizes.
     *
     * @return the tcp buffer sizes to use when this link is the system default.
     *
     * @hide
     */
    CARAPI GetTcpBufferSizes(
        /* [out] */ String* result);

    /**
     * Adds a {@link RouteInfo} to this {@code LinkProperties}, if not present. If the
     * {@link RouteInfo} had an interface name set and that differs from the interface set for this
     * {@code LinkProperties} an {@link IllegalArgumentException} will be thrown.  The proper
     * course is to add either un-named or properly named {@link RouteInfo}.
     *
     * @param route A {@link RouteInfo} to add to this object.
     * @return {@code false} if the route was already present, {@code true} if it was added.
     *
     * @hide
     */
    CARAPI AddRoute(
        /* [in] */ IRouteInfo* route,
        /* [out] */ Boolean* result);

    /**
     * Removes a {@link RouteInfo} from this {@code LinkProperties}, if present. The route must
     * specify an interface and the interface must match the interface of this
     * {@code LinkProperties}, or it will not be removed.
     *
     * @return {@code true} if the route was removed, {@code false} if it was not present.
     *
     * @hide
     */
    CARAPI RemoveRoute(
        /* [in] */ IRouteInfo* route,
        /* [out] */ Boolean* result);

    /**
     * Returns all the {@link RouteInfo} set on this link.
     *
     * @return An unmodifiable {@link List} of {@link RouteInfo} for this link.
     */
    CARAPI GetRoutes(
        /* [out] */ IList** result);

    /**
     * Returns all the routes on this link and all the links stacked above it.
     * @hide
     */
    CARAPI GetAllRoutes(
        /* [out] */ IList** result);

    /**
     * Sets the recommended {@link ProxyInfo} to use on this link, or {@code null} for none.
     * Note that Http Proxies are only a hint - the system recommends their use, but it does
     * not enforce it and applications may ignore them.
     *
     * @param proxy A {@link ProxyInfo} defining the Http Proxy to use on this link.
     * @hide
     */
    CARAPI SetHttpProxy(
        /* [in] */ IProxyInfo* proxy);

    /**
     * Gets the recommended {@link ProxyInfo} (or {@code null}) set on this link.
     *
     * @return The {@link ProxyInfo} set on this link
     */
    CARAPI GetHttpProxy(
        /* [out] */ IProxyInfo** result);

    /**
     * Adds a stacked link.
     *
     * If there is already a stacked link with the same interfacename as link,
     * that link is replaced with link. Otherwise, link is added to the list
     * of stacked links. If link is null, nothing changes.
     *
     * @param link The link to add.
     * @return true if the link was stacked, false otherwise.
     * @hide
     */
    CARAPI AddStackedLink(
        /* [in] */ ILinkProperties* link,
        /* [out] */ Boolean* result);

    /**
     * Removes a stacked link.
     *
     * If there a stacked link with the same interfacename as link, it is
     * removed. Otherwise, nothing changes.
     *
     * @param link The link to remove.
     * @return true if the link was removed, false otherwise.
     * @hide
     */
    CARAPI RemoveStackedLink(
        /* [in] */ ILinkProperties* link,
        /* [out] */ Boolean* result);

    /**
     * Returns all the links stacked on top of this link.
     * @hide
     */
    CARAPI GetStackedLinks(
        /* [out] */ IList** result);

    /**
     * Clears this object to its initial state.
     * @hide
     */
    CARAPI Clear();

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Returns true if this link has an IPv4 address.
     *
     * @return {@code true} if there is an IPv4 address, {@code false} otherwise.
     * @hide
     */
    CARAPI HasIPv4Address(
        /* [out] */ Boolean* result);

    /**
     * Returns true if this link has a global preferred IPv6 address.
     *
     * @return {@code true} if there is a global preferred IPv6 address, {@code false} otherwise.
     * @hide
     */
    CARAPI HasGlobalIPv6Address(
        /* [out] */ Boolean* result);

    /**
     * Returns true if this link has an IPv4 default route.
     *
     * @return {@code true} if there is an IPv4 default route, {@code false} otherwise.
     * @hide
     */
    CARAPI HasIPv4DefaultRoute(
        /* [out] */ Boolean* result);

    /**
     * Returns true if this link has an IPv6 default route.
     *
     * @return {@code true} if there is an IPv6 default route, {@code false} otherwise.
     * @hide
     */
    CARAPI HasIPv6DefaultRoute(
        /* [out] */ Boolean* result);

    /**
     * Returns true if this link has an IPv4 DNS server.
     *
     * @return {@code true} if there is an IPv4 DNS server, {@code false} otherwise.
     * @hide
     */
    CARAPI HasIPv4DnsServer(
        /* [out] */ Boolean* result);

    /**
     * Returns true if this link has an IPv6 DNS server.
     *
     * @return {@code true} if there is an IPv6 DNS server, {@code false} otherwise.
     * @hide
     */
    CARAPI HasIPv6DnsServer(
        /* [out] */ Boolean* result);

    /**
     * Returns true if this link is provisioned for global connectivity. For IPv6, this requires an
     * IP address, default route, and DNS server. For IPv4, this requires only an IPv4 address,
     * because WifiStateMachine accepts static configurations that only specify an address but not
     * DNS servers or a default route.
     *
     * @return {@code true} if the link is provisioned, {@code false} otherwise.
     * @hide
     */
    CARAPI IsProvisioned(
        /* [out] */ Boolean* result);

    /**
     * Compares this {@code LinkProperties} interface name against the target
     *
     * @param target LinkProperties to compare.
     * @return {@code true} if both are identical, {@code false} otherwise.
     * @hide
     */
    CARAPI IsIdenticalInterfaceName(
        /* [in] */ ILinkProperties* target,
        /* [out] */ Boolean* result);

    /**
     * Compares this {@code LinkProperties} interface addresses against the target
     *
     * @param target LinkProperties to compare.
     * @return {@code true} if both are identical, {@code false} otherwise.
     * @hide
     */
    CARAPI IsIdenticalAddresses(
        /* [in] */ ILinkProperties* target,
        /* [out] */ Boolean* result);

    /**
     * Compares this {@code LinkProperties} DNS addresses against the target
     *
     * @param target LinkProperties to compare.
     * @return {@code true} if both are identical, {@code false} otherwise.
     * @hide
     */
    CARAPI IsIdenticalDnses(
        /* [in] */ ILinkProperties* target,
        /* [out] */ Boolean* result);

    /**
     * Compares this {@code LinkProperties} Routes against the target
     *
     * @param target LinkProperties to compare.
     * @return {@code true} if both are identical, {@code false} otherwise.
     * @hide
     */
    CARAPI IsIdenticalRoutes(
        /* [in] */ ILinkProperties* target,
        /* [out] */ Boolean* result);

    /**
     * Compares this {@code LinkProperties} HttpProxy against the target
     *
     * @param target LinkProperties to compare.
     * @return {@code true} if both are identical, {@code false} otherwise.
     * @hide
     */
    CARAPI IsIdenticalHttpProxy(
        /* [in] */ ILinkProperties* target,
        /* [out] */ Boolean* result);

    /**
     * Compares this {@code LinkProperties} stacked links against the target
     *
     * @param target LinkProperties to compare.
     * @return {@code true} if both are identical, {@code false} otherwise.
     * @hide
     */
    CARAPI IsIdenticalStackedLinks(
        /* [in] */ ILinkProperties* target,
        /* [out] */ Boolean* result);

    /**
     * Compares this {@code LinkProperties} MTU against the target
     *
     * @param target LinkProperties to compare.
     * @return {@code true} if both are identical, {@code false} otherwise.
     * @hide
     */
    CARAPI IsIdenticalMtu(
        /* [in] */ ILinkProperties* target,
        /* [out] */ Boolean* result);

    /**
     * Compares this {@code LinkProperties} Tcp buffer sizes against the target.
     *
     * @param target LinkProperties to compare.
     * @return {@code true} if both are identical, {@code false} otherwise.
     * @hide
     */
    CARAPI IsIdenticalTcpBufferSizes(
        /* [in] */ ILinkProperties* target,
        /* [out] */ Boolean* result);

    /**
     * Compares this {@code LinkProperties} instance against the target
     * LinkProperties in {@code obj}. Two LinkPropertieses are equal if
     * all their fields are equal in values.
     *
     * For collection fields, such as mDnses, containsAll() is used to check
     * if two collections contains the same elements, independent of order.
     * There are two thoughts regarding containsAll()
     * 1. Duplicated elements. eg, (A, B, B) and (A, A, B) are equal.
     * 2. Worst case performance is O(n^2).
     *
     * @param obj the object to be tested for equality.
     * @return {@code true} if both objects are equal, {@code false} otherwise.
     */
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    /**
     * Compares the addresses in this LinkProperties with another
     * LinkProperties, examining only addresses on the base link.
     *
     * @param target a LinkProperties with the new list of addresses
     * @return the differences between the addresses.
     * @hide
     */
    CARAPI CompareAddresses(
        /* [in] */ ILinkProperties* target,
        /* [out] */ ILinkPropertiesCompareResult** result);

    /**
     * Compares the DNS addresses in this LinkProperties with another
     * LinkProperties, examining only DNS addresses on the base link.
     *
     * @param target a LinkProperties with the new list of dns addresses
     * @return the differences between the DNS addresses.
     * @hide
     */
    CARAPI CompareDnses(
        /* [in] */ ILinkProperties* target,
        /* [out] */ ILinkPropertiesCompareResult** result);

    /**
     * Compares all routes in this LinkProperties with another LinkProperties,
     * examining both the the base link and all stacked links.
     *
     * @param target a LinkProperties with the new list of routes
     * @return the differences between the routes.
     * @hide
     */
    CARAPI CompareAllRoutes(
        /* [in] */ ILinkProperties* target,
        /* [out] */ ILinkPropertiesCompareResult** result);

    /**
     * Compares all interface names in this LinkProperties with another
     * LinkProperties, examining both the the base link and all stacked links.
     *
     * @param target a LinkProperties with the new list of interface names
     * @return the differences between the interface names.
     * @hide
     */
    CARAPI CompareAllInterfaceNames(
        /* [in] */ ILinkProperties* target,
        /* [out] */ ILinkPropertiesCompareResult** result);

    /**
     * generate hashcode based on significant fields
     * Equal objects must produce the same hash code, while unequal objects
     * may have the same hash codes.
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

    /**
     * Check the valid MTU range based on IPv4 or IPv6.
     * @hide
     */
    static CARAPI IsValidMtu(
        /* [in] */ Int32 mtu,
        /* [in] */ Boolean ipv6,
        /* [out] */ Boolean* result);

private:
    CARAPI_(Int32) FindLinkAddressIndex(
        /* [in] */ ILinkAddress* address);

    CARAPI_(AutoPtr<IRouteInfo>) RouteWithInterface(
        /* [in] */ IRouteInfo* route);

private:
    // The interface described by the network link.
    String mIfaceName;
    AutoPtr<IArrayList> mLinkAddresses;
    AutoPtr<IArrayList> mDnses;
    AutoPtr<IArrayList> mRoutes;
    String mDomains;
    AutoPtr<IProxyInfo> mHttpProxy;
    Int32 mMtu;
    // in the format "rmem_min,rmem_def,rmem_max,wmem_min,wmem_def,wmem_max"
    String mTcpBufferSizes;

    static const Int32 MIN_MTU;
    static const Int32 MIN_MTU_V6;
    static const Int32 MAX_MTU;

    // Stores the properties of links that are "stacked" above this link.
    // Indexed by interface name to allow modification and to prevent duplicates being added.
    AutoPtr<IHashTable> mStackedLinks;
};

/**
 * @hide
 */
class LinkPropertiesCompareResult
    : public Object
    , public ILinkPropertiesCompareResult
{
public:
    CAR_INTERFACE_DECL()

    LinkPropertiesCompareResult();

    CARAPI constructor();

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetRemoved(
        /* [out] */ IList** result);

    CARAPI SetRemoved(
        /* [in] */ IList* removed);

    CARAPI GetAdded(
        /* [out] */ IList** result);

    CARAPI SetAdded(
        /* [in] */ IList* added);

private:
    AutoPtr<IList> mRemoved;
    AutoPtr<IList> mAdded;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_LINKPROPERTIES_H__
