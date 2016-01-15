/*
 * ~/libcore/luni/src/test/java/libcore/io/Base64Test.java
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
using Elastos::Core::StringUtils;
using Libcore::IO::IBase64;
using Libcore::IO::CBase64;
using Elastos::Utility::Arrays;

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

static void assertEncoded(String expected, int count, ...)
{
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(count);
    if (aBytes == NULL) {
        printf("No Memory\n");
        return;
    }

    va_list args;

    va_start(args, count);
    for (int i = 0; i < count; i++) {
        aBytes->Set(i, (Byte)va_arg(args, int));
    }
    va_end(args);

    AutoPtr<IBase64> bs64;
    CBase64::AcquireSingleton((IBase64**)&bs64);
    String str;
    bs64->Encode(aBytes, &str);

    assertEquals("", expected, str);
}


#if 0
/*
 *  Licensed to the Apache Software Foundation (ASF) under one or more
 *  contributor license agreements.  See the NOTICE file distributed with
 *  this work for additional information regarding copyright ownership.
 *  The ASF licenses this file to You under the Apache License, Version 2.0
 *  (the "License"); you may not use this file except in compliance with
 *  the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

package libcore.io;

import java.util.Arrays;
import junit.framework.TestCase;

public final class Base64Test extends TestCase {
#endif

#if 0
    public void testDecodeEmpty() throws Exception {
        assertEquals("[]", Arrays.toString(Base64.decode(new byte[0])));
    }
#endif
void testDecodeEmpty()
{
    unsigned char _aBytes[] = {};
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(sizeof(_aBytes));
    if (aBytes == NULL) {
        printf("No Memory\n");
        return;
    }
    memcpy(aBytes->GetPayload(), _aBytes, sizeof(_aBytes));

    AutoPtr<IBase64> bs64;
    CBase64::AcquireSingleton((IBase64**)&bs64);
    AutoPtr<ArrayOf<Byte> > rst;
    bs64->Decode(aBytes, (ArrayOf<Byte>**)&rst);

    String str;
    str = Arrays::ToString(rst);

    assertEquals("right", String("[]"), str);
}

#if 0
    public void testEncode() throws Exception {
        assertEncoded("");
        assertEncoded("Eg==", 0x12);
        assertEncoded("EjQ=", 0x12, 0x34 );
        assertEncoded("EjRW", 0x12, 0x34, 0x56);
        assertEncoded("EjRWeA==", 0x12, 0x34, 0x56, 0x78);
        assertEncoded("EjRWeJo=", 0x12, 0x34, 0x56, 0x78, 0x9A);
        assertEncoded("EjRWeJq8", 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc);
    }
#endif
void testEncode()
{
        assertEncoded(String(""), 0);
        assertEncoded(String("Eg=="), 1, 0x12);
        assertEncoded(String("EjQ="), 2, 0x12, 0x34 );
        assertEncoded(String("EjRW"), 3, 0x12, 0x34, 0x56);
        assertEncoded(String("EjRWeA=="), 4, 0x12, 0x34, 0x56, 0x78);
        assertEncoded(String("EjRWeJo="), 5, 0x12, 0x34, 0x56, 0x78, 0x9A);
        assertEncoded(String("EjRWeJq8"), 6, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc);
}


#if 0
    public void testEncodeDoesNotWrap() {
        int[] data = new int[61];
        Arrays.fill(data, 0xff);
        String expected = "///////////////////////////////////////////////////////////////////////"
                + "//////////w=="; // 84 chars
        assertEncoded(expected, data);
    }
#endif
void testEncodeDoesNotWrap()
{
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(61);
    if (aBytes == NULL) {
        printf("No Memory\n");
        return;
    }

    for (int i = 0; i < aBytes->GetLength(); i++) {
        aBytes->Set(i, 0xff);
    }

    AutoPtr<IBase64> bs64;
    CBase64::AcquireSingleton((IBase64**)&bs64);
    String str;
    bs64->Encode(aBytes, &str);

    String expected = String("///////////////////////////////////////////////////////////////////////")
            + String("//////////w=="); // 84 chars
    assertEquals("", expected, str);
}


#if 0
    public void assertEncoded(String expected , int... data) {
        byte[] dataBytes = new byte[data.length];
        for (int i = 0; i < data.length; i++) {
            dataBytes[i] = (byte) data[i];
        }
        assertEquals(expected, Base64.encode(dataBytes));
    }
#endif


//==============================================================================

int mainBase64Test(int argc, char *argv[])
{
    printf("\n==== libcore/math/Base64Test ====\n");
    testDecodeEmpty();
    testEncode();
    printf("\n==== end of libcore/math/Base64Test ====\n");

    return 0;
}

}
}


