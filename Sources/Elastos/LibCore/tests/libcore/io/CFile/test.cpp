//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
