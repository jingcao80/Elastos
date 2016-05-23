
#ifndef __ELASTOS_DROID_NET_LINKADDRESS_H__
#define __ELASTOS_DROID_NET_LINKADDRESS_H__

#include <Elastos.CoreLibrary.Net.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Net::IInetAddress;
using Elastos::Net::IInterfaceAddress;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Identifies an IP address on a network link.
 *
 * A {@code LinkAddress} consists of:
 * <ul>
 * <li>An IP address and prefix length (e.g., {@code 2001:db8::1/64} or {@code 192.0.2.1/24}).
 * The address must be unicast, as multicast addresses cannot be assigned to interfaces.
 * <li>Address flags: A bitmask of {@code OsConstants.IFA_F_*} values representing properties
 * of the address (e.g., {@code android.system.OsConstants.IFA_F_OPTIMISTIC}).
 * <li>Address scope: One of the {@code OsConstants.IFA_F_*} values; defines the scope in which
 * the address is unique (e.g.,
 * {@code android.system.OsConstants.RT_SCOPE_LINK} or
 * {@code android.system.OsConstants.RT_SCOPE_UNIVERSE}).
 * </ul>
 */
class LinkAddress
    : public Object
    , public ILinkAddress
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

public:
    /**
     * Utility function to determines the scope of a unicast address. Per RFC 4291 section 2.5 and
     * RFC 6724 section 3.2.
     * @hide
     */
    static CARAPI ScopeForUnicastAddress(
        /* [in] */ IInetAddress* addr,
        /* [out] */ Int32* result);

    CARAPI constructor();
    /**
     * Constructs a new {@code LinkAddress} from an {@code InetAddress} and prefix length, with
     * the specified flags and scope. Flags and scope are not checked for validity.
     * @param address The IP address.
     * @param prefixLength The prefix length.
     * @param flags A bitmask of {@code IFA_F_*} values representing properties of the address.
     * @param scope An integer defining the scope in which the address is unique (e.g.,
     *              {@link OsConstants#RT_SCOPE_LINK} or {@link OsConstants#RT_SCOPE_SITE}).
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 prefixLength,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 scope);

    /**
     * Constructs a new {@code LinkAddress} from an {@code InetAddress} and a prefix length.
     * The flags are set to zero and the scope is determined from the address.
     * @param address The IP address.
     * @param prefixLength The prefix length.
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 prefixLength);

    /**
     * Constructs a new {@code LinkAddress} from an {@code InterfaceAddress}.
     * The flags are set to zero and the scope is determined from the address.
     * @param interfaceAddress The interface address.
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IInterfaceAddress* interfaceAddress);

    /**
     * Constructs a new {@code LinkAddress} from a string such as "192.0.2.5/24" or
     * "2001:db8::1/64". The flags are set to zero and the scope is determined from the address.
     * @param string The string to parse.
     * @hide
     */
    CARAPI constructor(
        /* [in] */ const String& address);

    /**
     * Constructs a new {@code LinkAddress} from a string such as "192.0.2.5/24" or
     * "2001:db8::1/64", with the specified flags and scope.
     * @param string The string to parse.
     * @param flags The address flags.
     * @param scope The address scope.
     * @hide
     */
    CARAPI constructor(
        /* [in] */ const String& address,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 scope);

    /**
     * Returns a string representation of this address, such as "192.0.2.1/24" or "2001:db8::1/64".
     * The string representation does not contain the flags and scope, just the address and prefix
     * length.
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Compares this {@code LinkAddress} instance against {@code obj}. Two addresses are equal if
     * their address, prefix length, flags and scope are equal. Thus, for example, two addresses
     * that have the same address and prefix length are not equal if one of them is deprecated and
     * the other is not.
     *
     * @param obj the object to be tested for equality.
     * @return {@code true} if both objects are equal, {@code false} otherwise.
     */
    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    /**
     * Returns a hashcode for this address.
     */
    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    /**
     * Determines whether this {@code LinkAddress} and the provided {@code LinkAddress}
     * represent the same address. Two {@code LinkAddresses} represent the same address
     * if they have the same IP address and prefix length, even if their properties are
     * different.
     *
     * @param other the {@code LinkAddress} to compare to.
     * @return {@code true} if both objects have the same address and prefix length, {@code false}
     * otherwise.
     * @hide
     */
    CARAPI IsSameAddressAs(
        /* [in] */ ILinkAddress* other,
        /* [out] */ Boolean* result);

    /**
     * Returns the {@link InetAddress} of this {@code LinkAddress}.
     */
    CARAPI GetAddress(
        /* [out] */ IInetAddress** result);

    /**
     * Returns the prefix length of this {@code LinkAddress}.
     */
    CARAPI GetPrefixLength(
        /* [out] */ Int32* result);

    /**
     * Returns the prefix length of this {@code LinkAddress}.
     * TODO: Delete all callers and remove in favour of getPrefixLength().
     * @hide
     */
    CARAPI GetNetworkPrefixLength(
        /* [out] */ Int32* result);

    /**
     * Returns the flags of this {@code LinkAddress}.
     */
    CARAPI GetFlags(
        /* [out] */ Int32* result);

    /**
     * Returns the scope of this {@code LinkAddress}.
     */
    CARAPI GetScope(
        /* [out] */ Int32* result);

    /**
     * Returns true if this {@code LinkAddress} is global scope and preferred.
     * @hide
     */
    CARAPI IsGlobalPreferred(
        /* [out] */ Boolean* result);

    /**
     * Implement the Parcelable interface.
     */
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    /**
     * Utility function for the constructors.
     */
    CARAPI Init(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 prefixLength,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 scope);

    /**
     * IPv4 or IPv6 address.
     */
    AutoPtr<IInetAddress> mAddress;
    /**
     * Prefix length.
     */
    Int32 mPrefixLength;
    /**
     * Address flags. A bitmask of IFA_F_* values.
     */
    Int32 mFlags;
    /**
     * Address scope. One of the RT_SCOPE_* constants.
     */
    Int32 mScope;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_LINKADDRESS_H__
