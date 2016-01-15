
#include "org/apache/http/impl/entity/EntitySerializer.h"
#include "org/apache/http/impl/io/ChunkedOutputStream.h"
#include "org/apache/http/impl/io/IdentityOutputStream.h"
#include "org/apache/http/impl/io/ContentLengthOutputStream.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::Impl::IO::ChunkedOutputStream;
using Org::Apache::Http::Impl::IO::IdentityOutputStream;
using Org::Apache::Http::Impl::IO::ContentLengthOutputStream;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Entity {

EntitySerializer::EntitySerializer(
    /* [in] */ IContentLengthStrategy* lenStrategy)
{
    if (lenStrategy == NULL) {
        Logger::E("EntitySerializer", "Content length strategy may not be null");
        assert(0);
        // throw new IllegalArgumentException("Content length strategy may not be null");
    }
    mLenStrategy = lenStrategy;
}

ECode EntitySerializer::DoSerialize(
    /* [in] */ ISessionOutputBuffer* outbuffer,
    /* [in] */ IHttpMessage* message,
    /* [out] */ IOutputStream** output)
{
    VALIDATE_NOT_NULL(output)
    *output = NULL;

    Int64 len;
    mLenStrategy->DetermineLength(message, &len);
    AutoPtr<IOutputStream> o;
    if (len == IContentLengthStrategy::CHUNKED) {
        AutoPtr<IOutputStream> o = (IOutputStream*)new ChunkedOutputStream(outbuffer);
    }
    else if (len == IContentLengthStrategy::IDENTITY) {
        AutoPtr<IOutputStream> o = (IOutputStream*)new IdentityOutputStream(outbuffer);
    }
    else {
        AutoPtr<IOutputStream> o = (IOutputStream*)new ContentLengthOutputStream(outbuffer, len);
    }
    *output = o;
    REFCOUNT_ADD(*output);
    return NOERROR;
}

ECode EntitySerializer::Serialize(
    /* [in] */ ISessionOutputBuffer* outbuffer,
    /* [in] */ IHttpMessage* message,
    /* [in] */ IHttpEntity* entity)
{
    if (outbuffer == NULL) {
        Logger::E("EntitySerializer", "Session output buffer may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("Session output buffer may not be null");
    }
    if (message == NULL) {
        Logger::E("EntitySerializer", "HTTP message may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("HTTP message may not be null");
    }
    if (entity == NULL) {
        Logger::E("EntitySerializer", "HTTP entity may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("HTTP entity may not be null");
    }
    AutoPtr<IOutputStream> outstream;
    DoSerialize(outbuffer, message, (IOutputStream**)&outstream);
    entity->WriteTo(outstream);
    outstream->Close();
    return NOERROR;
}

} // namespace Entity
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org