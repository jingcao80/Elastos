
#include "org/apache/http/entity/CEntityTemplate.h"
#include "elastos/io/CByteArrayInputStream.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

CAR_OBJECT_IMPL(CEntityTemplate)

ECode CEntityTemplate::GetContentLength(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length)
    *length = -1;
    return NOERROR;
}

ECode CEntityTemplate::GetContent(
    /* [out] */ IInputStream** inputStream)
{
    VALIDATE_NOT_NULL(inputStream)
    Logger::E("CEntityTemplate", "Entity template does not implement getContent()");
    return  E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CEntityTemplate::IsRepeatable(
    /* [out] */ Boolean* isRepeatable)
{
    VALIDATE_NOT_NULL(isRepeatable)
    *isRepeatable = TRUE;
    return NOERROR;
}

ECode CEntityTemplate::WriteTo(
    /* [in] */ IOutputStream* outstream)
{
    if (outstream == NULL) {
        Logger::E("CEntityTemplate", "Output stream may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return mContentproducer->WriteTo(outstream);
}

ECode CEntityTemplate::IsStreaming(
    /* [out] */ Boolean* isStreaming)
{
    VALIDATE_NOT_NULL(isStreaming)
    *isStreaming = TRUE;
    return NOERROR;
}

ECode CEntityTemplate::ConsumeContent()
{
    return NOERROR;
}

ECode CEntityTemplate::constructor(
    /* [in] */ IContentProducer* contentproducer)
{
    if (contentproducer == NULL) {
        Logger::E("CEntityTemplate", "Content producer may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mContentproducer = contentproducer;
    return NOERROR;
}

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org
