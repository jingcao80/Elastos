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

#include "org/apache/http/protocol/ResponseConnControl.h"
#include "org/apache/http/CHttpVersion.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::IHttpStatus;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::CHttpVersion;
using Org::Apache::Http::IHttpRequest;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_INTERFACE_IMPL(ResponseConnControl, Object, IHttpResponseInterceptor)

ECode ResponseConnControl::Process(
    /* [in] */ IHttpResponse* response,
    /* [in] */ IHttpContext* context)
{
    if (response == NULL) {
        Logger::E("ResponseConnControl", "HTTP response may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (context == NULL) {
        Logger::E("ResponseConnControl", "HTTP context may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // Always drop connection after certain type of responses
    AutoPtr<IStatusLine> sl;
    response->GetStatusLine((IStatusLine**)&sl);
    Int32 status;
    sl->GetStatusCode(&status);
    if (status == IHttpStatus::SC_BAD_REQUEST ||
            status == IHttpStatus::SC_REQUEST_TIMEOUT ||
            status == IHttpStatus::SC_LENGTH_REQUIRED ||
            status == IHttpStatus::SC_REQUEST_TOO_LONG ||
            status == IHttpStatus::SC_REQUEST_URI_TOO_LONG ||
            status == IHttpStatus::SC_SERVICE_UNAVAILABLE ||
            status == IHttpStatus::SC_NOT_IMPLEMENTED) {
        IHttpMessage::Probe(response)->SetHeader(IHTTP::CONN_DIRECTIVE, IHTTP::CONN_CLOSE);
        return NOERROR;
    }
    // Always drop connection for HTTP/1.0 responses and below
    // if the content body cannot be correctly delimited
    AutoPtr<IHttpEntity> entity;
    response->GetEntity((IHttpEntity**)&entity);
    if (entity != NULL) {
        AutoPtr<IProtocolVersion> ver;
        sl->GetProtocolVersion((IProtocolVersion**)&ver);
        Int64 len;
        Boolean isChunked, lessEquals;
        if ((entity->GetContentLength(&len), len < 0) &&
                ((entity->IsChunked(&isChunked), !isChunked)
                        || (ver->LessEquals(IProtocolVersion::Probe(CHttpVersion::HTTP_1_0), &lessEquals), lessEquals))) {
            IHttpMessage::Probe(response)->SetHeader(IHTTP::CONN_DIRECTIVE, IHTTP::CONN_CLOSE);
            return NOERROR;
        }
    }
    // Drop connection if requested by the client
    AutoPtr<IInterface> attr;
    context->GetAttribute(IExecutionContext::HTTP_REQUEST, (IInterface**)&attr);
    AutoPtr<IHttpRequest> request = IHttpRequest::Probe(attr);
    if (request != NULL) {
        AutoPtr<IHeader> header;
        IHttpMessage::Probe(request)->GetFirstHeader(IHTTP::CONN_DIRECTIVE, (IHeader**)&header);
        if (header != NULL) {
            String value;
            header->GetValue(&value);
            IHttpMessage::Probe(response)->SetHeader(IHTTP::CONN_DIRECTIVE, value);
        }
    }
    return NOERROR;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
