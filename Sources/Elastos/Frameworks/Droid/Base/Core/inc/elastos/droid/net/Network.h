
#ifndef __ELASTOS_DROID_NET_NETWORK_H__
#define __ELASTOS_DROID_NET_NETWORK_H__

#include "Elastos.Droid.Net.h"
#include "_Elastos.Droid.Core.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastosx/net/SocketFactory.h>

using Com::Squareup::Okhttp::IConnectionPool;
using Com::Squareup::Okhttp::IHostResolver;
using Elastos::Net::ISocket;
using Elastos::Net::ISocketAddress;
using Elastos::Net::IURL;
using Elastos::Net::IURLConnection;
using Elastosx::Net::ISocketFactory;
using Elastosx::Net::SocketFactory;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Identifies a {@code Network}.  This is supplied to applications via
 * {@link ConnectivityManager.NetworkCallback} in response to the active
 * {@link ConnectivityManager#requestNetwork} or passive
 * {@link ConnectivityManager#registerNetworkCallback} calls.
 * It is used to direct traffic to the given {@code Network}, either on a {@link Socket} basis
 * through a targeted {@link SocketFactory} or process-wide via
 * {@link ConnectivityManager#setProcessDefaultNetwork}.
 */
class Network
    : public Object
    , public INetwork
    , public IParcelable
{
private:
    /**
     * A {@code SocketFactory} that produces {@code Socket}'s bound to this network.
     */
    class NetworkBoundSocketFactory
        : public SocketFactory
    {
    public:
        NetworkBoundSocketFactory(
            /* [in] */ Int32 netId,
            /* [in] */ Network* host);

        // @Override
        CARAPI CreateSocket(
            /* [in] */ const String& host,
            /* [in] */ Int32 port,
            /* [in] */ IInetAddress* localHost,
            /* [in] */ Int32 localPort,
            /* [out] */ ISocket** result);

        // @Override
        CARAPI CreateSocket(
            /* [in] */ IInetAddress* address,
            /* [in] */ Int32 port,
            /* [in] */ IInetAddress* localAddress,
            /* [in] */ Int32 localPort,
            /* [out] */ ISocket** result);

        // @Override
        CARAPI CreateSocket(
            /* [in] */ IInetAddress* host,
            /* [in] */ Int32 port,
            /* [out] */ ISocket** result);

        // @Override
        CARAPI CreateSocket(
            /* [in] */ const String& host,
            /* [in] */ Int32 port,
            /* [out] */ ISocket** result);

        // @Override
        CARAPI CreateSocket(
            /* [out] */ ISocket** result);

    private:
        CARAPI ConnectToHost(
            /* [in] */ const String& host,
            /* [in] */ Int32 port,
            /* [in] */ ISocketAddress* localAddress,
            /* [out] */ ISocket** result);

    private:
        const Int32 mNetId;

        Network* const mHost;
    };

public:
    CAR_INTERFACE_DECL()

    Network();

    CARAPI constructor();
    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ Int32 netId);

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ INetwork* that);

    /**
     * Operates the same as {@code InetAddress.getAllByName} except that host
     * resolution is done on this network.
     *
     * @param host the hostname or literal IP string to be resolved.
     * @return the array of addresses associated with the specified host.
     * @throws UnknownHostException if the address lookup fails.
     */
    CARAPI GetAllByName(
        /* [in] */ const String& host,
        /* [out, callee] */ ArrayOf<IInetAddress*>** result);

    /**
     * Operates the same as {@code InetAddress.getByName} except that host
     * resolution is done on this network.
     *
     * @param host
     *            the hostName to be resolved to an address or {@code null}.
     * @return the {@code InetAddress} instance representing the host.
     * @throws UnknownHostException
     *             if the address lookup fails.
     */
    CARAPI GetByName(
        /* [in] */ const String& host,
        /* [out] */ IInetAddress** result);

    /**
     * Returns a {@link SocketFactory} bound to this network.  Any {@link Socket} created by
     * this factory will have its traffic sent over this {@code Network}.  Note that if this
     * {@code Network} ever disconnects, this factory and any {@link Socket} it produced in the
     * past or future will cease to work.
     *
     * @return a {@link SocketFactory} which produces {@link Socket} instances bound to this
     *         {@code Network}.
     */
    CARAPI GetSocketFactory(
        /* [out] */ ISocketFactory** result);

    /**
     * Opens the specified {@link URL} on this {@code Network}, such that all traffic will be sent
     * on this Network. The URL protocol must be {@code HTTP} or {@code HTTPS}.
     *
     * @return a {@code URLConnection} to the resource referred to by this URL.
     * @throws MalformedURLException if the URL protocol is not HTTP or HTTPS.
     * @throws IOException if an error occurs while opening the connection.
     * @see java.net.URL#openConnection()
     */
    CARAPI OpenConnection(
        /* [in] */ IURL* url,
        /* [out] */ IURLConnection** result);

    /**
     * Binds the specified {@link Socket} to this {@code Network}. All data traffic on the socket
     * will be sent on this {@code Network}, irrespective of any process-wide network binding set by
     * {@link ConnectivityManager#setProcessDefaultNetwork}. The socket must not be connected.
     */
    CARAPI BindSocket(
        /* [in] */ ISocket* socket);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * @hide
     */
    CARAPI GetNetId(
        /* [out] */ Int32* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    // TODO: This creates a connection pool and host resolver for
    // every Network object, instead of one for every NetId. This is
    // suboptimal, because an app could potentially have more than one
    // Network object for the same NetId, causing increased memory footprint
    // and performance penalties due to lack of connection reuse (connection
    // setup time, congestion window growth time, etc.).
    //
    // Instead, investigate only having one connection pool and host resolver
    // for every NetId, perhaps by using a static HashMap of NetIds to
    // connection pools and host resolvers. The tricky part is deciding when
    // to remove a map entry; a WeakHashMap shouldn't be used because whether
    // a Network is referenced doesn't correlate with whether a new Network
    // will be instantiated in the near future with the same NetID. A good
    // solution would involve purging empty (or when all connections are timed
    // out) ConnectionPools.
    CARAPI MaybeInitHttpClient();

    // to initialize HTTP_KEEP_ALIVE only
    static Boolean GetHTTP_KEEP_ALIVE();

    // to initialize HTTP_MAX_CONNECTIONS only
    static Int32 GetHTTP_MAX_CONNECTIONS();

    // to initialize HTTP_KEEP_ALIVE_DURATION_MS only
    static Int32 GetHTTP_KEEP_ALIVE_DURATION_MS();

private:
    // Default connection pool values. These are evaluated at startup, just
    // like the OkHttp code. Also like the OkHttp code, we will throw parse
    // exceptions at class loading time if the properties are set but are not
    // valid integers.
    static const Boolean HTTP_KEEP_ALIVE;
    static const Int32 HTTP_MAX_CONNECTIONS;
    static const Int64 HTTP_KEEP_ALIVE_DURATION_MS; // 5 minutes.

    /**
     * @hide
     */
    // final in java
    Int32 mNetId;

    AutoPtr<IInterface> mLock;

    // Objects used to perform per-network operations such as getSocketFactory
    // and openConnection, and a lock to protect access to them.
    /* volatile */ AutoPtr<NetworkBoundSocketFactory> mNetworkBoundSocketFactory;

    // mLock should be used to control write access to mConnectionPool and mHostResolver.
    // maybeInitHttpClient() must be called prior to reading either variable.
    /* volatile */ AutoPtr<IConnectionPool> mConnectionPool;

    /* volatile */ AutoPtr<IHostResolver> mHostResolver;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Net::Network::NetworkBoundSocketFactory, IInterface)

#endif // __ELASTOS_DROID_NET_NETWORK_H__
