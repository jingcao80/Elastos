#include "elastos/droid/server/net/BaseNetworkObserver.h"

using Elastos::Droid::Net::EIID_IINetworkManagementEventObserver;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

CAR_INTERFACE_IMPL_2(BaseNetworkObserver, Object, IINetworkManagementEventObserver, IBinder)

BaseNetworkObserver::BaseNetworkObserver()
{
}

BaseNetworkObserver::~BaseNetworkObserver()
{
}

ECode BaseNetworkObserver::constructor()
{
    return NOERROR;
}

ECode BaseNetworkObserver::InterfaceStatusChanged(
    /* [in] */ const String& iface,
    /* [in] */ Boolean up)
{
    // default no-op
    return NOERROR;
}

ECode BaseNetworkObserver::InterfaceRemoved(
    /* [in] */ const String& iface)
{
    // default no-op
    return NOERROR;
}

ECode BaseNetworkObserver::AddressUpdated(
    /* [in] */ const String& iface,
    /* [in] */ ILinkAddress* address)
{
    // default no-op
    return NOERROR;
}

ECode BaseNetworkObserver::AddressRemoved(
    /* [in] */ const String& iface,
    /* [in] */ ILinkAddress* address)
{
    // default no-op
    return NOERROR;
}

ECode BaseNetworkObserver::InterfaceLinkStateChanged(
    /* [in] */ const String& iface,
    /* [in] */ Boolean up)
{
    // default no-op
    return NOERROR;
}

ECode BaseNetworkObserver::InterfaceAdded(
    /* [in] */ const String& iface)
{
    // default no-op
    return NOERROR;
}

ECode BaseNetworkObserver::InterfaceClassDataActivityChanged(
    /* [in] */ const String& label,
    /* [in] */ Boolean active,
    /* [in] */ Int64 tsNanos)
{
    // default no-op
    return NOERROR;
}

ECode BaseNetworkObserver::LimitReached(
    /* [in] */ const String& limitName,
    /* [in] */ const String& iface)
{
    // default no-op
    return NOERROR;
}

ECode BaseNetworkObserver::InterfaceDnsServerInfo(
    /* [in] */ const String& iface,
    /* [in] */ Int64 lifetime,
    /* [in] */ ArrayOf<String>* servers)
{
    // default no-op
    return NOERROR;
}

ECode BaseNetworkObserver::RouteUpdated(
    /* [in] */ IRouteInfo* route)
{
    // default no-op
    return NOERROR;
}

ECode BaseNetworkObserver::RouteRemoved(
    /* [in] */ IRouteInfo* route)
{
    // default no-op
    return NOERROR;
}

ECode BaseNetworkObserver::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} // namespace Net
} // namespace Server
} // namespace Droid
} // namespace Elastos