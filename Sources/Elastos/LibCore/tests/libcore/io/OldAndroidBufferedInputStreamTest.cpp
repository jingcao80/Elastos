/*
 * ~/libcore/luni/src/test/java/libcore/java/io/OldAndroidBufferedInputStreamTest.java
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
namespace OldAndroidBufferedInputStreamTest {

static String read(IInputStream *a);
static String skipRead(IInputStream *a);
static String read(IInputStream *a, int x);


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

import java.io.BufferedInputStream;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import junit.framework.Assert;
import junit.framework.TestCase;

/**
 * Tests to verify that simple functionality works for BufferedInputStreams.
 */
public class OldAndroidBufferedInputStreamTest extends TestCase {

    public void testBufferedInputStream() throws Exception {
        String str = "AbCdEfGhIjKlM\nOpQrStUvWxYz";
        ByteArrayInputStream aa = new ByteArrayInputStream(str.getBytes());
        ByteArrayInputStream ba = new ByteArrayInputStream(str.getBytes());
        ByteArrayInputStream ca = new ByteArrayInputStream(str.getBytes());
        ByteArrayInputStream da = new ByteArrayInputStream(str.getBytes());
        ByteArrayInputStream ea = new ByteArrayInputStream(str.getBytes());

        BufferedInputStream a = new BufferedInputStream(aa, 6);
        try {
            Assert.assertEquals(str, read(a));
        } finally {
            a.close();
        }

        BufferedInputStream b = new BufferedInputStream(ba, 7);
        try {
            Assert.assertEquals("AbCdEfGhIj", read(b, 10));
        } finally {
            b.close();
        }

        BufferedInputStream c = new BufferedInputStream(ca, 9);
        try {
            assertEquals("bdfhjl\nprtvxz", skipRead(c));
        } finally {
            c.close();
        }

        BufferedInputStream d = new BufferedInputStream(da, 9);
        try {
            assertEquals('A', d.read());
            d.mark(15);
            assertEquals('b', d.read());
            assertEquals('C', d.read());
            d.reset();
            assertEquals('b', d.read());
        } finally {
            d.close();
        }

        BufferedInputStream e = new BufferedInputStream(ea, 11);
        try {
            // test that we can ask for more than is present, and that we'll get
            // back only what is there.
            assertEquals(str, read(e, 10000));
        } finally {
            e.close();
        }
    }
#endif
static void testBufferedInputStream()
{
    ECode ec;
    String str = String("AbCdEfGhIjKlM\nOpQrStUvWxYz");

    AutoPtr<ArrayOf<Byte> > aBytes;
    aBytes = str.GetBytes(0);

    AutoPtr<IByteArrayInputStream> aa;
    AutoPtr<IByteArrayInputStream> ba;
    AutoPtr<IByteArrayInputStream> ca;
    AutoPtr<IByteArrayInputStream> da;
    AutoPtr<IByteArrayInputStream> ea;

    ec = CByteArrayInputStream::New((ArrayOf<Byte> *)aBytes, (IByteArrayInputStream**)&aa);
    ec = CByteArrayInputStream::New((ArrayOf<Byte> *)aBytes, (IByteArrayInputStream**)&ba);
    ec = CByteArrayInputStream::New((ArrayOf<Byte> *)aBytes, (IByteArrayInputStream**)&ca);
    ec = CByteArrayInputStream::New((ArrayOf<Byte> *)aBytes, (IByteArrayInputStream**)&da);
    ec = CByteArrayInputStream::New((ArrayOf<Byte> *)aBytes, (IByteArrayInputStream**)&ea);

    AutoPtr<IBufferedInputStream> a;
    AutoPtr<IBufferedInputStream> b;
    AutoPtr<IBufferedInputStream> c;
    AutoPtr<IBufferedInputStream> d;
    AutoPtr<IBufferedInputStream> e;

    AutoPtr<IInputStream> ais;
    ais = IInputStream::Probe((IByteArrayInputStream *)aa);
    ec = CBufferedInputStream::New((IInputStream *)ais, 6, (IBufferedInputStream**)&a);
    ais = NULL;
    ais = IInputStream::Probe((IBufferedInputStream *)a);

    String str1;
    str1 = read(ais);
    assertEquals("testBufferedInputStream", str, str1);

    AutoPtr<ICloseable> ac;
    ac = ICloseable::Probe((IBufferedInputStream *)a);
    ac->Close();


    AutoPtr<IInputStream> bis;
    bis = IInputStream::Probe((IByteArrayInputStream *)ba);
    ec = CBufferedInputStream::New((IInputStream *)bis, 7, (IBufferedInputStream**)&b);
    bis = NULL;
    bis = IInputStream::Probe((IBufferedInputStream *)b);

    String str2;
    str2 = read(bis, 10);
    assertEquals("testBufferedInputStream", String("AbCdEfGhIj"), str2);

    AutoPtr<ICloseable> bc;
    bc = ICloseable::Probe((IBufferedInputStream *)b);
    bc->Close();


    AutoPtr<IInputStream> cis;
    cis = IInputStream::Probe((IByteArrayInputStream *)ca);
    ec = CBufferedInputStream::New((IInputStream *)cis, 7, (IBufferedInputStream**)&c);
    cis = NULL;
    cis = IInputStream::Probe((IBufferedInputStream *)c);

    String str3;
    str3 = skipRead(cis);
    assertEquals("testBufferedInputStream", String("bdfhjl\nprtvxz"), str3);

    AutoPtr<ICloseable> cc;
    cc = ICloseable::Probe((IBufferedInputStream *)c);
    cc->Close();


    AutoPtr<IInputStream> dis;
    dis = IInputStream::Probe((IByteArrayInputStream *)da);
    ec = CBufferedInputStream::New((IInputStream *)dis, 7, (IBufferedInputStream**)&d);
    dis = NULL;
    dis = IInputStream::Probe((IBufferedInputStream *)d);

    Int32 value;
    dis->Read(&value);
    assertEquals("testBufferedInputStream", 'A', value);
    dis->Mark(15);
    dis->Read(&value);
    assertEquals("testBufferedInputStream", 'b', value);
    dis->Read(&value);
    assertEquals("testBufferedInputStream", 'C', value);
    dis->Reset();
    dis->Read(&value);
    assertEquals("testBufferedInputStream", 'b', value);

    AutoPtr<ICloseable> dc;
    dc = ICloseable::Probe((IBufferedInputStream *)d);
    dc->Close();


    AutoPtr<IInputStream> eis;
    eis = IInputStream::Probe((IByteArrayInputStream *)ea);
    ec = CBufferedInputStream::New((IInputStream *)eis, 11, (IBufferedInputStream**)&e);
    eis = NULL;
    eis = IInputStream::Probe((IBufferedInputStream *)e);

    String str4;
    str4 = read(eis, 10000);
    assertEquals("testBufferedInputStream", str, str4);

    AutoPtr<ICloseable> ecm;
    ecm = ICloseable::Probe((IBufferedInputStream *)e);
    ecm->Close();
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

    public static String read(InputStream a, int x) throws IOException {
        byte[] b = new byte[x];
        int len = a.read(b, 0, x);
        if (len < 0) {
            return "";
        }
        return new String(b, 0, len);
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



//==============================================================================

int mainOldAndroidBufferedInputStreamTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/OldAndroidBufferedInputStreamTest ====\n");
    testBufferedInputStream();
    printf("\n==== end of libcore/math/OldAndroidBufferedInputStreamTest ====\n");

    return 0;
}

}
}
