
#ifndef __ELASTOSX_NET_SSL_SSLCONTEXT_H__
#define __ELASTOSX_NET_SSL_SSLCONTEXT_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Security.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Security::IProvider;
using Elastos::Security::ISecureRandom;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * The public API for secure socket protocol implementations. It acts as factory
 * for {@code SSLSocketFactory}'s and {@code SSLEngine}s.
 */
class SSLContext
    : public Object
    , public ISSLContext
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the default SSLContext.
     *
     * The default SSL context can be set with {@link #setDefault}. If
     * not, one will be created with {@code
     * SSLContext.getInstance("Default")}, which will already be
     * initialized.
     *
     * @throws NoSuchAlgorithmException if there is a problem creating
     * the default instance.
     * @since 1.6
     */
    static CARAPI GetDefault(
        /* [out] */ ISSLContext** context);

    /**
     * Sets the default SSLContext instance as returned by {@link
     * #getDefault()} to a non-null initialized value.
     *
     * @throws NullPointerException on a null argument
     * @since 1.6
     */
    static CARAPI SetDefault(
        /* [in] */ ISSLContext* sslContext);

    /**
     * Creates a new {@code SSLContext} instance for the specified protocol.
     *
     * <p>The following protocols are supported:
     * <table>
     *     <thead>
     *         <tr>
     *             <th>Protocol</th>
     *             <th>API Levels</th>
     *         </tr>
     *     </thead>
     *     <tbody>
     *         <tr>
     *             <td>Default</td>
     *             <td>9+</td>
     *         </tr>
     *         <tr>
     *             <td>SSL</td>
     *             <td>9+</td>
     *         </tr>
     *         <tr>
     *             <td>SSLv3</td>
     *             <td>9+</td>
     *         </tr>
     *         <tr>
     *             <td>TLS</td>
     *             <td>1+</td>
     *         </tr>
     *         <tr>
     *             <td>TLSv1</td>
     *             <td>1+</td>
     *         </tr>
     *         <tr>
     *             <td>TLSv1.1</td>
     *             <td>16+</td>
     *         </tr>
     *         <tr>
     *             <td>TLSv1.2</td>
     *             <td>16+</td>
     *         </tr>
     *     </tbody>
     * </table>
     *
     * @param protocol
     *            the requested protocol to create a context for.
     * @return the created {@code SSLContext} instance.
     * @throws NoSuchAlgorithmException
     *             if no installed provider can provide the requested protocol
     * @throws NullPointerException
     *             if {@code protocol} is {@code null} (instead of
     *             NoSuchAlgorithmException as in 1.4 release)
     */
    static CARAPI GetInstance(
        /* [in] */ const String& protocol,
        /* [out] */ ISSLContext** instance);

    /**
     * Creates a new {@code SSLContext} instance for the specified protocol from
     * the specified provider.
     *
     * <p>The following combinations are supported:
     * <table>
     *     <thead>
     *         <tr>
     *             <th>Protocol</th>
     *             <th>Provider</th>
     *             <th>API Levels</th>
     *         </tr>
     *     </thead>
     *     <tbody>
     *         <tr>
     *             <td>Default</td>
     *             <td>AndroidOpenSSL</td>
     *             <td>9+</td>
     *         </tr>
     *         <tr>
     *             <td>SSL</td>
     *             <td>AndroidOpenSSL</td>
     *             <td>9+</td>
     *         </tr>
     *         <tr>
     *             <td>SSL</td>
     *             <td>HarmonyJSSE</td>
     *             <td>9-19</td>
     *         </tr>
     *         <tr>
     *             <td>SSLv3</td>
     *             <td>AndroidOpenSSL</td>
     *             <td>9+</td>
     *         </tr>
     *         <tr>
     *             <td>SSLv3</td>
     *             <td>HarmonyJSSE</td>
     *             <td>9-19</td>
     *         </tr>
     *         <tr>
     *             <td>TLS</td>
     *             <td>AndroidOpenSSL</td>
     *             <td>9+</td>
     *         </tr>
     *         <tr>
     *             <td>TLS</td>
     *             <td>HarmonyJSSE</td>
     *             <td>1-19</td>
     *         </tr>
     *         <tr>
     *             <td>TLSv1</td>
     *             <td>AndroidOpenSSL</td>
     *             <td>9+</td>
     *         </tr>
     *         <tr>
     *             <td>TLSv1</td>
     *             <td>HarmonyJSSE</td>
     *             <td>1-19</td>
     *         </tr>
     *         <tr>
     *             <td>TLSv1.1</td>
     *             <td>AndroidOpenSSL</td>
     *             <td>16+</td>
     *         </tr>
     *         <tr>
     *             <td>TLSv1.2</td>
     *             <td>AndroidOpenSSL</td>
     *             <td>16+</td>
     *         </tr>
     *     </tbody>
     * </table>
     *
     * <p><strong>NOTE:</strong> The best practice is to rely on platform
     * defaults rather than explicitly specify a provider.
     * {@link #getDefault()} and {@link #getInstance(String)} are normally
     * preferred over this method.
     *
     * @param protocol
     *            the requested protocol to create a context for.
     * @param provider
     *            the name of the provider that provides the requested protocol.
     * @return an {@code SSLContext} for the requested protocol.
     * @throws NoSuchAlgorithmException
     *             if the specified provider cannot provider the requested
     *             protocol.
     * @throws NoSuchProviderException
     *             if the specified provider does not exits.
     * @throws NullPointerException
     *             if {@code protocol} is {@code null} (instead of
     *             NoSuchAlgorithmException as in 1.4 release)
     */
    static CARAPI GetInstance(
        /* [in] */ const String& protocol,
        /* [in] */ const String& provider,
        /* [out] */ ISSLContext** instance);

    /**
     * Creates a new {@code SSLContext} instance for the specified protocol from
     * the specified provider.
     *
     * @param protocol
     *            the requested protocol to create a context for
     * @param provider
     *            the provider that provides the requested protocol.
     * @return an {@code SSLContext} for the requested protocol.
     * @throws NoSuchAlgorithmException
     *             if the specified provider cannot provide the requested
     *             protocol.
     * @throws NullPointerException
     *             if {@code protocol} is {@code null} (instead of
     *             NoSuchAlgorithmException as in 1.4 release)
     */
    static CARAPI GetInstance(
      /* [in] */ const String& protocol,
      /* [in] */ IProvider* provider,
      /* [out] */ ISSLContext** context);

    /**
     * Creates a new {@code SSLContext}.
     *
     * @param contextSpi
     *            the implementation delegate.
     * @param provider
     *            the provider.
     * @param protocol
     *            the protocol name.
     */
    CARAPI constructor(
        /* [in] */ ISSLContextSpi* contextSpi,
        /* [in] */ IProvider* provider,
        /* [in] */ const String& protocol);

    /**
     * Returns the name of the secure socket protocol of this instance.
     *
     * @return the name of the secure socket protocol of this instance.
     */
    CARAPI GetProtocol(
        /* [out] */ String* protocol);

    /**
     * Returns the provider of this {@code SSLContext} instance.
     *
     * @return the provider of this {@code SSLContext} instance.
     */
    CARAPI GetProvider(
        /* [out] */ IProvider** provider);

    /**
     * Initializes this {@code SSLContext} instance. Three aspects of the context can be configured
     * during initialization:
     * <ul>
     *   <li>Providers of key material for key exchange and peer authentication
     *       ({@link KeyManager} instances),</li>
     *   <li>Providers of trust decisions about peers ({@link TrustManager} instances),
     *   </li>
     *   <li>Provider of randomness ({@link SecureRandom} instance).</li>
     * </ul>
     *
     * <p>For each type of {@code KeyManager} or {@code TrustManager} used by this context, only the
     * first matching instance from {@code km} or {@code tm} will be used. For example, only the
     * first instance of {@link X509TrustManager} from {@code tm} will be used.
     *
     * <p>For any parameter set to {@code null} defaults will be used. In that case, the installed
     * security providers will be searched for the highest priority implementation of the required
     * primitives. For {@code km} and {@code tm}, the highest priority implementation
     * of {@link KeyManagerFactory} and {@link TrustManagerFactory} will be used to obtain the
     * required types of {@code KeyManager} and {@code TrustManager}. For {@code sr}, the default
     * {@code SecureRandom} implementation will be used.
     *
     * @param km
     *            the key sources or {@code null} for default.
     * @param tm
     *            the trust decision sources or {@code null} for default.
     * @param sr
     *            the randomness source or {@code null} for default.
     * @throws KeyManagementException
     *             if initializing this instance fails.
     */
    CARAPI Init(
        /* [in] */ ArrayOf<IKeyManager*>* km,
        /* [in] */ ArrayOf<ITrustManager*>* tm,
        /* [in] */ ISecureRandom* sr);

    /**
     * Returns a socket factory for this instance.
     *
     * @return a socket factory for this instance.
     */
    CARAPI GetSocketFactory(
        /* [out] */ ISSLSocketFactory** factory);

    /**
     * Returns a server socket factory for this instance.
     *
     * @return a server socket factory for this instance.
     */
    CARAPI GetServerSocketFactory(
        /* [out] */ ISSLServerSocketFactory** factory);

    /**
     * Creates an {@code SSLEngine} instance from this context.
     *
     * @return an {@code SSLEngine} instance from this context.
     * @throws UnsupportedOperationException
     *             if the provider does not support the operation.
     */
    CARAPI CreateSSLEngine(
        /* [out] */ ISSLEngine** engine);

    /**
     * Creates an {@code SSLEngine} instance from this context with the
     * specified hostname and port.
     *
     * @param peerHost
     *            the name of the host
     * @param peerPort
     *            the port
     * @return an {@code SSLEngine} instance from this context.
     * @throws UnsupportedOperationException
     *             if the provider does not support the operation.
     */
    CARAPI CreateSSLEngine(
        /* [in] */ const String& peerHost,
        /* [in] */ Int32 peerPort,
        /* [out] */ ISSLEngine** engine);

    /**
     * Returns the SSL session context that encapsulates the set of SSL sessions
     * that can be used for handshake of server-side SSL sockets.
     *
     * @return the SSL server session context for this context or {@code null}
     *         if the underlying provider does not provide an implementation of
     *         the {@code SSLSessionContext} interface.
     */
    CARAPI GetServerSessionContext(
        /* [out] */ ISSLSessionContext** context);

    /**
     * Returns the SSL session context that encapsulates the set of SSL sessions
     * that can be used for handshake of client-side SSL sockets.
     *
     * @return the SSL client session context for this context or {@code null}
     *         if the underlying provider does not provide an implementation of
     *         the {@code SSLSessionContext} interface.
     */
    CARAPI GetClientSessionContext(
        /* [out] */ ISSLSessionContext** context);

    /**
     * Returns the default SSL handshake parameters for SSLSockets
     * created by this SSLContext.
     *
     * @throws UnsupportedOperationException
     * @since 1.6
     */
    CARAPI GetDefaultSSLParameters(
        /* [out] */ ISSLParameters ** param);

    /**
     * Returns SSL handshake parameters for SSLSockets that includes
     * all supported cipher suites and protocols.
     *
     * @throws UnsupportedOperationException
     * @since 1.6
     */
    CARAPI GetSupportedSSLParameters(
        /* [out] */ ISSLParameters** param);

private:
    // StoreSSLContext service name
    static const String sSERVICE;// = "SSLContext";

    // Used to access common engine functionality
    //TODO :static const AutoPtr<IEngine> sENGINE;// = new Engine(SERVICE);

    /**
     * Default SSLContext that can be replaced with SSLContext.setDefault()
     */
    static AutoPtr<ISSLContext> sDEFAULT;

    AutoPtr<IProvider> mProvider;

    AutoPtr<ISSLContextSpi> mSpiImpl;

    String mProtocol;

    static Object mLock;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_SSLCONTEXT_H__
