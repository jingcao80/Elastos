
#include "org/apache/http/protocol/RequestExpectContinue.h"
#include "org/apache/http/CHttpVersion.h"
#include "org/apache/http/params/HttpProtocolParams.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHttpEntityEnclosingRequest;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::CHttpVersion;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::Params::HttpProtocolParams;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_INTERFACE_IMPL(RequestExpectContinue, Object, IHttpRequestInterceptor)

ECode RequestExpectContinue::Process(
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context)
{
    if (request == NULL) {
        Logger::E("RequestExpectContinue", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpEntityEnclosingRequest> enclosingRequest = IHttpEntityEnclosingRequest::Probe(request);
    if (enclosingRequest != NULL) {
        AutoPtr<IHttpEntity> entity;
        enclosingRequest->GetEntity((IHttpEntity**)&entity);
        // Do not send the expect header if request body is known to be empty
        Int64 len;
        if (entity != NULL && (entity->GetContentLength(&len), len != 0)) {
            AutoPtr<IRequestLine> rl;
            request->GetRequestLine((IRequestLine**)&rl);
            AutoPtr<IProtocolVersion> ver;
            rl->GetProtocolVersion((IProtocolVersion**)&ver);
            AutoPtr<IHttpParams> params;
            IHttpMessage::Probe(request)->GetParams((IHttpParams**)&params);
            Boolean lessEquals, result;
            if ((HttpProtocolParams::UseExpectContinue(params, &result), result)
                    && (ver->LessEquals(IProtocolVersion::Probe(CHttpVersion::HTTP_1_0), &lessEquals), !lessEquals)) {
                IHttpMessage::Probe(request)->AddHeader(IHTTP::EXPECT_DIRECTIVE, IHTTP::EXPECT_CONTINUE);
            }
        }
    }
    return NOERROR;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
