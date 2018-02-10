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
        AutoPtr<IIterator> it;
        linkAddresses->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> address;
            it->GetNext((IInterface**)&address);
            mLinkAddresses->Add(address);
        }
        AutoPtr<IList> dnses;
        source->GetDnsServers((IList**)&dnses);
        it = NULL;
        dnses->GetIterator((IIterator**)&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> dns;
            it->GetNext((IInterface**)&dns);
            mDnses->Add(dns);
        }
        source->GetDomains(&mDomains);
        AutoPtr<IList> routes;
        source->GetRoutes((IList**)&routes);
        it = NULL;
        routes->GetIterator((IIterator**)&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> route;
            it->GetNext((IInterface**)&route);
            mRoutes->Add(route);
        }

        AutoPtr<IProxyInfo> proxy;
        source->GetHttpProxy((IProxyInfo**)&proxy);
        if (proxy != NULL) {
            CProxyInfo::New(proxy, (IProxyInfo**)&mHttpProxy);
        }
        AutoPtr<ICollection> values;
        ((LinkProperties*)source)->mStackedLinks->GetValues((ICollection**)&values);
        it = NULL;
        values->GetIterator((IIterator**)&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> lp;
            it->GetNext((IInterface**)&lp);
            Boolean result;
            AddStackedLink(ILinkProperties::Probe(lp), &result);
        }
        Int32 mtu;
        source->GetMtu(&mtu);
        SetMtu(mtu);
        mTcpBufferSizes = ((LinkProperties*)source)->mTcpBufferSizes;
    }
    return NOERROR;
}

ECode LinkProperties::SetInterfaceName(
    /* [in] */ const String& iface)
{
    mIfaceName = iface;
    Int32 size;
    mRoutes->GetSize(&size);
    AutoPtr<IArrayList> newRoutes;
    CArrayList::New(size, (IArrayList**)&newRoutes);
    AutoPtr<IIterator> it;
    mRoutes->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        newRoutes->Add(RouteWithInterface(IRouteInfo::Probe(obj)));
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

    Int32 size;
    mStackedLinks->GetSize(&size);
    AutoPtr<IList> interfaceNames;
    CArrayList::New(size + 1, (IList**)&interfaceNames);
    if (!mIfaceName.IsNull()) {
        AutoPtr<ICharSequence> csq;
        CString::New(mIfaceName, (ICharSequence**)&csq);
        interfaceNames->Add(csq);
    }
    AutoPtr<ICollection> values;
    mStackedLinks->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IList> l;
        ILinkProperties::Probe(obj)->GetAllInterfaceNames((IList**)&l);
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
    AutoPtr<IIterator> it;
    mLinkAddresses->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IInetAddress> address;
        ILinkAddress::Probe(obj)->GetAddress((IInetAddress**)&address);
        addresses->Add(address);
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
    AutoPtr<IIterator> it;
    mLinkAddresses->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IInetAddress> address;
        ILinkAddress::Probe(obj)->GetAddress((IInetAddress**)&address);
        addresses->Add(address);
    }
    AutoPtr<ICollection> links;
    mStackedLinks->GetValues((ICollection**)&links);
    it = NULL;
    links->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IList> allAddresses;
        ILinkProperties::Probe(obj)->GetAllAddresses((IList**)&allAddresses);
        addresses->AddAll(ICollection::Probe(allAddresses));
    }
    *result = addresses;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

