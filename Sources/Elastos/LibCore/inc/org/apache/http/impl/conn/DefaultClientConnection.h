
#ifndef __ORG_APACHE_HTTP_IMPL_CONN_DEFAULTCLIENTCONNECTION_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_DEFAULTCLIENTCONNECTION_H__

#include "org/apache/http/impl/SocketHttpClientConnection.h"

using Elastos::Net::ISocket;
using Org::Apache::Http::IHttpHost;
using Org::Apache::Http::Conn::IOperatedClientConnection;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

/**
 * Default implementation of an operated client connection.
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 *
 *
 * <!-- empty lines to avoid svn diff problems -->
 * @version   $Revision: 673450 $ $Date: 2008-07-02 10:35:05 -0700 (Wed, 02 Jul 2008) $
 *
 * @since 4.0
 */
class DefaultClientConnection
    : public SocketHttpClientConnection
    , public IOperatedClientConnection
{
public:
    DefaultClientConnection();

    CAR_INTERFACE_DECL()

    CARAPI GetTargetHost(
        /* [out] */ IHttpHost** host);

    CARAPI IsSecure(
        /* [out] */ Boolean* isSecure);

    CARAPI GetSocket(
        /* [out] */ ISocket** socket);

    CARAPI Opening(
        /* [in] */ ISocket* sock,
        /* [in] */ IHttpHost* target);

    CARAPI OpenCompleted(
        /* [in] */ Boolean secure,
        /* [in] */ IHttpParams* params);

    CARAPI Shutdown();

    CARAPI Close();

    CARAPI Update(
        /* [in] */ ISocket* sock,
        /* [in] */ IHttpHost* target,
        /* [in] */ Boolean secure,
        /* [in] */ IHttpParams* params);

    CARAPI ReceiveResponseHeader(
        /* [out] */ IHttpResponse** httpResponse);

    CARAPI SendRequestHeader(
        /* [in] */ IHttpRequest* request);

protected:
    CARAPI CreateSessionInputBuffer(
        /* [in] */ ISocket* socket,
        /* [in] */ Int32 buffersize,
        /* [in] */ IHttpParams* params,
        /* [out] */ ISessionInputBuffer** inputBuffer);

    CARAPI CreateSessionOutputBuffer(
        /* [in] */ ISocket* socket,
        /* [in] */ Int32 buffersize,
        /* [in] */ IHttpParams* params,
        /* [out] */ ISessionOutputBuffer** outputBuffer);

    CARAPI_(AutoPtr<IHttpMessageParser>) CreateResponseParser(
        /* [in] */ ISessionInputBuffer* buffer,
        /* [in] */ IHttpResponseFactory* responseFactory,
        /* [in] */ IHttpParams* params);

private:
    // final Log log = LogFactory.getLog(getClass());
    // final Log headerLog = LogFactory.getLog("org.apache.http.headers");
    // final Log wireLog = LogFactory.getLog("org.apache.http.wire");

    /** The unconnected socket */
    AutoPtr<ISocket> mSocket;

    /** The target host of this connection. */
    AutoPtr<IHttpHost> mTargetHost;

    /** Whether this connection is secure. */
    Boolean mConnSecure;

    /** True if this connection was shutdown. */
    Boolean mShutdown;
};

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_DEFAULTCLIENTCONNECTION_H__
