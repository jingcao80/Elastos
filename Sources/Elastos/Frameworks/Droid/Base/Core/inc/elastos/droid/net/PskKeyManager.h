
#ifndef __ELASTOS_DROID_NET_PSKKEYMANAGER_H__
#define __ELASTOS_DROID_NET_PSKKEYMANAGER_H__

#include <Elastos.CoreLibrary.Extensions.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Net::ISocket;
using Elastosx::Crypto::ISecretKey;
using Elastosx::Net::Ssl::IKeyManager;
using Elastosx::Net::Ssl::ISSLEngine;

using Org::Conscrypt::IPSKKeyManager;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Provider of key material for pre-shared key (PSK) key exchange used in TLS-PSK cipher suites.
 *
 * <h3>Overview of TLS-PSK</h3>
 *
 * <p>TLS-PSK is a set of TLS/SSL cipher suites which rely on a symmetric pre-shared key (PSK) to
 * secure the TLS/SSL connection and mutually authenticate its peers. These cipher suites may be
 * a more natural fit compared to conventional public key based cipher suites in some scenarios
 * where communication between peers is bootstrapped via a separate step (for example, a pairing
 * step) and requires both peers to authenticate each other. In such scenarios a symmetric key (PSK)
 * can be exchanged during the bootstrapping step, removing the need to generate and exchange public
 * key pairs and X.509 certificates.</p>
 *
 * <p>When a TLS-PSK cipher suite is used, both peers have to use the same key for the TLS/SSL
 * handshake to succeed. Thus, both peers are implicitly authenticated by a successful handshake.
 * This removes the need to use a {@code TrustManager} in conjunction with this {@code KeyManager}.
 * </p>
 *
 * <h3>Supporting multiple keys</h3>
 *
 * <p>A peer may have multiple keys to choose from. To help choose the right key, during the
 * handshake the server can provide a <em>PSK identity hint</em> to the client, and the client can
 * provide a <em>PSK identity</em> to the server. The contents of these two pieces of information
 * are specific to application-level protocols.</p>
 *
 * <p><em>NOTE: Both the PSK identity hint and the PSK identity are transmitted in cleartext.
 * Moreover, these data are received and processed prior to peer having been authenticated. Thus,
 * they must not contain or leak key material or other sensitive information, and should be
 * treated (e.g., parsed) with caution, as untrusted data.</em></p>
 *
 * <p>The high-level flow leading to peers choosing a key during TLS/SSL handshake is as follows:
 * <ol>
 * <li>Server receives a handshake request from client.
 * <li>Server replies, optionally providing a PSK identity hint to client.</li>
 * <li>Client chooses the key.</li>
 * <li>Client provides a PSK identity of the chosen key to server.</li>
 * <li>Server chooses the key.</li>
 * </ol></p>
 *
 * <p>In the flow above, either peer can signal that they do not have a suitable key, in which case
 * the the handshake will be aborted immediately. This may enable a network attacker who does not
 * know the key to learn which PSK identity hints or PSK identities are supported. If this is a
 * concern then a randomly generated key should be used in the scenario where no key is available.
 * This will lead to the handshake aborting later, due to key mismatch -- same as in the scenario
 * where a key is available -- making it appear to the attacker that all PSK identity hints and PSK
 * identities are supported.</p>
 *
 * <h3>Maximum sizes</h3>
 *
 * <p>The maximum supported sizes are as follows:
 * <ul>
 * <li>256 bytes for keys (see {@link #MAX_KEY_LENGTH_BYTES}),</li>
 * <li>128 bytes for PSK identity and PSK identity hint (in modified UTF-8 representation) (see
 * {@link #MAX_IDENTITY_LENGTH_BYTES} and {@link #MAX_IDENTITY_HINT_LENGTH_BYTES}).</li>
 * </ul></p>
 *
 * <h3>Subclassing</h3>
 * Subclasses should normally provide their own implementation of {@code getKey} because the default
 * implementation returns no key, which aborts the handshake.
 *
 * <h3>Example</h3>
 * The following example illustrates how to create an {@code SSLContext} which enables the use of
 * TLS-PSK in {@code SSLSocket}, {@code SSLServerSocket} and {@code SSLEngine} instances obtained
 * from it.
 * <pre> {@code
 * PskKeyManager pskKeyManager = ...;
 *
 * SSLContext sslContext = SSLContext.getInstance("TLS");
 * sslContext.init(
 *         new KeyManager[] &#123;pskKeyManager&#125;,
 *         new TrustManager[0], // No TrustManagers needed for TLS-PSK
 *         null // Use the default source of entropy
 *         );
 *
 * SSLSocket sslSocket = (SSLSocket) sslContext.getSocketFactory().createSocket(...);
 * }</pre>
 */
class PskKeyManager
    : public Object
    , public IKeyManager
    , public IPSKKeyManager
    , public IPskKeyManager
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Gets the PSK identity hint to report to the client to help agree on the PSK for the provided
     * socket.
     *
     * <p>
     * The default implementation returns {@code null}.
     *
     * @return PSK identity hint to be provided to the client or {@code null} to provide no hint.
     */
    // @Override
    CARAPI ChooseServerKeyIdentityHint(
        /* [in] */ ISocket* socket,
        /* [out] */ String* result);

    /**
     * Gets the PSK identity hint to report to the client to help agree on the PSK for the provided
     * engine.
     *
     * <p>
     * The default implementation returns {@code null}.
     *
     * @return PSK identity hint to be provided to the client or {@code null} to provide no hint.
     */
    // @Override
    CARAPI ChooseServerKeyIdentityHint(
        /* [in] */ ISSLEngine* engine,
        /* [out] */ String* result);

    /**
     * Gets the PSK identity to report to the server to help agree on the PSK for the provided
     * socket.
     *
     * <p>
     * The default implementation returns an empty string.
     *
     * @param identityHint identity hint provided by the server or {@code null} if none provided.
     *
     * @return PSK identity to provide to the server. {@code null} is permitted but will be
     *         converted into an empty string.
     */
    // @Override
    CARAPI ChooseClientKeyIdentity(
        /* [in] */ const String& identityHint,
        /* [in] */ ISocket* socket,
        /* [out] */ String* result);

    /**
     * Gets the PSK identity to report to the server to help agree on the PSK for the provided
     * engine.
     *
     * <p>
     * The default implementation returns an empty string.
     *
     * @param identityHint identity hint provided by the server or {@code null} if none provided.
     *
     * @return PSK identity to provide to the server. {@code null} is permitted but will be
     *         converted into an empty string.
     */
    // @Override
    CARAPI ChooseClientKeyIdentity(
        /* [in] */ const String& identityHint,
        /* [in] */ ISSLEngine* engine,
        /* [out] */ String* result);

    /**
     * Gets the PSK to use for the provided socket.
     *
     * <p>
     * The default implementation returns {@code null}.
     *
     * @param identityHint identity hint provided by the server to help select the key or
     *        {@code null} if none provided.
     * @param identity identity provided by the client to help select the key.
     *
     * @return key or {@code null} to signal to peer that no suitable key is available and to abort
     *         the handshake.
     */
    // @Override
    CARAPI GetKey(
        /* [in] */ const String& identityHint,
        /* [in] */ const String& identity,
        /* [in] */ ISocket* socket,
        /* [out] */ ISecretKey** result);

    /**
     * Gets the PSK to use for the provided engine.
     *
     * <p>
     * The default implementation returns {@code null}.
     *
     * @param identityHint identity hint provided by the server to help select the key or
     *        {@code null} if none provided.
     * @param identity identity provided by the client to help select the key.
     *
     * @return key or {@code null} to signal to peer that no suitable key is available and to abort
     *         the handshake.
     */
    // @Override
    CARAPI GetKey(
        /* [in] */ const String& identityHint,
        /* [in] */ const String& identity,
        /* [in] */ ISSLEngine* engine,
        /* [out] */ ISecretKey** result);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_PSKKEYMANAGER_H__
