
#include "Elastos.CoreLibrary.IO.h"
#include "CStreamResult.h"
#include "FilePathToURI.h"

namespace Elastosx {
namespace Xml {
namespace Transform {
namespace Stream {

CAR_OBJECT_IMPL(CStreamResult)
CAR_INTERFACE_IMPL_2(CStreamResult, Object, IStreamResult, IResult)
ECode CStreamResult::constructor()
{
    return NOERROR;
}

ECode CStreamResult::constructor(
    /* [in] */ IOutputStream* outputStream)
{
    return SetOutputStream(outputStream);
}

ECode CStreamResult::constructor(
    /* [in] */ IWriter* writer)
{
    return SetWriter(writer);
}

ECode CStreamResult::constructor(
    /* [in] */ const String& systemId)
{
    mSystemId = systemId;
    return NOERROR;
}

ECode CStreamResult::constructor(
    /* [in] */ IFile* f)
{
    return SetSystemId(f);
}

ECode CStreamResult::SetOutputStream(
    /* [in] */ IOutputStream* outputStream)
{
    mOutputStream = outputStream;
    return NOERROR;
}

ECode CStreamResult::GetOutputStream(
    /* [out] */ IOutputStream** stream)
{
    VALIDATE_NOT_NULL(stream);
    *stream = mOutputStream;
    REFCOUNT_ADD(*stream);
    return NOERROR;
}

ECode CStreamResult::SetWriter(
    /* [in] */ IWriter* writer)
{
    mWriter = writer;
    return NOERROR;
}

ECode CStreamResult::GetWriter(
    /* [out] */ IWriter** writer)
{
    VALIDATE_NOT_NULL(writer);
    *writer = mWriter;
    REFCOUNT_ADD(*writer);
    return NOERROR;
}

ECode CStreamResult::SetSystemId(
    /* [in] */ const String& systemId)
{
    mSystemId = systemId;
    return NOERROR;
}

ECode CStreamResult::SetSystemId(
    /* [in] */ IFile* f)
{
    assert(f);
    String path;
    f->GetAbsolutePath(&path);
    mSystemId = FilePathToURI::Filepath2URI(path);
    return NOERROR;
}

ECode CStreamResult::GetSystemId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mSystemId;
    return NOERROR;
}

} // namespace Stream
} // namespace Transform
} // namespace Xml
} // namespace Elastosx
