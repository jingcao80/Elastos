
#ifndef __ELASTOS_DROID_NET_CSSLCERTIFICATESOCKETFACTORYHELPER_H__
#define __ELASTOS_DROID_NET_CSSLCERTIFICATESOCKETFACTORYHELPER_H__

#include "_Elastos_Droid_Net_CSSLCertificateSocketFactoryHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Net::ISocket;
using Elastosx::Net::ISocketFactory;
using Elastosx::Net::Ssl::ISSLSocketFactory;

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
CarClass(CSSLCertificateSocketFactoryHelper)
    , public Singleton
    , public ISSLCertificateSocketFactoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns a new socket factory instance with an optional handshake timeout.
     *
     * @param handshakeTimeoutMillis to use for SSL connection handshake, or 0
     *         for none.  The socket timeout is reset to 0 after the handshake.
     * @return a new SSLSocketFactory with the specified parameters
     */
    CARAPI GetDefault(
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
    CARAPI GetDefault(
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
    CARAPI GetInsecure(
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
    CARAPI GetHttpSocketFactory(
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
    CARAPI VerifyHostname(
        /* [in] */ ISocket* socket,
        /* [in] */ const String& hostname);
};

} //namespace Net
} //namespace Droid
} //namespace Elastos

#endif // __ELASTOS_DROID_NET_CSSLCERTIFICATESOCKETFACTORYHELPER_H__
