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

#include <Elastos.CoreLibrary.Net.h>
#include "elastos/droid/net/CLinkProperties.h"
#include "elastos/droid/net/CLinkPropertiesCompareResult.h"
#include "elastos/droid/net/CProxyInfo.h"
#include "elastos/droid/net/CRouteInfo.h"
#include "elastos/droid/net/LinkProperties.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/Singleton.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Text::TextUtils;

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Net::IInetAddressHelper;
using Elastos::Net::CInetAddressHelper;
using Elastos::Net::IInet4Address;
using Elastos::Net::IInet6Address;
using Elastos::Net::IInetAddress;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::CHashTable;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::IHashTable;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Net {

//====================================================================================
//              LinkProperties
//====================================================================================
CAR_INTERFACE_IMPL_2(LinkProperties, Object, ILinkProperties, IParcelable)

const Int32 LinkProperties::MIN_MTU = 68;
const Int32 LinkProperties::MIN_MTU_V6 = 1280;
const Int32 LinkProperties::MAX_MTU = 10000;

LinkProperties::LinkProperties()
{
    CArrayList::New((IArrayList**)&mLinkAddresses);
    CArrayList::New((IArrayList**)&mDnses);
    CArrayList::New((IArrayList**)&mRoutes);
    CHashTable::New((IHashTable**)&mStackedLinks);
}

ECode LinkProperties::constructor()
{
    return NOERROR;
}

ECode LinkProperties::constructor(
    /* [in] */ ILinkProperties* source)
{
    if (source != NULL) {
        source->GetInterfaceName(&mIfaceName);
        AutoPtr<IList> linkAddresses;
        source->GetLinkAddresses((IList**)&linkAddresses);
        AutoPtr<IIterator> iter;
        linkAddresses->GetIterator((IIterator**)&iter);
        AutoPtr<IInterface> l;
        while (RETN_OUT_VAL(iter, HasNext)) {
            iter->GetNext((IInterface**)&l);
            mLinkAddresses->Add(l);
            l = NULL;
        }
        AutoPtr<IList> dnses = RETN_OUT_VAL(source, GetDnsServers);
        iter = NULL;
        dnses->GetIterator((IIterator**)&iter);
        AutoPtr<IInterface> i;
        while (RETN_OUT_VAL(iter, HasNext)) {
            iter->GetNext((IInterface**)&i);
            mDnses->Add(i);
            i = NULL;
        }
        source->GetDomains(&mDomains);
        AutoPtr<IList> routes = (RETN_OUT_VAL(source, GetRoutes));
        iter = NULL;
        routes->GetIterator((IIterator**)&iter);
        AutoPtr<IInterface> r;
        while (RETN_OUT_VAL(iter, HasNext)) {
            iter->GetNext((IInterface**)&r);
            mRoutes->Add(r);
            r = NULL;
        }

        if (RETN_OUT_VAL(source, GetHttpProxy) == NULL) {
            mHttpProxy =  NULL;
        }
        else {
            CProxyInfo::New(RETN_OUT_VAL(source, GetHttpProxy), (IProxyInfo**)&mHttpProxy);
        }
        AutoPtr<ICollection> values;
        ((LinkProperties*)source)->mStackedLinks->GetValues((ICollection**)&values);
        iter = NULL;
        values->GetIterator((IIterator**)&iter);
        AutoPtr<IInterface> lp;
        while (RETN_OUT_VAL(iter, HasNext)) {
            iter->GetNext((IInterface**)&lp);
            RETN_OUT_VAL(this, AddStackedLink, ILinkProperties::Probe(lp));
        }
        SetMtu(RETN_OUT_VAL(source, GetMtu));
        mTcpBufferSizes = ((LinkProperties*)source)->mTcpBufferSizes;
    }
    return NOERROR;
}

ECode LinkProperties::SetInterfaceName(
    /* [in] */ const String& iface)
{
    mIfaceName = iface;
    AutoPtr<IArrayList> newRoutes;
    CArrayList::New(RETN_OUT_VAL(mRoutes, GetSize), (IArrayList**)&newRoutes);
    AutoPtr<IIterator> iter;
    mRoutes->GetIterator((IIterator**)&iter);
    while (RETN_OUT_VAL(iter, HasNext)) {
        newRoutes->Add(RouteWithInterface(IRouteInfo::Probe(RETN_OUT_VAL(iter, GetNext))));
    }
    mRoutes = newRoutes;
    return NOERROR;
}

ECode LinkProperties::GetInterfaceName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mIfaceName;
    return NOERROR;
}

