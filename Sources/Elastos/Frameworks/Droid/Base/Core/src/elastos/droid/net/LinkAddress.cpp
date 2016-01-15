
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/net/LinkAddress.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/system/OsConstants.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::System::OsConstants;

using Elastos::Core::IInteger32;
using Elastos::Net::IInet4Address;
using Elastos::Net::IInetAddress;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(LinkAddress, Object, ILinkAddress, IParcelable)

ECode LinkAddress::ScopeForUnicastAddress(
    /* [in] */ IInetAddress* addr,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (RETN_OUT_VAL(addr, IsAnyLocalAddress)) {
        *result = OsConstants::_RT_SCOPE_HOST;
        return NOERROR;
    }

    if (RETN_OUT_VAL(addr, IsLoopbackAddress) || RETN_OUT_VAL(addr, IsLinkLocalAddress)) {
        *result = OsConstants::_RT_SCOPE_LINK;
        return NOERROR;
    }

    // isSiteLocalAddress() returns true for private IPv4 addresses, but RFC 6724 section 3.2
    // says that they are assigned global scope.
    if ((IInet4Address::Probe(addr) == NULL) && RETN_OUT_VAL(addr, IsSiteLocalAddress)) {
        *result = OsConstants::_RT_SCOPE_SITE;
        return NOERROR;
    }
    *result = OsConstants::_RT_SCOPE_UNIVERSE;
    return NOERROR;
}

ECode LinkAddress::Init(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 prefixLength,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 scope)
{
    if (address == NULL ||
            RETN_OUT_VAL(address, IsMulticastAddress) ||
            prefixLength < 0 ||
            ((IInet4Address::Probe(address) != NULL ) && prefixLength > 32) ||
            (prefixLength > 128)) {
        String s;
        IObject::Probe(address)->ToString(&s);
        Logger::E("LinkAddress", "Bad LinkAddress params %s/%d", s.string(), prefixLength);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mAddress = address;
    mPrefixLength = prefixLength;
    mFlags = flags;
    mScope = scope;
    return NOERROR;
}

ECode LinkAddress::constructor(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 prefixLength,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 scope)
{
    return Init(address, prefixLength, flags, scope);
}

ECode LinkAddress::constructor(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 prefixLength)
{
    constructor(address, prefixLength, 0, 0);
    return ScopeForUnicastAddress(address, &mScope);
}

ECode LinkAddress::constructor(
    /* [in] */ IInterfaceAddress* interfaceAddress)
{
    return constructor(RETN_OUT_VAL(interfaceAddress, GetAddress),
            RETN_OUT_VAL(interfaceAddress, GetNetworkPrefixLength));
}

ECode LinkAddress::constructor(
    /* [in] */ const String& address)
{
    constructor(address, 0, 0);
    return ScopeForUnicastAddress(mAddress, &mScope);
}

ECode LinkAddress::constructor(
    /* [in] */ const String& address,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 scope)
{
    // This may throw an IllegalArgumentException; catching it is the caller's responsibility.
    AutoPtr<IPair> ipAndMask;
    NetworkUtils::ParseIpAndMask(address, (IPair**)&ipAndMask);
    Int32 second;
    IInteger32::Probe(RETN_OUT_VAL(ipAndMask, GetSecond))->GetValue(&second);
    return Init(IInetAddress::Probe(RETN_OUT_VAL(ipAndMask, GetFirst)),
            second, flags, scope);
}

ECode LinkAddress::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    result->AppendFormat("%s/%d", RETN_OUT_VAL(mAddress, GetHostAddress).string(), mPrefixLength);
    return NOERROR;
}

ECode LinkAddress::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) != IInterface::Probe(obj)) {
        *result = FALSE;
        return NOERROR;
    }
    if (ILinkAddress::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<LinkAddress> linkAddress = (LinkAddress*) ILinkAddress::Probe(obj);
    Boolean b;
    IObject::Probe(mAddress)->Equals(linkAddress->mAddress, &b);
    *result = b && mPrefixLength == linkAddress->mPrefixLength &&
            mFlags == linkAddress->mFlags &&
            mScope == linkAddress->mScope;
    return NOERROR;
}

ECode LinkAddress::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    IObject::Probe(mAddress)->GetHashCode(result);
    *result += 11 * mPrefixLength + 19 * mFlags + 43 * mScope;
    return NOERROR;
}

ECode LinkAddress::IsSameAddressAs(
    /* [in] */ ILinkAddress* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean b;
    IObject::Probe(mAddress)->Equals(((LinkAddress*)other)->mAddress, &b);
    *result = b && mPrefixLength == ((LinkAddress*)other)->mPrefixLength;
    return NOERROR;
}

ECode LinkAddress::GetAddress(
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mAddress;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode LinkAddress::GetPrefixLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPrefixLength;
    return NOERROR;
}

ECode LinkAddress::GetNetworkPrefixLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    GetPrefixLength(result);
    return NOERROR;
}

ECode LinkAddress::GetFlags(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mFlags;
    return NOERROR;
}

ECode LinkAddress::GetScope(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mScope;
    return NOERROR;
}

ECode LinkAddress::IsGlobalPreferred(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (mScope == OsConstants::_RT_SCOPE_UNIVERSE &&
            (mFlags & (OsConstants::_IFA_F_DADFAILED | OsConstants::_IFA_F_DEPRECATED | OsConstants::_IFA_F_TENTATIVE)) == 0L);
    return NOERROR;
}

ECode LinkAddress::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInterfacePtr((Handle32*)&mAddress);
    source->ReadInt32(&mPrefixLength);
    source->ReadInt32(&mFlags);
    source->ReadInt32(&mScope);
    return NOERROR;
}

ECode LinkAddress::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mAddress.Get());
    dest->WriteInt32(mPrefixLength);
    dest->WriteInt32(mFlags);
    dest->WriteInt32(mScope);
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
