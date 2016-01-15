
#ifndef __ORG_APACHE_HTTP_CONN_SCHEME_CPLAINSOCKETFACTORY_H_
#define __ORG_APACHE_HTTP_CONN_SCHEME_CPLAINSOCKETFACTORY_H_

#include "_Org_Apache_Http_Conn_Scheme_CPlainSocketFactory.h"
#include "elastos/core/Object.h"

using Elastos::Net::IInetAddress;
using Elastos::Net::ISocket;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Scheme {

class CPlainSocketFactoryHelper;

/**
 * The default class for creating sockets.
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 * @author Michael Becke
 */
CarClass(CPlainSocketFactory)
    , public Object
    , public IPlainSocketFactory
    , public ISocketFactory
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Gets the singleton instance of this class.
     * @return the one and only plain socket factory
     */
    static CARAPI_(AutoPtr<IPlainSocketFactory>) GetSocketFactory();

    /**
     * Creates a new, unconnected socket.
     * The socket should subsequently be passed to
     * {@link #connectSocket connectSocket}.
     *
     * @return  a new socket
     *
     * @throws IOException if an I/O error occurs while creating the socket
     */
    CARAPI CreateSocket(
        /* [out] */ ISocket** socket);

    /**
     * Connects a socket to the given host.
     *
     * @param sock      the socket to connect, as obtained from
     *                  {@link #createSocket createSocket}.
     *                  <code>null</code> indicates that a new socket
     *                  should be created and connected.
     * @param host      the host to connect to
     * @param port      the port to connect to on the host
     * @param localAddress the local address to bind the socket to, or
     *                  <code>null</code> for any
     * @param localPort the port on the local machine,
     *                  0 or a negative number for any
     * @param params    additional {@link HttpParams parameters} for connecting
     *
     * @return  the connected socket. The returned object may be different
     *          from the <code>sock</code> argument if this factory supports
     *          a layered protocol.
     *
     * @throws IOException if an I/O error occurs
     * @throws UnknownHostException if the IP address of the target host
     *          can not be determined
     * @throws ConnectTimeoutException if the socket cannot be connected
     *          within the time limit defined in the <code>params</code>
     */
    CARAPI ConnectSocket(
        /* [in] */ ISocket* sock,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* localAddress,
        /* [in] */ Int32 localPort,
        /* [in] */ IHttpParams* params,
        /* [out] */ ISocket** socket);

    /**
     * Checks whether a socket provides a secure connection.
     * The socket must be {@link #connectSocket connected}
     * by this factory.
     * The factory will <i>not</i> perform I/O operations
     * in this method.
     * <br/>
     * As a rule of thumb, plain sockets are not secure and
     * TLS/SSL sockets are secure. However, there may be
     * application specific deviations. For example, a plain
     * socket to a host in the same intranet ("trusted zone")
     * could be considered secure. On the other hand, a
     * TLS/SSL socket could be considered insecure based on
     * the cypher suite chosen for the connection.
     *
     * @param sock      the connected socket to check
     *
     * @return  <code>true</code> if the connection of the socket
     *          should be considered secure, or
     *          <code>false</code> if it should not
     *
     * @throws IllegalArgumentException
     *  if the argument is invalid, for example because it is
     *  not a connected socket or was created by a different
     *  socket factory.
     *  Note that socket factories are <i>not</i> required to
     *  check these conditions, they may simply return a default
     *  value when called with an invalid socket argument.
     */
    CARAPI IsSecure(
        /* [in] */ ISocket* sock,
        /* [out] */ Boolean* isSecure);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI constructor(
        /* [in] */ IHostNameResolver* nameResolver);

    CARAPI constructor();

private:
    /**
     * The factory singleton.
     */
    static const AutoPtr<IPlainSocketFactory> DEFAULT_FACTORY;

    AutoPtr<IHostNameResolver> mNameResolver;

    friend class CPlainSocketFactoryHelper;

};

} // namespace Scheme
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_SCHEME_CPLAINSOCKETFACTORY_H_
