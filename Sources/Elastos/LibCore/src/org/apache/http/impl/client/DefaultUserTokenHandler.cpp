
#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Security.h"
#include "org/apache/http/impl/client/DefaultUserTokenHandler.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastosx::Net::Ssl::ISSLSession;
using Org::Apache::Http::Auth::IAuthScheme;
using Org::Apache::Http::Auth::ICredentials;
using Org::Apache::Http::Client::EIID_IUserTokenHandler;
using Org::Apache::Http::Client::Protocol::IClientContext;
using Org::Apache::Http::Conn::IManagedClientConnection;
using Org::Apache::Http::Protocol::IExecutionContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

CAR_INTERFACE_IMPL(DefaultUserTokenHandler, Object, IUserTokenHandler)

ECode DefaultUserTokenHandler::GetUserToken(
    /* [in] */ IHttpContext* context,
    /* [out] */ IInterface** token)
{
    VALIDATE_NOT_NULL(token)
    AutoPtr<IPrincipal> userPrincipal;

    AutoPtr<IInterface> attr;
    context->GetAttribute(IClientContext::TARGET_AUTH_STATE, (IInterface**)&attr);
    AutoPtr<IAuthState> targetAuthState = IAuthState::Probe(attr);
    if (targetAuthState != NULL) {
        GetAuthPrincipal(targetAuthState, (IPrincipal**)&userPrincipal);
        if (userPrincipal == NULL) {
            AutoPtr<IInterface> proxyAttr;
            context->GetAttribute(IClientContext::PROXY_AUTH_STATE, (IInterface**)&proxyAttr);
            AutoPtr<IAuthState> proxyAuthState = IAuthState::Probe(proxyAttr);
            GetAuthPrincipal(proxyAuthState, (IPrincipal**)&userPrincipal);
        }
    }

    if (userPrincipal == NULL) {
        AutoPtr<IInterface> connAttr;
        context->GetAttribute(IExecutionContext::HTTP_CONNECTION, (IInterface**)&attr);
        AutoPtr<IManagedClientConnection> conn = IManagedClientConnection::Probe(connAttr);
        Boolean isOpen;
        if (IHttpConnection::Probe(conn)->IsOpen(&isOpen), isOpen) {
            AutoPtr<ISSLSession> sslsession;
            conn->GetSSLSession((ISSLSession**)&sslsession);
            if (sslsession != NULL) {
                sslsession->GetLocalPrincipal((IPrincipal**)&userPrincipal);
            }
        }
    }

    *token = (IInterface*)userPrincipal;
    REFCOUNT_ADD(*token)
    return NOERROR;
}

ECode DefaultUserTokenHandler::GetAuthPrincipal(
    /* [in] */ IAuthState* authState,
    /* [out] */ IPrincipal** principal)
{
    VALIDATE_NOT_NULL(principal)
    AutoPtr<IAuthScheme> scheme;
    authState->GetAuthScheme((IAuthScheme**)&scheme);
    Boolean isComplete, isConnectionBased;
    if (scheme != NULL && (scheme->IsComplete(&isComplete), isComplete)
            && (scheme->IsConnectionBased(&isConnectionBased), isConnectionBased)) {
        AutoPtr<ICredentials> creds;
        authState->GetCredentials((ICredentials**)&creds);
        if (creds != NULL) {
            return creds->GetUserPrincipal(principal);
        }
    }
    *principal = NULL;
    return NOERROR;
}

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org