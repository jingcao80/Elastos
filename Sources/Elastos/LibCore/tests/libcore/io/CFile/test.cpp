
#include <elastos/core/Object.h>

using namespace Elastos;
using Elastos::IO::IFile;
using Elastos::IO::CFile;

void testCreateFile()
{
    PFL
    String fp("/data/data/com.elastos.runtime/elastos/filetest.txt");
    AutoPtr<IFile> file;
    CFile::New(fp, (IFile**)&file);
    PFL
    Boolean succeeded;
    file->CreateNewFile(&succeeded);
    assert(succeeded == TRUE);
    PFL
}

void testListFile(const String& rootPath)
{
    AutoPtr<IFile> dir;
    CFile::New(rootPath, (IFile**)&dir);
    assert(dir != NULL);

    Boolean isDir;
    dir->IsDirectory(&isDir);
    assert(isDir);

    AutoPtr<ArrayOf<IFile*> > files;
    dir->ListFiles((ArrayOf<IFile*>**)&files);
    IFile* file;
    String fileName, path;
    Boolean canRead;
    for (Int32 i = 0; i < files->GetLength(); ++i) {
        file = (*files)[i];
        file->GetName(&fileName);
        file->CanRead(&canRead);

        if (file->IsDirectory(&isDir), isDir) {
            PFL_EX("==== dir: [%s], canRead: %d", fileName.string(), canRead);
            file->GetAbsolutePath(&path);
            testListFile(path);
        }
        else {
            PFL_EX(" >> file: [%s], canRead: %d", fileName.string(), canRead);
        }
    }
}

int main(int argc, char *argv[])
{
    testCreateFile();
    testListFile(String("/sdcard/Images/"));
    return 0;
}
