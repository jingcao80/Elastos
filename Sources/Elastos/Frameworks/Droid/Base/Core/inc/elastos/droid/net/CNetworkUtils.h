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

#ifndef __ELASTOS_DROID_NET_CNETWORKUTILS_H__
#define __ELASTOS_DROID_NET_CNETWORKUTILS_H__

#include "_Elastos_Droid_Net_CNetworkUtils.h"
#include <elastos/core/Singleton.h>

using Elastos::Net::IInet4Address;
using Elastos::Droid::Utility::IPair;
using Elastos::Utility::ICollection;
using Elastos::Net::IInetAddress;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Native methods for managing network interfaces.
 *
 * {@hide}
 */
CarClass(CNetworkUtils)
    , public Singleton
    , public INetworkUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Reset IPv6 or IPv4 sockets that are connected via the named interface.
     *
     * @param interfaceName is the interface to reset
     * @param mask {@see #RESET_IPV4_ADDRESSES} and {@see #RESET_IPV6_ADDRESSES}
     */
    CARAPI ResetConnections(
        /* [in] */ const String& interfaceName,
        /* [in] */ Int32 mask,
        /* [out] */ Int32* result);

    /**
     * Start the DHCP client daemon, in order to have it request addresses
     * for the named interface, and then configure the interface with those
     * addresses. This call blocks until it obtains a result (either success
     * or failure) from the daemon.
     * @param interfaceName the name of the interface to configure
     * @param dhcpResults if the request succeeds, this object is filled in with
     * the IP address information.
     * @return {@code true} for success, {@code false} for failure
     */
    CARAPI RunDhcp(
        /* [in] */ const String& interfaceName,
        /* [in] */ IDhcpResults* dhcpResults,
        /* [out] */ Boolean* result);

    /**
     * Initiate renewal on the Dhcp client daemon. This call blocks until it obtains
     * a result (either success or failure) from the daemon.
     * @param interfaceName the name of the interface to configure
     * @param dhcpResults if the request succeeds, this object is filled in with
     * the IP address information.
     * @return {@code true} for success, {@code false} for failure
     */
    CARAPI RunDhcpRenew(
        /* [in] */ const String& interfaceName,
        /* [in] */ IDhcpResults* dhcpResults,
        /* [out] */ Boolean* result);

    /**
     * Shut down the DHCP client daemon.
     * @param interfaceName the name of the interface for which the daemon
     * should be stopped
     * @return {@code true} for success, {@code false} for failure
     */
    CARAPI StopDhcp(
        /* [in] */ const String& interfaceName,
        /* [out] */ Boolean* result);

    /**
     * Release the current DHCP lease.
     * @param interfaceName the name of the interface for which the lease should
     * be released
     * @return {@code true} for success, {@code false} for failure
     */
    CARAPI ReleaseDhcpLease(
        /* [in] */ const String& interfaceName,
        /* [out] */ Boolean* result);

    /**
     * Return the last DHCP-related error message that was recorded.
     * <p/>NOTE: This string is not localized, but currently it is only
     * used in logging.
     * @return the most recent error message, if any
     */
    CARAPI GetDhcpError(
        /* [out] */ String* result);

    /**
     * Binds the current process to the network designated by {@code netId}.  All sockets created
     * in the future (and not explicitly bound via a bound {@link SocketFactory} (see
     * {@link Network#getSocketFactory}) will be bound to this network.  Note that if this
     * {@code Network} ever disconnects all sockets created in this way will cease to work.  This
     * is by design so an application doesn't accidentally use sockets it thinks are still bound to
     * a particular {@code Network}.  Passing NETID_UNSET clears the binding.
     */
    CARAPI BindProcessToNetwork(
        /* [in] */ Int32 netId,
        /* [out] */ Boolean* result);

    /**
     * Return the netId last passed to {@link #bindProcessToNetwork}, or NETID_UNSET if
     * {@link #unbindProcessToNetwork} has been called since {@link #bindProcessToNetwork}.
     */
    CARAPI GetNetworkBoundToProcess(
        /* [out] */ Int32* result);

    /**
     * Binds host resolutions performed by this process to the network designated by {@code netId}.
     * {@link #bindProcessToNetwork} takes precedence over this setting.  Passing NETID_UNSET clears
     * the binding.
     *
     * @deprecated This is strictly for legacy usage to support startUsingNetworkFeature().
     */
    CARAPI BindProcessToNetworkForHostResolution(
        /* [in] */ Int32 netId,
        /* [out] */ Boolean* result);

    /**
     * Explicitly binds {@code socketfd} to the network designated by {@code netId}.  This
     * overrides any binding via {@link #bindProcessToNetwork}.
     * @return 0 on success or negative errno on failure.
     */
    CARAPI BindSocketToNetwork(
        /* [in] */ Int32 socketfd,
        /* [in] */ Int32 netId,
        /* [out] */ Int32* result);

    /**
     * Protect {@code socketfd} from VPN connections.  After protecting, data sent through
     * this socket will go directly to the underlying network, so its traffic will not be
     * forwarded through the VPN.
     */
    CARAPI ProtectFromVpn(
        /* [in] */ Int32 socketfd,
        /* [out] */ Boolean* result);

    /**
     * Convert a IPv4 address from an integer to an InetAddress.
     * @param hostAddress an int corresponding to the IPv4 address in network byte order
     */
    CARAPI IntToInetAddress(
        /* [in] */ Int32 hostAddress,
        /* [out] */ IInetAddress** result);

    /**
     * Convert a IPv4 address from an InetAddress to an integer
     * @param inetAddr is an InetAddress corresponding to the IPv4 address
     * @return the IP address as an integer in network byte order
     */
    CARAPI InetAddressToInt(
        /* [in] */ IInet4Address* inetAddr,
        /* [out] */ Int32* result);

    /**
     * Convert a network prefix length to an IPv4 netmask integer
     * @param prefixLength
     * @return the IPv4 netmask as an integer in network byte order
     */
    CARAPI PrefixLengthToNetmaskInt(
        /* [in] */ Int32 prefixLength,
        /* [out] */ Int32* result);

    /**
     * Convert a IPv4 netmask integer to a prefix length
     * @param netmask as an integer in network byte order
     * @return the network prefix length
     */
    CARAPI NetmaskIntToPrefixLength(
        /* [in] */ Int32 netmask,
        /* [out] */ Int32* result);

    /**
     * Create an InetAddress from a string where the string must be a standard
     * representation of a V4 or V6 address.  Avoids doing a DNS lookup on failure
     * but it will throw an IllegalArgumentException in that case.
     * @param addrString
     * @return the InetAddress
     * @hide
     */
    CARAPI NumericToInetAddress(
        /* [in] */ const String& addrString,
        /* [out] */ IInetAddress** result);

    /**
     * Writes an InetAddress to a parcel. The address may be null. This is likely faster than
     * calling writeSerializable.
     */
    CARAPI ParcelInetAddress(
        /* [in] */ IParcel* parcel,
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 flags);

    /**
     * Reads an InetAddress from a parcel. Returns null if the address that was written was null
     * or if the data is invalid.
     */
    CARAPI UnparcelInetAddress(
        /* [in] */ IParcel* in,
        /* [out] */ IInetAddress** result);

    /**
     *  Masks a raw IP address byte array with the specified prefix length.
     */
    CARAPI MaskRawAddress(
        /* [in] */ ArrayOf<Byte>* array,
        /* [in] */ Int32 prefixLength);

    /**
     * Get InetAddress masked with prefixLength.  Will never return null.
     * @param address the IP address to mask with
     * @param prefixLength the prefixLength used to mask the IP
     */
    CARAPI GetNetworkPart(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 prefixLength,
        /* [out] */ IInetAddress** result);

    /**
     * Utility method to parse strings such as "192.0.2.5/24" or "2001:db8::cafe:d00d/64".
     * @hide
     */
    CARAPI ParseIpAndMask(
        /* [in] */ const String& ipAndMaskString,
        /* [out] */ IPair** result);

    /**
     * Check if IP address type is consistent between two InetAddress.
     * @return true if both are the same type.  False otherwise.
     */
    CARAPI AddressTypeMatches(
        /* [in] */ IInetAddress* left,
        /* [in] */ IInetAddress* right,
        /* [out] */ Boolean* result);

    /**
     * Convert a 32 char hex string into a Inet6Address.
     * throws a runtime exception if the string isn't 32 chars, isn't hex or can't be
     * made into an Inet6Address
     * @param addrHexString a 32 character hex string representing an IPv6 addr
     * @return addr an InetAddress representation for the string
     */
    CARAPI HexToInet6Address(
        /* [in] */ const String& addrHexString,
        /* [out] */ IInetAddress** result);

    /**
     * Create a string array of host addresses from a collection of InetAddresses
     * @param addrs a Collection of InetAddresses
     * @return an array of Strings containing their host addresses
     */
    CARAPI MakeStrings(
        /* [in] */ ICollection* addrs,
        /* [out, callee] */ ArrayOf<String>** result);

    /**
     * Trim leading zeros from IPv4 address strings
     * Our base libraries will interpret that as octel..
     * Must leave non v4 addresses and host names alone.
     * For example, 192.168.000.010 -> 192.168.0.10
     * TODO - fix base libraries and remove this function
     * @param addr a string representing an ip addr
     * @return a string propertly trimmed
     */
    CARAPI TrimV4AddrZeros(
        /* [in] */ const String& addr,
        /* [out] */ String* result);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_CNETWORKUTILS_H__
