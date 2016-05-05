
#include "Elastos.CoreLibrary.Text.h"
#include "org/apache/http/protocol/ResponseDate.h"
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
const AutoPtr<IHttpDateGenerator> ResponseDate::DATE_GENERATOR;// = InitDateGenerator();

CAR_INTERFACE_IMPL(ResponseDate, Object, IHttpResponseInterceptor)

ECode ResponseDate::Process(
    /* [in] */ IHttpResponse* response,
    /* [in] */ IHttpContext* context)
{
    if (response == NULL) {
        Logger::E("ResponseDate", "HTTP response may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IStatusLine> sl;
    response->GetStatusLine((IStatusLine**)&sl);
    Int32 status;
    sl->GetStatusCode(&status);
    Boolean contains;
    if ((status >= IHttpStatus::SC_OK) &&
            (IHttpMessage::Probe(response)->ContainsHeader(IHTTP::DATE_HEADER, &contains), !contains)) {
        String httpdate;
        DATE_GENERATOR->GetCurrentDate(&httpdate);
        IHttpMessage::Probe(response)->SetHeader(IHTTP::DATE_HEADER, httpdate);
    }
    return NOERROR;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
