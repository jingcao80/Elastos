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

#include "org/apache/http/impl/cookie/RFC2965Spec.h"
#include "org/apache/http/impl/cookie/RFC2965DomainAttributeHandler.h"
#include "org/apache/http/impl/cookie/RFC2965PortAttributeHandler.h"
#include "org/apache/http/impl/cookie/RFC2965CommentUrlAttributeHandler.h"
#include "org/apache/http/impl/cookie/RFC2965DiscardAttributeHandler.h"
#include "org/apache/http/impl/cookie/RFC2965VersionAttributeHandler.h"
#include "org/apache/http/impl/cookie/BasicClientCookie2.h"
#include "org/apache/http/impl/cookie/DateUtils.h"
#include "org/apache/http/cookie/CCookieOrigin.h"
#include "org/apache/http/message/CBufferedHeader.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/CArrayList.h"
#include "elastos/utility/logging/Logger.h"
#include "elastos/utility/etl/etl_hashmap.h"

using Elastos::Core::StringUtils;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHeaderElement;
using Org::Apache::Http::INameValuePair;
using Org::Apache::Http::Cookie::IClientCookie;
using Org::Apache::Http::Cookie::ISM;
using Org::Apache::Http::Cookie::CCookieOrigin;
using Org::Apache::Http::Message::CBufferedHeader;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

RFC2965Spec::RFC2965Spec(
    /* [in] */ ArrayOf<String>* datepatterns,
    /* [in] */ Boolean oneHeader)
    : RFC2109Spec()
{
    Init(datepatterns, oneHeader);
}

RFC2965Spec::RFC2965Spec()
    : RFC2109Spec()
{
    Init(NULL, FALSE);
}

void RFC2965Spec::Init(
    /* [in] */ ArrayOf<String>* datepatterns,
    /* [in] */ Boolean oneHeader)
{
    AutoPtr<ICookieAttributeHandler> domainHandler = (ICookieAttributeHandler*)new RFC2965DomainAttributeHandler();
    AutoPtr<ICookieAttributeHandler> portHandler = (ICookieAttributeHandler*)new RFC2965PortAttributeHandler();
    AutoPtr<ICookieAttributeHandler> commentHandler = (ICookieAttributeHandler*)new RFC2965CommentUrlAttributeHandler();
    AutoPtr<ICookieAttributeHandler> discardHandler = (ICookieAttributeHandler*)new RFC2965DiscardAttributeHandler();
    AutoPtr<ICookieAttributeHandler> versionHandler = (ICookieAttributeHandler*)new RFC2965VersionAttributeHandler();
    RegisterAttribHandler(IClientCookie::DOMAIN_ATTR, domainHandler);
    RegisterAttribHandler(IClientCookie::PORT_ATTR, portHandler);
    RegisterAttribHandler(IClientCookie::COMMENTURL_ATTR, commentHandler);
    RegisterAttribHandler(IClientCookie::DISCARD_ATTR, discardHandler);
    RegisterAttribHandler(IClientCookie::VERSION_ATTR, versionHandler);
}

AutoPtr<BasicClientCookie> RFC2965Spec::CreateCookie(
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [in] */ ICookieOrigin* origin)
{
    AutoPtr<BasicClientCookie> cookie = new BasicClientCookie(name, value);
    cookie->SetPath(GetDefaultPath(origin));
    cookie->SetDomain(GetDefaultDomain(origin));
    return cookie;
}

AutoPtr<BasicClientCookie> RFC2965Spec::CreateCookie2(
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [in] */ ICookieOrigin* origin)
{
    AutoPtr<BasicClientCookie2> cookie = new BasicClientCookie2(name, value);
    cookie->SetPath(GetDefaultPath(origin));
    cookie->SetDomain(GetDefaultDomain(origin));
    Int32 port;
    origin->GetPort(&port);
    AutoPtr< ArrayOf<Int32> > ports = ArrayOf<Int32>::Alloc(1);
    (*ports)[0] = port;
    cookie->SetPorts(ports);
    return (BasicClientCookie*)cookie;
}

