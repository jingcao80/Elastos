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

#include "org/apache/http/message/BasicHttpRequest.h"
#include "org/apache/http/message/CBasicRequestLine.h"
#include "org/apache/http/params/HttpProtocolParams.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Params::HttpProtocolParams;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_INTERFACE_IMPL(BasicHttpRequest, AbstractHttpMessage, IHttpRequest)

ECode BasicHttpRequest::GetProtocolVersion(
    /* [out] */ IProtocolVersion** protocolVersion)
{
    VALIDATE_NOT_NULL(protocolVersion)
    if (mRequestLine != NULL) {
        return mRequestLine->GetProtocolVersion(protocolVersion);
    }
    else {
        AutoPtr<IHttpParams> params;
        GetParams((IHttpParams**)&params);
        return HttpProtocolParams::GetVersion(params, protocolVersion);
    }
}

ECode BasicHttpRequest::GetRequestLine(
    /* [out] */ IRequestLine** requestLine)
{
    VALIDATE_NOT_NULL(requestLine)
    if (mRequestLine != NULL) {
        *requestLine = mRequestLine;
        REFCOUNT_ADD(*requestLine)
        return NOERROR;
    }
    else {
        AutoPtr<IHttpParams> params;
        GetParams((IHttpParams**)&params);
        AutoPtr<IProtocolVersion> ver;
        HttpProtocolParams::GetVersion(params, (IProtocolVersion**)&ver);
        return CBasicRequestLine::New(mMethod, mUri, ver, requestLine);
    }
}

ECode BasicHttpRequest::Init(
    /* [in] */ const String& method,
    /* [in] */ const String& uri)
{
    AbstractHttpMessage::Init();
    if (method.IsNull()) {
        Logger::E("BasicHttpRequest", "Method name may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (uri.IsNull()) {
        Logger::E("BasicHttpRequest", "Request URI may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mMethod = method;
    mUri = uri;
    mRequestLine = NULL;
    return NOERROR;
}

ECode BasicHttpRequest::Init(
    /* [in] */ const String& method,
    /* [in] */ const String& uri,
    /* [in] */ IProtocolVersion* ver)
{
    AutoPtr<IRequestLine> line;
    CBasicRequestLine::New(method, uri, ver, (IRequestLine**)&line);
    return Init(line);
}

ECode BasicHttpRequest::Init(
    /* [in] */ IRequestLine* requestline)
{
    AbstractHttpMessage::Init();
    if (requestline == NULL) {
        Logger::E("BasicHttpRequest", "Request line may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mRequestLine = requestline;
    requestline->GetMethod(&mMethod);
    requestline->GetUri(&mUri);
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
