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

#include "org/apache/http/protocol/ResponseContent.h"
#include "org/apache/http/CHttpVersion.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringUtils;
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

CAR_INTERFACE_IMPL(ResponseContent, Object, IHttpResponseInterceptor)

ECode ResponseContent::Process(
    /* [in] */ IHttpResponse* response,
    /* [in] */ IHttpContext* context)
{
    if (response == NULL) {
        Logger::E("ResponseContent", "HTTP response may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpMessage> message = IHttpMessage::Probe(response);
    Boolean contains;
    if (message->ContainsHeader(IHTTP::TRANSFER_ENCODING, &contains), contains) {
        Logger::E("ResponseContent", "Transfer-encoding header already present");
        return E_PROTOCOL_EXCEPTION;
    }
    if (message->ContainsHeader(IHTTP::CONTENT_LEN, &contains), contains) {
        Logger::E("ResponseContent", "Content-Length header already present");
        return E_PROTOCOL_EXCEPTION;
    }
    AutoPtr<IStatusLine> sl;
    response->GetStatusLine((IStatusLine**)&sl);
    AutoPtr<IProtocolVersion> ver;
    sl->GetProtocolVersion((IProtocolVersion**)&ver);
    AutoPtr<IHttpEntity> entity;
    response->GetEntity((IHttpEntity**)&entity);
    if (entity != NULL) {
        Int64 len;
        entity->GetContentLength(&len);
        Boolean isChunked, lessEquals;
        if ((entity->IsChunked(&isChunked), isChunked) ||
                (ver->LessEquals(IProtocolVersion::Probe(CHttpVersion::HTTP_1_0), &lessEquals), !lessEquals)) {
            message->AddHeader(IHTTP::TRANSFER_ENCODING, IHTTP::CHUNK_CODING);
        }
        else if (len >= 0) {
            message->AddHeader(IHTTP::CONTENT_LEN, StringUtils::ToString(len));
        }
        // Specify a content type if known
        AutoPtr<IHeader> type;
        if ((entity->GetContentType((IHeader**)&type), type != NULL)
                && (message->ContainsHeader(IHTTP::CONTENT_TYPE, &contains), !contains)) {
            message->AddHeader(type);
        }
        // Specify a content encoding if known
        AutoPtr<IHeader> encoding;
        if ((entity->GetContentEncoding((IHeader**)&encoding), encoding != NULL)
                && (message->ContainsHeader(IHTTP::CONTENT_ENCODING, &contains), !contains)) {
            message->AddHeader(encoding);
        }
    }
    else {
        Int32 status;
        sl->GetStatusCode(&status);
        if (status != IHttpStatus::SC_NO_CONTENT
                && status != IHttpStatus::SC_NOT_MODIFIED
                && status != IHttpStatus::SC_RESET_CONTENT) {
            message->AddHeader(IHTTP::CONTENT_LEN, String("0"));
        }
    }
    return NOERROR;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
