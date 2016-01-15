
#include "org/apache/http/entity/HttpEntityWrapper.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

CAR_INTERFACE_IMPL(HttpEntityWrapper, Object, IHttpEntity)

ECode HttpEntityWrapper::IsRepeatable(
    /* [out] */ Boolean* isRepeatable)
{
    VALIDATE_NOT_NULL(isRepeatable)
    return mWrappedEntity->IsRepeatable(isRepeatable);
}

ECode HttpEntityWrapper::IsChunked(
    /* [out] */ Boolean* isChunked)
{
    VALIDATE_NOT_NULL(isChunked)
    return mWrappedEntity->IsChunked(isChunked);
}

ECode HttpEntityWrapper::GetContentLength(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length)
    return mWrappedEntity->GetContentLength(length);
}

ECode HttpEntityWrapper::GetContentType(
    /* [out] */ IHeader** type)
{
    VALIDATE_NOT_NULL(type)
    return mWrappedEntity->GetContentType(type);
}

ECode HttpEntityWrapper::GetContentEncoding(
    /* [out] */ IHeader** encoding)
{
    VALIDATE_NOT_NULL(encoding)
    return mWrappedEntity->GetContentEncoding(encoding);
}

ECode HttpEntityWrapper::GetContent(
    /* [out] */ IInputStream** inputStream)
{
    VALIDATE_NOT_NULL(inputStream)
    return mWrappedEntity->GetContent(inputStream);
}

ECode HttpEntityWrapper::WriteTo(
    /* [in] */ IOutputStream* outstream)
{
    return mWrappedEntity->WriteTo(outstream);
}

ECode HttpEntityWrapper::IsStreaming(
    /* [out] */ Boolean* isStreaming)
{
    VALIDATE_NOT_NULL(isStreaming)
    return mWrappedEntity->IsStreaming(isStreaming);
}

ECode HttpEntityWrapper::ConsumeContent()
{
    return mWrappedEntity->ConsumeContent();
}

ECode HttpEntityWrapper::Init(
    /* [in] */ IHttpEntity* wrapped)
{
    if (wrapped == NULL) {
        Logger::E("HttpEntityWrapper", "wrapped entity must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mWrappedEntity = wrapped;
    return NOERROR;
}

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org