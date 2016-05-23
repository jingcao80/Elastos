
#ifndef __ELASTOS_DROID_NET_IPPREFIX_H__
#define __ELASTOS_DROID_NET_IPPREFIX_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Net::IInetAddress;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * This class represents an IP prefix, i.e., a contiguous block of IP addresses aligned on a
 * power of two boundary (also known as an "IP subnet"). A prefix is specified by two pieces of
 * information:
 *
 * <ul>
 * <li>A starting IP address (IPv4 or IPv6). This is the first IP address of the prefix.
 * <li>A prefix length. This specifies the length of the prefix by specifing the number of bits
 *     in the IP address, starting from the most significant bit in network byte order, that
 *     are constant for all addresses in the prefix.
 * </ul>
 *
 * For example, the prefix <code>192.0.2.0/24</code> covers the 256 IPv4 addresses from
 * <code>192.0.2.0</code> to <code>192.0.2.255</code>, inclusive, and the prefix
 * <code>2001:db8:1:2</code>  covers the 2^64 IPv6 addresses from <code>2001:db8:1:2::</code> to
 * <code>2001:db8:1:2:ffff:ffff:ffff:ffff</code>, inclusive.
 *
 * Objects of this class are immutable.
 */
class IpPrefix
    : public Object
    , public IIpPrefix
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

public:
    CARAPI constructor();
    /**
     * Constructs a new {@code IpPrefix} from a byte array containing an IPv4 or IPv6 address in
     * network byte order and a prefix length. Silently truncates the address to the prefix length,
     * so for example {@code 192.0.2.1/24} is silently converted to {@code 192.0.2.0/24}.
     *
     * @param address the IP address. Must be non-null and exactly 4 or 16 bytes long.
     * @param prefixLength the prefix length. Must be &gt;= 0 and &lt;= (32 or 128) (IPv4 or IPv6).
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* address,
        /* [in] */ Int32 prefixLength);

    /**
     * Constructs a new {@code IpPrefix} from an IPv4 or IPv6 address and a prefix length. Silently
     * truncates the address to the prefix length, so for example {@code 192.0.2.1/24} is silently
     * converted to {@code 192.0.2.0/24}.
     *
     * @param address the IP address. Must be non-null.
     * @param prefixLength the prefix length. Must be &gt;= 0 and &lt;= (32 or 128) (IPv4 or IPv6).
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 prefixLength);

    /**
     * Constructs a new IpPrefix from a string such as "192.0.2.1/24" or "2001:db8::1/64".
     * Silently truncates the address to the prefix length, so for example {@code 192.0.2.1/24}
     * is silently converted to {@code 192.0.2.0/24}.
     *
     * @param prefix the prefix to parse
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ const String& prefix);

    /**
     * Compares this {@code IpPrefix} object against the specified object in {@code obj}. Two
     * objects are equal if they have the same startAddress and prefixLength.
     *
     * @param obj the object to be tested for equality.
     * @return {@code true} if both objects are equal, {@code false} otherwise.
     */
    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    /**
     * Gets the hashcode of the represented IP prefix.
     *
     * @return the appropriate hashcode value.
     */
    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    /**
     * Returns a copy of the first IP address in the prefix. Modifying the returned object does not
     * change this object's contents.
     *
     * @return the address in the form of a byte array.
     */
    CARAPI GetAddress(
        /* [out] */ IInetAddress** result);

    /**
     * Returns a copy of the IP address bytes in network order (the highest order byte is the zeroth
     * element). Modifying the returned array does not change this object's contents.
     *
     * @return the address in the form of a byte array.
     */
    CARAPI GetRawAddress(
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * Returns the prefix length of this {@code IpPrefix}.
     *
     * @return the prefix length.
     */
    CARAPI GetPrefixLength(
        /* [out] */ Int32* result);

    /**
     * Returns a string representation of this {@code IpPrefix}.
     *
     * @return a string such as {@code "192.0.2.0/24"} or {@code "2001:db8:1:2::/64"}.
     */
    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Implement the Parcelable interface.
     */
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    CARAPI CheckAndMaskAddressAndPrefixLength();

private:
    AutoPtr<ArrayOf<Byte> > mAddress;

    // network byte order
    Int32 mPrefixLength;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_IPPREFIX_H__
