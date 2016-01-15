
#include "InterfaceAddress.h"
#include "StringBuilder.h"
#include "StringUtils.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(InterfaceAddress, Object, IInterfaceAddress)

InterfaceAddress::InterfaceAddress()
    : mPrefixLength(0)
{}

InterfaceAddress::InterfaceAddress(
    /* [in] */ IInetAddress* address,
    /* [in] */ IInetAddress* broadcastAddress,
    /* [in] */ IInetAddress* mask)
    : mAddress(address)
    , mBroadcastAddress(broadcastAddress)
{
    mPrefixLength = CountPrefixLength(mask);
}

InterfaceAddress::InterfaceAddress(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int16 prefixLength)
    : mAddress(address)
    , mPrefixLength(prefixLength)
{
}

ECode InterfaceAddress::constructor(
    /* [in] */ IInetAddress* address,
    /* [in] */ IInetAddress* broadcastAddress,
    /* [in] */ IInetAddress* mask)
{
    mAddress = address;
    mBroadcastAddress = broadcastAddress;
    mPrefixLength = CountPrefixLength(mask);
    return NOERROR;
}

ECode InterfaceAddress::constructor(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int16 prefixLength)
{
    mAddress = address;
    mBroadcastAddress = NULL;
    mPrefixLength = prefixLength;
    return NOERROR;
}

Int16 InterfaceAddress::CountPrefixLength(
    /* [in] */ IInetAddress* mask)
{
    assert(mask != NULL);

    Int16 count = 0;
    AutoPtr<ArrayOf<Byte> > maskIpAddress;
    mask->GetAddress((ArrayOf<Byte>**)&maskIpAddress);
    for (Int32 i = 0; i < maskIpAddress->GetLength(); ++i) {
        Byte b = (*maskIpAddress)[i];
        for (Int32 j = 0; j < 8; ++j) {
            if ((b & (1 << j)) != 0) {
                ++count;
            }
        }
    }

    return count;
}

ECode InterfaceAddress::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* equals)
{
    VALIDATE_NOT_NULL(equals)
    *equals = FALSE;

    IInterfaceAddress* oia = IInterfaceAddress::Probe(obj);
    if (obj == NULL) {
        return NOERROR;
    }

    InterfaceAddress* rhs =(InterfaceAddress*)oia;
    Boolean bval = TRUE;
    if (mAddress != NULL) {
        bval = Object::Equals(mAddress, rhs->mAddress);
    }
    if (bval && mBroadcastAddress != NULL){
        bval = Object::Equals(mBroadcastAddress, rhs->mBroadcastAddress);
    }
    *equals = bval && (rhs->mPrefixLength == mPrefixLength);
    return NOERROR;
}

ECode InterfaceAddress::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    Int32 hashCode = 0;
    if (mAddress != NULL) {
        hashCode += -1 * Object::GetHashCode(mAddress);
    }

    if (mBroadcastAddress != NULL) {
        hashCode += -1 * Object::GetHashCode(mBroadcastAddress);
    }

    hashCode += mPrefixLength;
    *hash = hashCode;
    return NOERROR;
}

/**
 * Returns a string containing this interface's address, prefix length, and broadcast address.
 * For example: {@code "/172.18.103.112/23 [/172.18.103.255]"} or
 * {@code "/0:0:0:0:0:0:0:1%1/128 [null]"}.
 */
ECode InterfaceAddress::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    String addr = Object::ToString(mAddress);
    String broadcast = Object::ToString(mBroadcastAddress);

    StringBuilder sb(addr);
    sb.AppendChar('/');
    sb.Append(mPrefixLength);
    sb.AppendChar('[');
    sb.Append(broadcast);
    sb.AppendChar(']');
    *str = sb.ToString();
    return NOERROR;
}

ECode InterfaceAddress::GetAddress(
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address);
    *address = mAddress;
    REFCOUNT_ADD(*address);
    return NOERROR;
}

ECode InterfaceAddress::GetBroadcast(
    /* [out] */ IInetAddress** broadcast)
{
    VALIDATE_NOT_NULL(broadcast);
    *broadcast = mBroadcastAddress;
    REFCOUNT_ADD(*broadcast);
    return NOERROR;
}

ECode InterfaceAddress::GetNetworkPrefixLength(
    /* [out] */ Int16* length)
{
    VALIDATE_NOT_NULL(length);
    *length = mPrefixLength;
    return NOERROR;
}


} // namespace Net
} // namespace Elastos
