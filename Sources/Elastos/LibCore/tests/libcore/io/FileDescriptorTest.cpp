/*
 * ~/libcore/luni/src/test/java/libcore/java/io/FileDescriptorTest.java
 */

#include <stdarg.h>

#include <elautoptr.h>
#include <elastos/coredef.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/Arrays.h>

using namespace Elastos;
using namespace Elastos::IO;
using namespace Elastos::Core;

using Elastos::Core::StringUtils;
using Libcore::IO::IBase64;
using Libcore::IO::CBase64;
using Elastos::Utility::Arrays;
using Elastos::Core::IAppendable;

namespace Elastos {
namespace IO {

static void assertEquals(const char *info, Int32 aspect, Int32 test)
{
    printf("aspect: %d, test: %d. %s\n", aspect, test, info);
    assert(aspect == test);
}

static void assertEquals(const char *info, String aspect, String test)
{
    printf("aspect: %s, test: %s. %s\n", aspect.string(), test.string(), info);
    assert(aspect.Equals(test) == 0);
}

#if 0
/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package libcore.java.io;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import junit.framework.TestCase;

public class FileDescriptorTest extends TestCase {
    public void testReadOnlyFileDescriptorSync() throws Exception {
        File f= File.createTempFile("FileDescriptorTest", "tmp");
        new RandomAccessFile(f, "r").getFD().sync();
    }
}
#endif
void testReadOnlyFileDescriptorSync()
{
    AutoPtr<IFile> f;
    AutoPtr<IFileHelper> fh;
    CFileHelper::AcquireSingleton((IFileHelper**)&fh);
    ECode ec = fh->CreateTempFile(String("FileDescriptorTest"), String("tmp"), (IFile**)&f);
    if (FAILED(ec) || f == NULL) {
        printf(" Failed to create CFile. Error %08X\n", ec);
    }

	AutoPtr<IRandomAccessFile> fra;
	ec = CRandomAccessFile::New(f, String("r"), (IRandomAccessFile**)&fra);
    if (FAILED(ec) || fra == NULL) {
        printf(" Failed to New IRandomAccessFile. Error %08X\n", ec);
    }

	AutoPtr<IFileDescriptor> fd;
	ec = fra->GetFD((IFileDescriptor**)&fd);
    if (FAILED(ec) || fd == NULL) {
        printf(" Failed to GetFD. Error %08X\n", ec);
    }

	ec = fd->Sync();
    if (FAILED(ec)) {
        printf(" Failed to Sync. Error %08X\n", ec);
    }
}


//==============================================================================

int mainFileDescriptorTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/FileDescriptorTest ====\n");
    testReadOnlyFileDescriptorSync();
    printf("\n==== end of libcore/math/FileDescriptorTest ====\n");

    return 0;
}

}
}

