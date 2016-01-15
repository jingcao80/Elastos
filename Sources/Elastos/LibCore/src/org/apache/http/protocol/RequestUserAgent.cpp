
#include "org/apache/http/protocol/RequestUserAgent.h"
#include "org/apache/http/params/HttpProtocolParams.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Params::HttpProtocolParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_INTERFACE_IMPL(RequestUserAgent, Object, IHttpRequestInterceptor)

ECode RequestUserAgent::Process(
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context)
{
    if (request == NULL) {
        Logger::E("RequestUserAgent", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpMessage> message = IHttpMessage::Probe(request);
    Boolean contains;
    if (message->ContainsHeader(IHTTP::USER_AGENT, &contains), !contains) {
        AutoPtr<IHttpParams> params;
        message->GetParams((IHttpParams**)&params);
        String useragent;
        HttpProtocolParams::GetUserAgent(params, &useragent);
        if (!useragent.IsNull()) {
            message->AddHeader(IHTTP::USER_AGENT, useragent);
        }
    }
    return NOERROR;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
