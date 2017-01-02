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

/*
 * ~/libcore/luni/src/test/java/libcore/java/io/FileInputStreamTest.java
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

#include "CMyThread.h"
CAR_OBJECT_IMPL(CMyThread)

ECode CMyThread::constructor(IFileDescriptor *ifd)
{
    mOutFd = ifd;

    return Thread::constructor();   // 实现 IThread 的 CAR 类必须调用基类 Thread 的某个 constructor 以便进行必要的初始化
}

ECode CMyThread::Run()
{
    printf("CMyThread::Run\n");
    AutoPtr<IFileOutputStream> fos;
    const int TOTAL_SIZE = 1024;

    ECode ec = CFileOutputStream::New(mOutFd, (IFileOutputStream**)&fos);
    if (FAILED(ec) || fos == NULL) {
        printf(" Failed to create CFile. Error %08X\n", ec);
    }

    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(TOTAL_SIZE);
    for (int i = 0; i < buffer->GetLength(); ++i) {
        buffer->Set(i, (Byte)i);
    }

    AutoPtr<IOutputStream> fo;
    fo = IOutputStream::Probe(fos);
    fo->Write(buffer);

    AutoPtr<IIoUtils> iou;
    CIoUtils::AcquireSingleton((IIoUtils**)&iou);
    AutoPtr<IFileDescriptor> fd;
    fd = IFileDescriptor::Probe(fos);
    iou->CloseQuietly(fd);
    iou->Close(mOutFd);

    return Thread::Run();
}


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
 * Copyright (C) 2010 The Android Open Source Project
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

import libcore.io.IoUtils;
import libcore.io.Libcore;

public final class FileInputStreamTest extends TestCase {
    private static final int TOTAL_SIZE = 1024;
    private static final int SKIP_SIZE = 100;

    private static class DataFeeder extends Thread {
        private FileDescriptor mOutFd;

        public DataFeeder(FileDescriptor fd) {
            mOutFd = fd;
        }

        @Override
        public void run() {
            try {
                FileOutputStream fos = new FileOutputStream(mOutFd);
                try {
                    byte[] buffer = new byte[TOTAL_SIZE];
                    for (int i = 0; i < buffer.length; ++i) {
                        buffer[i] = (byte) i;
                    }
                    fos.write(buffer);
                } finally {
                    IoUtils.closeQuietly(fos);
                    IoUtils.close(mOutFd);
                }
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }
    }
#endif



#if 0
    private void verifyData(FileInputStream is, int start, int count) throws IOException {
        byte buffer[] = new byte[count];
        assertEquals(count, is.read(buffer));
        for (int i = 0; i < count; ++i) {
            assertEquals((byte) (i + start), buffer[i]);
        }
    }
#endif
void verifyData(IInputStream *fi, int start, int count)
{
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(count);

    //AutoPtr<IInputStream> fi;
    Int32 number;
    //fi = IInputStream::Probe(is);
    fi->Read(buffer, &number);
    assertEquals("verifyData", count, number);

    for (int i = 0; i < count; ++i) {
        assertEquals("verifyData", i+start, (*buffer)[i]);
    }
}

#if 0
    public void testSkipInPipes() throws Exception {
        FileDescriptor[] pipe = Libcore.os.pipe();
        DataFeeder feeder = new DataFeeder(pipe[1]);
        try {
            feeder.start();
            FileInputStream fis = new FileInputStream(pipe[0]);
            fis.skip(SKIP_SIZE);
            verifyData(fis, SKIP_SIZE, TOTAL_SIZE - SKIP_SIZE);
            assertEquals(-1, fis.read());
            feeder.join(1000);
            assertFalse(feeder.isAlive());
        } finally {
            IoUtils.closeQuietly(pipe[0]);
        }
    }
#endif
void testSkipInPipes()
{
    const int TOTAL_SIZE = 1024;
    const int SKIP_SIZE = 100;

    AutoPtr<ArrayOf<IFileDescriptor*> > pipe;
    AutoPtr<ILibcore> libcore;
    AutoPtr<IOs> os;

    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    libcore->GetOs((IOs**)&os);
    os->Pipe((ArrayOf<IFileDescriptor*> **)&pipe);

    AutoPtr<IThread> feeder;
    CMyThread::New((*pipe)[1], (IThread**)&feeder);
    assert(feeder.Get() != NULL && "Thread is null!");
    feeder->Start();

    AutoPtr<IInputStream> fis;
    Int64 number;
    Int32 value;

    CFileInputStream::New((*pipe)[0], (IInputStream**)&fis);
    fis->Skip((Int64)SKIP_SIZE, &number);
    verifyData(fis, SKIP_SIZE, TOTAL_SIZE - SKIP_SIZE);
    fis->Read(&value);
    assertEquals("", -1, value);
    feeder->Join(1000);

    Boolean b;
    feeder->IsAlive(&b);

    assertEquals("", FALSE, b);

    AutoPtr<IIoUtils> iou;
    CIoUtils::AcquireSingleton((IIoUtils**)&iou);
    iou->CloseQuietly((*pipe)[0]);
}

#if 0
    public void testDirectories() throws Exception {
        try {
            new FileInputStream(".");
            fail();
        } catch (FileNotFoundException expected) {
        }
    }
#endif
void testDirectories()
{
    AutoPtr<IFileInputStream> fis;

    ECode ec = CFileInputStream::New(String(""), (IFileInputStream**)&fis);
    if (FAILED(ec) || fis == NULL) {
        printf(" catched (FileNotFoundException expected), Error %08X\n", ec);
    } else {
        printf(" Create CFileInStream \"\" success, Why???. Error %08X\n", ec);
    }
}

#if 0
    private File makeFile() throws Exception {
        File tmp = File.createTempFile("FileOutputStreamTest", "tmp");
        FileOutputStream fos = new FileOutputStream(tmp);
        fos.write(1);
        fos.write(1);
        fos.close();
        return tmp;
    }
#endif
IFile *makeFile()
{
    AutoPtr<IFile> f;
    AutoPtr<IFileHelper> fh;
    CFileHelper::AcquireSingleton((IFileHelper**)&fh);
    ECode ec = fh->CreateTempFile(String("FileOutputStreamTest"), String("tmp"), (IFile**)&f);
    if (FAILED(ec) || f == NULL) {
        printf(" Failed to create CFile. Error %08X\n", ec);
    }

    AutoPtr<IOutputStream> fo;
    fo = IOutputStream::Probe(f);

    fo->Write(1);
    fo->Write(1);
    fo->Close();

    REFCOUNT_ADD(f)

    return (IFile *)f;
}


#if 0
    public void testFileDescriptorOwnership() throws Exception {
        File tmp = makeFile();

        FileInputStream fis1 = new FileInputStream(tmp);
        FileInputStream fis2 = new FileInputStream(fis1.getFD());

        // Close the second FileDescriptor and check we can't use it...
        fis2.close();
        try {
            fis2.available();
            fail();
        } catch (IOException expected) {
        }
        try {
            fis2.read();
            fail();
        } catch (IOException expected) {
        }
        try {
            fis2.read(new byte[1], 0, 1);
            fail();
        } catch (IOException expected) {
        }
        try {
            fis2.skip(1);
            fail();
        } catch (IOException expected) {
        }
        // ...but that we can still use the first.
        assertFalse(fis1.read() == -1);

        // Close the first FileDescriptor and check we can't use it...
        fis1.close();
        try {
            fis1.available();
            fail();
        } catch (IOException expected) {
        }
        try {
            fis1.read();
            fail();
        } catch (IOException expected) {
        }
        try {
            fis1.read(new byte[1], 0, 1);
            fail();
        } catch (IOException expected) {
        }
        try {
            fis1.skip(1);
            fail();
        } catch (IOException expected) {
        }
    }
#endif
void testFileDescriptorOwnership()
{
    IFile *tmp = makeFile();

    AutoPtr<IFileInputStream> fis1;
    AutoPtr<IFileInputStream> fis2;

    ECode ec = CFileInputStream::New(tmp, (IFileInputStream**)&fis1);
    if (FAILED(ec) || fis1 == NULL) {
        printf("Failed to create CFileInputStream, Error %08X\n", ec);
    }

    AutoPtr<IFileDescriptor> fd;
    ec = fis1->GetFD((IFileDescriptor**)&fd);
    if (FAILED(ec) || fd == NULL) {
        printf(" Failed to GetFD. Error %08X\n", ec);
    }

    ec = CFileInputStream::New(tmp, (IFileInputStream**)&fis2);
    if (FAILED(ec) || fis2 == NULL) {
        printf("Failed to create CFileInputStream, Error %08X\n", ec);
    }

    AutoPtr<IInputStream> is1;
    AutoPtr<IInputStream> is2;
    is1 = IInputStream::Probe(is1);
    is2 = IInputStream::Probe(is2);

    // Close the second FileDescriptor and check we can't use it...
    is2->Close();

    Int32 number;
    ec = is2->Available(&number);
    if (FAILED(ec)) {
        printf("Failed to call is2->Available(), Error %08X\n", ec);
    }

    ec = is2->Read(&number);
    if (FAILED(ec) && number == -1) {
        printf("Failed to call is2->Read(), Error %08X\n", ec);
    }

    Int64 i64;
    ec = is2->Skip(1, &i64);
    if (FAILED(ec)) {
        printf("Failed to call is2->Skip(), Error %08X\n", ec);
    }

    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(1);
    ec = is2->Read(aBytes, 0, 1, &number);
    if (FAILED(ec)) {
        printf("Failed to call is2->Read(), Error %08X\n", ec);
    }

    ec = is1->Available(&number);
    if (FAILED(ec)) {
        printf("Failed to call is2->Available(), Error %08X\n", ec);
    }

    ec = is1->Read(&number);
    if (FAILED(ec) && number == -1) {
        printf("Failed to call is2->Read(), Error %08X\n", ec);
    }

    ec = is1->Read(aBytes, 0, 1, &number);
    if (FAILED(ec)) {
        printf("Failed to call is2->Read(), Error %08X\n", ec);
    }

    ec = is1->Skip(1, &i64);
    if (FAILED(ec)) {
        printf("Failed to call is2->Skip(), Error %08X\n", ec);
    }
}

#if 0
    public void testClose() throws Exception {
        File tmp = makeFile();
        FileInputStream fis = new FileInputStream(tmp);

        // Closing an already-closed stream is a no-op...
        fis.close();
        fis.close();

        // But any explicit activity is an error.
        try {
            fis.available();
            fail();
        } catch (IOException expected) {
        }
        try {
            fis.read();
            fail();
        } catch (IOException expected) {
        }
        try {
            fis.read(new byte[1], 0, 1);
            fail();
        } catch (IOException expected) {
        }
        try {
            fis.skip(1);
            fail();
        } catch (IOException expected) {
        }
        // Including 0-byte skips...
        try {
            fis.skip(0);
            fail();
        } catch (IOException expected) {
        }
        // ...but not 0-byte reads...
        fis.read(new byte[0], 0, 0);
    }
}
#endif
void testClose()
{
    IFile *tmp = makeFile();

    AutoPtr<IFileInputStream> fis;

    ECode ec = CFileInputStream::New(tmp, (IFileInputStream**)&fis);
    if (FAILED(ec) || fis == NULL) {
        printf("Failed to create CFileInputStream, Error %08X\n", ec);
    }

    AutoPtr<IInputStream> is;
    is = IInputStream::Probe(fis);

    // Close the second FileDescriptor and check we can't use it...
    is->Close();
    is->Close();

    Int32 number;
    ec = is->Available(&number);
    if (FAILED(ec)) {
        printf("Failed to call is->Available(), Error %08X\n", ec);
    }

    ec = is->Read(&number);
    if (FAILED(ec) && number == -1) {
        printf("Failed to call is2->Read(), Error %08X\n", ec);
    }

    Int64 i64;
    ec = is->Skip(1, &i64);
    if (FAILED(ec)) {
        printf("Failed to call is2->Skip(), Error %08X\n", ec);
    }

    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(1);
    ec = is->Read(aBytes, 0, 1, &number);
    if (FAILED(ec)) {
        printf("Failed to call is2->Read(), Error %08X\n", ec);
    }
}



//==============================================================================

int mainFileInputStreamTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/FileInputStreamTest ====\n");
    testSkipInPipes();
    testDirectories();
    testFileDescriptorOwnership();
    testClose();
    printf("\n==== end of libcore/math/FileInputStreamTest ====\n");

    return 0;
}

}
}

