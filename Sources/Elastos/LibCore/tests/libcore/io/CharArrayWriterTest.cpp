/*
 * ~/libcore/luni/src/test/java/libcore/java/io/CharArrayWriterTest.java
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

import java.io.CharArrayWriter;
import junit.framework.TestCase;

public final class CharArrayWriterTest extends TestCase {

    public void testCharArrayWriter() throws Exception {
        String str = "AbCdEfGhIjKlMnOpQrStUvWxYz";
        CharArrayWriter a = new CharArrayWriter();
        CharArrayWriter b = new CharArrayWriter();

        a.write(str, 0, 26);
        a.write('X');
        a.writeTo(b);

        assertEquals(27, a.size());
        assertEquals("AbCdEfGhIjKlMnOpQrStUvWxYzX", a.toString());

        b.write("alphabravodelta", 5, 5);
        b.append('X');
        assertEquals("AbCdEfGhIjKlMnOpQrStUvWxYzXbravoX", b.toString());
        b.append("omega");
        assertEquals("AbCdEfGhIjKlMnOpQrStUvWxYzXbravoXomega", b.toString());
    }
}
#endif
void testCharArrayWriter()
{
    String str = String("AbCdEfGhIjKlMnOpQrStUvWxYz");

    AutoPtr<ICharArrayWriter> a;
    CCharArrayWriter::New((ICharArrayWriter**)&a);
    AutoPtr<ICharArrayWriter> b;
    CCharArrayWriter::New((ICharArrayWriter**)&b);

    AutoPtr<IWriter> aWriter;
    aWriter = IWriter::Probe(a);

    AutoPtr<IWriter> bWriter;
    bWriter = IWriter::Probe(b);

    aWriter->Write(str, 0, 26);
    aWriter->Write('X');
    a->WriteTo(bWriter);

    Int32 i;
    a->GetSize(&i);
    assertEquals("", 27, i);

    String str2;
    str2 = Object::ToString(a);
    assertEquals("", String("AbCdEfGhIjKlMnOpQrStUvWxYzX"), str2);

    bWriter->Write(String("alphabravodelta"), 5, 5);

    AutoPtr<IAppendable> bAppendable;
    bAppendable = IAppendable::Probe(b);
    bAppendable->AppendChar(Char32('X'));

    String str3;
    str3 = Object::ToString(b);
    assertEquals("", String("AbCdEfGhIjKlMnOpQrStUvWxYzXbravoX"), str3);

    AutoPtr<ICharSequence> obj;
    CString::New(String("omega"), (ICharSequence**)&obj);
    bAppendable->Append((ICharSequence*)obj.Get());

    String str4;
    str4 = Object::ToString(b);
    assertEquals("", String("AbCdEfGhIjKlMnOpQrStUvWxYzXbravoXomega"), str4);
}


//==============================================================================

int mainCharArrayWriterTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/CharArrayWriterTest ====\n");
    testCharArrayWriter();
    printf("\n==== end of libcore/math/CharArrayWriterTest ====\n");

    return 0;
}

}
}
