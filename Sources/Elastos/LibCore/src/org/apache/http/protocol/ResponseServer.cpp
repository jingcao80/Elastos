
#include "org/apache/http/protocol/ResponseServer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Params::ICoreProtocolPNames;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_INTERFACE_IMPL(ResponseServer, Object, IHttpResponseInterceptor)

ECode ResponseServer::Process(
    /* [in] */ IHttpResponse* response,
    /* [in] */ IHttpContext* context)
{
    if (response == NULL) {
        Logger::E("ResponseServer", "HTTP response may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpMessage> message = IHttpMessage::Probe(response);
    Boolean contains;
    if (message->ContainsHeader(IHTTP::SERVER_HEADER, &contains), !contains) {
        AutoPtr<IHttpParams> params;
        message->GetParams((IHttpParams**)&params);
        AutoPtr<IInterface> param;
        params->GetParameter(ICoreProtocolPNames::ORIGIN_SERVER, (IInterface**)&param);
        AutoPtr<ICharSequence> cs = ICharSequence::Probe(param);
        String s;
        cs->ToString(&s);
        if (!s.IsNull()) {
            message->AddHeader(IHTTP::SERVER_HEADER, s);
        }
    }
    return NOERROR;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
