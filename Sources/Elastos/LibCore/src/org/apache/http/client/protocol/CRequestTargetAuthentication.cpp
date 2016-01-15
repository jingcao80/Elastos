
#include "org/apache/http/client/protocol/CRequestTargetAuthentication.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::Auth::IAUTH;
using Org::Apache::Http::Auth::IAuthScope;
using Org::Apache::Http::Auth::IAuthScheme;
using Org::Apache::Http::Auth::IAuthState;
using Org::Apache::Http::Auth::ICredentials;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Protocol {

CAR_INTERFACE_IMPL(CRequestTargetAuthentication, Object, IHttpRequestInterceptor)

CAR_OBJECT_IMPL(CRequestTargetAuthentication)

ECode CRequestTargetAuthentication::Process(
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context)
{
    if (request == NULL) {
        Logger::E("CRequestTargetAuthentication", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (context == NULL) {
        Logger::E("CRequestTargetAuthentication", "HTTP context may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IHttpMessage> message = IHttpMessage::Probe(request);
    Boolean isContains;
    if (message->ContainsHeader(IAUTH::WWW_AUTH_RESP, &isContains), isContains) {
        return NOERROR;
    }

    // Obtain authentication state
    AutoPtr<IInterface> o;
    context->GetAttribute(IClientContext::TARGET_AUTH_STATE, (IInterface**)&o);
    AutoPtr<IAuthState> authState = IAuthState::Probe(o);
    if (authState == NULL) {
        return NOERROR;
    }

    AutoPtr<IAuthScheme> authScheme;
    authState->GetAuthScheme((IAuthScheme**)&authScheme);
    if (authScheme == NULL) {
        return NOERROR;
    }

    AutoPtr<ICredentials> creds;
    authState->GetCredentials((ICredentials**)&creds);
    if (creds == NULL) {
        Logger::E("CRequestTargetAuthentication", "User credentials not available");
        // this.log.debug("User credentials not available");
        return NOERROR;
    }
    AutoPtr<IAuthScope> scope;
    Boolean result;
    if ((authState->GetAuthScope((IAuthScope**)&scope), scope != NULL)
            || (authScheme->IsConnectionBased(&result), !result)) {
        // try {
        AutoPtr<IHeader> header;
        ECode ec = authScheme->Authenticate(creds, request, (IHeader**)&header);
        if (FAILED(ec)) {
            Logger::E("CRequestTargetAuthentication", "Authentication error: 0x%08x", ec);
            return NOERROR;
        }
        message->AddHeader(header);
        // } catch (AuthenticationException ex) {
        //     if (this.log.isErrorEnabled()) {
        //         this.log.error("Authentication error: " + ex.getMessage());
        //     }
        // }
    }
    return NOERROR;
}

ECode CRequestTargetAuthentication::constructor()
{
    return NOERROR;
}

} // namespace Protocol
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org