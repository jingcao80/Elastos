
#include "org/apache/http/impl/cookie/NetscapeDomainHandler.h"
#include "elastos/utility/CStringTokenizer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::ILocale;
using Elastos::Utility::IStringTokenizer;
using Elastos::Utility::CStringTokenizer;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

NetscapeDomainHandler::NetscapeDomainHandler()
    : BasicDomainHandler()
{}

ECode NetscapeDomainHandler::Validate(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin)
{
    FAIL_RETURN(BasicDomainHandler::Validate(cookie, origin))
    // Perform Netscape Cookie draft specific validation
    String host, domain;
    origin->GetHost(&host);
    cookie->GetDomain(&domain);
    if (host.Contains(".")) {
        AutoPtr<IStringTokenizer> token;
        CStringTokenizer::New(domain, String("."), (IStringTokenizer**)&token);
        Int32 domainParts;
        token->CountTokens(&domainParts);

        if (IsSpecialDomain(domain)) {
            if (domainParts < 2) {
                Logger::E("NetscapeDomainHandler", "Domain attribute \"%s\" violates the Netscape cookie specification for special domains"
                        , domain.string());
                return E_MALFORMED_COOKIE_EXCEPTION;
            }
        }
        else {
            if (domainParts < 3) {
                Logger::E("NetscapeDomainHandler", "Domain attribute \"%s\" violates the Netscape cookie specification"
                        , domain.string());
                return E_MALFORMED_COOKIE_EXCEPTION;
            }
        }
    }
    return NOERROR;
}

Boolean NetscapeDomainHandler::IsSpecialDomain(
    /* [in] */ const String& domain)
{
    String ucDomain = domain.ToUpperCase(/*ILocale::ENGLISH*/);
    return ucDomain.EndWith(".COM")
            || ucDomain.EndWith(".EDU")
            || ucDomain.EndWith(".NET")
            || ucDomain.EndWith(".GOV")
            || ucDomain.EndWith(".MIL")
            || ucDomain.EndWith(".ORG")
            || ucDomain.EndWith(".INT");
}

ECode NetscapeDomainHandler::Match(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (cookie == NULL) {
        Logger::E("NetscapeDomainHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("NetscapeDomainHandler", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String host, domain;
    origin->GetHost(&host);
    cookie->GetDomain(&domain);
    if (domain.IsNull()) {
        *result = FALSE;
        return NOERROR;
    }
    *result = host.EndWith(domain);
    return NOERROR;
}


} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org