ECode LinkProperties::GetAllInterfaceNames(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> interfaceNames;
    CArrayList::New(RETN_OUT_VAL(mStackedLinks, GetSize) + 1, (IList**)&interfaceNames);
    if (mIfaceName != String(NULL)) {
        AutoPtr<ICharSequence> csq;
        CString::New(mIfaceName, (ICharSequence**)&csq);
        interfaceNames->Add(csq);
    }
    AutoPtr<IIterator> iter;
    RETN_OUT_VAL(mStackedLinks, GetValues)->GetIterator((IIterator**)&iter);
    while (RETN_OUT_VAL(iter, HasNext)) {
        AutoPtr<IList> l;
        ILinkProperties::Probe(RETN_OUT_VAL(iter, GetNext))->GetAllInterfaceNames((IList**)&l);
        interfaceNames->AddAll(ICollection::Probe(l));
    }
    *result = interfaceNames;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode LinkProperties::GetAddresses(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> addresses;
    CArrayList::New((IList**)&addresses);
    FOR_EACH(iter, mLinkAddresses) {
        AutoPtr<ILinkAddress> linkAddress = ILinkAddress::Probe(RETN_OUT_VAL(iter, GetNext));
        addresses->Add(RETN_OUT_VAL(linkAddress, GetAddress));
    }
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    return helper->UnmodifiableList(addresses, result);
}

ECode LinkProperties::GetAllAddresses(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> addresses;
    CArrayList::New((IList**)&addresses);
    FOR_EACH(iter, mLinkAddresses) {
        AutoPtr<ILinkAddress> linkAddress = ILinkAddress::Probe(RETN_OUT_VAL(iter, GetNext));
        addresses->Add(RETN_OUT_VAL(linkAddress, GetAddress));
    }
    FOR_EACH(iter_stacked, RETN_OUT_VAL(mStackedLinks, GetValues)) {
        AutoPtr<ILinkProperties> stacked = ILinkProperties::Probe(ReturnOutValue(iter_stacked, iter_stacked->GetNext));
        addresses->AddAll(ICollection::Probe(RETN_OUT_VAL(stacked, GetAllAddresses)));
    }
    *result = addresses;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

Int32 LinkProperties::FindLinkAddressIndex(
    /* [in] */ ILinkAddress* address)
{
    for (Int32 i = 0; i < RETN_OUT_VAL(mLinkAddresses, GetSize); i++) {
        AutoPtr<ILinkAddress> linkAddress = ILinkAddress::Probe(RETN_OUT_VAL(mLinkAddresses, Get, i));
        if (RETN_OUT_VAL(linkAddress, IsSameAddressAs, address)) {
            return i;
        }
    }
    return -1;
}

ECode LinkProperties::AddLinkAddress(
    /* [in] */ ILinkAddress* address,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (address == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    Int32 i = FindLinkAddressIndex(address);
    if (i < 0) {
        // Address was not present. Add it.
        mLinkAddresses->Add(address);
        *result = TRUE;
        return NOERROR;
    }
    else {
        AutoPtr<ILinkAddress> linkAddress = ILinkAddress::Probe(RETN_OUT_VAL(mLinkAddresses, Get, i));
        Boolean isEquals;
        IObject::Probe(linkAddress)->Equals(address, &isEquals);
        if (isEquals) {
            // Address was present and has same properties. Do nothing.
            *result = FALSE;
            return NOERROR;
        }
        else {
            // Address was present and has different properties. Update it.
            mLinkAddresses->Set(i, address);
            *result = TRUE;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode LinkProperties::RemoveLinkAddress(
    /* [in] */ ILinkAddress* toRemove,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 i = FindLinkAddressIndex(toRemove);
    if (i >= 0) {
        mLinkAddresses->Remove(i);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode LinkProperties::GetLinkAddresses(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    return helper->UnmodifiableList(IList::Probe(mLinkAddresses), result);
}

ECode LinkProperties::GetAllLinkAddresses(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> addresses;
    CArrayList::New((IList**)&addresses);
    addresses->AddAll(ICollection::Probe(mLinkAddresses));
    FOR_EACH(iter, RETN_OUT_VAL(mStackedLinks, GetValues)) {
        AutoPtr<ILinkProperties> stacked = ILinkProperties::Probe(RETN_OUT_VAL(iter, GetNext));
        addresses->AddAll(ICollection::Probe(RETN_OUT_VAL(stacked, GetAllLinkAddresses)));
    }
    *result = addresses;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode LinkProperties::SetLinkAddresses(
    /* [in] */ ICollection* addresses)
{
    mLinkAddresses->Clear();
    FOR_EACH(iter, addresses) {
        AutoPtr<ILinkAddress> address = ILinkAddress::Probe(RETN_OUT_VAL(iter, GetNext));
        RETN_OUT_VAL(this, AddLinkAddress, address);
    }
    return NOERROR;
}

ECode LinkProperties::AddDnsServer(
    /* [in] */ IInetAddress* dnsServer,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (dnsServer != NULL && !RETN_OUT_VAL(mDnses, Contains, IInterface::Probe(dnsServer))) {
        mDnses->Add(dnsServer);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode LinkProperties::SetDnsServers(
    /* [in] */ ICollection* dnsServers)
{
    mDnses->Clear();
    FOR_EACH(iter, dnsServers) {
        AutoPtr<IInetAddress> dnsServer = IInetAddress::Probe(RETN_OUT_VAL(iter, GetNext));
        RETN_OUT_VAL(this, AddDnsServer, dnsServer);
    }
    return NOERROR;
}

ECode LinkProperties::GetDnsServers(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    return helper->UnmodifiableList(IList::Probe(mDnses), result);
}

ECode LinkProperties::SetDomains(
    /* [in] */ const String& domains)
{
    mDomains = domains;
    return NOERROR;
}

ECode LinkProperties::GetDomains(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mDomains;
    return NOERROR;
}

ECode LinkProperties::SetMtu(
    /* [in] */ Int32 mtu)
{
    mMtu = mtu;
    return NOERROR;
}

ECode LinkProperties::GetMtu(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mMtu;
    return NOERROR;
}

ECode LinkProperties::SetTcpBufferSizes(
    /* [in] */ const String& tcpBufferSizes)
{
    mTcpBufferSizes = tcpBufferSizes;
    return NOERROR;
}

ECode LinkProperties::GetTcpBufferSizes(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mTcpBufferSizes;
    return NOERROR;
}

AutoPtr<IRouteInfo> LinkProperties::RouteWithInterface(
    /* [in] */ IRouteInfo* route)
{
    AutoPtr<IRouteInfo> rev;
    CRouteInfo::New(
        RETN_OUT_VAL(route, GetDestination),
        RETN_OUT_VAL(route, GetGateway),
        mIfaceName,
        RETN_OUT_VAL(route, GetType),
        (IRouteInfo**)&rev);
    return rev;
}

ECode LinkProperties::AddRoute(
    /* [in] */ IRouteInfo* route,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (route != NULL) {
        String routeIface;
        route->GetInterface(&routeIface);
        if (routeIface != NULL && !routeIface.Equals(mIfaceName)) {
            Logger::E("LinkProperties", "Route added with non-matching interface: %s vs. %s"
                , routeIface.string(), mIfaceName.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<IRouteInfo> route2 = RouteWithInterface(route);
        if (!RETN_OUT_VAL(mRoutes, Contains, IInterface::Probe(route2))) {
            mRoutes->Add(route2);
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode LinkProperties::RemoveRoute(
    /* [in] */ IRouteInfo* route,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = route != NULL &&
            mIfaceName.Equals(RETN_OUT_VAL(route, GetInterface)) &&
            RETN_OUT_VAL(mRoutes, Remove, IInterface::Probe(route));
    return NOERROR;
}

ECode LinkProperties::GetRoutes(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    return helper->UnmodifiableList(IList::Probe(mRoutes), result);
}

ECode LinkProperties::GetAllRoutes(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> routes;
    CArrayList::New((IList**)&routes);
    routes->AddAll(ICollection::Probe(mRoutes));
    FOR_EACH(iter, RETN_OUT_VAL(mStackedLinks, GetValues)) {
        AutoPtr<ILinkProperties> stacked = ILinkProperties::Probe(RETN_OUT_VAL(iter, GetNext));
        routes->AddAll(ICollection::Probe(RETN_OUT_VAL(stacked, GetAllRoutes)));
    }
    *result = routes;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode LinkProperties::SetHttpProxy(
    /* [in] */ IProxyInfo* proxy)
{
    mHttpProxy = proxy;
    return NOERROR;
}

ECode LinkProperties::GetHttpProxy(
    /* [out] */ IProxyInfo** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mHttpProxy;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode LinkProperties::AddStackedLink(
    /* [in] */ ILinkProperties* link,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (link != NULL && RETN_OUT_VAL(link, GetInterfaceName) != NULL) {
        AutoPtr<ICharSequence> csq;
        CString::New(RETN_OUT_VAL(link, GetInterfaceName), (ICharSequence**)&csq);
        mStackedLinks->Put(csq, link);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode LinkProperties::RemoveStackedLink(
    /* [in] */ ILinkProperties* link,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (link != NULL && RETN_OUT_VAL(link, GetInterfaceName) != NULL) {
        AutoPtr<ICharSequence> csq;
        CString::New(RETN_OUT_VAL(link, GetInterfaceName), (ICharSequence**)&csq);
        AutoPtr<IInterface> removed;
        mStackedLinks->Remove(csq, (IInterface**)&removed);
        *result = removed != NULL;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode LinkProperties::GetStackedLinks(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    if (RETN_OUT_VAL(mStackedLinks, IsEmpty)) {
        helper->GetEmptyList(result);
    }
    AutoPtr<IList> stacked;
    CArrayList::New((IList**)&stacked);
    FOR_EACH(iter, RETN_OUT_VAL(mStackedLinks, GetValues)) {
        AutoPtr<ILinkProperties> link = ILinkProperties::Probe(RETN_OUT_VAL(iter, GetNext));
        AutoPtr<ILinkProperties> new_link;
        CLinkProperties::New(link, (ILinkProperties**)&new_link);
        stacked->Add(ReturnOutValue<ILinkProperties, ILinkProperties>(CLinkProperties::New, link));
    }
    return helper->UnmodifiableList(IList::Probe(stacked), result);
}

ECode LinkProperties::Clear()
{
    mIfaceName = NULL;
    mLinkAddresses->Clear();
    mDnses->Clear();
    mDomains = NULL;
    mRoutes->Clear();
    mHttpProxy = NULL;
    mStackedLinks->Clear();
    mMtu = 0;
    mTcpBufferSizes = NULL;
    return NOERROR;
}

ECode LinkProperties::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String ifaceName = (mIfaceName == NULL ? String("") : String("InterfaceName: ") + mIfaceName + " ");
    String linkAddresses("LinkAddresses: [");
    FOR_EACH(iter, mLinkAddresses) {
        AutoPtr<ILinkAddress> addr = ILinkAddress::Probe(RETN_OUT_VAL(iter, GetNext));
        String s;
        IObject::Probe(addr)->ToString(&s);
        linkAddresses += s + ",";
    }
    linkAddresses += "] ";
    String dns("DnsAddresses: [");
    FOR_EACH(iter_dns, mDnses) {
        AutoPtr<IInetAddress> addr = IInetAddress::Probe(ReturnOutValue(iter_dns, iter_dns->GetNext));
        dns += RETN_OUT_VAL(addr, GetHostAddress) + ",";
    }
    dns += "] ";
    String domainName = String("Domains: ") + mDomains;
    String mtu;
    mtu.AppendFormat(" MTU: %d", mMtu);
    String tcpBuffSizes("");
    if (mTcpBufferSizes != NULL) {
        tcpBuffSizes = " TcpBufferSizes: ";
        tcpBuffSizes += mTcpBufferSizes;
    }
    String routes(" Routes: [");
    FOR_EACH(iter_route, mRoutes) {
        AutoPtr<IRouteInfo> route = IRouteInfo::Probe(ReturnOutValue(iter_route, iter_route->GetNext));
        String s;
        IObject::Probe(route)->ToString(&s);
        routes += s + ",";
    }
    routes += "] ";
    String s;
    if (mHttpProxy != NULL)
        IObject::Probe(mHttpProxy)->ToString(&s);
    String proxy = (mHttpProxy == NULL ? String("") : String(" HttpProxy: ") + s + " ");
    String stacked("");
    AutoPtr<ICollection> values;
    mStackedLinks->GetValues((ICollection**)&values);
    if (RETN_OUT_VAL(values, GetSize) > 0) {
        stacked += " Stacked: [";
        FOR_EACH(iter, values) {
            AutoPtr<ILinkProperties> link = ILinkProperties::Probe(RETN_OUT_VAL(iter, GetNext));
            String s;
            IObject::Probe(link)->ToString(&s);
            stacked += String(" [") + s + " ],";
        }
        stacked += "] ";
    }
    *result = String("{") + ifaceName + linkAddresses + routes + dns + domainName + mtu
        + tcpBuffSizes + proxy + stacked + "}";
    return NOERROR;
}

ECode LinkProperties::HasIPv4Address(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    FOR_EACH(iter, mLinkAddresses) {
        AutoPtr<ILinkAddress> address = ILinkAddress::Probe(RETN_OUT_VAL(iter, GetNext));
        if (IInet4Address::Probe(RETN_OUT_VAL(address, GetAddress)) != NULL) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode LinkProperties::HasGlobalIPv6Address(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    FOR_EACH(iter, mLinkAddresses) {
        AutoPtr<ILinkAddress> address = ILinkAddress::Probe(RETN_OUT_VAL(iter, GetNext));
        if (IInet6Address::Probe(RETN_OUT_VAL(address, GetAddress)) != NULL  && RETN_OUT_VAL(address, IsGlobalPreferred)) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode LinkProperties::HasIPv4DefaultRoute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    FOR_EACH(iter, mRoutes) {
        AutoPtr<IRouteInfo> r = IRouteInfo::Probe(RETN_OUT_VAL(iter, GetNext));
        if (RETN_OUT_VAL(r, IsIPv4Default)) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode LinkProperties::HasIPv6DefaultRoute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    FOR_EACH(iter, mRoutes) {
        AutoPtr<IRouteInfo> r = IRouteInfo::Probe(RETN_OUT_VAL(iter, GetNext));
        if (RETN_OUT_VAL(r, IsIPv6Default)) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode LinkProperties::HasIPv4DnsServer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    FOR_EACH(iter, mDnses) {
        AutoPtr<IInetAddress> ia = IInetAddress::Probe(RETN_OUT_VAL(iter, GetNext));
        if (IInet4Address::Probe(ia) != NULL) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode LinkProperties::HasIPv6DnsServer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    FOR_EACH(iter, mDnses) {
        AutoPtr<IInetAddress> ia = IInetAddress::Probe(RETN_OUT_VAL(iter, GetNext));
        if (IInet6Address::Probe(ia) != NULL) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode LinkProperties::IsProvisioned(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (RETN_OUT_VAL(this, HasIPv4Address) ||
            (RETN_OUT_VAL(this, HasGlobalIPv6Address) && RETN_OUT_VAL(this, HasIPv6DefaultRoute) && RETN_OUT_VAL(this, HasIPv6DnsServer)));
    return NOERROR;
}

ECode LinkProperties::IsIdenticalInterfaceName(
    /* [in] */ ILinkProperties* target,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = TextUtils::Equals(RETN_OUT_VAL(this, GetInterfaceName), RETN_OUT_VAL(target, GetInterfaceName));
    return NOERROR;
}

ECode LinkProperties::IsIdenticalAddresses(
    /* [in] */ ILinkProperties* target,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> targetAddresses;
    target->GetAddresses((IList**)&targetAddresses);
    AutoPtr<IList> sourceAddresses;
    GetAddresses((IList**)&sourceAddresses);
    *result = (RETN_OUT_VAL(sourceAddresses, GetSize) == RETN_OUT_VAL(targetAddresses, GetSize)) ?
                RETN_OUT_VAL(sourceAddresses, ContainsAll, ICollection::Probe(targetAddresses)) : FALSE;
    return NOERROR;
}

ECode LinkProperties::IsIdenticalDnses(
    /* [in] */ ILinkProperties* target,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> targetDnses;
    target->GetDnsServers((IList**)&targetDnses);
    String targetDomains;
    target->GetDomains(&targetDomains);
    if (mDomains == NULL) {
        if (targetDomains != NULL) {
            *result = FALSE;
            return NOERROR;
        }
    }
    else {
        if (mDomains.Equals(targetDomains) == FALSE) {
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = (RETN_OUT_VAL(mDnses, GetSize) == RETN_OUT_VAL(targetDnses, GetSize)) ?
                RETN_OUT_VAL(mDnses, ContainsAll, ICollection::Probe(targetDnses)) : FALSE;
    return NOERROR;
}

ECode LinkProperties::IsIdenticalRoutes(
    /* [in] */ ILinkProperties* target,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> targetRoutes;
    target->GetRoutes((IList**)&targetRoutes);
    *result = (RETN_OUT_VAL(mRoutes, GetSize) == RETN_OUT_VAL(targetRoutes, GetSize)) ?
                RETN_OUT_VAL(mRoutes, ContainsAll, ICollection::Probe(targetRoutes)) : FALSE;
    return NOERROR;
}

ECode LinkProperties::IsIdenticalHttpProxy(
    /* [in] */ ILinkProperties* target,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj = IInterface::Probe(RETN_OUT_VAL(target, GetHttpProxy));
    *result = RETN_OUT_VAL(this, GetHttpProxy) == NULL ? RETN_OUT_VAL(target, GetHttpProxy) == NULL :
            RETN_OUT_VAL(IObject::Probe(RETN_OUT_VAL(this, GetHttpProxy)), Equals, obj);
    return NOERROR;
}

ECode LinkProperties::IsIdenticalStackedLinks(
    /* [in] */ ILinkProperties* target,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ISet> keySet;
    ((LinkProperties*)target)->mStackedLinks->GetKeySet((ISet**)&keySet);
    if (!RETN_OUT_VAL(IObject::Probe(RETN_OUT_VAL(mStackedLinks, GetKeySet)), Equals, IInterface::Probe(keySet))) {
        *result = FALSE;
        return NOERROR;
    }
    FOR_EACH(iter, RETN_OUT_VAL(mStackedLinks, GetValues)) {
        AutoPtr<ILinkProperties> stacked = ILinkProperties::Probe(RETN_OUT_VAL(iter, GetNext));
        // Hashtable values can never be null.
        String iface;
        stacked->GetInterfaceName(&iface);
        AutoPtr<ICharSequence> csq;
        CString::New(iface, (ICharSequence**)&csq);
        AutoPtr<IInterface> value;
        ((LinkProperties*)target)->mStackedLinks->Get(csq, (IInterface**)&value);
        if (!RETN_OUT_VAL(IObject::Probe(stacked), Equals, value)) {
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode LinkProperties::IsIdenticalMtu(
    /* [in] */ ILinkProperties* target,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = RETN_OUT_VAL(this, GetMtu) == RETN_OUT_VAL(target, GetMtu);
    return NOERROR;
}

ECode LinkProperties::IsIdenticalTcpBufferSizes(
    /* [in] */ ILinkProperties* target,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mTcpBufferSizes.Equals(((LinkProperties*)target)->mTcpBufferSizes);
    return NOERROR;
}

ECode LinkProperties::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == IInterface::Probe(obj)) {
        *result = TRUE;
        return NOERROR;
    }

    if (ILinkProperties::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<ILinkProperties> target = ILinkProperties::Probe(obj);
    /**
     * This method does not check that stacked interfaces are equal, because
     * stacked interfaces are not so much a property of the link as a
     * description of connections between links.
     */
    *result = RETN_OUT_VAL(this, IsIdenticalInterfaceName, target) &&
            RETN_OUT_VAL(this, IsIdenticalAddresses, target) &&
            RETN_OUT_VAL(this, IsIdenticalDnses, target) &&
            RETN_OUT_VAL(this, IsIdenticalRoutes, target) &&
            RETN_OUT_VAL(this, IsIdenticalHttpProxy, target) &&
            RETN_OUT_VAL(this, IsIdenticalStackedLinks, target) &&
            RETN_OUT_VAL(this, IsIdenticalMtu, target) &&
            RETN_OUT_VAL(this, IsIdenticalTcpBufferSizes, target);
    return NOERROR;
}

ECode LinkProperties::CompareAddresses(
    /* [in] */ ILinkProperties* target,
    /* [out] */ ILinkPropertiesCompareResult** result)
{
    VALIDATE_NOT_NULL(result)

    /*
     * Duplicate the LinkAddresses into removed, we will be removing
     * address which are common between mLinkAddresses and target
     * leaving the addresses that are different. And address which
     * are in target but not in mLinkAddresses are placed in the
     * addedAddresses.
     */
    CLinkPropertiesCompareResult::New(result);
    (*result)->SetRemoved(IList::Probe(ReturnOutValue<IArrayList, ICollection>(CArrayList::New, ICollection::Probe(mLinkAddresses))));
    RETN_OUT_VAL((*result), GetAdded)->Clear();
    if (target != NULL) {
        FOR_EACH(iter, RETN_OUT_VAL(target, GetLinkAddresses)) {
            AutoPtr<ILinkAddress> newAddress = ILinkAddress::Probe(RETN_OUT_VAL(iter, GetNext));
            if (!RETN_OUT_VAL(RETN_OUT_VAL((*result), GetRemoved), Remove, IInterface::Probe(newAddress))) {
                RETN_OUT_VAL((*result), GetAdded)->Add(newAddress);
            }
        }
    }
    return NOERROR;
}

ECode LinkProperties::CompareDnses(
    /* [in] */ ILinkProperties* target,
    /* [out] */ ILinkPropertiesCompareResult** result)
{
    VALIDATE_NOT_NULL(result)

    /*
     * Duplicate the InetAddresses into removed, we will be removing
     * dns address which are common between mDnses and target
     * leaving the addresses that are different. And dns address which
     * are in target but not in mDnses are placed in the
     * addedAddresses.
     */
    CLinkPropertiesCompareResult::New(result);
    (*result)->SetRemoved(ReturnOutValue<IList, ICollection>(CArrayList::New, ICollection::Probe(mDnses)));
    RETN_OUT_VAL((*result), GetAdded)->Clear();
    if (target != NULL) {
        FOR_EACH(iter, RETN_OUT_VAL(target, GetDnsServers)) {
            AutoPtr<IInetAddress> newAddress = IInetAddress::Probe(RETN_OUT_VAL(iter, GetNext));
            if (!RETN_OUT_VAL(RETN_OUT_VAL((*result), GetRemoved), Remove, IInterface::Probe(newAddress))) {
                RETN_OUT_VAL((*result), GetAdded)->Add(newAddress);
            }
        }
    }
    return NOERROR;
}

ECode LinkProperties::CompareAllRoutes(
    /* [in] */ ILinkProperties* target,
    /* [out] */ ILinkPropertiesCompareResult** result)
{
    VALIDATE_NOT_NULL(result)

    /*
     * Duplicate the RouteInfos into removed, we will be removing
     * routes which are common between mRoutes and target
     * leaving the routes that are different. And route address which
     * are in target but not in mRoutes are placed in added.
     */
    CLinkPropertiesCompareResult::New(result);
    (*result)->SetRemoved(RETN_OUT_VAL(this, GetAllRoutes));
    RETN_OUT_VAL((*result), GetAdded)->Clear();
    if (target != NULL) {
        FOR_EACH(iter, RETN_OUT_VAL(target, GetAllRoutes)) {
            AutoPtr<IRouteInfo> r = IRouteInfo::Probe(RETN_OUT_VAL(iter, GetNext));
            if (!RETN_OUT_VAL(RETN_OUT_VAL((*result), GetRemoved), Remove, IInterface::Probe(r))) {
                RETN_OUT_VAL((*result), GetAdded)->Add(r);
            }
        }
    }
    return NOERROR;
}

ECode LinkProperties::CompareAllInterfaceNames(
    /* [in] */ ILinkProperties* target,
    /* [out] */ ILinkPropertiesCompareResult** result)
{
    VALIDATE_NOT_NULL(result)

    /*
     * Duplicate the interface names into removed, we will be removing
     * interface names which are common between this and target
     * leaving the interface names that are different. And interface names which
     * are in target but not in this are placed in added.
     */
    CLinkPropertiesCompareResult::New(result);
    (*result)->SetRemoved(RETN_OUT_VAL(this, GetAllInterfaceNames));
    AutoPtr<IList> list;
    (*result)->GetAdded((IList**)&list);
    list->Clear();
    if (target != NULL) {
        FOR_EACH(iter, RETN_OUT_VAL(target, GetAllInterfaceNames)) {
            AutoPtr<ICharSequence> r = ICharSequence::Probe(RETN_OUT_VAL(iter, GetNext));
            if (!RETN_OUT_VAL(RETN_OUT_VAL((*result), GetRemoved), Remove, IInterface::Probe(r))) {
                RETN_OUT_VAL((*result), GetAdded)->Add(r);
            }
        }
    }
    return NOERROR;
}

ECode LinkProperties::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = ((String(NULL) == mIfaceName) ? 0 : mIfaceName.GetHashCode())
            + RETN_OUT_VAL(mLinkAddresses, GetSize) * 31
            + RETN_OUT_VAL(mDnses, GetSize) * 37
            + (((String(NULL) == mDomains) ? 0 : mDomains.GetHashCode()))
            + RETN_OUT_VAL(mRoutes, GetSize) * 41
            + ((NULL == mHttpProxy) ? 0 : RETN_OUT_VAL(IObject::Probe(mHttpProxy), GetHashCode))
            + RETN_OUT_VAL(mStackedLinks, GetHashCode) * 47
            + mMtu * 51
            + ((String(NULL) == mTcpBufferSizes) ? 0 : mTcpBufferSizes.GetHashCode());
    return NOERROR;
}

ECode LinkProperties::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->ReadString(&mIfaceName);

    Int32 addressCount;
    parcel->ReadInt32(&addressCount);
    for (Int32 i = 0; i < addressCount; ++i) {
        AutoPtr<IInterface> obj;
        parcel->ReadInterfacePtr((Handle32*)&obj);
        Boolean result;
        AddLinkAddress(ILinkAddress::Probe(obj), &result);
    }

    Int32 dnsCount;
    parcel->ReadInt32(&dnsCount);
    for (Int32 i = 0; i < dnsCount; ++i) {
        AutoPtr<ArrayOf<Byte> > address;
        parcel->ReadArrayOf((Handle32*)&address);
        AutoPtr<IInetAddressHelper> naHelper;
        CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&naHelper);
        AutoPtr<IInetAddress> netAddress;
        naHelper->GetByAddress(address, (IInetAddress**)&netAddress);
        Boolean result;
        AddDnsServer(netAddress, &result);
    }

    parcel->ReadString(&mDomains);
    parcel->ReadInt32(&mMtu);
    parcel->ReadString(&mTcpBufferSizes);

    Int32 routeCount;
    parcel->ReadInt32(&routeCount);
    for (Int32 i = 0; i < routeCount; ++i) {
        AutoPtr<IInterface> obj;
        parcel->ReadInterfacePtr((Handle32*)&obj);
        Boolean result;
        AddRoute(IRouteInfo::Probe(obj), &result);
    }
    Byte hp;;
    parcel->ReadByte(&hp);
    if (hp == 1) {
        AutoPtr<IInterface> obj;
        parcel->ReadInterfacePtr((Handle32*)&obj);
        SetHttpProxy(IProxyInfo::Probe(obj));
    }

    AutoPtr<ArrayOf<IInterface*> > array;
    parcel->ReadArrayOf((Handle32*)&array);
    for (Int32 i = 0; i < array->GetLength(); ++i) {
        AutoPtr<ILinkProperties> lp = ILinkProperties::Probe((*array)[i]);
        Boolean result;
        AddStackedLink(lp, &result);
    }
    return NOERROR;
}

ECode LinkProperties::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mIfaceName);
    Int32 size;
    mLinkAddresses->GetSize(&size);
    dest->WriteInt32(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mLinkAddresses->Get(i, (IInterface**)&obj);
        dest->WriteInterfacePtr(obj);
    }

    size = 0;
    mDnses->GetSize(&size);
    dest->WriteInt32(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mDnses->Get(i, (IInterface**)&obj);
        IInetAddress* d = IInetAddress::Probe(obj);
        AutoPtr<ArrayOf<Byte> > address;
        d->GetAddress((ArrayOf<Byte>**)&address);
        dest->WriteArrayOf((Handle32)address.Get());
    }

    dest->WriteString(mDomains);
    dest->WriteInt32(mMtu);
    dest->WriteString(mTcpBufferSizes);

    size = 0;
    mRoutes->GetSize(&size);
    dest->WriteInt32(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mRoutes->Get(i, (IInterface**)&obj);
        dest->WriteInterfacePtr(obj);
    }
    if (mHttpProxy != NULL) {
        dest->WriteByte(1);
        dest->WriteInterfacePtr(mHttpProxy);
    }
    else {
        dest->WriteByte(0);
    }

    AutoPtr<ICollection> values;
    mStackedLinks->GetValues((ICollection**)&values);
    AutoPtr<ArrayOf<IInterface*> > array;
    values->ToArray((ArrayOf<IInterface*>**)&array);
    dest->WriteArrayOf((Handle32)array.Get());

    return NOERROR;
}


ECode LinkProperties::IsValidMtu(
    /* [in] */ Int32 mtu,
    /* [in] */ Boolean ipv6,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (ipv6) {
        if ((mtu >= MIN_MTU_V6 && mtu <= MAX_MTU)) {
            *result = TRUE;
            return NOERROR;
        }
    }
    else {
        if ((mtu >= MIN_MTU && mtu <= MAX_MTU)) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

//====================================================================================
//              LinkPropertiesCompareResult
//====================================================================================
CAR_INTERFACE_IMPL(LinkPropertiesCompareResult, Object, ILinkPropertiesCompareResult)

ECode LinkPropertiesCompareResult::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String retVal("removed=[");
    FOR_EACH(iter, mRemoved) {
        AutoPtr<IObject> addr = IObject::Probe(RETN_OUT_VAL(iter, GetNext));
        retVal += RETN_OUT_VAL(addr, ToString) + ",";
    }
    retVal += "] added=[";
    FOR_EACH (iter_added, mAdded) {
        AutoPtr<IObject> addr = IObject::Probe(RETN_OUT_VAL(iter_added, GetNext));
        retVal += RETN_OUT_VAL(addr, ToString) + ",";
    }
    retVal += "]";
    *result = retVal;
    return NOERROR;
}

ECode LinkPropertiesCompareResult::GetRemoved(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mRemoved;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode LinkPropertiesCompareResult::SetRemoved(
    /* [in] */ IList* removed)
{
    VALIDATE_NOT_NULL(removed)

    mRemoved = removed;
    return NOERROR;
}

ECode LinkPropertiesCompareResult::GetAdded(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mAdded;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode LinkPropertiesCompareResult::SetAdded(
    /* [in] */ IList* added)
{
    VALIDATE_NOT_NULL(added)

    mAdded = added;
    return NOERROR;
}

LinkPropertiesCompareResult::LinkPropertiesCompareResult()
{
    CArrayList::New((IList**)&mRemoved);
    CArrayList::New((IList**)&mAdded);
}

ECode LinkPropertiesCompareResult::constructor()
{
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
