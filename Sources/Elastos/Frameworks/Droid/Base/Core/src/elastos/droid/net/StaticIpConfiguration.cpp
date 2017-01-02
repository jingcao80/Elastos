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

#include "elastos/droid/net/StaticIpConfiguration.h"
#include "elastos/droid/net/CLinkProperties.h"
#include "elastos/droid/net/CRouteInfo.h"
#include "elastos/droid/net/IpConfiguration.h"
#include "elastos/droid/net/LinkProperties.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/net/NetworkUtils.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Net::ILinkAddress;

using Elastos::Core::StringBuffer;
using Elastos::Net::IInetAddress;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(StaticIpConfiguration, Object, IParcelable, IStaticIpConfiguration)

ECode StaticIpConfiguration::constructor()
{
    return CArrayList::New((IArrayList**)&mDnsServers);
}

ECode StaticIpConfiguration::constructor(
    /* [in] */ IStaticIpConfiguration* source)
{
    constructor();
    if (source != NULL) {
        // All of these except dnsServers are immutable, so no need to make copies.
        mIpAddress = ((StaticIpConfiguration*) source)->mIpAddress;
        mGateway = ((StaticIpConfiguration*) source)->mGateway;
        Boolean b;
        mDnsServers->AddAll(ICollection::Probe(((StaticIpConfiguration*) source)->mDnsServers), &b);
        mDomains = ((StaticIpConfiguration*) source)->mDomains;
    }
    return NOERROR;
}

ECode StaticIpConfiguration::Clear()
{
    mIpAddress = NULL;
    mGateway = NULL;
    mDnsServers->Clear();
    mDomains = NULL;
    return NOERROR;
}

ECode StaticIpConfiguration::GetRoutes(
    /* [in] */ const String& iface,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    CArrayList::New(2, result);
    if (mIpAddress != NULL) {
        AutoPtr<IRouteInfo> newRouteInfo;
        CRouteInfo::New(mIpAddress, NULL, iface, (IRouteInfo**)&newRouteInfo);
        (*result)->Add(newRouteInfo);
    }
    if (mGateway != NULL) {
        AutoPtr<IRouteInfo> newRouteInfo;
        CRouteInfo::New((ILinkAddress*) NULL, mGateway, iface, (IRouteInfo**)&newRouteInfo);
        (*result)->Add(newRouteInfo);
    }
    return NOERROR;
}

ECode StaticIpConfiguration::ToLinkProperties(
    /* [in] */ const String& iface,
    /* [out] */ ILinkProperties** result)
{
    VALIDATE_NOT_NULL(result)

    CLinkProperties::New(result);
    (*result)->SetInterfaceName(iface);
    if (mIpAddress != NULL) {
        Boolean b;
        (*result)->AddLinkAddress(mIpAddress, &b);
    }
    AutoPtr<IList> routes;
    GetRoutes(iface, (IList**)&routes);
    FOR_EACH(iter, routes) {
        AutoPtr<IRouteInfo> route = IRouteInfo::Probe(Ptr(iter)->Func(iter->GetNext));
        Boolean b;
        (*result)->AddRoute(route, &b);
    }
    FOR_EACH(iter_dns, mDnsServers) {
        AutoPtr<IInetAddress> dns = IInetAddress::Probe(Ptr(iter_dns)->Func(iter_dns->GetNext));
        Boolean b;
        (*result)->AddDnsServer(dns, &b);
    }
    return NOERROR;
}

ECode StaticIpConfiguration::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuffer str;
    str.Append("IP address ");
    String s;
    if (mIpAddress != NULL ) {
        IObject::Probe(mIpAddress)->ToString(&s);
        str.Append(s);
        str.Append(" ");
    }
    str.Append("Gateway ");
    if (mGateway != NULL) {
        mGateway->GetHostAddress(&s);
        str.Append(s);
        str.Append(" ");
    }
    str.Append(" DNS servers: [");
    FOR_EACH(iter_dns, mDnsServers) {
        AutoPtr<IInetAddress> dns = IInetAddress::Probe(Ptr(iter_dns)->Func(iter_dns->GetNext));
        str.Append(" ");
        dns->GetHostAddress(&s);
        str.Append(s);
    }
    str.Append(" ] Domains");
    if (mDomains != NULL) {
        str.Append(mDomains);
    }
    *result = str.ToString();
    return NOERROR;
}

