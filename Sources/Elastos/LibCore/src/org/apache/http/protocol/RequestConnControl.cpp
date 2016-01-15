
#include "org/apache/http/protocol/RequestConnControl.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHttpMessage;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_INTERFACE_IMPL(RequestConnControl, Object, IHttpRequestInterceptor)

ECode RequestConnControl::Process(
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context)
{
    if (request == NULL) {
        Logger::E("RequestConnControl", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpMessage> message = IHttpMessage::Probe(request);
    Boolean contains;
    if (message->ContainsHeader(IHTTP::CONN_DIRECTIVE, &contains), !contains) {
        // Default policy is to keep connection alive
        // whenever possible
        message->AddHeader(IHTTP::CONN_DIRECTIVE, IHTTP::CONN_KEEP_ALIVE);
    }
    return NOERROR;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