ECode RFC2965Spec::Parse(
    /* [in] */ IHeader* header,
    /* [in] */ ICookieOrigin* origin,
    /* [out] */ IList** _cookies)
{
    VALIDATE_NOT_NULL(_cookies)
    *_cookies = NULL;

    if (header == NULL) {
        Logger::E("RFC2965Spec", "Header may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("RFC2965Spec", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    origin = AdjustEffectiveHost(origin);

    AutoPtr< ArrayOf<IHeaderElement*> > elems;
    header->GetElements((ArrayOf<IHeaderElement*>**)&elems);

    AutoPtr<IArrayList> arrayList;
    CArrayList::New(elems->GetLength(), (IArrayList**)&arrayList);
    AutoPtr<IList> cookies = IList::Probe(arrayList);
    for (Int32 i = 0; i < elems->GetLength(); i++) {
        AutoPtr<IHeaderElement> headerelement  = (*elems)[i];

        String name, value;
        headerelement->GetName(&name);
        headerelement->GetValue(&value);
        if (name.IsNull() || name.GetLength() == 0) {
            Logger::E("RFC2965Spec", "Cookie name may not be empty");
            return E_MALFORMED_COOKIE_EXCEPTION;
        }

        AutoPtr<BasicClientCookie> cookie;
        String hn;
        if (header->GetName(&hn), hn.Equals(ISM::SET_COOKIE2)) {
            cookie = CreateCookie2(name, value, origin);
        }
        else {
            cookie = CreateCookie(name, value, origin);
        }

        // cycle through the parameters
        AutoPtr< ArrayOf<INameValuePair*> > attribs;
        headerelement->GetParameters((ArrayOf<INameValuePair*>**)&attribs);

        // Eliminate duplicate attributes. The first occurrence takes precedence
        // See RFC2965: 3.2  Origin Server Role
        HashMap<String, AutoPtr<INameValuePair> > attribmap(attribs->GetLength());
        for (Int32 j = attribs->GetLength() - 1; j >= 0; j--) {
            AutoPtr<INameValuePair> param = (*attribs)[j];
            String n;
            param->GetName(&n);
            n = n.ToLowerCase(/*ILocale::ENGLISH*/);
            attribmap[n] = param;
        }
        HashMap<String, AutoPtr<INameValuePair> >::Iterator it = attribmap.Begin();
        for (; it != attribmap.End(); ++it) {
            AutoPtr<INameValuePair> attrib = it->mSecond;
            String s;
            attrib->GetName(&s);
            s = s.ToLowerCase(/*ILocale::ENGLISH*/);

            String v;
            attrib->GetValue(&v);
            cookie->SetAttribute(s, v);

            AutoPtr<ICookieAttributeHandler> handler;
            FindAttribHandler(s, (ICookieAttributeHandler**)&handler);
            if (handler != NULL) {
                handler->Parse(cookie, v);
            }
        }
        cookies->Add(cookie->Probe(EIID_IInterface));
    }
    *_cookies = cookies;
    REFCOUNT_ADD(*_cookies)
    return NOERROR;
}

ECode RFC2965Spec::Validate(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin)
{
    if (cookie == NULL) {
        Logger::E("RFC2965Spec", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("RFC2965Spec", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    origin = AdjustEffectiveHost(origin);
    return RFC2109Spec::Validate(cookie, origin);
}

ECode RFC2965Spec::Match(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (cookie == NULL) {
        Logger::E("RFC2965Spec", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("RFC2965Spec", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    origin = AdjustEffectiveHost(origin);
    return RFC2109Spec::Match(cookie, origin, result);
}

void RFC2965Spec::FormatCookieAsVer(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ ICookie* cookie,
    /* [in] */ Int32 version)
{
    RFC2109Spec::FormatCookieAsVer(buffer, cookie, version);
    // format port attribute
    AutoPtr<IClientCookie> clientCookie = IClientCookie::Probe(cookie);
    if (clientCookie!= NULL) {
        // Test if the port attribute as set by the origin server is not blank
        String s;
        clientCookie->GetAttribute(IClientCookie::PORT_ATTR, &s);
        if (!s.IsNull()) {
            buffer->Append(String("; $Port"));
            buffer->Append(String("=\""));
            if (s.Trim().GetLength() > 0) {
                AutoPtr< ArrayOf<Int32> > ports;
                cookie->GetPorts((ArrayOf<Int32>**)&ports);
                if (ports != NULL) {
                    for (Int32 i = 0; i < ports->GetLength(); i++) {
                        if (i > 0) {
                            buffer->Append(String(","));
                        }
                        buffer->Append(StringUtils::ToString((*ports)[i]));
                    }
                }
            }
            buffer->Append(String("\""));
        }
    }
}

AutoPtr<ICookieOrigin> RFC2965Spec::AdjustEffectiveHost(
    /* [in] */ ICookieOrigin* origin)
{
    String host;
    origin->GetHost(&host);

    // Test if the host name appears to be a fully qualified DNS name,
    // IPv4 address or IPv6 address
    Boolean isLocalHost = TRUE;
    for (Int32 i = 0; i < host.GetLength(); i++) {
        Char32 ch = host.GetChar(i);
        if (ch == '.' || ch == ':') {
            isLocalHost = FALSE;
            break;
        }
    }
    if (isLocalHost) {
        host += String(".local");
        AutoPtr<ICookieOrigin> o;
        Int32 port;
        origin->GetPort(&port);
        String path;
        origin->GetPath(&path);
        Boolean isSecure;
        origin->IsSecure(&isSecure);
        CCookieOrigin::New(host, port, path, isSecure, (ICookieOrigin**)&o);
        return o;
    }
    else {
        return origin;
    }
}

ECode RFC2965Spec::GetVersion(
    /* [out] */ Int32* version)
{
    VALIDATE_NOT_NULL(version)
    *version = 1;
    return NOERROR;
}

ECode RFC2965Spec::GetVersionHeader(
    /* [out] */ IHeader** header)
{
    VALIDATE_NOT_NULL(header)
    AutoPtr<ICharArrayBuffer> buffer;
    CCharArrayBuffer::New(40, (ICharArrayBuffer**)&buffer);
    buffer->Append(ISM::COOKIE2);
    buffer->Append(String(": "));
    buffer->Append(String("$Version="));
    Int32 version;
    GetVersion(&version);
    buffer->Append(StringUtils::ToString(version));
    AutoPtr<IFormattedHeader> bh;
    CBufferedHeader::New(buffer, (IFormattedHeader**)&bh);
    *header = IHeader::Probe(bh);
    REFCOUNT_ADD(*header)
    return NOERROR;
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org