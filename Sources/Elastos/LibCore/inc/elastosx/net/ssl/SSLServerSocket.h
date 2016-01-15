
#ifndef __ELASTOSX_NET_SSL_SSLSERVERSOCKET_H__
#define __ELASTOSX_NET_SSL_SSLSERVERSOCKET_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "ServerSocket.h"

using Elastos::Net::IInetAddress;
using Elastos::Net::ServerSocket;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * The extension of {@code ServerSocket} which provides secure server sockets
 * based on protocols like SSL, TLS, or others.
 */
class SSLServerSocket
    : public ServerSocket
    , public ISSLServerSocket
{
public:
    CAR_INTERFACE_DECL()

    SSLServerSocket();

    /**
     * Only to be used by subclasses.
     * <p>
     * Creates a TCP server socket with the default authentication context.
     *
     * @throws IOException
     *             if creating the socket fails.
     */
    CARAPI constructor();

    /**
     * Only to be used by subclasses.
     * <p>
     * Creates a TCP server socket on the specified port with the default
     * authentication context. The connection's default backlog size is 50
     * connections.
     * @param port
     *            the port to listen on.
     * @throws IOException
     *             if creating the socket fails.
     */
    CARAPI constructor(
        /* [in] */ Int32 port);

    /**
     * Only to be used by subclasses.
     * <p>
     * Creates a TCP server socket on the specified port using the specified
     * backlog and the default authentication context.
     *
     * @param port
     *            the port to listen on.
     * @param backlog
     *            the number of pending connections to queue.
     * @throws IOException
     *             if creating the socket fails.
     */
    CARAPI constructor(
        /* [in] */ Int32 port,
        /* [in] */ Int32 backlog);

    /**
     * Only to be used by subclasses.
     * <p>
     * Creates a TCP server socket on the specified port, using the specified
     * backlog, listening on the specified interface, and using the default
     * authentication context.
     *
     * @param port
     *            the port the listen on.
     * @param backlog
     *            the number of pending connections to queue.
     * @param address
     *            the address of the interface to accept connections on.
     * @throws IOException
     *             if creating the socket fails.
     */
    CARAPI constructor(
        /* [in] */ Int32 port,
        /* [in] */ Int32 backlog,
        /* [in] */ IInetAddress* address);

    /**
     * Returns the names of the enabled cipher suites to be used for new
     * connections.
     *
     * @return the names of the enabled cipher suites to be used for new
     *         connections.
     */
    virtual CARAPI GetEnabledCipherSuites(
        /* [out, callee] */ ArrayOf<String>** suites) = 0;

    /**
     * Sets the names of the cipher suites to be enabled for new connections.
     * Only cipher suites returned by {@link #getSupportedCipherSuites()} are
     * allowed.
     *
     * @param suites
     *            the names of the to be enabled cipher suites.
     * @throws IllegalArgumentException
     *             if one of the cipher suite names is not supported.
     */
    virtual CARAPI SetEnabledCipherSuites(
        /* [in] */ ArrayOf<String>* suites) = 0;

    /**
     * Returns the names of the supported cipher suites.
     *
     * @return the names of the supported cipher suites.
     */
    virtual CARAPI GetSupportedCipherSuites(
        /* [out, callee] */ ArrayOf<String>** suites) = 0;

    /**
     * Returns the names of the supported protocols.
     *
     * @return the names of the supported protocols.
     */
    virtual CARAPI GetSupportedProtocols(
        /* [out, callee] */ ArrayOf<String>** protocols) = 0;

    /**
     * Returns the names of the enabled protocols to be used for new
     * connections.
     *
     * @return the names of the enabled protocols to be used for new
     *         connections.
     */
    virtual CARAPI GetEnabledProtocols(
        /* [out, callee] */ ArrayOf<String>** protocols) = 0;

    /**
     * Sets the names of the protocols to be enabled for new connections. Only
     * protocols returned by {@link #getSupportedProtocols()} are allowed.
     *
     * @param protocols
     *            the names of the to be enabled protocols.
     * @throws IllegalArgumentException
     *             if one of the protocols is not supported.
     */
    virtual CARAPI SetEnabledProtocols(
        /* [in] */ ArrayOf<String>* protocols) = 0;

    /**
     * Sets whether server-mode connections will be configured to require client
     * authentication. The client authentication is one of the following:
     * <ul>
     * <li>authentication required</li>
     * <li>authentication requested</li>
     * <li>no authentication needed</li>
     * </ul>
     * This method overrides the setting of {@link #setWantClientAuth(boolean)}.
     *
     * @param need
     *            {@code true} if client authentication is required,
     *            {@code false} if no authentication is needed.
     */
    virtual CARAPI SetNeedClientAuth(
        /* [in] */ Boolean need) = 0;

    /**
     * Returns whether server-mode connections will be configured to require
     * client authentication.
     *
     * @return {@code true} if client authentication is required, {@code false}
     *         if no client authentication is needed.
     */
    virtual CARAPI GetNeedClientAuth(
        /* [out] */ Boolean* need) = 0;

    /**
     * Sets whether server-mode connections will be configured to request client
     * authentication. The client authentication is one of the following:
     * <ul>
     * <li>authentication required</li>
     * <li>authentication requested</li>
     * <li>no authentication needed</li>
     * </ul>
     * This method overrides the setting of {@link #setNeedClientAuth(boolean)}.
     *
     * @param want
     *            {@code true} if client authentication should be requested,
     *            {@code false} if no authentication is needed.
     */
    virtual CARAPI SetWantClientAuth(
        /* [in] */ Boolean want) = 0;

    /**
     * Returns whether server-mode connections will be configured to request
     * client authentication.
     *
     * @return {@code true} is client authentication will be requested,
     *         {@code false} if no client authentication is needed.
     */
    virtual CARAPI GetWantClientAuth(
        /* [out] */ Boolean* want) = 0;

    /**
     * Sets whether new connections should act in client mode when handshaking.
     *
     * @param mode
     *            {@code true} if new connections should act in client mode,
     *            {@code false} if not.
     */
    virtual CARAPI SetUseClientMode(
        /* [in] */ Boolean mode) = 0;

    /**
     * Returns whether new connection will act in client mode when handshaking.
     *
     * @return {@code true} if new connections will act in client mode when
     *         handshaking, {@code false} if not.
     */
    virtual CARAPI GetUseClientMode(
        /* [out] */ Boolean* mode) = 0;

    /**
     * Sets whether new SSL sessions may be established for new connections.
     *
     * @param flag
     *            {@code true} if new SSL sessions may be established,
     *            {@code false} if existing SSL sessions must be reused.
     */
    virtual CARAPI SetEnableSessionCreation(
        /* [in] */ Boolean flag) = 0;

    /**
     * Returns whether new SSL sessions may be established for new connections.
     *
     * @return {@code true} if new SSL sessions may be established,
     *         {@code false} if existing SSL sessions must be reused.
     */
    virtual CARAPI GetEnableSessionCreation(
        /* [out] */ Boolean* enabled) = 0;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_SSLSERVERSOCKET_H__
