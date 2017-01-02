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

#include "org/apache/http/impl/entity/StrictContentLengthStrategy.h"
#include "org/apache/http/CHttpVersion.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::CHttpVersion;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::Entity::EIID_IContentLengthStrategy;
using Org::Apache::Http::Protocol::IHTTP;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Entity {

CAR_INTERFACE_IMPL(StrictContentLengthStrategy, Object, IContentLengthStrategy)

ECode StrictContentLengthStrategy::DetermineLength(
    /* [in] */ IHttpMessage* message,
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length)
    *length = 0;
    if (message == NULL) {
        Logger::E("StrictContentLengthStrategy", "HTTP message may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Although Transfer-Encoding is specified as a list, in practice
    // it is either missing or has the single value "chunked". So we
    // treat it as a single-valued header here.
    AutoPtr<IHeader> transferEncodingHeader, contentLengthHeader;
    message->GetFirstHeader(IHTTP::TRANSFER_ENCODING, (IHeader**)&transferEncodingHeader);
    message->GetFirstHeader(IHTTP::CONTENT_LEN, (IHeader**)&contentLengthHeader);
    if (transferEncodingHeader != NULL) {
        String s;
        transferEncodingHeader->GetValue(&s);
        if (IHTTP::CHUNK_CODING.EqualsIgnoreCase(s)) {
            AutoPtr<IProtocolVersion> ver;
            message->GetProtocolVersion((IProtocolVersion**)&ver);
            Boolean lessEquals;
            if (ver->LessEquals(IProtocolVersion::Probe(CHttpVersion::HTTP_1_0), &lessEquals)) {
                Logger::E("StrictContentLengthStrategy", "Chunked transfer encoding not allowed for %p", ver.Get());
                return E_PROTOCOL_EXCEPTION;
            }
            *length = CHUNKED;
            return NOERROR;
        }
        else if (IHTTP::IDENTITY_CODING.EqualsIgnoreCase(s)) {
            *length = IDENTITY;
            return NOERROR;
        }
        else {
            Logger::E("StrictContentLengthStrategy", "Unsupported transfer encoding: %s", s.string());
            return E_PROTOCOL_EXCEPTION;
        }
    }
    else if (contentLengthHeader != NULL) {
        String s;
        contentLengthHeader->GetValue(&s);
        // try {
        *length = StringUtils::ParseInt64(s);
        return NOERROR;
        // } catch (NumberFormatException e) {
        //     throw new ProtocolException("Invalid content length: " + s);
        // }
    }
    else {
        *length = IDENTITY;
        return NOERROR;
    }
}

} // namespace Entity
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org