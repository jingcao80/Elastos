
#include "org/apache/http/impl/client/DefaultProxyAuthenticationHandler.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::IHttpStatus;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::Auth::IAUTH;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

DefaultProxyAuthenticationHandler::DefaultProxyAuthenticationHandler()
    : AbstractAuthenticationHandler()
{}

ECode DefaultProxyAuthenticationHandler::IsAuthenticationRequested(
    /* [in] */ IHttpResponse* response,
    /* [in] */ IHttpContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (response == NULL) {
        Logger::E("DefaultProxyAuthenticationHandler", "HTTP response may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IStatusLine> statusLine;
    response->GetStatusLine((IStatusLine**)&statusLine);
    Int32 status;
    statusLine->GetStatusCode(&status);
    *result = status == IHttpStatus::SC_PROXY_AUTHENTICATION_REQUIRED;
    return NOERROR;
}

ECode DefaultProxyAuthenticationHandler::GetChallenges(
    /* [in] */ IHttpResponse* response,
    /* [in] */ IHttpContext* context,
    /* [out] */ IMap** callenges)
{
    VALIDATE_NOT_NULL(callenges)
    *callenges = NULL;
    if (response == NULL) {
        Logger::E("DefaultProxyAuthenticationHandler", "HTTP response may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr< ArrayOf<IHeader*> > headers;
    IHttpMessage::Probe(response)->GetHeaders(IAUTH::PROXY_AUTH, (ArrayOf<IHeader*>**)&headers);
    return ParseChallenges(headers, callenges);
}

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org