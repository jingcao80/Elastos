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
 * ~/libcore/luni/src/test/java/libcore/java/io/DataOutputStreamTest.java
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

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

static String toHexString(ArrayOf<Byte> *array)
{
    if (array == NULL) {
        return (String)NULL;
    }
    if (array->GetLength() == 0) {
        return String("[]");
    }

    StringBuilder sb;
    char buf[32];
    String str;

    sb.AppendChar((Char32)'[');
    snprintf(buf, 32, "%02x", (*array)[0] & 0xff);
    sb.Append(String(buf));
    for (int i = 1; i < array->GetLength(); i++) {
        sb.Append(String(", "));
        snprintf(buf, 32, "%02x", (*array)[i] & 0xff);
        sb.Append(String(buf));
    }
    sb.AppendChar(']');

    str = sb.ToString();

    return str;
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

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.UTFDataFormatException;
import java.util.Arrays;
import junit.framework.TestCase;

public final class DataOutputStreamTest extends TestCase {
    private ByteArrayOutputStream bytes = new ByteArrayOutputStream();
    private DataOutputStream os = new DataOutputStream(bytes);
#endif

#if 0
    public void test_writeBoolean() throws Exception {
        os.writeBoolean(true);
        os.writeBoolean(false);
        assertEquals("[01, 00]", toHexString(bytes.toByteArray()));
    }
#endif
void test_writeBoolean()
{
    AutoPtr<IByteArrayOutputStream> bytes;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&bytes);
    AutoPtr<IDataOutputStream> os;
    os = IDataOutputStream::Probe(bytes);

    AutoPtr<IDataOutput> dos;
    dos = IDataOutput::Probe(os);

    dos->WriteBoolean(TRUE);
    dos->WriteBoolean(FALSE);

    AutoPtr<ArrayOf<Byte> > aBytes;

    bytes->ToByteArray((ArrayOf<Byte>**)&aBytes);

    assertEquals("", String("[01, 00]"), toHexString(aBytes));
}

#if 0
    public void test_writeByte() throws Exception {
        os.writeByte(-1);
        os.writeByte(0);
        os.writeByte(1);
        os.writeByte(129);
        // writeByte takes only the bottom byte from its int parameter.
        os.writeByte(0x1234);
        assertEquals("[ff, 00, 01, 81, 34]", toHexString(bytes.toByteArray()));
    }

    public void test_writeBytes() throws Exception {
        // writeBytes takes only the bottom byte from each character.
        os.writeBytes("0\u12341");
        assertEquals("[30, 34, 31]", toHexString(bytes.toByteArray()));
    }

    public void test_writeChar() throws Exception {
        // writeChar writes two-byte big-endian characters.
        os.writeChar('0');
        os.writeChar(0x1234);
        assertEquals("[00, 30, 12, 34]", toHexString(bytes.toByteArray()));
    }

    public void test_writeChars() throws Exception {
        // writeChars writes two-byte big-endian characters.
        os.writeChars("0\u12341");
        assertEquals("[00, 30, 12, 34, 00, 31]", toHexString(bytes.toByteArray()));
    }

    public void test_writeDouble() throws Exception {
        os.writeDouble(Double.longBitsToDouble(0x0123456789abcdefL));
        assertEquals("[01, 23, 45, 67, 89, ab, cd, ef]", toHexString(bytes.toByteArray()));
    }

    public void test_writeFloat() throws Exception {
        os.writeFloat(Float.intBitsToFloat(0x01234567));
        assertEquals("[01, 23, 45, 67]", toHexString(bytes.toByteArray()));
    }

    public void test_writeInt() throws Exception {
        os.writeInt(0x01234567);
        assertEquals("[01, 23, 45, 67]", toHexString(bytes.toByteArray()));
    }

    public void test_writeLong() throws Exception {
        os.writeLong(0x0123456789abcdefL);
        assertEquals("[01, 23, 45, 67, 89, ab, cd, ef]", toHexString(bytes.toByteArray()));
    }

    public void test_writeShort() throws Exception {
        // writeShort only writes the bottommost 16 bits of its int parameter.
        os.writeShort(0x01234567);
        assertEquals("[45, 67]", toHexString(bytes.toByteArray()));
    }

    public void test_writeUTF() throws Exception {
        // The limit is 65535 *bytes* but we want to test 2- and 3-byte characters too.
        char[] chars = new char[65535 - 1 - 2];
        Arrays.fill(chars, 0, chars.length, 'a');
        chars[0] = '\u0666'; // a two-byte character
        chars[1] = '\u2603'; // a three-byte character
        String maxLength = new String(chars);
        os.writeUTF(maxLength);
        byte[] expected = new byte[2 + 65535];
        expected[0] = (byte) 0xff;
        expected[1] = (byte) 0xff;
        // U+0666 = 0xD9 0xA6
        expected[2] = (byte) 0xd9;
        expected[3] = (byte) 0xa6;
        // U+2603 = 0xE2 0x98 0x83
        expected[4] = (byte) 0xe2;
        expected[5] = (byte) 0x98;
        expected[6] = (byte) 0x83;
        Arrays.fill(expected, 7, expected.length, (byte) 'a');
        assertEquals(Arrays.toString(expected), Arrays.toString(bytes.toByteArray()));
    }

    public void test_writeUTF_NUL() throws Exception {
        // This is a special case, represented with two non-zero bytes.
        os.writeUTF("\u0000");
        assertEquals("[00, 02, c0, 80]", toHexString(bytes.toByteArray()));
    }

    public void test_writeUTF_too_long() throws Exception {
        String tooLong = new String(new char[65536]);
        try {
            os.writeUTF(tooLong);
            fail("should throw UTFDataFormatException");
        } catch (UTFDataFormatException expected) {
        }
        assertEquals("[]", toHexString(bytes.toByteArray()));
    }

    /**
     * Returns a string representation of a byte array that's more useful for debugging.
     * TODO: move this somewhere where it's available to all tests.
     */
    public static String toHexString(byte[] array) {
        if (array == null) {
            return null;
        }
        if (array.length == 0) {
            return "[]";
        }
        StringBuilder sb = new StringBuilder();
        sb.append('[');
        // TODO: don't use String.format if we put this in the library. Too expensive!
        sb.append(String.format("%02x", array[0] & 0xff));
        for (int i = 1; i < array.length; i++) {
            sb.append(", ");
            sb.append(String.format("%02x", array[i] & 0xff));
        }
        sb.append(']');
        return sb.toString();
    }
}
#endif


//==============================================================================

int mainDataOutputStreamTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/DataOutputStreamTest ====\n");
    test_writeBoolean();
    printf("\n==== end of libcore/math/DataOutputStreamTest ====\n");

    return 0;
}

}
}

