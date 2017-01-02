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

#include "org/apache/http/impl/cookie/RFC2965PortAttributeHandler.h"
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

CAR_INTERFACE_IMPL(RFC2965PortAttributeHandler, Object, ICookieAttributeHandler)

ECode RFC2965PortAttributeHandler::ParsePortAttribute(
    /* [in] */ const String& portValue,
    /* [out] */ ArrayOf<Int32>** _ports)
{
    VALIDATE_NOT_NULL(_ports)
    *_ports = NULL;
    AutoPtr<IStringTokenizer> st;
    CStringTokenizer::New(portValue, String(","), (IStringTokenizer**)&st);
    Int32 count;
    st->CountTokens(&count);
    AutoPtr< ArrayOf<Int32> > ports = ArrayOf<Int32>::Alloc(count);
    // try {
    Int32 i = 0;
    Boolean hasMore;
    while (st->HasMoreTokens(&hasMore), hasMore) {
        String t;
        st->GetNextToken(&t);
        (*ports)[i] = StringUtils::ParseInt32(t.Trim());
        if ((*ports)[i] < 0) {
            Logger::E("RFC2965PortAttributeHandler", "Invalid Port attribute.");
            return E_MALFORMED_COOKIE_EXCEPTION;
        }
        ++i;
    }
    // } catch (NumberFormatException e) {
    //     throw new MalformedCookieException ("Invalid Port "
    //                                         + "attribute: " + e.getMessage());
    // }
    *_ports = ports;
    REFCOUNT_ADD(*_ports)
    return NOERROR;
}

Boolean RFC2965PortAttributeHandler::PortMatch(
    /* [in] */ Int32 port,
    /* [in] */ ArrayOf<Int32>* ports)
{
    Boolean portInList = FALSE;
    for (Int32 i = 0; i < ports->GetLength(); i++) {
        if (port == (*ports)[i]) {
            portInList = TRUE;
            break;
        }
    }
    return portInList;
}

ECode RFC2965PortAttributeHandler::Parse(
    /* [in] */ ISetCookie* cookie,
    /* [in] */ const String& portValue)
{
    if (cookie == NULL) {
        Logger::E("RFC2965PortAttributeHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ISetCookie2> cookie2 = ISetCookie2::Probe(cookie);
    if (cookie2 != NULL) {
        if (!portValue.IsNull() && portValue.Trim().GetLength() > 0) {
            AutoPtr< ArrayOf<Int32> > ports;
            ParsePortAttribute(portValue, (ArrayOf<Int32>**)&ports);
            cookie2->SetPorts(ports);
        }
    }
    return NOERROR;
}

ECode RFC2965PortAttributeHandler::Validate(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin)
{
    if (cookie == NULL) {
        Logger::E("RFC2965PortAttributeHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("RFC2965PortAttributeHandler", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 port;
    origin->GetPort(&port);
    AutoPtr<IClientCookie> clientCookie = IClientCookie::Probe(cookie);
    Boolean contain;
    if (clientCookie != NULL && (clientCookie->ContainsAttribute(IClientCookie::PORT_ATTR, &contain), contain)) {
        AutoPtr< ArrayOf<Int32> > ports;
        cookie->GetPorts((ArrayOf<Int32>**)&ports);
        if (!PortMatch(port, ports)) {
            Logger::E("RFC2965PortAttributeHandler", "Port attribute violates RFC 2965: Request port not found in cookie's port list.");
            return E_MALFORMED_COOKIE_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode RFC2965PortAttributeHandler::Match(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (cookie == NULL) {
        Logger::E("RFC2965PortAttributeHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("RFC2965PortAttributeHandler", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 port;
    origin->GetPort(&port);
    AutoPtr<IClientCookie> clientCookie = IClientCookie::Probe(cookie);
    Boolean contain;
    if (clientCookie != NULL && (clientCookie->ContainsAttribute(IClientCookie::PORT_ATTR, &contain), contain)) {
        AutoPtr< ArrayOf<Int32> > ports;
        cookie->GetPorts((ArrayOf<Int32>**)&ports);
        if (ports == NULL) {
            // Invalid cookie state: port not specified
            *result = FALSE;
            return NOERROR;
        }
        if (!PortMatch(port, ports)) {
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org