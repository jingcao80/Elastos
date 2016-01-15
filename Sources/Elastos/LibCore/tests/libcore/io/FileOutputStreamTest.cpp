/*
 * ~/libcore/luni/src/test/java/libcore/java/io/FileOutputStreamTest.java
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
using Elastos::Utility::Arrays;
using Elastos::Core::IAppendable;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Libcore::IO::CIoUtils;
using Libcore::IO::IIoUtils;
using Elastos::IO::IFileDescriptor;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IOs;

namespace Elastos {
namespace HelloCar {

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
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import junit.framework.TestCase;

public class FileOutputStreamTest extends TestCase {
    public void testFileDescriptorOwnership() throws Exception {
        File tmp = File.createTempFile("FileOutputStreamTest", "tmp");
        FileOutputStream fos1 = new FileOutputStream(tmp);
        FileOutputStream fos2 = new FileOutputStream(fos1.getFD());

        // Close the second FileDescriptor and check we can't use it...
        fos2.close();
        try {
            fos2.write(1);
            fail();
        } catch (IOException expected) {
        }
        try {
            fos2.write(new byte[1], 0, 1);
            fail();
        } catch (IOException expected) {
        }
        // ...but that we can still use the first.
        fos1.write(1);

        // Close the first FileDescriptor and check we can't use it...
        fos1.close();
        try {
            fos1.write(1);
            fail();
        } catch (IOException expected) {
        }
        try {
            fos1.write(new byte[1], 0, 1);
            fail();
        } catch (IOException expected) {
        }
    }
#endif
static void testFileDescriptorOwnership()
{
    AutoPtr<IFile> f;
    AutoPtr<IFileHelper> fh;
    AutoPtr<IFileOutputStream> fos1;
    AutoPtr<IFileOutputStream> fos2;
    AutoPtr<IFileDescriptor> fd;
    AutoPtr<IOutputStream> fo2;
    ECode ec;

    CFileHelper::AcquireSingleton((IFileHelper**)&fh);
    ec = fh->CreateTempFile(String("FileOutputStreamTest"), String("tmp"), (IFile**)&f);
    if (FAILED(ec) || f == NULL) {
        printf(" Failed to create CFile. Error %08X\n", ec);
    }

    ec = CFileOutputStream::New(f, (IFileOutputStream **)&fos1);
    if (FAILED(ec) || fos1 == NULL) {
        printf("Failed to create CFileInputStream, Error %08X\n", ec);
    }

    ec = fos1->GetFD((IFileDescriptor**)&fd);
    if (FAILED(ec) || fd == NULL) {
        printf(" Failed to GetFD. Error %08X\n", ec);
    }

    ec = CFileOutputStream::New(fd, (IFileOutputStream **)&fos2);
    if (FAILED(ec) || fos2 == NULL) {
        printf("Failed to create CFileOutputStream, Error %08X\n", ec);
    }

    fo2 = IOutputStream::Probe(fos2);
    fo2->Close();

    ec = fo2->Write(1);
    if (FAILED(ec)) {
        printf("Failed to call fo2->Write(1), Error %08X\n", ec);
    }

    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(1);
    if (aBytes == NULL) {
        printf("No Memory\n");
        return;
    }

    ec = fo2->Write(aBytes);
    if (FAILED(ec)) {
        printf("Failed to call fo2->Write(aBytes), Error %08X\n", ec);
    }

    ec = fo2->Write(1);
    if (FAILED(ec)) {
        printf("Failed to call fo2->Write(1), Error %08X\n", ec);
    }

    ec = fo2->Write(aBytes);
    if (FAILED(ec)) {
        printf("Failed to call fo2->Write(aBytes), Error %08X\n", ec);
    }
}

#if 0
    public void testClose() throws Exception {
        FileOutputStream fos = new FileOutputStream(File.createTempFile("FileOutputStreamTest", "tmp"));

        // Closing an already-closed stream is a no-op...
        fos.close();
        fos.close();
        // ...as is flushing...
        fos.flush();

        // ...but any explicit write is an error.
        byte[] bytes = "hello".getBytes();
        try {
            fos.write(bytes);
            fail();
        } catch (IOException expected) {
        }
        try {
            fos.write(bytes, 0, 2);
            fail();
        } catch (IOException expected) {
        }
        try {
            fos.write(42);
            fail();
        } catch (IOException expected) {
        }

        // ...except a 0-byte write.
        fos.write(new byte[0], 0, 0);
    }
}
#endif
static void testClose()
{
    AutoPtr<IFile> f;
    AutoPtr<IFileHelper> fh;
    AutoPtr<IFileOutputStream> fos;
    AutoPtr<IFileDescriptor> fd;
    AutoPtr<IOutputStream> fo;
    AutoPtr<IFlushable> ff;
    ECode ec;

    CFileHelper::AcquireSingleton((IFileHelper**)&fh);
    ec = fh->CreateTempFile(String("FileOutputStreamTest"), String("tmp"), (IFile**)&f);
    if (FAILED(ec) || f == NULL) {
        printf(" Failed to create CFile. Error %08X\n", ec);
    }

    ec = CFileOutputStream::New(f, (IFileOutputStream **)&fos);
    if (FAILED(ec) || fos == NULL) {
        printf("Failed to create CFileInputStream, Error %08X\n", ec);
    }

    fo = IOutputStream::Probe(fos);
    fo->Close();
    fo->Close();

    ff = IFlushable::Probe(fos);
    ff->Flush();

    String str = String("hello");
    AutoPtr<ArrayOf<Byte> > aBytes;
    aBytes = str.GetBytes();

    ec = fo->Write(aBytes);
    if (FAILED(ec)) {
        printf("Yes! IOException expected, ECode %08X\n", ec);
    }

    ec = fo->Write(aBytes, 0, 2);
    if (FAILED(ec)) {
        printf("Yes! IOException expected, ECode %08X\n", ec);
    }

    ec = fo->Write(42);
    if (FAILED(ec)) {
        printf("Yes! IOException expected, ECode %08X\n", ec);
    }

    AutoPtr<ArrayOf<Byte> > bBytes = ArrayOf<Byte>::Alloc(0);

    ec = fo->Write(bBytes, 0, 0);
    if (FAILED(ec)) {
        printf("Yes! IOException expected, ECode %08X\n", ec);
    }
}


//==============================================================================

int mainFileOutputStreamTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/FileOutputStreamTest ====\n");
    testFileDescriptorOwnership();
    testClose();
    printf("\n==== end of libcore/math/FileOutputStreamTest ====\n");

    return 0;
}

}
}
