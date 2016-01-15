
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/internal/http/multipart/FilePartSource.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {
namespace Multipart {

CAR_INTERFACE_IMPL_2(FilePartSource, Object, IFilePartSource, IPartSource)

ECode FilePartSource::constructor(
    /* [in] */ IFile* file)
{
    mFile = file;
    if (file != NULL) {
        Boolean isFile;
        file->IsFile(&isFile);
        if (!isFile) {
            Logger::E(String("FilePartSource"), String("File is not a normal file."));
            return E_FILE_NOT_FOUND_EXCEPTION;
        }

        Boolean canRead;
        file->CanRead(&canRead);
        if (!canRead) {
            Logger::E(String("FilePartSource"), String("File is not readable."));
            return E_FILE_NOT_FOUND_EXCEPTION;
        }

        file->GetName(&mFileName);
    }

    return NOERROR;
}

ECode FilePartSource::constructor(
    /* [in] */ const String& fileName,
    /* [in] */ IFile* file)
{
    FAIL_RETURN(constructor(file));

    if (fileName != NULL) {
        mFileName = fileName;
    }

    return NOERROR;
}

ECode FilePartSource::GetLength(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length);

    if (mFile != NULL) {
        return mFile->GetLength(length);
    }

    *length = 0;
    return NOERROR;
}

ECode FilePartSource::GetFileName(
    /* [out] */ String* fileName)
{
    VALIDATE_NOT_NULL(fileName);

    *fileName = (mFileName == NULL) ? String("noname") : mFileName;
    return NOERROR;
}

ECode FilePartSource::CreateInputStream(
    /* [out] */ IInputStream** stream)
{
    VALIDATE_NOT_NULL(stream);

    if (mFile != NULL) {
        return CFileInputStream::New(mFile, stream);
    }

    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(0);
    return CByteArrayInputStream::New(bytes, stream);
}

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos
