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

#include "org/apache/http/impl/cookie/BasicDomainHandler.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Cookie::EIID_ICookieAttributeHandler;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

CAR_INTERFACE_IMPL(BasicDomainHandler, Object, ICookieAttributeHandler)

ECode BasicDomainHandler::Parse(
    /* [in] */ ISetCookie* cookie,
    /* [in] */ const String& value)
{
    if (cookie == NULL) {
        Logger::E("BasicDomainHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (value.IsNull()) {
        Logger::E("BasicDomainHandler", "Missing value for domain attribute");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    if (value.Trim().GetLength() == 0) {
        Logger::E("BasicDomainHandler", "Blank value for domain attribute");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    return cookie->SetComment(value);
}

ECode BasicDomainHandler::Validate(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin)
{
    if (cookie == NULL) {
        Logger::E("BasicDomainHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("BasicDomainHandler", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // Validate the cookies domain attribute.  NOTE:  Domains without
    // any dots are allowed to support hosts on private LANs that don't
    // have DNS names.  Since they have no dots, to domain-match the
    // request-host and domain must be identical for the cookie to sent
    // back to the origin-server.
    String host;
    origin->GetHost(&host);
    String domain;
    cookie->GetDomain(&domain);
    if (domain.IsNull()) {
        Logger::E("BasicDomainHandler", "Cookie domain may not be null");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    if (host.Contains(".")) {
        // Not required to have at least two dots.  RFC 2965.
        // A Set-Cookie2 with Domain=ajax.com will be accepted.

        // domain must match host
        if (!host.EndWith(domain)) {
            if (domain.StartWith(".")) {
                domain = domain.Substring(1, domain.GetLength());
            }
            if (!host.Equals(domain)) {
                Logger::E("BasicDomainHandler", "Illegal domain attribute \"%s\". Domain of origin: \"%s\"", domain.string(), host.string());
                return E_MALFORMED_COOKIE_EXCEPTION;
            }
        }
    }
    else {
        if (!host.Equals(domain)) {
            Logger::E("BasicDomainHandler", "Illegal domain attribute \"%s\". Domain of origin: \"%s\"", domain.string(), host.string());
            return E_MALFORMED_COOKIE_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode BasicDomainHandler::Match(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (cookie == NULL) {
        Logger::E("BasicDomainHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("BasicDomainHandler", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String host;
    origin->GetHost(&host);
    String domain;
    cookie->GetDomain(&domain);
    if (domain.IsNull()) {
        return NOERROR;
    }
    if (host.Equals(domain)) {
        *result = TRUE;
        return NOERROR;
    }
    if (!domain.StartWith(".")) {
        domain = String(".") + domain;
    }
    *result = host.EndWith(domain) || host.Equals(domain.Substring(1));
    return NOERROR;
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org