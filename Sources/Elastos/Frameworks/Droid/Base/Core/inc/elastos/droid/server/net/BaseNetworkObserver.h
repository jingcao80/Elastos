#ifndef __ELASTOS_DROID_SERVER_NET_BASENETWORKOBSERVER_H__
#define __ELASTOS_DROID_SERVER_NET_BASENETWORKOBSERVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IINetworkManagementEventObserver;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::IRouteInfo;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

/**
 * Base {@link INetworkManagementEventObserver} that provides no-op
 * implementations which can be overridden.
 *
 * @hide
 */
class ECO_PUBLIC BaseNetworkObserver
    : public Object
    , public IINetworkManagementEventObserver
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    BaseNetworkObserver();

    virtual ~BaseNetworkObserver();

    CARAPI constructor();

    //@Override
    CARAPI InterfaceStatusChanged(
        /* [in] */ const String& iface,
        /* [in] */ Boolean up);

    //@Override
    CARAPI InterfaceRemoved(
        /* [in] */ const String& iface);

    //@Override
    CARAPI AddressUpdated(
        /* [in] */ const String& iface,
        /* [in] */ ILinkAddress* address);

    //@Override
    CARAPI AddressRemoved(
        /* [in] */ const String& iface,
        /* [in] */ ILinkAddress* address);

    //@Override
    CARAPI InterfaceLinkStateChanged(
        /* [in] */ const String& iface,
        /* [in] */ Boolean up);

    //@Override
    CARAPI InterfaceAdded(
        /* [in] */ const String& iface);

    //@Override
    CARAPI InterfaceClassDataActivityChanged(
        /* [in] */ const String& label,
        /* [in] */ Boolean active,
        /* [in] */ Int64 tsNanos);

    //@Override
    CARAPI LimitReached(
        /* [in] */ const String& limitName,
        /* [in] */ const String& iface);

    //@Override
    CARAPI InterfaceDnsServerInfo(
        /* [in] */ const String& iface,
        /* [in] */ Int64 lifetime,
        /* [in] */ ArrayOf<String>* servers);

    //@Override
    CARAPI RouteUpdated(
        /* [in] */ IRouteInfo* route);

    //@Override
    CARAPI RouteRemoved(
        /* [in] */ IRouteInfo* route);

    ///pay for someone's error,he write a ToString in Binder.car
    CARAPI ToString(
        /* [out] */ String* str);
    ///pay for someone's error
};

} // namespace Net
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_NET_BASENETWORKOBSERVER_H__