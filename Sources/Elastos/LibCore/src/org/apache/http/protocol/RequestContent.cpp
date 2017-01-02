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

#include "org/apache/http/protocol/RequestContent.h"
#include "org/apache/http/CHttpVersion.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHttpEntityEnclosingRequest;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::CHttpVersion;
using Org::Apache::Http::IHttpRequest;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_INTERFACE_IMPL(RequestContent, Object, IHttpRequestInterceptor)

ECode RequestContent::Process(
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context)
{
    if (request == NULL) {
        Logger::E("RequestContent", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpEntityEnclosingRequest> enclosingRequest = IHttpEntityEnclosingRequest::Probe(request);
    if (enclosingRequest != NULL) {
        Boolean contains;
        if (IHttpMessage::Probe(request)->ContainsHeader(IHTTP::TRANSFER_ENCODING, &contains), contains) {
            Logger::E("RequestContent", "Transfer-encoding header already present");
            return E_PROTOCOL_EXCEPTION;
        }
        if (IHttpMessage::Probe(request)->ContainsHeader(IHTTP::CONTENT_LEN, &contains), contains) {
            Logger::E("RequestContent", "Content-Length header already present");
            return E_PROTOCOL_EXCEPTION;
        }
        AutoPtr<IRequestLine> rl;
        request->GetRequestLine((IRequestLine**)&rl);
        AutoPtr<IProtocolVersion> ver;
        rl->GetProtocolVersion((IProtocolVersion**)&ver);
        AutoPtr<IHttpEntity> entity;
        enclosingRequest->GetEntity((IHttpEntity**)&entity);
        AutoPtr<IHttpMessage> message = IHttpMessage::Probe(request);
        if (entity == NULL) {
            message->AddHeader(IHTTP::CONTENT_LEN, String("0"));
            return NOERROR;
        }
        // Must specify a transfer encoding or a content length
        Boolean isChunked;
        Int64 len;
        if ((entity->IsChunked(&isChunked), isChunked) || (entity->GetContentLength(&len), len < 0)) {
            Boolean lessEquals;
            if (ver->LessEquals(IProtocolVersion::Probe(CHttpVersion::HTTP_1_0), &lessEquals), lessEquals) {
                Logger::E("RequestContent", "Chunked transfer encoding not allowed for %p", ver.Get());
                return E_PROTOCOL_EXCEPTION;
            }
            message->AddHeader(IHTTP::TRANSFER_ENCODING, IHTTP::CHUNK_CODING);
        }
        else {
            Int64 length;
            entity->GetContentLength(&length);
            message->AddHeader(IHTTP::CONTENT_LEN, StringUtils::ToString(length));
        }
        // Specify a content type if known
        AutoPtr<IHeader> type;
        if ((entity->GetContentType((IHeader**)&type), type != NULL)
                && (IHttpMessage::Probe(request)->ContainsHeader(IHTTP::CONTENT_TYPE, &contains), !contains)) {
            message->AddHeader(type);
        }
        // Specify a content encoding if known
        AutoPtr<IHeader> encoding;
        if ((entity->GetContentEncoding((IHeader**)&encoding), encoding != NULL)
                && (IHttpMessage::Probe(request)->ContainsHeader(IHTTP::CONTENT_ENCODING, &contains), !contains)) {
            message->AddHeader(encoding);
        }
    }
    return NOERROR;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
