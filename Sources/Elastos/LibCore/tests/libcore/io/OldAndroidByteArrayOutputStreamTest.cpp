/*
 * ~/libcore/luni/src/test/java/libcore/java/io/OldAndroidByteArrayOutputStreamTest.java
 */

#include <stdarg.h>

#include <elautoptr.h>
#include <elastos/coredef.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::StringUtils;
using Elastos::Core::IAppendable;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Libcore::IO::CIoUtils;
using Libcore::IO::IIoUtils;
using Elastos::IO::IFileDescriptor;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IOs;
using Elastos::IO::IInputStream;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IStringWriter;
using Elastos::IO::CStringWriter;
using Elastos::IO::IBufferedWriter;
using Elastos::IO::CBufferedWriter;
using Elastos::IO::IWriter;
using Elastos::IO::ICloseable;
using Elastos::IO::IFlushable;
using Elastos::Utility::Arrays;
using Elastos::Utility::Zip::ICheckedInputStream;
using Elastos::Utility::Zip::CCheckedInputStream;
using Elastos::Utility::Zip::CCRC32;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace OldAndroidByteArrayOutputStreamTest {

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
 * Copyright (C) 2008 The Android Open Source Project
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

import java.io.ByteArrayOutputStream;
import junit.framework.TestCase;

/**
 * A basic test for ByteArrayOutputStream.
 */
public class OldAndroidByteArrayOutputStreamTest extends TestCase {

    public void testByteArrayOutputStream() throws Exception {
        String str = "AbCdEfGhIjKlMnOpQrStUvWxYz";
        ByteArrayOutputStream a = new ByteArrayOutputStream();
        ByteArrayOutputStream b = new ByteArrayOutputStream(10);

        a.write(str.getBytes(), 0, 26);
        a.write('X');
        a.writeTo(b);

        assertEquals(27, a.size());
        assertEquals("AbCdEfGhIjKlMnOpQrStUvWxYzX", a.toString());
        assertEquals("AbCdEfGhIjKlMnOpQrStUvWxYzX", b.toString());
    }
}
#endif
void testByteArrayOutputStream()
{
    ECode ec;

    String str("AbCdEfGhIjKlMnOpQrStUvWxYz");
    AutoPtr<IByteArrayOutputStream> a;
    AutoPtr<IByteArrayOutputStream> b;

    ec = CByteArrayOutputStream::New((IByteArrayOutputStream **)&a);
    ec = CByteArrayOutputStream::New(10, (IByteArrayOutputStream **)&b);

    AutoPtr<IOutputStream> foa;
    AutoPtr<IOutputStream> fob;
    foa = IOutputStream::Probe(a);
    fob = IOutputStream::Probe(b);

    foa->Write(str.GetBytes(), 0, 26);
    foa->Write('X');
    a->WriteTo(fob);

    Int32 size;
    a->GetSize(&size);
    assertEquals("", 27, size);

    String str1;
    String str2;
    IObject::Probe(a)->ToString(&str1);
    IObject::Probe(b)->ToString(&str2);

    assertEquals("testByteArrayOutputStream", String("AbCdEfGhIjKlMnOpQrStUvWxYzX"), str1);
    assertEquals("testByteArrayOutputStream", String("AbCdEfGhIjKlMnOpQrStUvWxYzX"), str2);
}

//==============================================================================

int mainOldAndroidByteArrayOutputStreamTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/OldAndroidByteArrayOutputStreamTest ====\n");
    testByteArrayOutputStream();
    printf("\n==== end of libcore/math/OldAndroidByteArrayOutputStreamTest ====\n");

    return 0;
}

}
}
