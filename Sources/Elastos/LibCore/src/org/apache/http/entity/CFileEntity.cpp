
#include "org/apache/http/entity/CFileEntity.h"
#include "elastos/io/CFileInputStream.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFlushable;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

CAR_INTERFACE_IMPL(CFileEntity, AbstractHttpEntity, ICloneable)

CAR_OBJECT_IMPL(CFileEntity)

ECode CFileEntity::IsRepeatable(
    /* [out] */ Boolean* isRepeatable)
{
    VALIDATE_NOT_NULL(isRepeatable)
    *isRepeatable = TRUE;
    return NOERROR;
}

ECode CFileEntity::GetContentLength(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length)
    return mFile->GetLength(length);
}

ECode CFileEntity::GetContent(
    /* [out] */ IInputStream** inputStream)
{
    VALIDATE_NOT_NULL(inputStream)
    AutoPtr<IFileInputStream> stream;
    CFileInputStream::New(mFile, (IFileInputStream**)&stream);
    *inputStream = IInputStream::Probe(stream);
    REFCOUNT_ADD(*inputStream)
    return NOERROR;
}

ECode CFileEntity::WriteTo(
    /* [in] */ IOutputStream* outstream)
{
    if (outstream == NULL) {
        Logger::E("CFileEntity", "Output stream may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IFileInputStream> stream;
    CFileInputStream::New(mFile, (IFileInputStream**)&stream);
    AutoPtr<IInputStream> instream = IInputStream::Probe(stream);
    // try {
    AutoPtr< ArrayOf<Byte> > tmp = ArrayOf<Byte>::Alloc(4096);
    Int32 l;
    while (instream->Read(tmp, &l), l != -1) {
        outstream->Write(tmp, 0, l);
    }
    IFlushable::Probe(outstream)->Flush();
    ICloseable::Probe(instream)->Close();
    // } finally {
    //     instream.close();
    // }
    return NOERROR;
}

ECode CFileEntity::IsStreaming(
    /* [out] */ Boolean* isStreaming)
{
    VALIDATE_NOT_NULL(isStreaming)
    *isStreaming = FALSE;
    return NOERROR;
}

ECode CFileEntity::Clone(
    /* [out] */ IInterface** o)
{
    VALIDATE_NOT_NULL(o)
    AutoPtr<IAbstractHttpEntity> entity;
    CFileEntity::New((IAbstractHttpEntity**)&entity);
    AutoPtr<CFileEntity> entityCls = (CFileEntity*)entity.Get();
    entityCls->mContentType = mContentType;
    entityCls->mContentEncoding = mContentEncoding;
    entityCls->mChunked = mChunked;
    entityCls->mFile = mFile;
    *o = entity->Probe(EIID_IInterface);
    REFCOUNT_ADD(*o)
    return NOERROR;
}

ECode CFileEntity::constructor(
    /* [in] */ IFile* file,
    /* [in] */ const String& contentType)
{
    if (file == NULL) {
        Logger::E("CFileEntity", "File may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mFile = file;
    SetContentType(contentType);
    return NOERROR;
}

ECode CFileEntity::constructor()
{
    return NOERROR;
}

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org
