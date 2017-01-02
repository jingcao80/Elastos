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

#include "org/apache/http/cookie/CCookieOrigin.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::ILocale;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Cookie {

CCookieOrigin::CCookieOrigin()
    : mPort(0)
    , mSecure(FALSE)
{}

CAR_INTERFACE_IMPL(CCookieOrigin, Object, ICookieOrigin)

CAR_OBJECT_IMPL(CCookieOrigin)

ECode CCookieOrigin::GetHost(
    /* [out] */ String* host)
{
    VALIDATE_NOT_NULL(host)
    *host = mHost;
    return NOERROR;
}

ECode CCookieOrigin::GetPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path)
    *path = mPath;
    return NOERROR;
}

ECode CCookieOrigin::GetPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port)
    *port = mPort;
    return NOERROR;
}

ECode CCookieOrigin::IsSecure(
    /* [out] */ Boolean* isSecure)
{
    VALIDATE_NOT_NULL(isSecure)
    *isSecure = mSecure;
    return NOERROR;
}

ECode CCookieOrigin::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder buffer;
    buffer.AppendChar('[');
    if (mSecure) {
        buffer.Append("(secure)");
    }
    buffer.Append(mHost);
    buffer.AppendChar(':');
    buffer.Append(StringUtils::ToString(mPort));
    buffer.Append(mPath);
    buffer.AppendChar(']');
    *str = buffer.ToString();
    return NOERROR;
}

ECode CCookieOrigin::constructor(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ const String& path,
    /* [in] */ Boolean secure)
{
    if (host.IsNull()) {
        Logger::E("CCookieOrigin", "Host of origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (host.Trim().GetLength() == 0) {
        Logger::E("CCookieOrigin", "Host of origin may not be blank");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (port < 0) {
        Logger::E("CCookieOrigin", "Invalid port: %d", port);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (path.IsNull()) {
        Logger::E("CCookieOrigin", "Path of origin may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mHost = host.ToLowerCase(/*ILocale::ENGLISH*/);
    mPort = port;
    if (path.Trim().GetLength() != 0) {
        mPath = path;
    }
    else {
        mPath = String("/");
    }
    mSecure = secure;
    return NOERROR;
}

} // namespace Cookie
} // namespace Http
} // namespace Apache
} // namespace Org
