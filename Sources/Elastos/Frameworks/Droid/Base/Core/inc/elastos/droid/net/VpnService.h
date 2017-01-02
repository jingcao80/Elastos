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

#ifndef __ELASTOS_DROID_NET_VPNSERVICE_H__
#define __ELASTOS_DROID_NET_VPNSERVICE_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/os/Binder.h"

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Net::IVpnConfig;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;

using Elastos::Net::IDatagramSocket;
using Elastos::Net::IInetAddress;
using Elastos::Net::ISocket;
using Elastos::Utility::IList;

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
class VpnService
    : public Elastos::Droid::App::Service
    , public IVpnService
{
private:
    /**
     * Use raw Binder instead of AIDL since now there is only one usage.
     */
    class Callback
        : public Binder
    {
    public:
        Callback(
            /* [in] */ VpnService* host);

    protected:
        // @Override
        CARAPI OnTransact(
            /* [in] */ Int32 code,
            /* [in] */ IParcel* data,
            /* [in] */ IParcel* reply,
            /* [in] */ Int32 flags,
            /* [out] */ Boolean* result);

    private:
        VpnService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * Prepare to establish a VPN connection. This method returns {@code null}
     * if the VPN application is already prepared or if the user has previously
     * consented to the VPN application. Otherwise, it returns an
     * {@link Intent} to a system activity. The application should launch the
     * activity using {@link Activity#startActivityForResult} to get itself
     * prepared. The activity may pop up a dialog to require user action, and
     * the result will come back via its {@link Activity#onActivityResult}.
     * If the result is {@link Activity#RESULT_OK}, the application becomes
     * prepared and is granted to use other methods in this class.
     *
     * <p>Only one application can be granted at the same time. The right
     * is revoked when another application is granted. The application
     * losing the right will be notified via its {@link #onRevoke}. Unless
     * it becomes prepared again, subsequent calls to other methods in this
     * class will fail.
     *
     * <p>The user may disable the VPN at any time while it is activated, in
     * which case this method will return an intent the next time it is
     * executed to obtain the user's consent again.
     *
     * @see #onRevoke
     */
    static CARAPI Prepare(
        /* [in] */ IContext* context,
        /* [out] */ IIntent** result);

    /**
     * Protect a socket from VPN connections. After protecting, data sent
     * through this socket will go directly to the underlying network,
     * so its traffic will not be forwarded through the VPN.
     * This method is useful if some connections need to be kept
     * outside of VPN. For example, a VPN tunnel should protect itself if its
     * destination is covered by VPN routes. Otherwise its outgoing packets
     * will be sent back to the VPN interface and cause an infinite loop. This
     * method will fail if the application is not prepared or is revoked.
     *
     * <p class="note">The socket is NOT closed by this method.
     *
     * @return {@code true} on success.
     */
    CARAPI Protect(
        /* [in] */ Int32 socket,
        /* [out] */ Boolean* result);

    /**
     * Convenience method to protect a {@link Socket} from VPN connections.
     *
     * @return {@code true} on success.
     * @see #protect(int)
     */
    CARAPI Protect(
        /* [in] */ ISocket* socket,
        /* [out] */ Boolean* result);

    /**
     * Convenience method to protect a {@link DatagramSocket} from VPN
     * connections.
     *
     * @return {@code true} on success.
     * @see #protect(int)
     */
    CARAPI Protect(
        /* [in] */ IDatagramSocket* socket,
        /* [out] */ Boolean* result);

    /**
     * Adds a network address to the VPN interface.
     *
     * Both IPv4 and IPv6 addresses are supported. The VPN must already be established. Fails if the
     * address is already in use or cannot be assigned to the interface for any other reason.
     *
     * Adding an address implicitly allows traffic from that address family (i.e., IPv4 or IPv6) to
     * be routed over the VPN. @see Builder#allowFamily
     *
     * @throws {@link IllegalArgumentException} if the address is invalid.
     *
     * @param address The IP address (IPv4 or IPv6) to assign to the VPN interface.
     * @param prefixLength The prefix length of the address.
     *
     * @return {@code true} on success.
     * @see Builder#addAddress
     *
     * @hide
     */
    CARAPI AddAddress(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 prefixLength,
        /* [out] */ Boolean* result);

    /**
     * Removes a network address from the VPN interface.
     *
     * Both IPv4 and IPv6 addresses are supported. The VPN must already be established. Fails if the
     * address is not assigned to the VPN interface, or if it is the only address assigned (thus
     * cannot be removed), or if the address cannot be removed for any other reason.
     *
     * After removing an address, if there are no addresses, routes or DNS servers of a particular
     * address family (i.e., IPv4 or IPv6) configured on the VPN, that <b>DOES NOT</b> block that
     * family from being routed. In other words, once an address family has been allowed, it stays
     * allowed for the rest of the VPN's session. @see Builder#allowFamily
     *
     * @throws {@link IllegalArgumentException} if the address is invalid.
     *
     * @param address The IP address (IPv4 or IPv6) to assign to the VPN interface.
     * @param prefixLength The prefix length of the address.
     *
     * @return {@code true} on success.
     *
     * @hide
     */
    CARAPI RemoveAddress(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 prefixLength,
        /* [out] */ Boolean* result);

    /**
     * Return the communication interface to the service. This method returns
     * {@code null} on {@link Intent}s other than {@link #SERVICE_INTERFACE}
     * action. Applications overriding this method must identify the intent
     * and return the corresponding interface accordingly.
     *
     * @see Service#onBind
     */
    // @Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** result);

    /**
     * Invoked when the application is revoked. At this moment, the VPN
     * interface is already deactivated by the system. The application should
     * close the file descriptor and shut down gracefully. The default
     * implementation of this method is calling {@link Service#stopSelf()}.
     *
     * <p class="note">Calls to this method may not happen on the main thread
     * of the process.
     *
     * @see #prepare
     */
    CARAPI OnRevoke();

private:
    /**
     * Use IConnectivityManager since those methods are hidden and not
     * available in ConnectivityManager.
     */
    static CARAPI_(AutoPtr<IIConnectivityManager>) GetService();

    /**
     * Private method to validate address and prefixLength.
     */
    static CARAPI Check(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 prefixLength);

    friend class VpnServiceBuilder;
};


/**
 * Helper class to create a VPN interface. This class should be always
 * used within the scope of the outer {@link VpnService}.
 *
 * @see VpnService
 */
class VpnServiceBuilder
    : public Object
    , public IVpnServiceBuilder
{
public:
    CAR_INTERFACE_DECL()

    VpnServiceBuilder();

    CARAPI constructor(
        /* [in] */ IVpnService* host);

    /**
     * Set the name of this session. It will be displayed in
     * system-managed dialogs and notifications. This is recommended
     * not required.
     */
    CARAPI SetSession(
        /* [in] */ const String& session);

    /**
     * Set the {@link PendingIntent} to an activity for users to
     * configure the VPN connection. If it is not set, the button
     * to configure will not be shown in system-managed dialogs.
     */
    CARAPI SetConfigureIntent(
        /* [in] */ IPendingIntent* intent);

    /**
     * Set the maximum transmission unit (MTU) of the VPN interface. If
     * it is not set, the default value in the operating system will be
     * used.
     *
     * @throws IllegalArgumentException if the value is not positive.
     */
    CARAPI SetMtu(
        /* [in] */ Int32 mtu);

    /**
     * Add a network address to the VPN interface. Both IPv4 and IPv6
     * addresses are supported. At least one address must be set before
     * calling {@link #establish}.
     *
     * Adding an address implicitly allows traffic from that address family
     * (i.e., IPv4 or IPv6) to be routed over the VPN. @see #allowFamily
     *
     * @throws IllegalArgumentException if the address is invalid.
     */
    CARAPI AddAddress(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 prefixLength);

    /**
     * Convenience method to add a network address to the VPN interface
     * using a numeric address string. See {@link InetAddress} for the
     * definitions of numeric address formats.
     *
     * Adding an address implicitly allows traffic from that address family
     * (i.e., IPv4 or IPv6) to be routed over the VPN. @see #allowFamily
     *
     * @throws IllegalArgumentException if the address is invalid.
     * @see #addAddress(InetAddress, int)
     */
    CARAPI AddAddress(
        /* [in] */ const String& address,
        /* [in] */ Int32 prefixLength);

    /**
     * Add a network route to the VPN interface. Both IPv4 and IPv6
     * routes are supported.
     *
     * Adding a route implicitly allows traffic from that address family
     * (i.e., IPv4 or IPv6) to be routed over the VPN. @see #allowFamily
     *
     * @throws IllegalArgumentException if the route is invalid.
     */
    CARAPI AddRoute(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 prefixLength);

    /**
     * Convenience method to add a network route to the VPN interface
     * using a numeric address string. See {@link InetAddress} for the
     * definitions of numeric address formats.
     *
     * Adding a route implicitly allows traffic from that address family
     * (i.e., IPv4 or IPv6) to be routed over the VPN. @see #allowFamily
     *
     * @throws IllegalArgumentException if the route is invalid.
     * @see #addRoute(InetAddress, int)
     */
    CARAPI AddRoute(
        /* [in] */ const String& address,
        /* [in] */ Int32 prefixLength);

    /**
     * Add a DNS server to the VPN connection. Both IPv4 and IPv6
     * addresses are supported. If none is set, the DNS servers of
     * the default network will be used.
     *
     * Adding a server implicitly allows traffic from that address family
     * (i.e., IPv4 or IPv6) to be routed over the VPN. @see #allowFamily
     *
     * @throws IllegalArgumentException if the address is invalid.
     */
    CARAPI AddDnsServer(
        /* [in] */ IInetAddress* address);

    /**
     * Convenience method to add a DNS server to the VPN connection
     * using a numeric address string. See {@link InetAddress} for the
     * definitions of numeric address formats.
     *
     * Adding a server implicitly allows traffic from that address family
     * (i.e., IPv4 or IPv6) to be routed over the VPN. @see #allowFamily
     *
     * @throws IllegalArgumentException if the address is invalid.
     * @see #addDnsServer(InetAddress)
     */
    CARAPI AddDnsServer(
        /* [in] */ const String& address);

    /**
     * Add a search domain to the DNS resolver.
     */
    CARAPI AddSearchDomain(
        /* [in] */ const String& domain);

    /**
     * Allows traffic from the specified address family.
     *
     * By default, if no address, route or DNS server of a specific family (IPv4 or IPv6) is
     * added to this VPN, then all outgoing traffic of that family is blocked. If any address,
     * route or DNS server is added, that family is allowed.
     *
     * This method allows an address family to be unblocked even without adding an address,
     * route or DNS server of that family. Traffic of that family will then typically
     * fall-through to the underlying network if it's supported.
     *
     * {@code family} must be either {@code AF_INET} (for IPv4) or {@code AF_INET6} (for IPv6).
     * {@link IllegalArgumentException} is thrown if it's neither.
     *
     * @param family The address family ({@code AF_INET} or {@code AF_INET6}) to allow.
     *
     * @return this {@link Builder} object to facilitate chaining of method calls.
     */
    CARAPI AllowFamily(
        /* [in] */ Int32 family);

    /**
     * Adds an application that's allowed to access the VPN connection.
     *
     * If this method is called at least once, only applications added through this method (and
     * no others) are allowed access. Else (if this method is never called), all applications
     * are allowed by default.  If some applications are added, other, un-added applications
     * will use networking as if the VPN wasn't running.
     *
     * A {@link Builder} may have only a set of allowed applications OR a set of disallowed
     * ones, but not both. Calling this method after {@link #addDisallowedApplication} has
     * already been called, or vice versa, will throw an {@link UnsupportedOperationException}.
     *
     * {@code packageName} must be the canonical name of a currently installed application.
     * {@link PackageManager.NameNotFoundException} is thrown if there's no such application.
     *
     * @throws {@link PackageManager.NameNotFoundException} If the application isn't installed.
     *
     * @param packageName The full name (e.g.: "com.google.apps.contacts") of an application.
     *
     * @return this {@link Builder} object to facilitate chaining method calls.
     */
    CARAPI AddAllowedApplication(
        /* [in] */ const String& packageName);

    /**
     * Adds an application that's denied access to the VPN connection.
     *
     * By default, all applications are allowed access, except for those denied through this
     * method.  Denied applications will use networking as if the VPN wasn't running.
     *
     * A {@link Builder} may have only a set of allowed applications OR a set of disallowed
     * ones, but not both. Calling this method after {@link #addAllowedApplication} has already
     * been called, or vice versa, will throw an {@link UnsupportedOperationException}.
     *
     * {@code packageName} must be the canonical name of a currently installed application.
     * {@link PackageManager.NameNotFoundException} is thrown if there's no such application.
     *
     * @throws {@link PackageManager.NameNotFoundException} If the application isn't installed.
     *
     * @param packageName The full name (e.g.: "com.google.apps.contacts") of an application.
     *
     * @return this {@link Builder} object to facilitate chaining method calls.
     */
    CARAPI AddDisallowedApplication(
        /* [in] */ const String& packageName);

    /**
     * Allows all apps to bypass this VPN connection.
     *
     * By default, all traffic from apps is forwarded through the VPN interface and it is not
     * possible for apps to side-step the VPN. If this method is called, apps may use methods
     * such as {@link ConnectivityManager#setProcessDefaultNetwork} to instead send/receive
     * directly over the underlying network or any other network they have permissions for.
     *
     * @return this {@link Builder} object to facilitate chaining of method calls.
     */
    CARAPI AllowBypass();

    /**
     * Sets the VPN interface's file descriptor to be in blocking/non-blocking mode.
     *
     * By default, the file descriptor returned by {@link #establish} is non-blocking.
     *
     * @param blocking True to put the descriptor into blocking mode; false for non-blocking.
     *
     * @return this {@link Builder} object to facilitate chaining method calls.
     */
    CARAPI SetBlocking(
        /* [in] */ Boolean blocking);

    /**
     * Create a VPN interface using the parameters supplied to this
     * builder. The interface works on IP packets, and a file descriptor
     * is returned for the application to access them. Each read
     * retrieves an outgoing packet which was routed to the interface.
     * Each write injects an incoming packet just like it was received
     * from the interface. The file descriptor is put into non-blocking
     * mode by default to avoid blocking Java threads. To use the file
     * descriptor completely in native space, see
     * {@link ParcelFileDescriptor#detachFd()}. The application MUST
     * close the file descriptor when the VPN connection is terminated.
     * The VPN interface will be removed and the network will be
     * restored by the system automatically.
     *
     * <p>To avoid conflicts, there can be only one active VPN interface
     * at the same time. Usually network parameters are never changed
     * during the lifetime of a VPN connection. It is also common for an
     * application to create a new file descriptor after closing the
     * previous one. However, it is rare but not impossible to have two
     * interfaces while performing a seamless handover. In this case, the
     * old interface will be deactivated when the new one is created
     * successfully. Both file descriptors are valid but now outgoing
     * packets will be routed to the new interface. Therefore, after
     * draining the old file descriptor, the application MUST close it
     * and start using the new file descriptor. If the new interface
     * cannot be created, the existing interface and its file descriptor
     * remain untouched.
     *
     * <p>An exception will be thrown if the interface cannot be created
     * for any reason. However, this method returns {@code null} if the
     * application is not prepared or is revoked. This helps solve
     * possible race conditions between other VPN applications.
     *
     * @return {@link ParcelFileDescriptor} of the VPN interface, or
     *         {@code null} if the application is not prepared.
     * @throws IllegalArgumentException if a parameter is not accepted
     *         by the operating system.
     * @throws IllegalStateException if a parameter cannot be applied
     *         by the operating system.
     * @throws SecurityException if the service is not properly declared
     *         in {@code AndroidManifest.xml}.
     * @see VpnService
     */
    CARAPI Establish(
        /* [out] */ IParcelFileDescriptor** result);

private:
    CARAPI VerifyApp(
        /* [in] */ const String& packageName);

    CARAPI_(AutoPtr<IVpnConfig>) CreateConfig();
    CARAPI_(AutoPtr<IList>) CreateList();

private:
    AutoPtr<IVpnConfig> mConfig;
    AutoPtr<IList> mAddresses;
    AutoPtr<IList> mRoutes;
    VpnService* mHost;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_VPNSERVICE_H__
