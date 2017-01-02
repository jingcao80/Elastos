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

#include "org/apache/http/impl/cookie/RFC2109VersionHandler.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

RFC2109VersionHandler::RFC2109VersionHandler()
{}

ECode RFC2109VersionHandler::Parse(
    /* [in] */ ISetCookie* cookie,
    /* [in] */ const String& value)
{
    if (cookie == NULL) {
        Logger::E("RFC2109VersionHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (value.IsNull()) {
        Logger::E("RFC2109VersionHandler", "Missing value for version attribute");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    if (value.Trim().GetLength() == 0) {
        Logger::E("RFC2109VersionHandler", "Blank value for version attribute");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    // try {
    return cookie->SetVersion(StringUtils::ParseInt32(value));
    // } catch (NumberFormatException e) {
    //     throw new MalformedCookieException("Invalid version: "
    //         + e.getMessage());
    // }
}

ECode RFC2109VersionHandler::Validate(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin)
{
    if (cookie == NULL) {
        Logger::E("RFC2109VersionHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 version;
    if (cookie->GetVersion(&version), version < 0) {
        Logger::E("RFC2109VersionHandler", "Cookie version may not be negative");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org