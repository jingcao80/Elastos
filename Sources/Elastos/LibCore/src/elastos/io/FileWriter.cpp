
#include "FileWriter.h"
#include "CFileOutputStream.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(FileWriter, OutputStreamWriter, IFileWriter)

ECode FileWriter::constructor(
    /* [in] */ IFile* file)
{
    AutoPtr<IOutputStream> fileOutputStream;
    FAIL_RETURN((CFileOutputStream::New(file, (IOutputStream**)&fileOutputStream)))
    return OutputStreamWriter::constructor(fileOutputStream);
}

ECode FileWriter::constructor(
    /* [in] */ IFile* file,
    /* [in] */ Boolean append)
{
    AutoPtr<IOutputStream> fileOutputStream;
    FAIL_RETURN((CFileOutputStream::New(file, append, (IOutputStream**)&fileOutputStream)))
    return OutputStreamWriter::constructor(fileOutputStream);
}

ECode FileWriter::constructor(
    /* [in] */ IFileDescriptor* fd)
{
    AutoPtr<IOutputStream> fileOutputStream;
    FAIL_RETURN((CFileOutputStream::New(fd, (IOutputStream**)&fileOutputStream)))
    return OutputStreamWriter::constructor(fileOutputStream);
}

ECode FileWriter::constructor(
    /* [in] */ const String& filename)
{
    AutoPtr<IOutputStream> fileOutputStream;
    FAIL_RETURN((CFileOutputStream::New(filename, (IOutputStream**)&fileOutputStream)))
    return OutputStreamWriter::constructor(fileOutputStream);
}

ECode FileWriter::constructor(
    /* [in] */ const String& filename,
    /* [in] */ Boolean append)
{
    AutoPtr<IOutputStream> fileOutputStream;
    FAIL_RETURN((CFileOutputStream::New(filename, append, (IOutputStream**)&fileOutputStream)))
    return OutputStreamWriter::constructor(fileOutputStream);
}

} // namespace IO
} // namespace Elastos
