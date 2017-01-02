//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "org/apache/http/impl/cookie/RFC2109DomainHandler.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::ILocale;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Cookie::EIID_ICookieAttributeHandler;
using Org::Apache::Http::Cookie::Params::ICookieSpecPNames;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

CAR_INTERFACE_IMPL(RFC2109DomainHandler, Object, ICookieAttributeHandler)

ECode RFC2109DomainHandler::Parse(
    /* [in] */ ISetCookie* cookie,
    /* [in] */ const String& value)
{
    if (cookie == NULL) {
        Logger::E("RFC2109DomainHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (value.IsNull()) {
        Logger::E("RFC2109DomainHandler", "Missing value for domain attribute");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    if (value.Trim().GetLength() == 0) {
        Logger::E("RFC2109DomainHandler", "Blank value for domain attribute");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    return cookie->SetDomain(value);
}

ECode RFC2109DomainHandler::Validate(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin)
{
    if (cookie == NULL) {
        Logger::E("RFC2109DomainHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("RFC2109DomainHandler", "Cookie origin not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String host, domain;
    origin->GetHost(&host);
    cookie->GetDomain(&domain);
    if (domain.IsNull()) {
        Logger::E("RFC2109DomainHandler", "Cookie domain may not be null");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    if (!domain.Equals(host)) {
        Int32 dotIndex = domain.IndexOf('.');
        if (dotIndex == -1) {
            Logger::E("RFC2109DomainHandler", "Domain attribute \"%s\" does not match the host \"%s\""
                    , domain.string(), host.string());
            return E_MALFORMED_COOKIE_EXCEPTION;
        }
        // domain must start with dot
        if (!domain.StartWith(".")) {
            Logger::E("RFC2109DomainHandler", "Domain attribute \"%s\" violates RFC 2109: domain must start with a dot"
                    , domain.string());
            return E_MALFORMED_COOKIE_EXCEPTION;
        }
        // domain must have at least one embedded dot
        dotIndex = domain.IndexOf('.', 1);
        if (dotIndex < 0 || dotIndex == domain.GetLength() - 1) {
            Logger::E("RFC2109DomainHandler", "Domain attribute \"%s\" violates RFC 2109: domain must contain an embedded dot"
                    , domain.string());
            return E_MALFORMED_COOKIE_EXCEPTION;
        }
        host = host.ToLowerCase(/*ILocale::ENGLISH*/);
        if (!host.EndWith(domain)) {
            Logger::E("RFC2109DomainHandler", "Illegal domain attribute \"%s\". Domain of origin: \"%s\""
                    , domain.string(), host.string());
            return E_MALFORMED_COOKIE_EXCEPTION;
        }
        // host minus domain may not contain any dots
        String hostWithoutDomain = host.Substring(0, host.GetLength() - domain.GetLength());
        if (hostWithoutDomain.IndexOf('.') != -1) {
            Logger::E("RFC2109DomainHandler", "Domain attribute \"%s\" violates RFC 2109: host minus domain may not contain any dots"
                    , domain.string());
            return E_MALFORMED_COOKIE_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode RFC2109DomainHandler::Match(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (cookie == NULL) {
        Logger::E("RFC2109DomainHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("RFC2109DomainHandler", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String host, domain;
    origin->GetHost(&host);
    cookie->GetDomain(&domain);
    if (domain.IsNull()) {
        return NOERROR;
    }
    *result = host.Equals(domain) || (domain.StartWith(".") && host.EndWith(domain));
    return NOERROR;
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org