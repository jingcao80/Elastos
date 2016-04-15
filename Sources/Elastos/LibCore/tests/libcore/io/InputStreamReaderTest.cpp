/*
 * ~/libcore/luni/src/test/java/libcore/java/io/InputStreamReaderTest.java
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
using namespace Elastos::Utility::Zip;
using namespace Elastosx::Crypto;

using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::Zip::CCRC32;
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
using Elastos::IO::IInputStream;
using Elastos::Utility::Zip::ICheckedInputStream;
using Elastos::Utility::Zip::CCheckedInputStream;

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

import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import junit.framework.TestCase;

public final class InputStreamReaderTest extends TestCase {

    /**
     * This bug claims that InputStreamReader blocks unnecessarily:
     * http://code.google.com/p/android/issues/detail?id=10252
     */
    public void testReadDoesNotBlockUnnecessarily() throws IOException {
        PipedInputStream pin = new PipedInputStream();
        PipedOutputStream pos = new PipedOutputStream(pin);
        pos.write("hello".getBytes("UTF-8"));

        InputStreamReader reader = new InputStreamReader(pin);
        char[] buffer = new char[1024];
        int count = reader.read(buffer);
        assertEquals(5, count);
    }
}
#endif
static void testReadDoesNotBlockUnnecessarily()
{
    ECode ec;
    AutoPtr<IPipedInputStream> pin;
    AutoPtr<IPipedOutputStream> pos;
    AutoPtr<IInputStreamReader> isr;
    AutoPtr<IInputStream> is;
    AutoPtr<IOutputStream> os ;

    ec = CPipedInputStream::New((IPipedInputStream**)&pin);
    ec = CPipedOutputStream::New((IPipedInputStream *)pin, (IPipedOutputStream**)&pos);

    String str = String("hello");
    AutoPtr<ArrayOf<Byte> > aBytes;
    aBytes = str.GetBytes();

    os = IOutputStream::Probe(pos);
    os->Write(aBytes);

    is = IInputStream::Probe(pin);
    ec = CInputStreamReader::New((IInputStream *)is, (IInputStreamReader**)&isr);

    is = NULL;
    is = IInputStream::Probe(isr);

    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(1024);
    Int32 count;
    is->Read(buffer, &count);

    assertEquals("testReadDoesNotBlockUnnecessarily", 5, count);
}


//==============================================================================

int mainInputStreamReaderTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/InputStreamReaderTest ====\n");
    testReadDoesNotBlockUnnecessarily();
    printf("\n==== end of libcore/math/InputStreamReaderTest ====\n");

    return 0;
}

}
}
