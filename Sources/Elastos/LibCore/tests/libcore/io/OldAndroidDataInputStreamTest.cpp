/*
 * ~/libcore/luni/src/test/java/libcore/java/io/OldAndroidDataInputStreamTest.java
 */

#include <elautoptr.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::StringUtils;
using Elastos::Core::IAppendable;
using Elastos::Utility::Arrays;
using Elastos::Utility::Zip::CCRC32;
using Elastos::Utility::Zip::ICheckedInputStream;
using Elastos::Utility::Zip::CCheckedInputStream;
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
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IReader;
using Elastos::IO::ICharArrayReader;
using Elastos::IO::CCharArrayReader;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IDataInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IDataInput;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::IDataOutput;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::IDataOutput;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;

namespace Elastos {
namespace OldAndroidDataInputStreamTest {

static String read(IInputStream *a);
static String skipRead(IInputStream *a);
static String read(IInputStream *a, int x);
static String markRead(IInputStream *a, int x, int y);


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

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import junit.framework.Assert;
import junit.framework.TestCase;

public class OldAndroidDataInputStreamTest extends TestCase {

    public void testDataInputStream() throws Exception {
        String str = "AbCdEfGhIjKlM\nOpQ\rStUvWxYz";
        ByteArrayInputStream aa = new ByteArrayInputStream(str.getBytes());
        ByteArrayInputStream ba = new ByteArrayInputStream(str.getBytes());
        ByteArrayInputStream ca = new ByteArrayInputStream(str.getBytes());
        ByteArrayInputStream da = new ByteArrayInputStream(str.getBytes());

        DataInputStream a = new DataInputStream(aa);
        try {
            Assert.assertEquals(str, read(a));
        } finally {
            a.close();
        }

        DataInputStream b = new DataInputStream(ba);
        try {
            Assert.assertEquals("AbCdEfGhIj", read(b, 10));
        } finally {
            b.close();
        }

        DataInputStream c = new DataInputStream(ca);
        try {
            Assert.assertEquals("bdfhjl\np\rtvxz", skipRead(c));
        } finally {
            c.close();
        }

        DataInputStream d = new DataInputStream(da);
        try {
            assertEquals("AbCdEfGhIjKlM", d.readLine());
            assertEquals("OpQ", d.readLine());
            assertEquals("StUvWxYz", d.readLine());
        } finally {
            d.close();
        }

        ByteArrayOutputStream e = new ByteArrayOutputStream();
        DataOutputStream f = new DataOutputStream(e);
        try {
            f.writeBoolean(true);
            f.writeByte('a');
            f.writeBytes("BCD");
            f.writeChar('e');
            f.writeChars("FGH");
            f.writeUTF("ijklm");
            f.writeDouble(1);
            f.writeFloat(2);
            f.writeInt(3);
            f.writeLong(4);
            f.writeShort(5);
        } finally {
            f.close();
        }

        ByteArrayInputStream ga = new ByteArrayInputStream(e.toByteArray());
        DataInputStream g = new DataInputStream(ga);

        try {
            assertTrue(g.readBoolean());
            assertEquals('a', g.readByte());
            assertEquals(2, g.skipBytes(2));
            assertEquals('D', g.readByte());
            assertEquals('e', g.readChar());
            assertEquals('F', g.readChar());
            assertEquals('G', g.readChar());
            assertEquals('H', g.readChar());
            assertEquals("ijklm", g.readUTF());
            assertEquals(1, g.readDouble(), 0);
            assertEquals(2f, g.readFloat(), 0f);
            assertEquals(3, g.readInt());
            assertEquals(4, g.readLong());
            assertEquals(5, g.readShort());
        } finally {
            g.close();
        }
    }
#endif
static void testDataInputStream()
{
    ECode ec;
    String str("AbCdEfGhIjKlM\nOpQ\rStUvWxYz");

    AutoPtr<IByteArrayInputStream> aa;
    AutoPtr<IByteArrayInputStream> ba;
    AutoPtr<IByteArrayInputStream> ca;
    AutoPtr<IByteArrayInputStream> da;

    ec = CByteArrayInputStream::New(str.GetBytes(), (IByteArrayInputStream **)&aa);
    ec = CByteArrayInputStream::New(str.GetBytes(), (IByteArrayInputStream **)&ba);
    ec = CByteArrayInputStream::New(str.GetBytes(), (IByteArrayInputStream **)&ca);
    ec = CByteArrayInputStream::New(str.GetBytes(), (IByteArrayInputStream **)&da);

    AutoPtr<IDataInputStream> a;
    AutoPtr<IDataInputStream> b;
    AutoPtr<IDataInputStream> c;
    AutoPtr<IDataInputStream> d;

    AutoPtr<IInputStream> isa;
    AutoPtr<IInputStream> isb;
    AutoPtr<IInputStream> isc;
    AutoPtr<IInputStream> isd;

    AutoPtr<IInputStream> isa2;
    AutoPtr<IInputStream> isb2;
    AutoPtr<IInputStream> isc2;
    AutoPtr<IInputStream> isd2;

    isa = IInputStream::Probe(aa);
    isb = IInputStream::Probe(ba);
    isc = IInputStream::Probe(ca);
    isd = IInputStream::Probe(da);

    ec = CDataInputStream::New(isa, (IDataInputStream **)&a);
    isa2 = IInputStream::Probe(a);
    assertEquals("testDataInputStream", str, read(isa2));
    ICloseable::Probe(isa)->Close();

    ec = CDataInputStream::New(isb, (IDataInputStream **)&b);
    isb2 = IInputStream::Probe(b);
    assertEquals("testDataInputStream", str, read(isb2, 10));
    ICloseable::Probe(isb)->Close();

    ec = CDataInputStream::New(isc, (IDataInputStream **)&c);
    isc2 = IInputStream::Probe(c);
    assertEquals("testDataInputStream", str, skipRead(isc2));
    ICloseable::Probe(isc)->Close();

    ec = CDataInputStream::New(isd, (IDataInputStream **)&d);
    isd2 = IInputStream::Probe(d);
    AutoPtr<IDataInput> idi;
    idi = IDataInput::Probe(d);

    {
        String str2;
        idi->ReadLine(&str2);
        assertEquals("testDataInputStream", String("AbCdEfGhIjKlM"), str2);
    }


    {
        String str2;
        idi->ReadLine(&str2);
        assertEquals("testDataInputStream", String("OpQ"), str2);
    }

    {
        String str2;
        idi->ReadLine(&str2);
        assertEquals("testDataInputStream", String("StUvWxYz"), str2);
    }
    ICloseable::Probe(isd)->Close();


    AutoPtr<IByteArrayOutputStream> e;
    AutoPtr<IOutputStream> os;
    AutoPtr<IDataOutputStream> f;
    AutoPtr<IDataOutput> ido;

    ec = CByteArrayOutputStream::New((IByteArrayOutputStream **)&e);
    os = IOutputStream::Probe(e);
    ec = CDataOutputStream::New(os, (IDataOutputStream **)&f);
    ido = IDataOutput::Probe(f);
    ido->WriteBoolean(true);
    ido->WriteByte('a');
    ido->WriteBytes(String("BCD"));
    ido->WriteChar('e');
    ido->WriteChars(String("FGH"));
    ido->WriteUTF(String("ijklm"));
    ido->WriteDouble(1);
    ido->WriteFloat(2);
    ido->WriteInt32(3);
    ido->WriteInt64(4);
    ido->WriteInt16(5);
    ICloseable::Probe(ido)->Close();

    AutoPtr<IByteArrayInputStream> ga;
    AutoPtr<ArrayOf<Byte> > bytes;
    AutoPtr<IDataInputStream> g;
    AutoPtr<IDataInput> di;
    AutoPtr<IInputStream> is;
    e->ToByteArray((ArrayOf<Byte> **)&bytes);

    is = IInputStream::Probe(ga);
    ec = CByteArrayInputStream::New(bytes, (IByteArrayInputStream **)&ga);
    ec = CDataInputStream::New(is, (IDataInputStream **)&g);

    di = IDataInput::Probe(g);

    Boolean value;
    Byte byte;
    Char32 c32;
    String str3;
    Double dd;
    Float ff;
    Int16 i16;
    Int32 i32;
    Int64 i64;
    di->ReadBoolean(&value);
    assertEquals("testDataInputStream", 1, value);

    di->ReadByte(&byte);
    assertEquals("testDataInputStream", 'a', byte);

    di->SkipBytes(2, &i32);
    assertEquals("testDataInputStream", 2, i32);

    di->ReadByte(&byte);
    assertEquals("testDataInputStream", 'D', byte);

    di->ReadChar(&c32);
    assertEquals("testDataInputStream", 'e', c32);

    di->ReadChar(&c32);
    assertEquals("testDataInputStream", 'F', c32);

    di->ReadChar(&c32);
    assertEquals("testDataInputStream", 'G', c32);

    di->ReadChar(&c32);
    assertEquals("testDataInputStream", 'H', c32);

    di->ReadUTF(&str3);
    assertEquals("testDataInputStream", String("ijklm"), str3);

    di->ReadDouble(&dd);

    di->ReadFloat(&ff);

    di->ReadInt32(&i32);
    di->ReadInt64(&i64);
    di->ReadInt16(&i16);

    ICloseable::Probe(g)->Close();
/*
        ByteArrayInputStream ga = new ByteArrayInputStream(e.toByteArray());
        DataInputStream g = new DataInputStream(ga);

        try {
            assertTrue(g.readBoolean());
            assertEquals('a', g.readByte());
            assertEquals(2, g.skipBytes(2));
            assertEquals('D', g.readByte());
            assertEquals('e', g.readChar());
            assertEquals('F', g.readChar());
            assertEquals('G', g.readChar());
            assertEquals('H', g.readChar());
            assertEquals("ijklm", g.readUTF());
            assertEquals(1, g.readDouble(), 0);
            assertEquals(2f, g.readFloat(), 0f);
            assertEquals(3, g.readInt());
            assertEquals(4, g.readLong());
            assertEquals(5, g.readShort());
        } finally {
            g.close();
        }
*/
}

#if 0
    public static String read(InputStream a) throws IOException {
        int r;
        StringBuilder builder = new StringBuilder();
        do {
            r = a.read();
            if (r != -1)
                builder.append((char) r);
        } while (r != -1);
        return builder.toString();
    }

