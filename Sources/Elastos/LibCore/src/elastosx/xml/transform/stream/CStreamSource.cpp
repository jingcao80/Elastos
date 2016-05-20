
#include "Elastos.CoreLibrary.IO.h"
#include "CStreamSource.h"
#include "FilePathToURI.h"

namespace Elastosx {
namespace Xml {
namespace Transform {
namespace Stream {

CAR_OBJECT_IMPL(CStreamSource)
CAR_INTERFACE_IMPL_2(CStreamSource, Object, IStreamSource, ISource)
ECode CStreamSource::constructor()
{
    return NOERROR;
}

ECode CStreamSource::constructor(
    /* [in] */ IInputStream* inputStream)
{
    return SetInputStream(inputStream);
}

ECode CStreamSource::constructor(
    /* [in] */ IInputStream* inputStream,
    /* [in] */ const String& systemId)
{
    SetInputStream(inputStream);
    return SetSystemId(systemId);
}

ECode CStreamSource::constructor(
    /* [in] */ IReader* reader)
{
    return SetReader(reader);
}

ECode CStreamSource::constructor(
    /* [in] */ IReader* reader,
    /* [in] */ const String& systemId)
{
    SetReader(reader);
    return SetSystemId(systemId);
}

ECode CStreamSource::constructor(
    /* [in] */ const String& systemId)
{
    mSystemId = systemId;
    return NOERROR;
}

ECode CStreamSource::constructor(
    /* [in] */ IFile* f)
{
    return SetSystemId(f);
}

ECode CStreamSource::SetInputStream(
    /* [in] */ IInputStream* inputStream)
{
    mInputStream = inputStream;
    return NOERROR;
}

ECode CStreamSource::GetInputStream(
    /* [out] */ IInputStream** stream)
{
    VALIDATE_NOT_NULL(stream);
    *stream = mInputStream;
    REFCOUNT_ADD(*stream);
    return NOERROR;
}

ECode CStreamSource::SetReader(
    /* [in] */ IReader* reader)
{
    mReader = reader;
    return NOERROR;
}

ECode CStreamSource::GetReader(
    /* [out] */ IReader** reader)
{
    VALIDATE_NOT_NULL(reader);
    *reader = mReader;
    REFCOUNT_ADD(*reader);
    return NOERROR;
}

ECode CStreamSource::SetPublicId(
    /* [in] */ const String& publicId)
{
    mPublicId = publicId;
    return NOERROR;
}

ECode CStreamSource::GetPublicId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mPublicId;
    return NOERROR;
}

ECode CStreamSource::SetSystemId(
    /* [in] */ const String& systemId)
{
    mSystemId = systemId;
    return NOERROR;
}

ECode CStreamSource::GetSystemId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mSystemId;
    return NOERROR;
}

ECode CStreamSource::SetSystemId(
    /* [in] */ IFile* f)
{
    assert(f);
    String path;
    f->GetAbsolutePath(&path);
    mSystemId = FilePathToURI::Filepath2URI(path);
    return NOERROR;
}

} // namespace Stream
} // namespace Transform
} // namespace Xml
} // namespace Elastosx
