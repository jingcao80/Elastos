
#include "elastos/droid/net/RouteInfo.h"
#include "elastos/droid/net/CIpPrefix.h"
#include "elastos/droid/net/CLinkAddress.h"
#include "elastos/droid/net/CRouteInfo.h"
#include "elastos/droid/net/IpPrefix.h"
#include "elastos/droid/net/LinkProperties.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/net/ReturnOutValue.h"
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
                // TODO: Waiting for Inet6Address
                assert(0);
                // AutoPtr<IInet6AddressHelper> inet6Helper;
                // CInet6AddressHelper::AcquireSingleton((IInet6AddressHelper**)&inet6Helper)
                // AutoPtr<IInet6Address> net6any;
                // inet6Helper->GetANY((IInet6Address**)&net6any);
                // CIpPrefix::New(net6any, 0, &destination);
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
            // TODO: Waiting for Inet6Address
            assert(0);
            // AutoPtr<IInet6AddressHelper> inet6Helper;
            // CInet6AddressHelper::AcquireSingleton((IInet6AddressHelper**)&inet6Helper)
            // AutoPtr<IInet6Address> net6any;
            // inet6Helper->GetANY((IInet6Address**)&net6any);
            // gateway = net6any;
        }
    }
    mHasGateway = (!Ptr(gateway)->Func(gateway->IsAnyLocalAddress));
    if ((IInet4Address::Probe(destAddress) != NULL &&
            ((IInet4Address::Probe(gateway) != NULL) == FALSE)) ||
            (IInet6Address::Probe(destAddress) != NULL &&
            ((IInet6Address::Probe(gateway) != NULL) == FALSE))) {
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
    if (destination == NULL) {
    AutoPtr<IInetAddress> destAddress;
    destination->GetAddress((IInetAddress**)&destAddress);
        CIpPrefix::New(destAddress, Ptr(destination)->Func(destination->GetPrefixLength), (IIpPrefix**)&ipPrefix);
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

    if (host == NULL) FUNC_RETURN(NULL)
    if (IInet4Address::Probe(host) != NULL) {
        AutoPtr<IIpPrefix> newIpPrefix;
        CIpPrefix::New(host, 32, (IIpPrefix**)&newIpPrefix);
        return CRouteInfo::New(newIpPrefix, gateway, iface, result);
    } else {
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
    return ((IInet4Address::Probe(destAddress) != NULL) &&
            Ptr(mDestination)->Func(mDestination->GetPrefixLength) == 32) ||
           ((IInet6Address::Probe(destAddress) != NULL) &&
            Ptr(mDestination)->Func(mDestination->GetPrefixLength) == 128);
}

ECode RouteInfo::GetDestination(
    /* [out] */ IIpPrefix** result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mDestination)
}

ECode RouteInfo::GetDestinationLinkAddress(
    /* [out] */ ILinkAddress** result)
{
    return CLinkAddress::New(Ptr(mDestination)->Func(mDestination->GetAddress),
            Ptr(mDestination)->Func(mDestination->GetPrefixLength), result);
}

ECode RouteInfo::GetGateway(
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mGateway)
}

ECode RouteInfo::GetInterface(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mInterface)
}

ECode RouteInfo::GetType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mType)
}

ECode RouteInfo::IsDefaultRoute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mType == IRouteInfo::RTN_UNICAST && Ptr(mDestination)->Func(mDestination->GetPrefixLength) == 0;
    return NOERROR;
}

ECode RouteInfo::IsIPv4Default(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = Ptr(this)->Func(this->IsDefaultRoute) && IInet4Address::Probe(Ptr(mDestination)->Func(mDestination->GetAddress)) != NULL;
    return NOERROR;
}

ECode RouteInfo::IsIPv6Default(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = Ptr(this)->Func(this->IsDefaultRoute) && IInet6Address::Probe(Ptr(mDestination)->Func(mDestination->GetAddress)) != NULL;
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

    if (destination == NULL) FUNC_RETURN(FALSE)
    // match the route destination and destination with prefix length
    AutoPtr<IInetAddress> dstNet;
    NetworkUtils::GetNetworkPart(destination,
            Ptr(mDestination)->Func(mDestination->GetPrefixLength),
            (IInetAddress**)&dstNet);
    return IObject::Probe(Ptr(mDestination)->Func(mDestination->GetAddress))->Equals(dstNet, result);
}

ECode RouteInfo::SelectBestRoute(
    /* [in] */ ICollection* routes,
    /* [in] */ IInetAddress* dest,
    /* [out] */ IRouteInfo** result)
{
    VALIDATE_NOT_NULL(result)

    if ((routes == NULL) || (dest == NULL)) FUNC_RETURN(NULL)
    AutoPtr<IRouteInfo> bestRoute;
    // pick a longest prefix match under same address type
    FOR_EACH(iter, routes) {
        AutoPtr<IRouteInfo> route = IRouteInfo::Probe(Ptr(iter)->Func(iter->GetNext));
        AutoPtr<IInetAddress> destAddress;
        ((RouteInfo*)route.Get())->mDestination->GetAddress((IInetAddress**)&destAddress);
        Boolean isMatch;
        NetworkUtils::AddressTypeMatches(destAddress, dest, &isMatch);
        if (isMatch) {
            Int32 bestPrefixLength;
            ((RouteInfo*)bestRoute.Get())->mDestination->GetPrefixLength(&bestPrefixLength);
            Int32 thisPrefixLength;
            ((RouteInfo*)route.Get())->mDestination->GetPrefixLength(&thisPrefixLength);
            if ((bestRoute != NULL) &&
                    (bestPrefixLength >= thisPrefixLength)) {
                continue;
            }
            Boolean isMatch;
            route->Matches(dest, &isMatch);
            if (isMatch) bestRoute = route;
        }
    }
    FUNC_RETURN(bestRoute)
}

ECode RouteInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String val("");
    if (mDestination != NULL) IObject::Probe(mDestination)->ToString(&val);
    if (mType == IRouteInfo::RTN_UNREACHABLE) {
        val += " unreachable";
    } else if (mType == IRouteInfo::RTN_THROW) {
        val += " throw";
    } else {
        val += " ->";
        if (mGateway != NULL) val += String(" ") + Ptr(mGateway)->Func(mGateway->GetHostAddress);
        if (mInterface != NULL) val += String(" ") + mInterface;
        if (mType != IRouteInfo::RTN_UNICAST) {
            val += String(" unknown type ") + StringUtils::ToString(mType);
        }
    }
    FUNC_RETURN(val)
}

ECode RouteInfo::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == IInterface::Probe(obj)) FUNC_RETURN(TRUE)
    if (!(IRouteInfo::Probe(obj) != NULL)) FUNC_RETURN(FALSE)
    AutoPtr<IRouteInfo> target = IRouteInfo::Probe(obj);
    *result = Objects::Equals(mDestination, Ptr(target)->Func(target->GetDestination)) &&
            Objects::Equals(mGateway, Ptr(target)->Func(target->GetGateway)) &&
            mInterface.Equals(Ptr(target)->Func(target->GetInterface)) &&
            mType == Ptr(target)->Func(target->GetType);
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
