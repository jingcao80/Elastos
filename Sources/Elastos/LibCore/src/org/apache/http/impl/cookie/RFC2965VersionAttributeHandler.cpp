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

#include "org/apache/http/impl/cookie/RFC2965VersionAttributeHandler.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/CStringTokenizer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringUtils;
using Elastos::Utility::IStringTokenizer;
using Elastos::Utility::CStringTokenizer;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Cookie::EIID_ICookieAttributeHandler;
using Org::Apache::Http::Cookie::IClientCookie;
using Org::Apache::Http::Cookie::ISetCookie;
using Org::Apache::Http::Cookie::ISetCookie2;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

CAR_INTERFACE_IMPL(RFC2965VersionAttributeHandler, Object, ICookieAttributeHandler)

ECode RFC2965VersionAttributeHandler::Parse(
    /* [in] */ ISetCookie* cookie,
    /* [in] */ const String& value)
{
    if (cookie == NULL) {
        Logger::E("RFC2965VersionAttributeHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
     if (value.IsNull()) {
        Logger::E("RFC2965VersionAttributeHandler", "Missing value for version attribute");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    Int32 version = -1;
    // try {
    version = StringUtils::ParseInt32(value);
    // } catch (NumberFormatException e) {
    //     version = -1;
    // }
    if (version < 0) {
        Logger::E("RFC2965VersionAttributeHandler", "Invalid cookie version.");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    return cookie->SetVersion(version);
}

ECode RFC2965VersionAttributeHandler::Validate(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin)
{
    if (cookie == NULL) {
        Logger::E("RFC2965VersionAttributeHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ISetCookie2> cookie2 = ISetCookie2::Probe(cookie);
    if (cookie2 != NULL) {
        AutoPtr<IClientCookie> clientCookie = IClientCookie::Probe(cookie);
        Boolean contain;
        if (clientCookie != NULL && (clientCookie->ContainsAttribute(IClientCookie::VERSION_ATTR, &contain), !contain)) {
            Logger::E("RFC2965VersionAttributeHandler", "Violates RFC 2965. Version attribute is required.");
            return E_MALFORMED_COOKIE_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode RFC2965VersionAttributeHandler::Match(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org