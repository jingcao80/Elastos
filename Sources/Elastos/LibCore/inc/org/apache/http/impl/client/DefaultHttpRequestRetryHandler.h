
#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_DEFAULTHTTPREQUESTRETRYHANDLER_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_DEFAULTHTTPREQUESTRETRYHANDLER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::Client::IHttpRequestRetryHandler;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

/**
 * The default {@link HttpRequestRetryHandler} used by request executors.
 *
 * @author Michael Becke
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 */
class DefaultHttpRequestRetryHandler
    : public Object
    , public IHttpRequestRetryHandler
{
public:
    /**
     * Default constructor
     */
    DefaultHttpRequestRetryHandler(
        /* [in] */ Int32 retryCount,
        /* [in] */ Boolean requestSentRetryEnabled);

    /**
     * Default constructor.
     */
    DefaultHttpRequestRetryHandler();

    CAR_INTERFACE_DECL()

    /**
     * Used <code>retryCount</code> and <code>requestSentRetryEnabled</code> to determine
     * if the given method should be retried.
     */
    CARAPI RetryRequest(
        /* [in] */ ECode exception,
        /* [in] */ Int32 executionCount,
        /* [in] */ IHttpContext* context,
        /* [out] */ Boolean* retried);

    /**
     * @return <code>true</code> if this handler will retry methods that have
     * successfully sent their request, <code>false</code> otherwise
     */
    CARAPI_(Boolean) IsRequestSentRetryEnabled();

    /**
     * @return the maximum number of times a method will be retried
     */
    CARAPI_(Int32) GetRetryCount();

private:
    /** the number of times a method will be retried */
    Int32 mRetryCount;

    /** Whether or not methods that have successfully sent their request will be retried */
    Boolean mRequestSentRetryEnabled;
};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_DEFAULTHTTPREQUESTRETRYHANDLER_H__
