
#ifndef __ELASTOSX_NET_SSL_SSLSOCKETFACTORY_H__
#define __ELASTOSX_NET_SSL_SSLSOCKETFACTORY_H__

#include <elastosx/net/SocketFactory.h>

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * The abstract factory implementation to create {@code SSLSocket}s.
 */
class ECO_PUBLIC SSLSocketFactory
    : public SocketFactory
    , public ISSLSocketFactory
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the default {@code SSLSocketFactory} instance. The default is
     * defined by the security property {@code 'ssl.SocketFactory.provider'}.
     *
     * @return the default ssl socket factory instance.
     */
    static CARAPI GetDefault(
        /* [out] */ ISocketFactory** factory);

    /**
     * Returns the names of the cipher suites that are enabled by default.
     *
     * @return the names of the cipher suites that are enabled by default.
     */
    virtual CARAPI GetDefaultCipherSuites(
        /* [out, callee] */ ArrayOf<String>** suites) = 0;

    /**
     * Returns the names of the cipher suites that are supported and could be
     * enabled for an SSL connection.
     *
     * @return the names of the cipher suites that are supported.
     */
    virtual CARAPI GetSupportedCipherSuites(
        /* [out, callee] */ ArrayOf<String>** suites) = 0;

    /**
     * Creates an {@code SSLSocket} over the specified socket that is connected
     * to the specified host at the specified port.
     *
     * @param s
     *            the socket.
     * @param host
     *            the host.
     * @param port
     *            the port number.
     * @param autoClose
     *            {@code true} if socket {@code s} should be closed when the
     *            created socket is closed, {@code false} if the socket
     *            {@code s} should be left open.
     * @return the creates ssl socket.
     * @throws IOException
     *             if creating the socket fails.
     * @throws java.net.UnknownHostException
     *             if the host is unknown.
     */
    virtual CARAPI CreateSocket(
        /* [in] */ ISocket* s,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ Boolean autoClose,
        /* [out] */ ISocket** sock) = 0;

protected:
    SSLSocketFactory();

    virtual ~SSLSocketFactory();

private:
    // FIXME EXPORT CONTROL

    // The default SSL socket factory
    ECO_LOCAL static AutoPtr<ISocketFactory> sDefaultSocketFactory;

    ECO_LOCAL static Int32 sLastCacheVersion;

    ECO_LOCAL static Object sLock;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_SSLSOCKETFACTORY_H__
