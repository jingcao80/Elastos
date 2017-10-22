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

#include "Elastos.Droid.Utility.h"
#include "elastos/droid/net/LinkAddress.h"
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

    Boolean anyLocalAddr;
    if (addr->IsAnyLocalAddress(&anyLocalAddr), anyLocalAddr) {
        *result = OsConstants::_RT_SCOPE_HOST;
        return NOERROR;
    }

    Boolean loopbackAddr, linkLocalAddr;
    if ((addr->IsLoopbackAddress(&loopbackAddr), loopbackAddr) ||
        (addr->IsLinkLocalAddress(&linkLocalAddr), linkLocalAddr)) {
        *result = OsConstants::_RT_SCOPE_LINK;
        return NOERROR;
    }

    // isSiteLocalAddress() returns true for private IPv4 addresses, but RFC 6724 section 3.2
    // says that they are assigned global scope.
    Boolean siteLocalAddr;
    if ((IInet4Address::Probe(addr) == NULL) &&
        (addr->IsSiteLocalAddress(&siteLocalAddr), siteLocalAddr)) {
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
    Boolean multicastAddr;
    if (address == NULL ||
            (address->IsMulticastAddress(&multicastAddr), multicastAddr) ||
            prefixLength < 0 ||
            ((IInet4Address::Probe(address) != NULL ) && prefixLength > 32) ||
            (prefixLength > 128)) {
        Logger::E("LinkAddress", "Bad LinkAddress params %s/%d", TO_CSTR(address), prefixLength);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mAddress = address;
    mPrefixLength = prefixLength;
    mFlags = flags;
    mScope = scope;
    return NOERROR;
}

ECode LinkAddress::constructor()
{
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
    AutoPtr<IInetAddress> address;
    interfaceAddress->GetAddress((IInetAddress**)&address);
    Int16 prefixLength;
    interfaceAddress->GetNetworkPrefixLength(&prefixLength);
    return constructor(address, prefixLength);
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
    AutoPtr<IInterface> first, second;
    ipAndMask->GetFirst((IInterface**)&first);
    ipAndMask->GetSecond((IInterface**)&second);
    Int32 prefixLength;
    IInteger32::Probe(second)->GetValue(&prefixLength);
    return Init(IInetAddress::Probe(first), prefixLength, flags, scope);
}

ECode LinkAddress::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String address("");
    if (mAddress != NULL) {
        mAddress->GetHostAddress(&address);
    }
    result->AppendFormat("%s/%d", address.string(), mPrefixLength);
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
    mAddress = NULL;
    NetworkUtils::UnparcelInetAddress(source, (IInetAddress**)&mAddress);
    source->ReadInt32(&mPrefixLength);
    source->ReadInt32(&mFlags);
    source->ReadInt32(&mScope);
    return NOERROR;
}

ECode LinkAddress::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    NetworkUtils::ParcelInetAddress(dest, mAddress, 0);
    dest->WriteInt32(mPrefixLength);
    dest->WriteInt32(mFlags);
    dest->WriteInt32(mScope);
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
