#ifndef __ELASTOS_DROID_SERVER_NET_NETLINKTRACKER_H__
#define __ELASTOS_DROID_SERVER_NET_NETLINKTRACKER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Core.Server.h"
#include "elastos/droid/server/net/BaseNetworkObserver.h"

using Elastos::Droid::Net::IINetworkManagementEventObserver;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::IRouteInfo;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Server::Net::INetlinkTracker;
using Elastos::Droid::Server::Net::INetlinkTrackerCallback;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

/**
 * Keeps track of link configuration received from Netlink.
 *
 * Instances of this class are expected to be owned by subsystems such as Wi-Fi
 * or Ethernet that manage one or more network interfaces. Each interface to be
 * tracked needs its own {@code NetlinkTracker}.
 *
 * An instance of this class is constructed by passing in an interface name and
 * a callback. The owner is then responsible for registering the tracker with
 * NetworkManagementService. When the class receives update notifications from
 * the NetworkManagementService notification threads, it applies the update to
 * its local LinkProperties, and if something has changed, notifies its owner of
 * the update via the callback.
 *
 * The owner can then call {@code getLinkProperties()} in order to find out
 * what changed. If in the meantime the LinkProperties stored here have changed,
 * this class will return the current LinkProperties. Because each change
 * triggers an update callback after the change is made, the owner may get more
 * callbacks than strictly necessary (some of which may be no-ops), but will not
 * be out of sync once all callbacks have been processed.
 *
 * Threading model:
 *
 * - The owner of this class is expected to create it, register it, and call
 *   getLinkProperties or clearLinkProperties on its thread.
 * - Most of the methods in the class are inherited from BaseNetworkObserver
 *   and are called by NetworkManagementService notification threads.
 * - All accesses to mLinkProperties must be synchronized(this). All the other
 *   member variables are immutable once the object is constructed.
 *
 * This class currently tracks IPv4 and IPv6 addresses. In the future it will
 * track routes and DNS servers.
 *
 * @hide
 */
class NetlinkTracker
    : public BaseNetworkObserver
    , public INetlinkTracker
{
public:
    CAR_INTERFACE_DECL()

    NetlinkTracker();

    CARAPI constructor(
        /* [in] */ const String& iface,
        /* [in] */ INetlinkTrackerCallback* callback);

    //@Override
    CARAPI AddressUpdated(
        /* [in] */ const String& iface,
        /* [in] */ ILinkAddress* address);

    //@Override
    CARAPI AddressRemoved(
        /* [in] */ const String& iface,
        /* [in] */ ILinkAddress* address);

    //@Override
    CARAPI RouteUpdated(
        /* [in] */ IRouteInfo* route);

    //@Override
    CARAPI RouteRemoved(
        /* [in] */ IRouteInfo* route);

    //@Override
    CARAPI InterfaceDnsServerInfo(
        /* [in] */ const String& iface,
        /* [in] */ Int64 lifetime,
        /* [in] */ ArrayOf<String>* addresses);

    /**
     * Returns a copy of this object's LinkProperties.
     */
    CARAPI GetLinkProperties(
        /* [out] */ ILinkProperties** lp);

    CARAPI ClearLinkProperties();

private:
    CARAPI_(void) MaybeLog(
        /* [in] */ const String& operation,
        /* [in] */ const String& iface,
        /* [in] */ ILinkAddress* address);

    CARAPI_(void) MaybeLog(
        /* [in] */ const String& operation,
        /* [in] */ IInterface* o);

private:
    String TAG;
    String mInterfaceName;
    AutoPtr<INetlinkTrackerCallback> mCallback;
    AutoPtr<ILinkProperties> mLinkProperties;
    AutoPtr<IDnsServerRepository> mDnsServerRepository;

    const static Boolean DBG;
};

} // namespace Net
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_NET_NETLINKTRACKER_H__