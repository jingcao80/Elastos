
#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_BASICRESPONSEHANDLER_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_BASICRESPONSEHANDLER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::Client::IResponseHandler;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

/**
 * A {@link ResponseHandler} that returns the response body as a String
 * for successful (2xx) responses. If the response code was >= 300, the response
 * body is consumed and an {@link HttpResponseException} is thrown.
 *
 * If this is used with
 * {@link org.apache.http.client.HttpClient#execute(
 *  org.apache.http.client.methods.HttpUriRequest, ResponseHandler),
 * HttpClient may handle redirects (3xx responses) internally.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 677240 $
 *
 * @since 4.0
 */
class BasicResponseHandler
    : public Object
    , public IResponseHandler
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the response body as a String if the response was successful (a
     * 2xx status code). If no response body exists, this returns null. If the
     * response was unsuccessful (>= 300 status code), throws an
     * {@link HttpResponseException}.
     */
    CARAPI HandleResponse(
        /* [in] */ IHttpResponse* response,
        /* [out] */ IInterface** value);

};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_BASICRESPONSEHANDLER_H__
