
#ifndef __ELASTOSX_NET_SSL_CSSLCONTEXTHELPER_H__
#define __ELASTOSX_NET_SSL_CSSLCONTEXTHELPER_H__

#include "_Elastosx_Net_Ssl_CSSLContextHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Security::IProvider;

namespace Elastosx {
namespace Net {
namespace Ssl {

CarClass(CSSLContextHelper)
    , public Singleton
    , public ISSLContextHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

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
    CARAPI GetDefault(
        /* [out] */ ISSLContext** context);

    /**
     * Sets the default SSLContext instance as returned by {@link
     * #getDefault()} to a non-null initialized value.
     *
     * @throws NullPointerException on a null argument
     * @since 1.6
     */
    CARAPI SetDefault(
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
    CARAPI GetInstance(
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
    CARAPI GetInstance(
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
    CARAPI GetInstance(
      /* [in] */ const String& protocol,
      /* [in] */ IProvider* provider,
      /* [out] */ ISSLContext** context);
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CSSLCONTEXTHELPER_H__