    public static String read(InputStream a, int x) throws IOException {
        byte[] b = new byte[x];
        int len = a.read(b, 0, x);
        if (len < 0) {
            return "";
        }
        return new String(b, 0, len);
    }

    public static String skipRead(InputStream a) throws IOException {
        int r;
        StringBuilder builder = new StringBuilder();
        do {
            a.skip(1);
            r = a.read();
            if (r != -1)
                builder.append((char) r);
        } while (r != -1);
        return builder.toString();
    }

    public static String markRead(InputStream a, int x, int y) throws IOException {
        int m = 0;
        int r;
        StringBuilder builder = new StringBuilder();
        do {
            m++;
            r = a.read();
            if (m == x)
                a.mark((x + y));
            if (m == (x + y))
                a.reset();

            if (r != -1)
                builder.append((char) r);
        } while (r != -1);
        return builder.toString();
    }
}
#endif
static String read(IInputStream *a)
{
    int r;
    StringBuilder builder;

    do {
        a->Read(&r);
        if (r != -1)
            builder.AppendChar((char)r);
    } while (r != -1);
    return builder.ToString();
}

static String skipRead(IInputStream *a)
{
    int r;
    StringBuilder builder;
    Int64 number;

    do {
        a->Skip((Int64)1, &number);
        a->Read(&r);
        if (r != -1)
            builder.AppendChar((char)r);
    } while (r != -1);
    return builder.ToString();
}

static String read(IInputStream *a, int x)
{
    AutoPtr<ArrayOf<Byte> > b = ArrayOf<Byte>::Alloc(x);

    int len;

    a->Read((ArrayOf<Byte> *)b, 0, x, &len);
    if (len < 0) {
        return String("");
    }
    String str = String((const char *)b->GetPayload(), len);
    return str;
}

static String markRead(IInputStream *a, int x, int y)
{
    int m = 0;
    int r;
    StringBuilder builder;
    do {
        m++;
        a->Read(&r);
        if (m == x)
            a->Mark((x + y));
        if (m == (x + y))
            a->Reset();

        if (r != -1)
            builder.AppendChar((char)r);
    } while (r != -1);
    return builder.ToString();
}

//==============================================================================

int mainOldAndroidDataInputStreamTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/OldAndroidDataInputStreamTest ====\n");
    testDataInputStream();
    printf("\n==== end of libcore/math/OldAndroidDataInputStreamTest ====\n");

    return 0;
}

}
}
