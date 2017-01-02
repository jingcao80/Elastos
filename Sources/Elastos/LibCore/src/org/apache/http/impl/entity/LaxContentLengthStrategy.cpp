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

#include "org/apache/http/impl/entity/LaxContentLengthStrategy.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::Entity::EIID_IContentLengthStrategy;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Params::ICoreProtocolPNames;
using Org::Apache::Http::Protocol::IHTTP;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Entity {

CAR_INTERFACE_IMPL(LaxContentLengthStrategy, Object, IContentLengthStrategy)

ECode LaxContentLengthStrategy::DetermineLength(
    /* [in] */ IHttpMessage* message,
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length)
    *length = 0;
    if (message == NULL) {
        Logger::E("LaxContentLengthStrategy", "HTTP message may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IHttpParams> params;
    message->GetParams((IHttpParams**)&params);
    Boolean strict;
    params->IsParameterTrue(ICoreProtocolPNames::STRICT_TRANSFER_ENCODING, &strict);

    AutoPtr<IHeader> transferEncodingHeader, contentLengthHeader;
    message->GetFirstHeader(IHTTP::TRANSFER_ENCODING, (IHeader**)&transferEncodingHeader);
    message->GetFirstHeader(IHTTP::CONTENT_LEN, (IHeader**)&contentLengthHeader);
    // We use Transfer-Encoding if present and ignore Content-Length.
    // RFC2616, 4.4 item number 3
    if (transferEncodingHeader != NULL) {
        AutoPtr< ArrayOf<IHeaderElement*> > encodings;
        // try {
        ECode ec = transferEncodingHeader->GetElements((ArrayOf<IHeaderElement*>**)&encodings);
        if (ec == (ECode)E_PARSE_EXCEPTION) {
            Logger::E("LaxContentLengthStrategy", "Invalid Transfer-Encoding header value: %p", transferEncodingHeader.Get());
            return E_PROTOCOL_EXCEPTION;
        }
        // } catch (ParseException px) {
        //     throw new ProtocolException
        //         ("Invalid Transfer-Encoding header value: " +
        //          transferEncodingHeader, px);
        // }
        if (strict) {
            // Currently only chunk and identity are supported
            for (Int32 i = 0; i < encodings->GetLength(); i++) {
                String encoding;
                (*encodings)[i]->GetName(&encoding);
                if (!encoding.IsNull() && encoding.GetLength() > 0
                        && !encoding.EqualsIgnoreCase(IHTTP::CHUNK_CODING)
                        && !encoding.EqualsIgnoreCase(IHTTP::IDENTITY_CODING)) {
                    Logger::E("LaxContentLengthStrategy", "Unsupported transfer encoding: %s", encoding.string());
                    return E_PROTOCOL_EXCEPTION;
                }
            }
        }
        // The chunked encoding must be the last one applied RFC2616, 14.41
        Int32 len = encodings->GetLength();
        String value, name;
        if (transferEncodingHeader->GetValue(&value), IHTTP::IDENTITY_CODING.EqualsIgnoreCase(value)) {
            *length = IDENTITY;
            return NOERROR;
        }
        else if ((len > 0) && ((*encodings)[len - 1]->GetName(&name), IHTTP::CHUNK_CODING.EqualsIgnoreCase(name))) {
            *length = CHUNKED;
            return NOERROR;
        }
        else {
            if (strict) {
                Logger::E("LaxContentLengthStrategy", "Chunk-encoding must be the last one applied");
                return E_PROTOCOL_EXCEPTION;
            }
            *length = IDENTITY;
            return NOERROR;
        }
    }
    else if (contentLengthHeader != NULL) {
        Int64 contentlen = -1;
        AutoPtr< ArrayOf<IHeader*> > headers;
        message->GetHeaders(IHTTP::CONTENT_LEN, (ArrayOf<IHeader*>**)&headers);
        if (strict && headers->GetLength() > 1) {
            Logger::E("LaxContentLengthStrategy", "Multiple content length headers");
            return E_PROTOCOL_EXCEPTION;
        }
        for (Int32 i = headers->GetLength() - 1; i >= 0; i--) {
            AutoPtr<IHeader> header = (*headers)[i];
            // try {
            String value;
            header->GetValue(&value);
            contentlen = StringUtils::ParseInt64(value);
            break;
            // } catch (NumberFormatException e) {
            //     if (strict) {
            //         throw new ProtocolException("Invalid content length: " + header.getValue());
            //     }
            // }
            // See if we can have better luck with another header, if present
        }
        if (contentlen >= 0) {
            *length = contentlen;
        }
        else {
            *length = IDENTITY;
        }
    }
    else {
        *length = IDENTITY;
    }
    return NOERROR;
}

} // namespace Entity
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org