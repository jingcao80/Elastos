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

#include "org/apache/http/client/protocol/CRequestAddCookies.h"
#include "org/apache/http/client/params/HttpClientParams.h"
#include "org/apache/http/cookie/CCookieOrigin.h"
#include "elastos/net/CURI.h"
#include "elastos/utility/CArrayList.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Net::IURI;
using Elastos::Net::CURI;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::IHttpHost;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::Client::ICookieStore;
using Org::Apache::Http::Client::Methods::IHttpUriRequest;
using Org::Apache::Http::Client::Params::HttpClientParams;
using Org::Apache::Http::Conn::IManagedClientConnection;
using Org::Apache::Http::Cookie::ICookie;
using Org::Apache::Http::Cookie::ICookieOrigin;
using Org::Apache::Http::Cookie::CCookieOrigin;
using Org::Apache::Http::Cookie::ICookieSpec;
using Org::Apache::Http::Cookie::ICookieSpecRegistry;
using Org::Apache::Http::Protocol::IExecutionContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Protocol {

CAR_INTERFACE_IMPL(CRequestAddCookies, Object, IHttpRequestInterceptor)

CAR_OBJECT_IMPL(CRequestAddCookies)

ECode CRequestAddCookies::Process(
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context)
{
    if (request == NULL) {
        Logger::E("CRequestAddCookies", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (context == NULL) {
        Logger::E("CRequestAddCookies", "HTTP context may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Obtain cookie store
    AutoPtr<IInterface> o;
    context->GetAttribute(IClientContext::COOKIE_STORE, (IInterface**)&o);
    AutoPtr<ICookieStore> cookieStore = ICookieStore::Probe(o);
    if (cookieStore == NULL) {
        Logger::E("CRequestAddCookies", "Cookie store not available in HTTP context");
        // this.log.info("Cookie store not available in HTTP context");
        return NOERROR;
    }

    // Obtain the registry of cookie specs
    o = NULL;
    context->GetAttribute(IClientContext::COOKIESPEC_REGISTRY, (IInterface**)&o);
    AutoPtr<ICookieSpecRegistry> registry = ICookieSpecRegistry::Probe(o);
    if (registry == NULL) {
        Logger::E("CRequestAddCookies", "CookieSpec registry not available in HTTP context");
        // this.log.info("CookieSpec registry not available in HTTP context");
        return NOERROR;
    }

    // Obtain the target host (required)
    o = NULL;
    context->GetAttribute(IExecutionContext::HTTP_TARGET_HOST, (IInterface**)&o);
    AutoPtr<IHttpHost> targetHost = IHttpHost::Probe(o);
    if (targetHost == NULL) {
        Logger::E("CRequestAddCookies", "Target host not specified in HTTP context");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Obtain the client connection (required)
    o = NULL;
    context->GetAttribute(IExecutionContext::HTTP_CONNECTION, (IInterface**)&o);
    AutoPtr<IManagedClientConnection> conn = IManagedClientConnection::Probe(o);
    if (conn == NULL) {
        Logger::E("CRequestAddCookies", "Client connection not specified in HTTP context");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IHttpMessage> message = IHttpMessage::Probe(request);
    AutoPtr<IHttpParams> params;
    message->GetParams((IHttpParams**)&params);
    String policy;
    HttpClientParams::GetCookiePolicy(params, &policy);
    // if (this.log.isDebugEnabled()) {
    //     this.log.debug("CookieSpec selected: " + policy);
    // }

    AutoPtr<IURI> requestURI;
    if (IHttpUriRequest::Probe(request) != NULL) {
        IHttpUriRequest::Probe(request)->GetURI((IURI**)&requestURI);
    }
    else {
        // try {
        AutoPtr<IRequestLine> requestLine;
        request->GetRequestLine((IRequestLine**)&requestLine);
        String uri;
        requestLine->GetUri(&uri);
        if (FAILED(CURI::New(uri, (IURI**)&requestURI))) {
            Logger::E("CRequestAddCookies", "Invalid request URI: %s", uri.string());
            return E_PROTOCOL_EXCEPTION;
        }
        // } catch (URISyntaxException ex) {
        //     throw new ProtocolException("Invalid request URI: " +
        //             request.getRequestLine().getUri(), ex);
        // }
    }

    String hostName;
    targetHost->GetHostName(&hostName);
    Int32 port;
    targetHost->GetPort(&port);
    if (port < 0) {
        IHttpInetConnection::Probe(conn)->GetRemotePort(&port);
    }

    String path;
    requestURI->GetPath(&path);
    Boolean isSecure;
    conn->IsSecure(&isSecure);
    AutoPtr<ICookieOrigin> cookieOrigin;
    CCookieOrigin::New(hostName, port, path, isSecure, (ICookieOrigin**)&cookieOrigin);

    // Get an instance of the selected cookie policy
    AutoPtr<IHttpParams> p;
    message->GetParams((IHttpParams**)&p);
    AutoPtr<ICookieSpec> cookieSpec;
    registry->GetCookieSpec(policy, p, (ICookieSpec**)&cookieSpec);

    // Get all cookies available in the HTTP state
    AutoPtr<IList> cs;
    cookieStore->GetCookies((IList**)&cs);
    AutoPtr<IArrayList> cookies;
    CArrayList::New(ICollection::Probe(cs), (IArrayList**)&cookies);

    // Find cookies matching the given origin
    AutoPtr<IArrayList> matchedCookies;
    CArrayList::New((IArrayList**)&matchedCookies);
    AutoPtr<ICollection> matchedCol = ICollection::Probe(matchedCookies);
    AutoPtr<IIterator> it;
    cookies->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while(it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICookie> cookie = ICookie::Probe(obj);
        Boolean match;
        if (cookieSpec->Match(cookie, cookieOrigin, &match), match) {
            // if (this.log.isDebugEnabled()) {
            //     this.log.debug("Cookie " + cookie + " match " + cookieOrigin);
            // }
            matchedCol->Add(cookie);
        }
    }
    // Generate Cookie request headers
    Boolean isEmpty;
    if (matchedCol->IsEmpty(&isEmpty), !isEmpty) {
        AutoPtr<IList> headers;
        AutoPtr<IList> matchedCookiesList = IList::Probe(matchedCookies);
        cookieSpec->FormatCookies(matchedCookiesList, (IList**)&headers);
        AutoPtr<IIterator> headersIt;
        headers->GetIterator((IIterator**)&headersIt);
        while(headersIt->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> header;
            headersIt->GetNext((IInterface**)&header);
            message->AddHeader(IHeader::Probe(header));
        }
    }

    Int32 ver;
    cookieSpec->GetVersion(&ver);
    if (ver > 0) {
        Boolean needVersionHeader = FALSE;
        AutoPtr<IIterator> matchedIt;
        matchedCookies->GetIterator((IIterator**)&matchedIt);
        while(matchedIt->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            matchedIt->GetNext((IInterface**)&obj);
            AutoPtr<ICookie> cookie = ICookie::Probe(obj);
            Int32 cookieVer;
            if (cookie->GetVersion(&cookieVer), ver != cookieVer) {
                needVersionHeader = TRUE;
            }
        }

        if (needVersionHeader) {
            AutoPtr<IHeader> header;
            cookieSpec->GetVersionHeader((IHeader**)&header);
            if (header != NULL) {
                // Advertise cookie version support
                message->AddHeader(header);
            }
        }
    }

    // Stick the CookieSpec and CookieOrigin instances to the HTTP context
    // so they could be obtained by the response interceptor
    context->SetAttribute(IClientContext::COOKIE_SPEC, IObject::Probe(cookieSpec));
    context->SetAttribute(IClientContext::COOKIE_ORIGIN, IObject::Probe(cookieOrigin));
    return NOERROR;
}

ECode CRequestAddCookies::constructor()
{
    return NOERROR;
}

} // namespace Protocol
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org