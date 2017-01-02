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
using Elastos::IO::IInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;

int main(int argc, char *argv[])
{
    String fp = String("/data/data/com.elastos.runtime/elastos/iotest.txt");

    AutoPtr<IFile> file;
    ECode ec = CFile::New(fp, (IFile**)&file);
    ASSERT_SUCCEEDED(ec)

    Boolean exist;
    file->Exists(&exist);
    if (!exist) {
        printf("error: %p does not exists\n", fp.string());
        return 0;
    }

    AutoPtr<IFileInputStream> fis;
    ec = CFileInputStream::New(fp, (IFileInputStream**)&fis);
    ASSERT_SUCCEEDED(ec)

    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(1024);
	Int32 number;
    ec = IInputStream::Probe(fis)->Read(bytes, 0, 6, &number);
    ASSERT_SUCCEEDED(ec)

    printf("==== File: %s, Line: %d, Num: %d ====\n", __FILE__, __LINE__, number);
	for (Int32 i = 0; i < number; i++) {
		printf("%d ", (*bytes)[i]);
	}
	printf("\n");

    ICloseable::Probe(fis)->Close();
    return 0;
}
