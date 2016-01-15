
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/internal/http/multipart/ByteArrayPartSource.h"

using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {
namespace Multipart {

CAR_INTERFACE_IMPL_2(ByteArrayPartSource, Object, IByteArrayPartSource, IPartSource)

ECode ByteArrayPartSource::constructor(
    /* [in] */ const String& fileName,
    /* [in] */ ArrayOf<Byte>* bytes)
{
    mFileName = fileName;
    mBytes = bytes;
    return NOERROR;
}

ECode ByteArrayPartSource::GetLength(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length);
    *length = mBytes->GetLength();
    return NOERROR;
}

ECode ByteArrayPartSource::GetFileName(
    /* [out] */ String* fileName)
{
    VALIDATE_NOT_NULL(fileName);
    *fileName = mFileName;
    return NOERROR;
}

ECode ByteArrayPartSource::CreateInputStream(
    /* [out] */ IInputStream** stream)
{
    return CByteArrayInputStream::New(mBytes, stream);
}

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos
