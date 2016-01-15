
#include "org/apache/http/impl/client/DefaultHttpRequestRetryHandler.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::IBoolean;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Client::EIID_IHttpRequestRetryHandler;
using Org::Apache::Http::Protocol::IExecutionContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

DefaultHttpRequestRetryHandler::DefaultHttpRequestRetryHandler(
    /* [in] */ Int32 retryCount,
    /* [in] */ Boolean requestSentRetryEnabled)
    : mRetryCount(retryCount)
    , mRequestSentRetryEnabled(requestSentRetryEnabled)
{}

DefaultHttpRequestRetryHandler::DefaultHttpRequestRetryHandler()
    : mRetryCount(3)
    , mRequestSentRetryEnabled(FALSE)
{}

CAR_INTERFACE_IMPL(DefaultHttpRequestRetryHandler, Object, IHttpRequestRetryHandler)

ECode DefaultHttpRequestRetryHandler::RetryRequest(
    /* [in] */ ECode exception,
    /* [in] */ Int32 executionCount,
    /* [in] */ IHttpContext* context,
    /* [out] */ Boolean* retried)
{
    VALIDATE_NOT_NULL(retried)
    *retried = FALSE;
    // if (exception == null) {
    //     throw new IllegalArgumentException("Exception parameter may not be null");
    // }
    if (context == NULL) {
        Logger::E("DefaultHttpRequestRetryHandler", "HTTP context may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (executionCount > mRetryCount) {
        // Do not retry if over max retry count
        *retried = FALSE;
        return NOERROR;
    }
    if (exception == (ECode)E_NO_HTTP_RESPONSE_EXCEPTION) {
        // Retry if the server dropped connection on us
        *retried = TRUE;
        return NOERROR;
    }
    if (exception == (ECode)E_INTERRUPTED_IO_EXCEPTION) {
        // Timeout
        *retried = FALSE;
        return NOERROR;
    }
    if (exception == (ECode)E_UNKNOWN_HOST_EXCEPTION) {
        // Unknown host
        *retried = FALSE;
        return NOERROR;
    }
    if (exception == (ECode)E_SSL_HANDSHAKE_EXCEPTION) {
        // SSL handshake exception
        *retried = FALSE;
        return NOERROR;
    }
    AutoPtr<IInterface> attr;
    context->GetAttribute(IExecutionContext::HTTP_REQ_SENT, (IInterface**)&attr);
    AutoPtr<IBoolean> b = IBoolean::Probe(attr);
    Boolean value;
    Boolean sent = (b != NULL && (b->GetValue(&value), value));
    if (!sent || mRequestSentRetryEnabled) {
        // Retry if the request has not been sent fully or
        // if it's OK to retry methods that have been sent
        *retried = TRUE;
        return NOERROR;
    }
    // otherwise do not retry
    *retried = FALSE;
    return NOERROR;
}

Boolean DefaultHttpRequestRetryHandler::IsRequestSentRetryEnabled()
{
    return mRequestSentRetryEnabled;
}

Int32 DefaultHttpRequestRetryHandler::GetRetryCount()
{
    return mRetryCount;
}

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org