
#ifndef __ELASTOS_DROID_NET_HTTP_HTTPCONNECTION_H__
#define __ELASTOS_DROID_NET_HTTP_HTTPCONNECTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/http/Connection.h"

using Elastos::Droid::Content::IContext;
using Org::Apache::Http::IHttpHost;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * A requestConnection connecting to a normal (non secure) http server
 *
 * {@hide}
 */
class HttpConnection
    : public Connection
{
public:
    HttpConnection();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IHttpHost* host,
        /* [in] */ IRequestFeeder* requestFeeder);

    /**
     * Opens the connection to a http server
     *
     * @return the opened low level connection
     * @throws IOException if the connection fails for any reason.
     */
    // @Override
    CARAPI OpenConnection(
        /* [in] */ IRequest* req,
        /* [out] */ IElastosHttpClientConnection** result);

    /**
     * Closes the low level connection.
     *
     * If an exception is thrown then it is assumed that the
     * connection will have been closed (to the extent possible)
     * anyway and the caller does not need to take any further action.
     *
     */
    CARAPI CloseConnection();

    /**
     * Restart a secure connection suspended waiting for user interaction.
     */
    CARAPI RestartConnection(
        /* [in] */ Boolean abort);

    CARAPI GetScheme(
        /* [out] */ String* result);
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_HTTPCONNECTION_H__
