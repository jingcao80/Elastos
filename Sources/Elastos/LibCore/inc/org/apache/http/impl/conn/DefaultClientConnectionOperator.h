
#ifndef __ORG_APACHE_HTTP_IMPL_CONN_DEFAULTCLIENTCONNECTIONOPERATOR_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_DEFAULTCLIENTCONNECTIONOPERATOR_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Net::IInetAddress;
using Elastos::Net::ISocket;
using Org::Apache::Http::IHttpHost;
using Org::Apache::Http::Conn::IOperatedClientConnection;
using Org::Apache::Http::Conn::IClientConnectionOperator;
using Org::Apache::Http::Conn::Scheme::ISchemeRegistry;
using Org::Apache::Http::Conn::Scheme::IPlainSocketFactory;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

/**
 * Default implementation of a
 * {@link ClientConnectionOperator ClientConnectionOperator}.
 * It uses a {@link SchemeRegistry SchemeRegistry} to look up
 * {@link SocketFactory SocketFactory} objects.
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 *
 *
 * <!-- empty lines to avoid svn diff problems -->
 * @version   $Revision: 652193 $ $Date: 2008-04-29 17:10:36 -0700 (Tue, 29 Apr 2008) $
 *
 * @since 4.0
 */
class DefaultClientConnectionOperator
    : public Object
    , public IClientConnectionOperator
{
public:
    /**
     * Creates a new client connection operator for the given scheme registry.
     *
     * @param schemes   the scheme registry
     */
    DefaultClientConnectionOperator(
        /* [in] */ ISchemeRegistry* schemes);

    CAR_INTERFACE_DECL()

    CARAPI CreateConnection(
        /* [out] */ IOperatedClientConnection** connection);

    CARAPI OpenConnection(
        /* [in] */ IOperatedClientConnection* conn,
        /* [in] */ IHttpHost* target,
        /* [in] */ IInetAddress* local,
        /* [in] */ IHttpContext* context,
        /* [in] */ IHttpParams* params);

    CARAPI UpdateSecureConnection(
        /* [in] */ IOperatedClientConnection* conn,
        /* [in] */ IHttpHost* target,
        /* [in] */ IHttpContext* context,
        /* [in] */ IHttpParams* params);

protected:
    /**
     * Performs standard initializations on a newly created socket.
     *
     * @param sock      the socket to prepare
     * @param context   the context for the connection
     * @param params    the parameters from which to prepare the socket
     *
     * @throws IOException      in case of an IO problem
     */
    CARAPI PrepareSocket(
        /* [in] */ ISocket* sock,
        /* [in] */ IHttpContext* context,
        /* [in] */ IHttpParams* params);

protected:
    /** The scheme registry for looking up socket factories. */
    AutoPtr<ISchemeRegistry> mSchemeRegistry;

private:
    static const AutoPtr<IPlainSocketFactory> sStaticPlainSocketFactory;

};

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_DEFAULTCLIENTCONNECTIONOPERATOR_H__