Int32 LinkProperties::FindLinkAddressIndex(
    /* [in] */ ILinkAddress* address)
{
    Int32 size;
    mLinkAddresses->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mLinkAddresses->Get(i, ((IInterface**)&obj));
        Boolean same;
        if (ILinkAddress::Probe(obj)->IsSameAddressAs(address, &same), same) {
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
        AutoPtr<IInterface> linkAddress;
        mLinkAddresses->Get(i, (IInterface**)&linkAddress);
        if (Object::Equals(linkAddress, address)) {
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
    AutoPtr<ICollection> links;
    mStackedLinks->GetValues((ICollection**)&links);
    AutoPtr<IIterator> it;
    links->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> link;
        it->GetNext((IInterface**)&link);
        AutoPtr<IList> linkAddresses;
        ILinkProperties::Probe(link)->GetAllLinkAddresses((IList**)&linkAddresses);
        addresses->AddAll(ICollection::Probe(linkAddresses));
    }
    *result = addresses;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode LinkProperties::SetLinkAddresses(
    /* [in] */ ICollection* addresses)
{
    mLinkAddresses->Clear();
    AutoPtr<IIterator> it;
    addresses->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> link;
        it->GetNext((IInterface**)&link);
        Boolean result;
        AddLinkAddress(ILinkAddress::Probe(link), &result);
    }
    return NOERROR;
}

ECode LinkProperties::AddDnsServer(
    /* [in] */ IInetAddress* dnsServer,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean contains;
    if (dnsServer != NULL && (mDnses->Contains(dnsServer, &contains), !contains)) {
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
    AutoPtr<IIterator> it;
    dnsServers->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> dnsServer;
        it->GetNext((IInterface**)&dnsServer);
        Boolean result;
        AddDnsServer(IInetAddress::Probe(dnsServer), &result);
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
    AutoPtr<IIpPrefix> dest;
    route->GetDestination((IIpPrefix**)&dest);
    AutoPtr<IInetAddress> gateway;
    route->GetGateway((IInetAddress**)&gateway);
    Int32 type;
    route->GetType(&type);
    AutoPtr<IRouteInfo> rev;
    CRouteInfo::New(dest, gateway, mIfaceName, type, (IRouteInfo**)&rev);
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
        Boolean contains;
        if (mRoutes->Contains(route2, &contains), !contains) {
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

    String itf;
    *result = route != NULL &&
            (route->GetInterface(&itf), mIfaceName.Equals(itf)) &&
            (mRoutes->Remove(route, result), *result);
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
    AutoPtr<ICollection> links;
    mStackedLinks->GetValues((ICollection**)&links);
    AutoPtr<IIterator> it;
    links->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext) ,hasNext) {
        AutoPtr<IInterface> link;
        it->GetNext((IInterface**)&link);
        AutoPtr<IList> allRoutes;
        ILinkProperties::Probe(link)->GetAllRoutes((IList**)&allRoutes);
        routes->AddAll(ICollection::Probe(allRoutes));
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

    String itfName;
    if (link != NULL && (link->GetInterfaceName(&itfName), !itfName.IsNull())) {
        AutoPtr<ICharSequence> csq;
        CString::New(itfName, (ICharSequence**)&csq);
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

    String itfName;
    if (link != NULL && (link->GetInterfaceName(&itfName), !itfName.IsNull())) {
        AutoPtr<ICharSequence> csq;
        CString::New(itfName, (ICharSequence**)&csq);
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
    Boolean empty;
    if (mStackedLinks->IsEmpty(&empty), empty) {
        helper->GetEmptyList(result);
    }
    AutoPtr<IList> stacked;
    CArrayList::New((IList**)&stacked);
    AutoPtr<ICollection> links;
    mStackedLinks->GetValues((ICollection**)&links);
    AutoPtr<IIterator> it;
    links->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext) ,hasNext) {
        AutoPtr<IInterface> link;
        it->GetNext((IInterface**)&link);
        AutoPtr<ILinkProperties> newLink;
        CLinkProperties::New(ILinkProperties::Probe(link), (ILinkProperties**)&newLink);
        stacked->Add(newLink);
    }
    return helper->UnmodifiableList(stacked, result);
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
    AutoPtr<IIterator> it;
    mLinkAddresses->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> link;
        it->GetNext((IInterface**)&link);
        String s;
        IObject::Probe(link)->ToString(&s);
        linkAddresses += s + ",";
    }
    linkAddresses += "] ";

    String dns("DnsAddresses: [");
    it = NULL;
    mDnses->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        String hostAddr;
        IInetAddress::Probe(obj)->GetHostAddress(&hostAddr);
        dns += hostAddr + ",";
    }
    dns += "] ";

    String domainName = String("Domains: ") + mDomains;

    String mtu = String::Format(" MTU: %d", mMtu);

    String tcpBuffSizes("");
    if (mTcpBufferSizes != NULL) {
        tcpBuffSizes = " TcpBufferSizes: ";
        tcpBuffSizes += mTcpBufferSizes;
    }

    String routes(" Routes: [");
    it = NULL;
    mRoutes->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> route;
        it->GetNext((IInterface**)&route);
        String s;
        IObject::Probe(route)->ToString(&s);
        routes += s + ",";
    }
    routes += "] ";
    String proxy = (mHttpProxy == NULL ? String("") : String(" HttpProxy: ") + TO_STR(mHttpProxy) + " ");

    String stacked("");
    AutoPtr<ICollection> values;
    mStackedLinks->GetValues((ICollection**)&values);
    Int32 size;
    if (values->GetSize(&size), size > 0) {
        stacked += " Stacked: [";
        it = NULL;
        values->GetIterator((IIterator**)&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> link;
            it->GetNext((IInterface**)&link);
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

    AutoPtr<IIterator> it;
    mLinkAddresses->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> link;
        it->GetNext((IInterface**)&link);
        AutoPtr<IInetAddress> address;
        ILinkAddress::Probe(link)->GetAddress((IInetAddress**)&address);
        if (IInet4Address::Probe(address) != NULL) {
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

    AutoPtr<IIterator> it;
    mLinkAddresses->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> link;
        it->GetNext((IInterface**)&link);
        AutoPtr<IInetAddress> address;
        ILinkAddress::Probe(link)->GetAddress((IInetAddress**)&address);
        Boolean globalPreferred;
        if (IInet6Address::Probe(address) != NULL &&
            (ILinkAddress::Probe(link)->IsGlobalPreferred(&globalPreferred), globalPreferred)) {
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

    AutoPtr<IIterator> it;
    mRoutes->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> route;
        it->GetNext((IInterface**)&route);
        Boolean ipv4Default;
        if (IRouteInfo::Probe(route)->IsIPv4Default(&ipv4Default), ipv4Default) {
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

    AutoPtr<IIterator> it;
    mRoutes->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> route;
        it->GetNext((IInterface**)&route);
        Boolean ipv6Default;
        if (IRouteInfo::Probe(route)->IsIPv6Default(&ipv6Default), ipv6Default) {
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

    AutoPtr<IIterator> it;
    mDnses->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> dns;
        it->GetNext((IInterface**)&dns);
        if (IInet4Address::Probe(dns) != NULL) {
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

    AutoPtr<IIterator> it;
    mDnses->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> dns;
        it->GetNext((IInterface**)&dns);
        if (IInet6Address::Probe(dns) != NULL) {
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

    Boolean ipv4Address, ipv6Address, ipv6DefaultRoute, ipv6DnsServer;
    *result = (HasIPv4Address(&ipv4Address), ipv4Address) ||
            ((HasGlobalIPv6Address(&ipv6Address), ipv6Address) &&
             (HasIPv6DefaultRoute(&ipv6DefaultRoute), ipv6DefaultRoute) &&
             (HasIPv6DnsServer(&ipv6DnsServer), ipv6DnsServer));
    return NOERROR;
}

ECode LinkProperties::IsIdenticalInterfaceName(
    /* [in] */ ILinkProperties* target,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    String itfName1, itfName2;
    GetInterfaceName(&itfName1);
    target->GetInterfaceName(&itfName2);
    *result = TextUtils::Equals(itfName1, itfName2);
    return NOERROR;
}

ECode LinkProperties::IsIdenticalAddresses(
    /* [in] */ ILinkProperties* target,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IList> targetAddresses;
    target->GetAddresses((IList**)&targetAddresses);
    AutoPtr<IList> sourceAddresses;
    GetAddresses((IList**)&sourceAddresses);
    Int32 srcSize, tarSize;
    sourceAddresses->GetSize(&srcSize);
    targetAddresses->GetSize(&tarSize);
    if (srcSize == tarSize) {
        return sourceAddresses->ContainsAll(ICollection::Probe(targetAddresses), result);
    }
    return NOERROR;
}

ECode LinkProperties::IsIdenticalDnses(
    /* [in] */ ILinkProperties* target,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IList> targetDnses;
    target->GetDnsServers((IList**)&targetDnses);
    String targetDomains;
    target->GetDomains(&targetDomains);
    if (mDomains.IsNull()) {
        if (!targetDomains.IsNull()) {
            *result = FALSE;
            return NOERROR;
        }
    }
    else {
        if (!mDomains.Equals(targetDomains)) {
            *result = FALSE;
            return NOERROR;
        }
    }
    Int32 srcSize, tarSize;
    mDnses->GetSize(&srcSize);
    targetDnses->GetSize(&tarSize);
    if (srcSize == tarSize) {
        return mDnses->ContainsAll(ICollection::Probe(targetDnses), result);
    }
    return NOERROR;
}

ECode LinkProperties::IsIdenticalRoutes(
    /* [in] */ ILinkProperties* target,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IList> targetRoutes;
    target->GetRoutes((IList**)&targetRoutes);
    Int32 srcSize, tarSize;
    mRoutes->GetSize(&srcSize);
    targetRoutes->GetSize(&tarSize);
    if (srcSize == tarSize) {
        return mRoutes->ContainsAll(ICollection::Probe(targetRoutes), result);
    }
    return NOERROR;
}

ECode LinkProperties::IsIdenticalHttpProxy(
    /* [in] */ ILinkProperties* target,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IProxyInfo> srcProxy, tarProxy;
    GetHttpProxy((IProxyInfo**)&srcProxy);
    target->GetHttpProxy((IProxyInfo**)&tarProxy);
    *result = srcProxy == NULL ? tarProxy == NULL : Object::Equals(srcProxy, tarProxy);
    return NOERROR;
}

ECode LinkProperties::IsIdenticalStackedLinks(
    /* [in] */ ILinkProperties* target,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ISet> srcKeySet, tarKeySet;
    mStackedLinks->GetKeySet((ISet**)&srcKeySet);
    ((LinkProperties*)target)->mStackedLinks->GetKeySet((ISet**)&tarKeySet);
    Boolean equal;
    if (srcKeySet->Equals(tarKeySet, &equal), !equal) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<ICollection> values;
    mStackedLinks->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> link;
        it->GetNext((IInterface**)&link);
        // Hashtable values can never be null.
        String iface;
        ILinkProperties::Probe(link)->GetInterfaceName(&iface);
        AutoPtr<ICharSequence> csq;
        CString::New(iface, (ICharSequence**)&csq);
        AutoPtr<IInterface> value;
        ((LinkProperties*)target)->mStackedLinks->Get(csq, (IInterface**)&value);
        if (!Object::Equals(link, value)) {
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

    Int32 srcMtu, tarMtu;
    GetMtu(&srcMtu);
    target->GetMtu(&tarMtu);
    *result = srcMtu == tarMtu;
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

    if (!ILinkProperties::Probe(obj)) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<ILinkProperties> target = ILinkProperties::Probe(obj);
    /**
     * This method does not check that stacked interfaces are equal, because
     * stacked interfaces are not so much a property of the link as a
     * description of connections between links.
     */
    Boolean identical;
    *result = (IsIdenticalInterfaceName(target, &identical), identical) &&
              (IsIdenticalAddresses(target, &identical), identical) &&
              (IsIdenticalDnses(target, &identical), identical) &&
              (IsIdenticalRoutes(target, &identical), identical) &&
              (IsIdenticalHttpProxy(target, &identical), identical) &&
              (IsIdenticalStackedLinks(target, &identical), identical) &&
              (IsIdenticalMtu(target, &identical), identical) &&
              (IsIdenticalTcpBufferSizes(target, &identical), identical);
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
    AutoPtr<ILinkPropertiesCompareResult> compareResult;
    CLinkPropertiesCompareResult::New((ILinkPropertiesCompareResult**)&compareResult);
    AutoPtr<IList> links;
    CArrayList::New(ICollection::Probe(mLinkAddresses), (IList**)&links);
    compareResult->SetRemoved(links);
    AutoPtr<IList> added, removed;
    compareResult->GetRemoved((IList**)&removed);
    compareResult->GetAdded((IList**)&added);
    added->Clear();
    if (target != NULL) {
        AutoPtr<IList> tarLinks;
        target->GetLinkAddresses((IList**)&tarLinks);
        AutoPtr<IIterator> it;
        tarLinks->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> newAddress;
            it->GetNext((IInterface**)&newAddress);
            Boolean succeeded;
            if (removed->Remove(newAddress, &succeeded), !succeeded) {
                added->Add(newAddress);
            }
        }
    }
    *result = compareResult;
    REFCOUNT_ADD(*result);
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
    AutoPtr<ILinkPropertiesCompareResult> compareResult;
    CLinkPropertiesCompareResult::New((ILinkPropertiesCompareResult**)&compareResult);
    AutoPtr<IList> dnses;
    CArrayList::New(ICollection::Probe(mDnses), (IList**)&dnses);
    compareResult->SetRemoved(dnses);
    AutoPtr<IList> added, removed;
    compareResult->GetRemoved((IList**)&removed);
    compareResult->GetAdded((IList**)&added);
    added->Clear();
    if (target != NULL) {
        AutoPtr<IList> tarDnses;
        target->GetDnsServers((IList**)&tarDnses);
        AutoPtr<IIterator> it;
        tarDnses->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> newAddress;
            it->GetNext((IInterface**)&newAddress);
            Boolean succeeded;
            if (removed->Remove(newAddress, &succeeded), !succeeded) {
                added->Add(newAddress);
            }
        }
    }
    *result = compareResult;
    REFCOUNT_ADD(*result);
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
    AutoPtr<ILinkPropertiesCompareResult> compareResult;
    CLinkPropertiesCompareResult::New((ILinkPropertiesCompareResult**)&compareResult);
    AutoPtr<IList> routes;
    GetAllRoutes((IList**)&routes);
    compareResult->SetRemoved(routes);
    AutoPtr<IList> added, removed;
    compareResult->GetRemoved((IList**)&removed);
    compareResult->GetAdded((IList**)&added);
    added->Clear();
    if (target != NULL) {
        AutoPtr<IList> tarRoutes;
        target->GetAllRoutes((IList**)&tarRoutes);
        AutoPtr<IIterator> it;
        tarRoutes->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> r;
            it->GetNext((IInterface**)&r);
            Boolean succeeded;
            if (removed->Remove(r, &succeeded), !succeeded) {
                added->Add(r);
            }
        }
    }
    *result = compareResult;
    REFCOUNT_ADD(*result);
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
    AutoPtr<ILinkPropertiesCompareResult> compareResult;
    CLinkPropertiesCompareResult::New((ILinkPropertiesCompareResult**)&compareResult);
    AutoPtr<IList> interfaceNames;
    GetAllInterfaceNames((IList**)&interfaceNames);
    compareResult->SetRemoved(interfaceNames);
    AutoPtr<IList> added, removed;
    compareResult->GetRemoved((IList**)&removed);
    compareResult->GetAdded((IList**)&added);
    added->Clear();
    if (target != NULL) {
        AutoPtr<IList> tarInterfaceNames;
        target->GetAllInterfaceNames((IList**)&tarInterfaceNames);
        AutoPtr<IIterator> it;
        tarInterfaceNames->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> r;
            it->GetNext((IInterface**)&r);
            Boolean succeeded;
            if (removed->Remove(r, &succeeded), !succeeded) {
                added->Add(r);
            }
        }
    }
    *result = compareResult;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode LinkProperties::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = 0;
    if (!mIfaceName.IsNull()) {
        *result += mIfaceName.GetHashCode();
    }
    Int32 size;
    mLinkAddresses->GetSize(&size);
    *result += size * 31;
    mDnses->GetSize(&size);
    *result += size * 37;
    if (!mDomains.IsNull()) {
        *result += mDomains.GetHashCode();
    }
    mRoutes->GetSize(&size);
    *result += size * 41;
    if (mHttpProxy != NULL) {
        *result += Object::GetHashCode(mHttpProxy);
    }
    *result += Object::GetHashCode(mStackedLinks) * 47;
    *result += mMtu * 51;
    if (!mTcpBufferSizes.IsNull()) {
        *result += mTcpBufferSizes.GetHashCode();
    }
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
        parcel->ReadInterfacePtr((HANDLE*)&obj);
        Boolean result;
        AddLinkAddress(ILinkAddress::Probe(obj), &result);
    }

    Int32 dnsCount;
    parcel->ReadInt32(&dnsCount);
    for (Int32 i = 0; i < dnsCount; ++i) {
        AutoPtr<ArrayOf<Byte> > address;
        parcel->ReadArrayOf((HANDLE*)&address);
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
        parcel->ReadInterfacePtr((HANDLE*)&obj);
        Boolean result;
        AddRoute(IRouteInfo::Probe(obj), &result);
    }
    Byte hp;;
    parcel->ReadByte(&hp);
    if (hp == 1) {
        AutoPtr<IInterface> obj;
        parcel->ReadInterfacePtr((HANDLE*)&obj);
        SetHttpProxy(IProxyInfo::Probe(obj));
    }

    AutoPtr<ArrayOf<IInterface*> > array;
    parcel->ReadArrayOf((HANDLE*)&array);
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
        dest->WriteArrayOf((HANDLE)address.Get());
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
    dest->WriteArrayOf((HANDLE)array.Get());

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
    AutoPtr<IIterator> it;
    mRemoved->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        retVal += Object::ToString(obj) + ",";
    }
    retVal += "] added=[";
    it = NULL;
    mAdded->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        retVal += Object::ToString(obj) + ",";
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
