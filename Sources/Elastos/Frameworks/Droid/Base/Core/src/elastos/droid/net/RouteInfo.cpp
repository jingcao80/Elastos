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

#include "elastos/droid/net/RouteInfo.h"
#include "elastos/droid/net/CIpPrefix.h"
#include "elastos/droid/net/CLinkAddress.h"
#include "elastos/droid/net/CRouteInfo.h"
#include "elastos/droid/net/IpPrefix.h"
#include "elastos/droid/net/LinkProperties.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/net/NetworkUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Net::NetworkUtils;

using Elastos::Core::StringUtils;
using Elastos::Net::CInet4AddressHelper;
using Elastos::Net::CInet6AddressHelper;
using Elastos::Net::IInet4Address;
using Elastos::Net::IInet4AddressHelper;
using Elastos::Net::IInet6Address;
using Elastos::Net::IInet6AddressHelper;
using Elastos::Net::IInetAddress;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(RouteInfo, Object, IParcelable, IRouteInfo)

RouteInfo::RouteInfo()
    : mType(0)
    , mIsHost(FALSE)
    , mHasGateway(FALSE)
{}

ECode RouteInfo::constructor()
{
    return NOERROR;
}

ECode RouteInfo::constructor(
    /* [in] */ IIpPrefix* destination,
    /* [in] */ IInetAddress* gateway,
    /* [in] */ const String& iface,
    /* [in] */ Int32 type)
{
    switch (type) {
        case IRouteInfo::RTN_UNICAST:
        case IRouteInfo::RTN_UNREACHABLE:
        case IRouteInfo::RTN_THROW:
            // TODO: It would be nice to ensure that route types that don't have nexthops or
            // interfaces, such as unreachable or throw, can't be created if an interface or
            // a gateway is specified. This is a bit too complicated to do at the moment
            // because:
            //
            // - LinkProperties sets the interface on routes added to it, and modifies the
            //   interfaces of all the routes when its interface name changes.
            // - Even when the gateway is null, we store a non-null gateway here.
            //
            // For now, we just rely on the code that sets routes to do things properly.
            break;
        default:
            Logger::E("RouteInfo", "Unknown route type %d", type);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInet4AddressHelper> inet4AddressHelper;
    CInet4AddressHelper::AcquireSingleton((IInet4AddressHelper**)&inet4AddressHelper);
    AutoPtr<IInetAddress> net4any;
    inet4AddressHelper->GetANY((IInetAddress**)&net4any);
    if (destination == NULL) {
        if (gateway != NULL) {
            if (IInet4Address::Probe(gateway) != NULL) {
                 CIpPrefix::New(net4any, 0, &destination);
            } else {
                AutoPtr<IInet6AddressHelper> inet6Helper;
                CInet6AddressHelper::AcquireSingleton((IInet6AddressHelper**)&inet6Helper);
                AutoPtr<IInetAddress> net6any;
                inet6Helper->GetANY((IInetAddress**)&net6any);
                CIpPrefix::New(net6any, 0, &destination);
            }
        } else {
            // no destination, no gateway. invalid.
            Logger::E("RouteInfo", "Invalid arguments passed in: NULL, NULL");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    // TODO: set mGateway to null if there is no gateway. This is more correct, saves space, and
    // matches the documented behaviour. Before we can do this we need to fix all callers (e.g.,
    // ConnectivityService) to stop doing things like r.getGateway().equals(), ... .
    AutoPtr<IInetAddress> destAddress;
    destination->GetAddress((IInetAddress**)&destAddress);
    if (gateway == NULL) {
        if (IInet4Address::Probe(destAddress) != NULL) {
            gateway = net4any;
        } else {
            AutoPtr<IInet6AddressHelper> inet6Helper;
            CInet6AddressHelper::AcquireSingleton((IInet6AddressHelper**)&inet6Helper);
            AutoPtr<IInetAddress> net6any;
            inet6Helper->GetANY((IInetAddress**)&net6any);
            gateway = net6any;
        }
    }
    Boolean anyLocalAddress;
    gateway->IsAnyLocalAddress(&anyLocalAddress);
    mHasGateway = !anyLocalAddress;
    if ((IInet4Address::Probe(destAddress) != NULL &&
            IInet4Address::Probe(gateway) == NULL) ||
            (IInet6Address::Probe(destAddress) != NULL &&
            IInet6Address::Probe(gateway) == NULL)) {
        Logger::E("RouteInfo", "address family mismatch in RouteInfo constructor");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mDestination = destination;  // IpPrefix objects are immutable.
    mGateway = gateway;          // InetAddress objects are immutable.
    mInterface = iface;          // Strings are immutable.
    mType = type;
    mIsHost = IsHost();
    return NOERROR;
}

ECode RouteInfo::constructor(
    /* [in] */ IIpPrefix* destination,
    /* [in] */ IInetAddress* gateway,
    /* [in] */ const String& iface)
{
    return constructor(destination, gateway, iface, IRouteInfo::RTN_UNICAST);
}

ECode RouteInfo::constructor(
    /* [in] */ ILinkAddress* destination,
    /* [in] */ IInetAddress* gateway,
    /* [in] */ const String& iface)
{
    AutoPtr<IIpPrefix> ipPrefix;
    if (destination != NULL) {
        AutoPtr<IInetAddress> destAddress;
        destination->GetAddress((IInetAddress**)&destAddress);
        Int32 prefixLength;
        destination->GetPrefixLength(&prefixLength);
        CIpPrefix::New(destAddress, prefixLength, (IIpPrefix**)&ipPrefix);
    }
    return constructor(ipPrefix, gateway, iface);
}

ECode RouteInfo::constructor(
    /* [in] */ IIpPrefix* destination,
    /* [in] */ IInetAddress* gateway)
{
    return constructor(destination, gateway, String(NULL));
}

ECode RouteInfo::constructor(
    /* [in] */ ILinkAddress* destination,
    /* [in] */ IInetAddress* gateway)
{
    return constructor(destination, gateway, String(NULL));
}

ECode RouteInfo::constructor(
    /* [in] */ IInetAddress* gateway)
{
    return constructor((IIpPrefix*) NULL, gateway, String(NULL));
}

ECode RouteInfo::constructor(
    /* [in] */ IIpPrefix* destination)
{
    return constructor(destination, NULL, String(NULL));
}

ECode RouteInfo::constructor(
    /* [in] */ ILinkAddress* destination)
{
    return constructor(destination, NULL, String(NULL));
}

ECode RouteInfo::constructor(
    /* [in] */ IIpPrefix* destination,
    /* [in] */ Int32 type)
{
    return constructor(destination, NULL, String(NULL), type);
}

ECode RouteInfo::MakeHostRoute(
    /* [in] */ IInetAddress* host,
    /* [in] */ const String& iface,
    /* [out] */ IRouteInfo** result)
{
    return MakeHostRoute(host, NULL, iface, result);
}

ECode RouteInfo::MakeHostRoute(
    /* [in] */ IInetAddress* host,
    /* [in] */ IInetAddress* gateway,
    /* [in] */ const String& iface,
    /* [out] */ IRouteInfo** result)
{
    VALIDATE_NOT_NULL(result)

    if (host == NULL) {
        *result = NULL;
        return NOERROR;
    }
    if (IInet4Address::Probe(host) != NULL) {
        AutoPtr<IIpPrefix> newIpPrefix;
        CIpPrefix::New(host, 32, (IIpPrefix**)&newIpPrefix);
        return CRouteInfo::New(newIpPrefix, gateway, iface, result);
    }
    else {
        AutoPtr<IIpPrefix> newIpPrefix;
        CIpPrefix::New(host, 128, (IIpPrefix**)&newIpPrefix);
        return CRouteInfo::New(newIpPrefix, gateway, iface, result);
    }
    return NOERROR;
}

Boolean RouteInfo::IsHost()
{
    AutoPtr<IInetAddress> destAddress;
    mDestination->GetAddress((IInetAddress**)&destAddress);
    Int32 prefixLength;
    mDestination->GetPrefixLength(&prefixLength);
    return (IInet4Address::Probe(destAddress) != NULL && prefixLength == 32) ||
           (IInet6Address::Probe(destAddress) != NULL && prefixLength == 128);
}

ECode RouteInfo::GetDestination(
    /* [out] */ IIpPrefix** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mDestination;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode RouteInfo::GetDestinationLinkAddress(
    /* [out] */ ILinkAddress** result)
{
    AutoPtr<IInetAddress> address;
    mDestination->GetAddress((IInetAddress**)&address);
    Int32 prefixLength;
    mDestination->GetPrefixLength(&prefixLength);
    return CLinkAddress::New(address, prefixLength, result);
}

ECode RouteInfo::GetGateway(
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mGateway;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode RouteInfo::GetInterface(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mInterface;
    return NOERROR;
}

ECode RouteInfo::GetType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mType;
    return NOERROR;
}

ECode RouteInfo::IsDefaultRoute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 prefixLength;
    *result = mType == IRouteInfo::RTN_UNICAST &&
            (mDestination->GetPrefixLength(&prefixLength), prefixLength == 0);
    return NOERROR;
}

ECode RouteInfo::IsIPv4Default(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean defaultRoute;
    IsDefaultRoute(&defaultRoute);
    AutoPtr<IInetAddress> address;
    mDestination->GetAddress((IInetAddress**)&address);
    *result = defaultRoute && IInet4Address::Probe(address) != NULL;
    return NOERROR;
}

ECode RouteInfo::IsIPv6Default(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean defaultRoute;
    IsDefaultRoute(&defaultRoute);
    AutoPtr<IInetAddress> address;
    mDestination->GetAddress((IInetAddress**)&address);
    *result = defaultRoute && IInet6Address::Probe(address) != NULL;
    return NOERROR;
}

ECode RouteInfo::IsHostRoute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mIsHost;
    return NOERROR;
}

ECode RouteInfo::HasGateway(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mHasGateway;
    return NOERROR;
}

ECode RouteInfo::Matches(
    /* [in] */ IInetAddress* destination,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (destination == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    // match the route destination and destination with prefix length
    Int32 prefixLength;
    mDestination->GetPrefixLength(&prefixLength);
    AutoPtr<IInetAddress> dstNet;
    NetworkUtils::GetNetworkPart(destination, prefixLength, (IInetAddress**)&dstNet);
    AutoPtr<IInetAddress> srcNet;
    mDestination->GetAddress((IInetAddress**)&srcNet);
    return IObject::Probe(srcNet)->Equals(dstNet, result);
}

ECode RouteInfo::SelectBestRoute(
    /* [in] */ ICollection* routes,
    /* [in] */ IInetAddress* dest,
    /* [out] */ IRouteInfo** result)
{
    VALIDATE_NOT_NULL(result)

    if (routes == NULL || dest == NULL) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<RouteInfo> bestRoute;
    // pick a longest prefix match under same address type
    AutoPtr<IIterator> it;
    routes->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        RouteInfo* route = (RouteInfo*)IRouteInfo::Probe(obj);
        AutoPtr<IInetAddress> destAddress;
        route->mDestination->GetAddress((IInetAddress**)&destAddress);
        Boolean isMatch;
        if (NetworkUtils::AddressTypeMatches(destAddress, dest, &isMatch), isMatch) {
            Int32 bestPrefixLength, thisPrefixLength;
            if ((bestRoute != NULL) &&
                    (bestRoute->mDestination->GetPrefixLength(&bestPrefixLength),
                     route->mDestination->GetPrefixLength(&thisPrefixLength),
                     bestPrefixLength >= thisPrefixLength)) {
                continue;
            }
            if (route->Matches(dest, &isMatch), isMatch) bestRoute = route;
        }
    }
    *result = bestRoute;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode RouteInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String val("");
    if (mDestination != NULL) IObject::Probe(mDestination)->ToString(&val);
    if (mType == IRouteInfo::RTN_UNREACHABLE) {
        val += " unreachable";
    }
    else if (mType == IRouteInfo::RTN_THROW) {
        val += " throw";
    }
    else {
        val += " ->";
        if (mGateway != NULL) {
            String host;
            mGateway->GetHostAddress(&host);
            val += String(" ") + host;
        }
        if (mInterface != NULL) val += String(" ") + mInterface;
        if (mType != IRouteInfo::RTN_UNICAST) {
            val += String(" unknown type ") + StringUtils::ToString(mType);
        }
    }
    *result = val;
    return NOERROR;
}

ECode RouteInfo::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == IInterface::Probe(obj)) {
        *result = TRUE;
        return NOERROR;
    }
    if (IRouteInfo::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IRouteInfo> target = IRouteInfo::Probe(obj);
    AutoPtr<IIpPrefix> destination;
    target->GetDestination((IIpPrefix**)&destination);
    AutoPtr<IInetAddress> gateway;
    target->GetGateway((IInetAddress**)&gateway);
    String itf;
    target->GetInterface(&itf);
    Int32 type;
    target->GetType(&type);
    *result = Objects::Equals(mDestination, destination) &&
            Objects::Equals(mGateway, gateway) &&
            mInterface.Equals(itf) && mType == type;
    return NOERROR;
}

ECode RouteInfo::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 hashCode;
    IObject::Probe(mDestination)->GetHashCode(&hashCode);
    *result = (hashCode * 41);
    if (mGateway != NULL) {
        IObject::Probe(mGateway)->GetHashCode(&hashCode);
        *result += hashCode * 47;
    }
    if (mInterface != NULL) {
        hashCode = mInterface.GetHashCode();
        *result += hashCode * 67;
    }
    *result +=  mType * 71;
    return NOERROR;
}

ECode RouteInfo::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    AutoPtr<IInterface> obj;
    parcel->ReadInterfacePtr((Handle32*)&obj);
    mDestination = IIpPrefix::Probe(obj);
    obj = NULL;
    parcel->ReadInterfacePtr((Handle32*)&obj);
    mGateway = IInetAddress::Probe(obj);
    parcel->ReadString(&mInterface);
    parcel->ReadInt32(&mType);
    parcel->ReadBoolean(&mIsHost);
    parcel->ReadBoolean(&mHasGateway);
    return NOERROR;
}

ECode RouteInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mDestination.Get());
    dest->WriteInterfacePtr(mGateway.Get());
    dest->WriteString(mInterface);
    dest->WriteInt32(mType);
    dest->WriteBoolean(mIsHost);
    dest->WriteBoolean(mHasGateway);
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
