
#include "Elastos.CoreLibrary.Text.h"
#include "org/apache/http/protocol/RequestDate.h"
#include "org/apache/http/protocol/CHttpDateGenerator.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHttpStatus;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::IProtocolVersion;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

static AutoPtr<IHttpDateGenerator> InitDateGenerator()
{
    AutoPtr<IHttpDateGenerator> generator;
    CHttpDateGenerator::New((IHttpDateGenerator**)&generator);
    return generator;
}
//TODO:
const AutoPtr<IHttpDateGenerator> RequestDate::DATE_GENERATOR;// = InitDateGenerator();

CAR_INTERFACE_IMPL(RequestDate, Object, IHttpRequestInterceptor)

ECode RequestDate::Process(
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context)
{
    if (request == NULL) {
        Logger::E("RequestDate", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpEntityEnclosingRequest> enclosingRequest = IHttpEntityEnclosingRequest::Probe(request);
    Boolean contains;
    if (enclosingRequest != NULL &&
            (IHttpMessage::Probe(request)->ContainsHeader(IHTTP::DATE_HEADER, &contains), !contains)) {
        String httpdate;
        DATE_GENERATOR->GetCurrentDate(&httpdate);
        IHttpMessage::Probe(request)->SetHeader(IHTTP::DATE_HEADER, httpdate);
    }
    return NOERROR;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
