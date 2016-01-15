
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