
#include "FileReader.h"
#include "CFileInputStream.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(FileReader, InputStreamReader, IFileReader)

ECode FileReader::constructor(
    /* [in] */ IFile* file)
{
    AutoPtr<IInputStream> is;
    FAIL_RETURN(CFileInputStream::New(file, (IInputStream**)&is))
    return InputStreamReader::constructor(is);
}

ECode FileReader::constructor(
    /* [in] */ IFileDescriptor* file)
{
    AutoPtr<IInputStream> is;
    CFileInputStream::New(file, (IInputStream**)&is);
    return InputStreamReader::constructor(is);
}

ECode FileReader::constructor(
    /* [in] */ const String& file)
{
    AutoPtr<IInputStream> is;
    FAIL_RETURN(CFileInputStream::New(file, (IInputStream**)&is))
    return InputStreamReader::constructor(is);
}


} // namespace IO
} // namespace Elastos