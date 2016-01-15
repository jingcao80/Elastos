
#include "org/apache/http/entity/CByteArrayEntity.h"
#include "elastos/io/CByteArrayInputStream.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IFlushable;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

CAR_INTERFACE_IMPL(CByteArrayEntity, AbstractHttpEntity, ICloneable)

CAR_OBJECT_IMPL(CByteArrayEntity)

ECode CByteArrayEntity::IsRepeatable(
    /* [out] */ Boolean* isRepeatable)
{
    VALIDATE_NOT_NULL(isRepeatable)
    *isRepeatable = TRUE;
    return NOERROR;
}

ECode CByteArrayEntity::GetContentLength(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length)
    *length = mContent->GetLength();
    return NOERROR;
}

ECode CByteArrayEntity::GetContent(
    /* [out] */ IInputStream** inputStream)
{
    VALIDATE_NOT_NULL(inputStream)
    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(mContent, (IByteArrayInputStream**)&stream);
    *inputStream = IInputStream::Probe(stream);
    REFCOUNT_ADD(*inputStream)
    return NOERROR;
}

ECode CByteArrayEntity::WriteTo(
    /* [in] */ IOutputStream* outstream)
{
    if (outstream == NULL) {
        Logger::E("CByteArrayEntity", "Output stream may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    outstream->Write(mContent);
    IFlushable::Probe(outstream)->Flush();
    return NOERROR;
}

ECode CByteArrayEntity::IsStreaming(
    /* [out] */ Boolean* isStreaming)
{
    VALIDATE_NOT_NULL(isStreaming)
    *isStreaming = FALSE;
    return NOERROR;
}

ECode CByteArrayEntity::Clone(
    /* [out] */ IInterface** o)
{
    VALIDATE_NOT_NULL(o)
    AutoPtr<IAbstractHttpEntity> entity;
    CByteArrayEntity::New(mContent, (IAbstractHttpEntity**)&entity);
    AutoPtr<CByteArrayEntity> entityCls = (CByteArrayEntity*)entity.Get();
    entityCls->mContentType = mContentType;
    entityCls->mContentEncoding = mContentEncoding;
    entityCls->mChunked = mChunked;
    *o = entity->Probe(EIID_IInterface);
    REFCOUNT_ADD(*o)
    return NOERROR;
}

ECode CByteArrayEntity::constructor(
    /* [in] */ ArrayOf<Byte>* b)
{
    if (b == NULL) {
        Logger::E("CByteArrayEntity", "Source byte array may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mContent = b;
    return NOERROR;
}

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org
