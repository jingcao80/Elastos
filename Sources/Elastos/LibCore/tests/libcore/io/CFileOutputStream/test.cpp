#include <elastos/core/Object.h>

using namespace Elastos;
using Elastos::IO::IOutputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;

int main(int argc, char *argv[])
{
    printf("==== File: %s, Line: %d ====\n", __FILE__, __LINE__);
    String filePath = String("/data/data/com.elastos.runtime/elastos/iotest.txt");

    AutoPtr<IFile> file;
    ECode ec = CFile::New(filePath, (IFile**)&file);
    ASSERT_SUCCEEDED(ec)

    Boolean exist;
    file->Exists(&exist);
    if (!exist) {
        Boolean succeeded = FALSE;
        ASSERT_SUCCEEDED(file->CreateNewFile(&succeeded))
        printf("%p does not exists, create a new one : %d.\n", filePath.string(), succeeded);
    }

    AutoPtr<IFileOutputStream> fos;
    ec = CFileOutputStream::New(filePath, (IFileOutputStream**)&fos);
    ASSERT_SUCCEEDED(ec)

    printf("==== File: %s, Line: %d ====\n", __FILE__, __LINE__);
    ASSERT_SUCCEEDED(IOutputStream::Probe(fos)->Write('A'))
    ASSERT_SUCCEEDED(IOutputStream::Probe(fos)->Write('B'))
    ASSERT_SUCCEEDED(IOutputStream::Probe(fos)->Write('C'))

    printf("==== File: %s, Line: %d ====\n", __FILE__, __LINE__);
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(3);
    (*bytes)[0] = 'D';
    (*bytes)[1] = 'E';
    (*bytes)[2] = 'F';
    ASSERT_SUCCEEDED(IOutputStream::Probe(fos)->Write(bytes, 0, 3))
    printf("==== File: %s, Line: %d ====\n", __FILE__, __LINE__);

    ICloseable::Probe(fos)->Close();
    return 0;
}
