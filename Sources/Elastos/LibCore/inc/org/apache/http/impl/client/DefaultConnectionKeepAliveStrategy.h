
#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_DEFAULTCONNECTIONKEEPALIVESTRATEGY_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_DEFAULTCONNECTIONKEEPALIVESTRATEGY_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::Conn::IConnectionKeepAliveStrategy;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

/**
 * Default implementation of a strategy deciding duration
 * that a connection can remain idle.
 *
 * The default implementation looks solely at the 'Keep-Alive'
 * header's timeout token.
 *
 * @author <a href="mailto:sberlin at gmail.com">Sam Berlin</a>
 *
 * @version $Revision: $
 *
 * @since 4.0
 */
class DefaultConnectionKeepAliveStrategy
    : public Object
    , public IConnectionKeepAliveStrategy
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the response body as a String if the response was successful (a
     * 2xx status code). If no response body exists, this returns null. If the
     * response was unsuccessful (>= 300 status code), throws an
     * {@link HttpResponseException}.
     */
    CARAPI GetKeepAliveDuration(
        /* [in] */ IHttpResponse* response,
        /* [in] */ IHttpContext* context,
        /* [out] */ Int64* duration);

};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_DEFAULTCONNECTIONKEEPALIVESTRATEGY_H__
