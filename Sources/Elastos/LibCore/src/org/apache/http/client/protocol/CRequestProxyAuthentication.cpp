
#include "org/apache/http/client/protocol/CRequestProxyAuthentication.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::Auth::IAuthScope;
using Org::Apache::Http::Auth::IAUTH;
using Org::Apache::Http::Auth::IAuthScheme;
using Org::Apache::Http::Auth::IAuthState;
using Org::Apache::Http::Auth::ICredentials;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Protocol {

CAR_INTERFACE_IMPL(CRequestProxyAuthentication, Object, IHttpRequestInterceptor)

CAR_OBJECT_IMPL(CRequestProxyAuthentication)

ECode CRequestProxyAuthentication::Process(
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context)
{
    if (request == NULL) {
        Logger::E("CRequestProxyAuthentication", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (context == NULL) {
        Logger::E("CRequestProxyAuthentication", "HTTP context may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IHttpMessage> message = IHttpMessage::Probe(request);
    Boolean isContains;
    if (message->ContainsHeader(IAUTH::PROXY_AUTH_RESP, &isContains), isContains) {
        return NOERROR;
    }

    // Obtain authentication state
    AutoPtr<IInterface> o;
    context->GetAttribute(IClientContext::PROXY_AUTH_STATE, (IInterface**)&o);
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
        Logger::E("CRequestProxyAuthentication", "User credentials not available");
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
            Logger::E("CRequestProxyAuthentication", "Proxy authentication error: 0x%08x", ec);
            return NOERROR;
        }
        message->AddHeader(header);
        // } catch (AuthenticationException ex) {
        //     if (this.log.isErrorEnabled()) {
        //         this.log.error("Proxy authentication error: " + ex.getMessage());
        //     }
        // }
    }
    return NOERROR;
}

ECode CRequestProxyAuthentication::constructor()
{
    return NOERROR;
}

} // namespace Protocol
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org