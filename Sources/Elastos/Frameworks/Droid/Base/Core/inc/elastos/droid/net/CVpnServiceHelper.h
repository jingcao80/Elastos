
#ifndef __ELASTOS_DROID_NET_CVPNSERVICEHELPER_H__
#define __ELASTOS_DROID_NET_CVPNSERVICEHELPER_H__

#include "_Elastos_Droid_Net_CVpnServiceHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * VpnService is a base class for applications to extend and build their
 * own VPN solutions. In general, it creates a virtual network interface,
 * configures addresses and routing rules, and returns a file descriptor
 * to the application. Each read from the descriptor retrieves an outgoing
 * packet which was routed to the interface. Each write to the descriptor
 * injects an incoming packet just like it was received from the interface.
 * The interface is running on Internet Protocol (IP), so packets are
 * always started with IP headers. The application then completes a VPN
 * connection by processing and exchanging packets with the remote server
 * over a tunnel.
 *
 * <p>Letting applications intercept packets raises huge security concerns.
 * A VPN application can easily break the network. Besides, two of them may
 * conflict with each other. The system takes several actions to address
 * these issues. Here are some key points:
 * <ul>
 *   <li>User action is required the first time an application creates a VPN
 *       connection.</li>
 *   <li>There can be only one VPN connection running at the same time. The
 *       existing interface is deactivated when a new one is created.</li>
 *   <li>A system-managed notification is shown during the lifetime of a
 *       VPN connection.</li>
 *   <li>A system-managed dialog gives the information of the current VPN
 *       connection. It also provides a button to disconnect.</li>
 *   <li>The network is restored automatically when the file descriptor is
 *       closed. It also covers the cases when a VPN application is crashed
 *       or killed by the system.</li>
 * </ul>
 *
 * <p>There are two primary methods in this class: {@link #prepare} and
 * {@link Builder#establish}. The former deals with user action and stops
 * the VPN connection created by another application. The latter creates
 * a VPN interface using the parameters supplied to the {@link Builder}.
 * An application must call {@link #prepare} to grant the right to use
 * other methods in this class, and the right can be revoked at any time.
 * Here are the general steps to create a VPN connection:
 * <ol>
 *   <li>When the user presses the button to connect, call {@link #prepare}
 *       and launch the returned intent, if non-null.</li>
 *   <li>When the application becomes prepared, start the service.</li>
 *   <li>Create a tunnel to the remote server and negotiate the network
 *       parameters for the VPN connection.</li>
 *   <li>Supply those parameters to a {@link Builder} and create a VPN
 *       interface by calling {@link Builder#establish}.</li>
 *   <li>Process and exchange packets between the tunnel and the returned
 *       file descriptor.</li>
 *   <li>When {@link #onRevoke} is invoked, close the file descriptor and
 *       shut down the tunnel gracefully.</li>
 * </ol>
 *
 * <p>Services extended this class need to be declared with appropriate
 * permission and intent filter. Their access must be secured by
 * {@link android.Manifest.permission#BIND_VPN_SERVICE} permission, and
 * their intent filter must match {@link #SERVICE_INTERFACE} action. Here
 * is an example of declaring a VPN service in {@code AndroidManifest.xml}:
 * <pre>
 * &lt;service android:name=".ExampleVpnService"
 *         android:permission="android.permission.BIND_VPN_SERVICE"&gt;
 *     &lt;intent-filter&gt;
 *         &lt;action android:name="android.net.VpnService"/&gt;
 *     &lt;/intent-filter&gt;
 * &lt;/service&gt;</pre>
 *
 * @see Builder
 */
CarClass(CVpnServiceHelper)
    , public Singleton
    , public IVpnServiceHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Prepare(
        /* [in] */ IContext* context,
        /* [out] */ IIntent** result);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_CVPNSERVICEHELPER_H__
