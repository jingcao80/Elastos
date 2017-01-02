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
 * ~/libcore/luni/src/test/java/libcore/java/io/OldAndroidBufferedOutputStreamTest.java
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
//using Elastos::IO::CObjectOutputStream;
using Elastos::IO::IObjectOutputStream;

namespace Elastos {
namespace OldAndroidBufferedOutputStreamTest {

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

import java.io.BufferedOutputStream;
import java.io.ByteArrayOutputStream;
import junit.framework.TestCase;

/**
 * Tests to verify that simple functionality works for BufferedOutputStreams.
 */
public class OldAndroidBufferedOutputStreamTest extends TestCase {

    public void testBufferedOutputStream() throws Exception {
        String str = "AbCdEfGhIjKlMnOpQrStUvWxYz";
        ByteArrayOutputStream aa = new ByteArrayOutputStream();
        BufferedOutputStream a = new BufferedOutputStream(aa, 15);
        try {
            a.write(str.getBytes(), 0, 26);
            a.write('A');

            assertEquals(26, aa.size());
            assertEquals(aa.toString(), str);

            a.flush();

            assertEquals(27, aa.size());
            assertEquals("AbCdEfGhIjKlMnOpQrStUvWxYzA", aa.toString());
        } finally {
            a.close();
        }
    }
}
#endif
static void testBufferedOutputStream()
{
    ECode ec;
    Int32 size;
    String str1;
    String str = String("AbCdEfGhIjKlMnOpQrStUvWxYz");

    AutoPtr<IByteArrayOutputStream> aa;
    AutoPtr<IBufferedOutputStream> a;
    AutoPtr<IOutputStream> outs;

    ec = CByteArrayOutputStream::New((IByteArrayOutputStream**)&aa);
    outs = IOutputStream::Probe((IByteArrayOutputStream *)aa);
    ec = CBufferedOutputStream::New((IOutputStream *)outs, 15, (IBufferedOutputStream**)&a);

    AutoPtr<IOutputStream> ios;
    AutoPtr<IFlushable> ifs;
    ios = IOutputStream::Probe((IBufferedOutputStream *)a);
    ifs = IFlushable::Probe((IOutputStream *)ios);

    AutoPtr<ArrayOf<Byte> > aBytes;
    aBytes = str.GetBytes();

    ios->Write((ArrayOf<Byte> *)aBytes, 0, 26);
    ios->Write('A');

    aa->GetSize(&size);
    assertEquals("testBufferedOutputStream", 26, size);
    aa->ToString(&str1);
    assertEquals("testBufferedOutputStream", str1, str);
    ifs->Flush();
    aa->GetSize(&size);
    assertEquals("testBufferedOutputStream", 27, size);

    aa->ToString(&str1);
    assertEquals("testBufferedOutputStream", str1, String("AbCdEfGhIjKlMnOpQrStUvWxYzA"));

    AutoPtr<ICloseable> ac;
    ac = ICloseable::Probe((IBufferedOutputStream *)a);
    ac->Close();
}


//==============================================================================

int mainOldAndroidBufferedOutputStreamTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/OldAndroidBufferedOutputStreamTest ====\n");
    testBufferedOutputStream();
    printf("\n==== end of libcore/math/OldAndroidBufferedOutputStreamTest ====\n");

    return 0;
}

}
}
