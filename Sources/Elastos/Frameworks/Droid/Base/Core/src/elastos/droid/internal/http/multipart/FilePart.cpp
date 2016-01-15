
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/internal/http/multipart/FilePart.h"
#include "elastos/droid/internal/http/multipart/CFilePartSource.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::IO::IInputStream;
using Elastos::Utility::Logging::Logger;
// using Org::Apache::Commons::Logging::ILogFactory;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {
namespace Multipart {

// TODO:
// static AutoPtr<ILog> InitStaticLOG()
// {
//     AutoPtr<ILogFactory> factory;
//     CLogFactory::AcquireSingleton((ILogFactory**)&factory);
//     AutoPtr<ILog> log;
//     factory->GetLog(String("FilePart"), (ILog**)&log)
//     return log;
// }
// const AutoPtr<ILog> FilePart::LOG = InitStaticLOG();

extern AutoPtr<ArrayOf<Byte> > GetAsciiBytes(
        /* [in] */ const String& data);

const String FilePart::FILE_NAME = String("; filename=");
const AutoPtr<ArrayOf<Byte> > FilePart::FILE_NAME_BYTES = GetAsciiBytes(FILE_NAME);

CAR_INTERFACE_IMPL(FilePart, PartBase, IFilePart)

ECode FilePart::constructor(
    /* [in] */ const String& name,
    /* [in] */ IPartSource* partSource,
    /* [in] */ const String& contentType,
    /* [in] */ const String& charset)
{
    FAIL_RETURN(PartBase::constructor(name,
        contentType.IsNull() ? DEFAULT_CONTENT_TYPE : contentType,
        charset.IsNull() ? String("ISO-8859-1") : charset,
        DEFAULT_TRANSFER_ENCODING))

    if (partSource == NULL) {
        Logger::E(String("FilePart"), String("Source may not be null"));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mSource = partSource;
    return NOERROR;
}

ECode FilePart::constructor(
    /* [in] */ const String& name,
    /* [in] */ IPartSource* partSource)
{
    return constructor(name, partSource, String(NULL), String(NULL));
}

ECode FilePart::constructor(
    /* [in] */ const String& name,
    /* [in] */ IFile* file)
{
    AutoPtr<IPartSource> partSource;
    FAIL_RETURN(CFilePartSource::New(file, (IPartSource**)&partSource));
    return constructor(name, partSource, String(NULL), String(NULL));
}

ECode FilePart::constructor(
    /* [in] */ const String& name,
    /* [in] */ IFile* file,
    /* [in] */ const String& contentType,
    /* [in] */ const String& charset)
{
    AutoPtr<IPartSource> partSource;
    FAIL_RETURN(CFilePartSource::New(file, (IPartSource**)&partSource));
    return constructor(name, partSource, contentType, charset);
}

ECode FilePart::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& fileName,
    /* [in] */ IFile* file)
{
    AutoPtr<IPartSource> partSource;
    FAIL_RETURN(CFilePartSource::New(fileName, file, (IPartSource**)&partSource));
    return constructor(name, partSource, String(NULL), String(NULL));
}

ECode FilePart::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& fileName,
    /* [in] */ IFile* file,
    /* [in] */ const String& contentType,
    /* [in] */ const String& charset)
{
    AutoPtr<IPartSource> partSource;
    FAIL_RETURN(CFilePartSource::New(fileName, file, (IPartSource**)&partSource));
    return constructor(name, partSource, contentType, charset);
}

ECode FilePart::SendDispositionHeader(
    /* [in] */ IOutputStream* out)
{
    // LOG->Trace(String("enter sendDispositionHeader(OutputStream out)"));
    FAIL_RETURN(PartBase::SendDispositionHeader(out))
    String filename;
    mSource->GetFileName(&filename);
    if (filename != NULL) {
        FAIL_RETURN(out->Write(FILE_NAME_BYTES))
        FAIL_RETURN(out->Write(QUOTE_BYTES))
        FAIL_RETURN(out->Write(GetAsciiBytes(filename)))
        FAIL_RETURN(out->Write(QUOTE_BYTES))
    }
    return NOERROR;
}

ECode FilePart::SendData(
    /* [in] */ IOutputStream* outStream)
{
    // LOG->Trace(String("enter sendData(OutputStream out)"));
    Int64 length;
    LengthOfData(&length);
    if (length == 0) {

        // this file contains no data, so there is nothing to send.
        // we don't want to create a zero length buffer as this will
        // cause an infinite loop when reading.
        // LOG->Debug(String("No data to send."));
        return NOERROR;
    }

    AutoPtr<ArrayOf<Byte> > tmp = ArrayOf<Byte>::Alloc(4096);
    AutoPtr<IInputStream> inStream;
    mSource->CreateInputStream((IInputStream**)&inStream);

    Int32 len;
    ECode ec;
    while ((ec = inStream->Read(tmp, &len)) == NOERROR && len >= 0) {
        ec = outStream->Write(tmp, 0, len);
        if(ec != NOERROR) {
            break;
        }
    }

    // we're done with the stream, close it
    inStream->Close();

    return ec;
}

ECode FilePart::GetSource(
    /* [out] */ IPartSource** partSource)
{
    // LOG->Trace(String("enter getSource()"));

    VALIDATE_NOT_NULL(partSource);
    *partSource = mSource;
    REFCOUNT_ADD(*partSource);
    return NOERROR;
}

ECode FilePart::LengthOfData(
    /* [out] */ Int64* length)
{
    // LOG->Trace(String("enter lengthOfData()"));

    VALIDATE_NOT_NULL(length);
    return mSource->GetLength(length);
}

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos
