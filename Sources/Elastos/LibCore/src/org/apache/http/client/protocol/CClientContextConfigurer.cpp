
#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/client/protocol/CClientContextConfigurer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Protocol {

CAR_INTERFACE_IMPL_2(CClientContextConfigurer, Object, IClientContextConfigurer, IClientContext)

CAR_OBJECT_IMPL(CClientContextConfigurer)

ECode CClientContextConfigurer::SetCookieSpecRegistry(
    /* [in] */ ICookieSpecRegistry* registry)
{
    return mContext->SetAttribute(COOKIESPEC_REGISTRY, registry);
}

ECode CClientContextConfigurer::SetAuthSchemeRegistry(
    /* [in] */ IAuthSchemeRegistry* registry)
{
    return mContext->SetAttribute(AUTHSCHEME_REGISTRY, registry);
}

ECode CClientContextConfigurer::SetCookieStore(
    /* [in] */ ICookieStore* store)
{
    return mContext->SetAttribute(COOKIE_STORE, store);
}

ECode CClientContextConfigurer::SetCredentialsProvider(
    /* [in] */ ICredentialsProvider* provider)
{
    return mContext->SetAttribute(CREDS_PROVIDER, provider);
}

ECode CClientContextConfigurer::SetAuthSchemePref(
    /* [in] */ IList* list)
{
    return mContext->SetAttribute(AUTH_SCHEME_PREF, list);
}

ECode CClientContextConfigurer::constructor(
    /* [in] */ IHttpContext* context)
{
    if (context == NULL) {
        Logger::E("CClientContextConfigurer", "HTTP context may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mContext = context;
    return NOERROR;
}

} // namespace Protocol
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org