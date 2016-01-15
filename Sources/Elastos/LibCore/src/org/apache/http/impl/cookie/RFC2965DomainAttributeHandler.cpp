
#include "org/apache/http/impl/cookie/RFC2965DomainAttributeHandler.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::ILocale;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Cookie::EIID_ICookieAttributeHandler;
using Org::Apache::Http::Cookie::IClientCookie;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

CAR_INTERFACE_IMPL(RFC2965DomainAttributeHandler, Object, ICookieAttributeHandler)

ECode RFC2965DomainAttributeHandler::Parse(
    /* [in] */ ISetCookie* cookie,
    /* [in] */ const String& _domain)
{
    String domain = _domain;
    if (cookie == NULL) {
        Logger::E("RFC2965DomainAttributeHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (domain.IsNull()) {
        Logger::E("RFC2109VersionHandler", "Missing value for domain attribute");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    if (domain.Trim().GetLength() == 0) {
        Logger::E("RFC2109VersionHandler", "Blank value for domain attribute");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    domain = domain.ToLowerCase(/*ILocale::ENGLISH*/);
    if (!domain.StartWith(".")) {
        // Per RFC 2965 section 3.2.2
        // "... If an explicitly specified value does not start with
        // a dot, the user agent supplies a leading dot ..."
        // That effectively implies that the domain attribute
        // MAY NOT be an IP address of a host name
        domain = String(".") + domain;
    }
    return cookie->SetDomain(domain);
}

Boolean RFC2965DomainAttributeHandler::DomainMatch(
    /* [in] */ const String& host,
    /* [in] */ const String& domain)
{
    Boolean match = host.Equals(domain) || (domain.StartWith(".") && host.EndWith(domain));
    return match;
}

ECode RFC2965DomainAttributeHandler::Validate(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin)
{
    if (cookie == NULL) {
        Logger::E("RFC2965DomainAttributeHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("RFC2965DomainAttributeHandler", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String host;
    origin->GetHost(&host);
    host = host.ToLowerCase(/*ILocale::ENGLISH*/);
    String domain;
    if (cookie->GetDomain(&domain), domain.IsNull()) {
        Logger::E("RFC2109VersionHandler", "Invalid cookie state: domain not specified");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    String cookieDomain = domain.ToLowerCase(/*ILocale::ENGLISH*/);

    AutoPtr<IClientCookie> clientCookie = IClientCookie::Probe(cookie);
    Boolean contains;
    if (clientCookie != NULL && (clientCookie->ContainsAttribute(IClientCookie::DOMAIN_ATTR, &contains), contains)) {
        // Domain attribute must start with a dot
        if (!cookieDomain.StartWith(".")) {
            Logger::E("RFC2109VersionHandler", "Domain attribute \"%s\" violates RFC 2109: domain must start with a dot"
                    , domain.string());
            return E_MALFORMED_COOKIE_EXCEPTION;
        }

        // Domain attribute must contain at least one embedded dot,
        // or the value must be equal to .local.
        Int32 dotIndex = cookieDomain.IndexOf('.', 1);
        if (((dotIndex < 0) || (dotIndex == cookieDomain.GetLength() - 1))
                && (!cookieDomain.Equals(".local"))) {
            Logger::E("RFC2109VersionHandler", "Domain attribute \"%s\" violates RFC 2965: the value contains no embedded dots and the value is not .local"
                    , domain.string());
            return E_MALFORMED_COOKIE_EXCEPTION;
        }

        // The effective host name must domain-match domain attribute.
        if (!DomainMatch(host, cookieDomain)) {
            Logger::E("RFC2109VersionHandler", "Domain attribute \"%s\" violates RFC 2965: effective host name does not domain-match domain attribute."
                    , domain.string());
            return E_MALFORMED_COOKIE_EXCEPTION;
        }

        // effective host name minus domain must not contain any dots
        String effectiveHostWithoutDomain = host.Substring(
                0, host.GetLength() - cookieDomain.GetLength());
        if (effectiveHostWithoutDomain.IndexOf('.') != -1) {
            Logger::E("RFC2109VersionHandler", "Domain attribute \"%s\" violates RFC 2965: effective host minus domain may not contain any dots"
                    , domain.string());
            return E_MALFORMED_COOKIE_EXCEPTION;
        }
    }
    else {
        // Domain was not specified in header. In this case, domain must
        // string match request host (case-insensitive).
        if (!domain.Equals(host)) {
            Logger::E("RFC2109VersionHandler", "Illegal domain attribute: \"%s\".Domain of origin: \"%s\""
                    , domain.string(), host.string());
            return E_MALFORMED_COOKIE_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode RFC2965DomainAttributeHandler::Match(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (cookie == NULL) {
        Logger::E("RFC2965DomainAttributeHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("RFC2965DomainAttributeHandler", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String host;
    origin->GetHost(&host);
    host = host.ToLowerCase(/*ILocale::ENGLISH*/);
    String cookieDomain;
    cookie->GetDomain(&cookieDomain);

    // The effective host name MUST domain-match the Domain
    // attribute of the cookie.
    if (!DomainMatch(host, cookieDomain)) {
        *result = FALSE;
        return NOERROR;
    }
    // effective host name minus domain must not contain any dots
    String effectiveHostWithoutDomain = host.Substring(
            0, host.GetLength() - cookieDomain.GetLength());
    *result = effectiveHostWithoutDomain.IndexOf('.') == -1;
    return NOERROR;
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org