/*
 * ~/libcore/luni/src/test/java/libcore/java/io/OldAndroidBufferedWriterTest.java
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

namespace Elastos {
namespace OldAndroidBufferedWriterTest {

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

import java.io.BufferedWriter;
import java.io.StringWriter;
import junit.framework.TestCase;

/**
 * Some basic tests for BufferedWriter.
 */
public class OldAndroidBufferedWriterTest extends TestCase {

    public void testBufferedWriter() throws Exception {
        String str = "AbCdEfGhIjKlMnOpQrStUvWxYz";
        StringWriter aa = new StringWriter();

        BufferedWriter a = new BufferedWriter(aa, 20);
        try {
            a.write(str.toCharArray(), 0, 26);
            a.write('X');
            a.flush();
            assertEquals("AbCdEfGhIjKlMnOpQrStUvWxYzX", aa.toString());

            a.write("alphabravodelta", 5, 5);
            a.flush();
            assertEquals("AbCdEfGhIjKlMnOpQrStUvWxYzXbravo", aa.toString());
            a.newLine();
            a.write("I'm on a new line.");
            a.flush();
            assertEquals("AbCdEfGhIjKlMnOpQrStUvWxYzXbravo\nI\'m on a new line.", aa.toString());
        } finally {
            a.close();
        }
    }
}
#endif
void testBufferedWriter()
{
    ECode ec;
    String str("AbCdEfGhIjKlMnOpQrStUvWxYz");
    AutoPtr<IStringWriter> aa;
    AutoPtr<IBufferedWriter> a;
    AutoPtr<IWriter> iw;

    ec = CStringWriter::New((IStringWriter **)&aa);

    iw = IWriter::Probe(aa);
    ec = CBufferedWriter::New(iw, 20, (IBufferedWriter **)&a);
    iw = NULL;

    AutoPtr<IBufferedWriter> iba;
    AutoPtr<ICloseable> ac;

    iw = IWriter::Probe((IBufferedWriter *)a);
    ac = ICloseable::Probe((IBufferedWriter *)a);

    iw->Write(str, 0, 26);

    iw->Write('X');

    AutoPtr<IFlushable> ff;
    ff = IFlushable::Probe((IBufferedWriter *)a);
    ff->Flush();

    String str1;
    str1 = Object::ToString((IStringWriter *)aa);

    assertEquals("testBufferedWriter", String("AbCdEfGhIjKlMnOpQrStUvWxYzX"), str1);

    iw->Write(String("alphabravodelta"), 5, 5);
    ff->Flush();

    String str2;
    IObject::Probe(aa)->ToString(&str2);
    assertEquals("testBufferedWriter", String("AbCdEfGhIjKlMnOpQrStUvWxYzXbravo"), str2);

    a->NewLine();
    iw->Write(String("I'm on a new line."));
    ff->Flush();

    String str3;
    str3 = Object::ToString((IStringWriter *)aa);
    assertEquals("testBufferedWriter", String("AbCdEfGhIjKlMnOpQrStUvWxYzXbravo\nI\'m on a new line."), str3);

    ac->Close();
}

//==============================================================================

int mainOldAndroidBufferedWriterTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/OldAndroidBufferedWriterTest ====\n");
    testBufferedWriter();
    printf("\n==== end of libcore/math/OldAndroidBufferedWriterTest ====\n");

    return 0;
}

}
}
