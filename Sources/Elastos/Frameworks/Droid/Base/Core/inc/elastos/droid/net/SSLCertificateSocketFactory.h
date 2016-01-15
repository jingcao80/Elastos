
#ifndef __ELASTOS_DROID_NET_SSLCERTIFICATESOCKETFACTORY_H__
#define __ELASTOS_DROID_NET_SSLCERTIFICATESOCKETFACTORY_H__

#include <Elastos.CoreLibrary.Security.h>
#include "Elastos.Droid.Net.h"
#include "_Elastos.Droid.Core.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastosx/net/ssl/SSLSocketFactory.h>

using Elastos::Core::IArrayOf;
using Elastos::Security::IPrivateKey;
using Elastos::Security::Cert::IX509Certificate;
using Elastosx::Net::Ssl::IKeyManager;
using Elastosx::Net::Ssl::ITrustManager;
using Elastosx::Net::Ssl::IX509TrustManager;
using Elastosx::Net::Ssl::SSLSocketFactory;

using Org::Conscrypt::IOpenSSLSocketImpl;
using Org::Conscrypt::ISSLClientSessionCache;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * SSLSocketFactory implementation with several extra features:
 *
 * <ul>
 * <li>Timeout specification for SSL handshake operations
 * <li>Hostname verification in most cases (see WARNINGs below)
 * <li>Optional SSL session caching with {@link SSLSessionCache}
 * <li>Optionally bypass all SSL certificate checks
 * </ul>
 *
 * The handshake timeout does not apply to actual TCP socket connection.
 * If you want a connection timeout as well, use {@link #createSocket()}
 * and {@link Socket#connect(SocketAddress, int)}, after which you
 * must verify the identity of the server you are connected to.
 *
 * <p class="caution"><b>Most {@link SSLSocketFactory} implementations do not
 * verify the server's identity, allowing man-in-the-middle attacks.</b>
 * This implementation does check the server's certificate hostname, but only
 * for createSocket variants that specify a hostname.  When using methods that
 * use {@link InetAddress} or which return an unconnected socket, you MUST
 * verify the server's identity yourself to ensure a secure connection.</p>
 *
 * <p>One way to verify the server's identity is to use
 * {@link HttpsURLConnection#getDefaultHostnameVerifier()} to get a
 * {@link HostnameVerifier} to verify the certificate hostname.
 *
 * <p>On development devices, "setprop socket.relaxsslcheck yes" bypasses all
 * SSL certificate and hostname checks for testing purposes.  This setting
 * requires root access.
 */
class SSLCertificateSocketFactory
    : public SSLSocketFactory
    , public ISSLCertificateSocketFactory
{
private:
    class InnerSub_TrustManager
        : public Object
        , public ITrustManager
        , public IX509TrustManager
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI GetAcceptedIssuers(
            /* [out, callee] */ ArrayOf<IX509Certificate*>** result);

        CARAPI CheckClientTrusted(
            /* [in] */ ArrayOf<IX509Certificate*>* certs,
            /* [in] */ const String& authType);

        CARAPI CheckServerTrusted(
            /* [in] */ ArrayOf<IX509Certificate*>* certs,
            /* [in] */ const String& authType);
    };

public:
    CAR_INTERFACE_DECL()

    SSLCertificateSocketFactory();

    /** @deprecated Use {@link #getDefault(int)} instead. */
    // @Deprecated
    CARAPI constructor(
        /* [in] */ Int32 handshakeTimeoutMillis);

    /**
     * Returns a new socket factory instance with an optional handshake timeout.
     *
     * @param handshakeTimeoutMillis to use for SSL connection handshake, or 0
     *         for none.  The socket timeout is reset to 0 after the handshake.
     * @return a new SSLSocketFactory with the specified parameters
     */
    static CARAPI GetDefault(
        /* [in] */ Int32 handshakeTimeoutMillis,
        /* [out] */ ISocketFactory** result);

    /**
     * Returns a new socket factory instance with an optional handshake timeout
     * and SSL session cache.
     *
     * @param handshakeTimeoutMillis to use for SSL connection handshake, or 0
     *         for none.  The socket timeout is reset to 0 after the handshake.
     * @param cache The {@link SSLSessionCache} to use, or null for no cache.
     * @return a new SSLSocketFactory with the specified parameters
     */
    static CARAPI GetDefault(
        /* [in] */ Int32 handshakeTimeoutMillis,
        /* [in] */ ISSLSessionCache* cache,
        /* [out] */ ISSLSocketFactory** result);

    /**
     * Returns a new instance of a socket factory with all SSL security checks
     * disabled, using an optional handshake timeout and SSL session cache.
     *
     * <p class="caution"><b>Warning:</b> Sockets created using this factory
     * are vulnerable to man-in-the-middle attacks!</p>
     *
     * @param handshakeTimeoutMillis to use for SSL connection handshake, or 0
     *         for none.  The socket timeout is reset to 0 after the handshake.
     * @param cache The {@link SSLSessionCache} to use, or null for no cache.
     * @return an insecure SSLSocketFactory with the specified parameters
     */
    static CARAPI GetInsecure(
        /* [in] */ Int32 handshakeTimeoutMillis,
        /* [in] */ ISSLSessionCache* cache,
        /* [out] */ ISSLSocketFactory** result);

    /**
     * Returns a socket factory (also named SSLSocketFactory, but in a different
     * namespace) for use with the Apache HTTP stack.
     *
     * @param handshakeTimeoutMillis to use for SSL connection handshake, or 0
     *         for none.  The socket timeout is reset to 0 after the handshake.
     * @param cache The {@link SSLSessionCache} to use, or null for no cache.
     * @return a new SocketFactory with the specified parameters
     */
    static CARAPI GetHttpSocketFactory(
        /* [in] */ Int32 handshakeTimeoutMillis,
        /* [in] */ ISSLSessionCache* cache,
        /* [out] */ Org::Apache::Http::Conn::SSL::ISSLSocketFactory** result);

    /**
     * Verify the hostname of the certificate used by the other end of a
     * connected socket.  You MUST call this if you did not supply a hostname
     * to {@link #createSocket()}.  It is harmless to call this method
     * redundantly if the hostname has already been verified.
     *
     * <p>Wildcard certificates are allowed to verify any matching hostname,
     * so "foo.bar.example.com" is verified if the peer has a certificate
     * for "*.example.com".
     *
     * @param socket An SSL socket which has been connected to a server
     * @param hostname The expected hostname of the remote server
     * @throws IOException if something goes wrong handshaking with the server
     * @throws SSLPeerUnverifiedException if the server cannot prove its identity
     *
     * @hide
     */
    static CARAPI VerifyHostname(
        /* [in] */ ISocket* socket,
        /* [in] */ const String& hostname);

    /**
     * Sets the {@link TrustManager}s to be used for connections made by this factory.
     */
    CARAPI SetTrustManagers(
        /* [in] */ ArrayOf<ITrustManager*>* trustManager);

    /**
     * Sets the <a href="http://technotes.googlecode.com/git/nextprotoneg.html">Next
     * Protocol Negotiation (NPN)</a> protocols that this peer is interested in.
     *
     * <p>For servers this is the sequence of protocols to advertise as
     * supported, in order of preference. This list is sent unencrypted to
     * all clients that support NPN.
     *
     * <p>For clients this is a list of supported protocols to match against the
     * server's list. If there is no protocol supported by both client and
     * server then the first protocol in the client's list will be selected.
     * The order of the client's protocols is otherwise insignificant.
     *
     * @param npnProtocols a non-empty list of protocol byte arrays. All arrays
     *     must be non-empty and of length less than 256.
     */
    CARAPI SetNpnProtocols(
        /* [in] */ ArrayOf<IArrayOf*>* npnProtocols);

    /**
     * Sets the
     * <a href="http://tools.ietf.org/html/draft-ietf-tls-applayerprotoneg-01">
     * Application Layer Protocol Negotiation (ALPN)</a> protocols that this peer
     * is interested in.
     *
     * <p>For servers this is the sequence of protocols to advertise as
     * supported, in order of preference. This list is sent unencrypted to
     * all clients that support ALPN.
     *
     * <p>For clients this is a list of supported protocols to match against the
     * server's list. If there is no protocol supported by both client and
     * server then the first protocol in the client's list will be selected.
     * The order of the client's protocols is otherwise insignificant.
     *
     * @param protocols a non-empty list of protocol byte arrays. All arrays
     *     must be non-empty and of length less than 256.
     * @hide
     */
    CARAPI SetAlpnProtocols(
        /* [in] */ ArrayOf<IArrayOf*>* protocols);

    /**
     * Returns an array containing the concatenation of length-prefixed byte
     * strings.
     */
    static CARAPI ToLengthPrefixedList(
        /* [in] */ ArrayOf<IArrayOf*>* items,
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * Returns the <a href="http://technotes.googlecode.com/git/nextprotoneg.html">Next
     * Protocol Negotiation (NPN)</a> protocol selected by client and server, or
     * null if no protocol was negotiated.
     *
     * @param socket a socket created by this factory.
     * @throws IllegalArgumentException if the socket was not created by this factory.
     */
    CARAPI GetNpnSelectedProtocol(
        /* [in] */ ISocket* socket,
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * Returns the
     * <a href="http://tools.ietf.org/html/draft-ietf-tls-applayerprotoneg-01">Application
     * Layer Protocol Negotiation (ALPN)</a> protocol selected by client and server, or null
     * if no protocol was negotiated.
     *
     * @param socket a socket created by this factory.
     * @throws IllegalArgumentException if the socket was not created by this factory.
     * @hide
     */
    CARAPI GetAlpnSelectedProtocol(
        /* [in] */ ISocket* socket,
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * Sets the {@link KeyManager}s to be used for connections made by this factory.
     */
    CARAPI SetKeyManagers(
        /* [in] */ ArrayOf<IKeyManager*>* keyManagers);

    /**
     * Sets the private key to be used for TLS Channel ID by connections made by this
     * factory.
     *
     * @param privateKey private key (enables TLS Channel ID) or {@code null} for no key (disables
     *        TLS Channel ID). The private key has to be an Elliptic Curve (EC) key based on the
     *        NIST P-256 curve (aka SECG secp256r1 or ANSI X9.62 prime256v1).
     *
     * @hide
     */
    CARAPI SetChannelIdPrivateKey(
        /* [in] */ IPrivateKey* privateKey);

    /**
     * Enables <a href="http://tools.ietf.org/html/rfc5077#section-3.2">session ticket</a>
     * support on the given socket.
     *
     * @param socket a socket created by this factory
     * @param useSessionTickets {@code true} to enable session ticket support on this socket.
     * @throws IllegalArgumentException if the socket was not created by this factory.
     */
    CARAPI SetUseSessionTickets(
        /* [in] */ ISocket* socket,
        /* [in] */ Boolean useSessionTickets);

    /**
     * Turns on <a href="http://tools.ietf.org/html/rfc6066#section-3">Server
     * Name Indication (SNI)</a> on a given socket.
     *
     * @param socket a socket created by this factory.
     * @param hostName the desired SNI hostname, null to disable.
     * @throws IllegalArgumentException if the socket was not created by this factory.
     */
    CARAPI SetHostname(
        /* [in] */ ISocket* socket,
        /* [in] */ const String& hostName);

    /**
     * Sets this socket's SO_SNDTIMEO write timeout in milliseconds.
     * Use 0 for no timeout.
     * To take effect, this option must be set before the blocking method was called.
     *
     * @param socket a socket created by this factory.
     * @param timeout the desired write timeout in milliseconds.
     * @throws IllegalArgumentException if the socket was not created by this factory.
     *
     * @hide
     */
    CARAPI SetSoWriteTimeout(
        /* [in] */ ISocket* socket,
        /* [in] */ Int32 writeTimeoutMilliseconds);

    /**
     * {@inheritDoc}
     *
     * <p>This method verifies the peer's certificate hostname after connecting
     * (unless created with {@link #getInsecure(int, SSLSessionCache)}).
     */
    // @Override
    CARAPI CreateSocket(
        /* [in] */ ISocket* k,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ Boolean close,
        /* [out] */ ISocket** result);

    /**
     * Creates a new socket which is not connected to any remote host.
     * You must use {@link Socket#connect} to connect the socket.
     *
     * <p class="caution"><b>Warning:</b> Hostname verification is not performed
     * with this method.  You MUST verify the server's identity after connecting
     * the socket to avoid man-in-the-middle attacks.</p>
     */
    // @Override
    CARAPI CreateSocket(
        /* [out] */ ISocket** result);

    /**
     * {@inheritDoc}
     *
     * <p class="caution"><b>Warning:</b> Hostname verification is not performed
     * with this method.  You MUST verify the server's identity after connecting
     * the socket to avoid man-in-the-middle attacks.</p>
     */
    // @Override
    CARAPI CreateSocket(
        /* [in] */ IInetAddress* addr,
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* localAddr,
        /* [in] */ Int32 localPort,
        /* [out] */ ISocket** result);

    /**
     * {@inheritDoc}
     *
     * <p class="caution"><b>Warning:</b> Hostname verification is not performed
     * with this method.  You MUST verify the server's identity after connecting
     * the socket to avoid man-in-the-middle attacks.</p>
     */
    // @Override
    CARAPI CreateSocket(
        /* [in] */ IInetAddress* addr,
        /* [in] */ Int32 port,
        /* [out] */ ISocket** result);

    /**
     * {@inheritDoc}
     *
     * <p>This method verifies the peer's certificate hostname after connecting
     * (unless created with {@link #getInsecure(int, SSLSessionCache)}).
     */
    // @Override
    CARAPI CreateSocket(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* localAddr,
        /* [in] */ Int32 localPort,
        /* [out] */ ISocket** result);

    /**
     * {@inheritDoc}
     *
     * <p>This method verifies the peer's certificate hostname after connecting
     * (unless created with {@link #getInsecure(int, SSLSessionCache)}).
     */
    // @Override
    CARAPI CreateSocket(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [out] */ ISocket** result);

    // @Override
    CARAPI GetDefaultCipherSuites(
        /* [out, callee] */ ArrayOf<String>** result);

    // @Override
    CARAPI GetSupportedCipherSuites(
        /* [out, callee] */ ArrayOf<String>** result);

private:
    CARAPI constructor(
        /* [in] */ Int32 handshakeTimeoutMillis,
        /* [in] */ ISSLSessionCache* cache,
        /* [in] */ Boolean secure);

    CARAPI MakeSocketFactory(
        /* [in] */ ArrayOf<IKeyManager*>* keyManagers,
        /* [in] */ ArrayOf<ITrustManager*>* trustManagers,
        /* [out] */ ISSLSocketFactory** result);

    static CARAPI_(Boolean) IsSslCheckRelaxed();

    CARAPI_(AutoPtr<ISSLSocketFactory>) GetDelegate();

    static CARAPI CastToOpenSSLSocket(
        /* [in] */ ISocket* socket,
        /* [out] */ IOpenSSLSocketImpl** result);

private:
    /* const */ AutoPtr<ArrayOf<ITrustManager*> > INSECURE_TRUST_MANAGER;

    static const String TAG;

    AutoPtr<ISSLSocketFactory> mInsecureFactory;

    AutoPtr<ISSLSocketFactory> mSecureFactory;

    AutoPtr<ArrayOf<ITrustManager*> > mTrustManagers;

    AutoPtr<ArrayOf<IKeyManager*> > mKeyManagers;

    AutoPtr<ArrayOf<Byte> > mNpnProtocols;

    AutoPtr<ArrayOf<Byte> > mAlpnProtocols;

    AutoPtr<IPrivateKey> mChannelIdPrivateKey;

    Int32 mHandshakeTimeoutMillis;

    AutoPtr<ISSLClientSessionCache> mSessionCache;

    Boolean mSecure;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_SSLCERTIFICATESOCKETFACTORY_H__