ECode StaticIpConfiguration::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 rev = 13;
    rev = 47 * rev + (mIpAddress == NULL ? 0 : Ptr(IObject::Probe(mIpAddress))->Func(IObject::Probe(mIpAddress)->GetHashCode));
    rev = 47 * rev + (mGateway == NULL ? 0 : Ptr(IObject::Probe(mGateway))->Func(IObject::Probe(mGateway)->GetHashCode));
    rev = 47 * rev + (mDomains == NULL ? 0 : mDomains.GetHashCode());
    rev = 47 * rev + Ptr(IObject::Probe(mDnsServers))->Func(IObject::Probe(mDnsServers)->GetHashCode);
    *result = rev;
    return NOERROR;
}

ECode StaticIpConfiguration::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == IInterface::Probe(obj)) FUNC_RETURN(TRUE)
    if (!(IStaticIpConfiguration::Probe(obj) != NULL)) FUNC_RETURN(FALSE)
    AutoPtr<StaticIpConfiguration> other = (StaticIpConfiguration*) IStaticIpConfiguration::Probe(obj);
    *result = other != NULL &&
            Objects::Equals(mIpAddress, other->mIpAddress) &&
            Objects::Equals(mGateway, other->mGateway) &&
            Objects::Equals(mDnsServers, other->mDnsServers) &&
            mDomains.Equals(other->mDomains);
    return NOERROR;
}

ECode StaticIpConfiguration::ReadFromParcel(
        /* [in] */ IParcel* parcel)
{
    AutoPtr<IInterface> obj;
    parcel->ReadInterfacePtr((Handle32*)&obj);
    mIpAddress = ILinkAddress::Probe(obj);
    NetworkUtils::UnparcelInetAddress(parcel, (IInetAddress**)&mGateway);
    Int32 size;
    parcel->ReadInt32(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInetAddress> iAddress;
        NetworkUtils::UnparcelInetAddress(parcel, (IInetAddress**)&iAddress);
        mDnsServers->Add(iAddress);
    }
    return NOERROR;
}

ECode StaticIpConfiguration::WriteToParcel(
        /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mIpAddress.Get());
    NetworkUtils::ParcelInetAddress(dest, mGateway, 0);
    Int32 size = 0;
    mDnsServers->GetSize(&size);
    dest->WriteInt32(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mDnsServers->Get(i, (IInterface**)&obj);
        IInetAddress* dnsServer = IInetAddress::Probe(obj);
        NetworkUtils::ParcelInetAddress(dest, dnsServer, 0);
    }
    return NOERROR;
}

ECode StaticIpConfiguration::GetIpAddress(
    /* [out] */ ILinkAddress** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mIpAddress;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode StaticIpConfiguration::SetIpAddress(
    /* [in] */ ILinkAddress* ipAddress)
{
    VALIDATE_NOT_NULL(ipAddress)

    mIpAddress = ipAddress;
    return NOERROR;
}

ECode StaticIpConfiguration::GetGateway(
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mGateway;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode StaticIpConfiguration::SetGateway(
    /* [in] */ IInetAddress* gateway)
{
    VALIDATE_NOT_NULL(gateway)

    mGateway = gateway;
    return NOERROR;
}

ECode StaticIpConfiguration::GetDnsServers(
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mDnsServers;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode StaticIpConfiguration::GetDomains(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mDomains;
    return NOERROR;
}

ECode StaticIpConfiguration::SetDomains(
    /* [in] */ const String& domains)
{
    mDomains = domains;
    return NOERROR;
}


} // namespace Net
} // namespace Droid
} // namespace Elastos
