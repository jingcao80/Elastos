/*
 * ~/libcore/luni/src/test/java/libcore/java/io/OldAndroidByteArrayInputStreamTest.java
 */

#include <stdarg.h>

#include <elautoptr.h>
#include <elastos/coredef.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/Arrays.h>

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
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;

namespace Elastos {
namespace OldAndroidByteArrayInputStreamTest {


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
import java.io.IOException;
import java.io.InputStream;
import junit.framework.Assert;
import junit.framework.TestCase;

/**
 * Tests to verify that simple functionality works for ByteArrayInputStreams.
 */
public class OldAndroidByteArrayInputStreamTest extends TestCase {

    public void testByteArrayInputStream() throws Exception {
        String str = "AbCdEfGhIjKlMnOpQrStUvWxYz";

        ByteArrayInputStream a = new ByteArrayInputStream(str.getBytes());
        ByteArrayInputStream b = new ByteArrayInputStream(str.getBytes());
        ByteArrayInputStream c = new ByteArrayInputStream(str.getBytes());
        ByteArrayInputStream d = new ByteArrayInputStream(str.getBytes());

        Assert.assertEquals(str, read(a));
        Assert.assertEquals("AbCdEfGhIj", read(b, 10));
        Assert.assertEquals("bdfhjlnprtvxz", skipRead(c));
        Assert.assertEquals("AbCdEfGdEfGhIjKlMnOpQrStUvWxYz", markRead(d, 3, 4));
    }
#endif
void testByteArrayInputStream()
{
    ECode ec;
    String str = String("AbCdEfGhIjKlMnOpQrStUvWxYz");

    AutoPtr<IByteArrayInputStream> a;
    AutoPtr<IByteArrayInputStream> b;
    AutoPtr<IByteArrayInputStream> c;
    AutoPtr<IByteArrayInputStream> d;

    ec = CByteArrayInputStream::New((ArrayOf<Byte> *)str.GetBytes(), (IByteArrayInputStream**)&a);
    ec = CByteArrayInputStream::New((ArrayOf<Byte> *)str.GetBytes(), (IByteArrayInputStream**)&b);
    ec = CByteArrayInputStream::New((ArrayOf<Byte> *)str.GetBytes(), (IByteArrayInputStream**)&c);
    ec = CByteArrayInputStream::New((ArrayOf<Byte> *)str.GetBytes(), (IByteArrayInputStream**)&d);

    AutoPtr<IInputStream> ia;
    AutoPtr<IInputStream> ib;
    AutoPtr<IInputStream> ic;
    AutoPtr<IInputStream> id;

    ia = IInputStream::Probe((IByteArrayInputStream *)a);
    ib = IInputStream::Probe((IByteArrayInputStream *)a);
    ic = IInputStream::Probe((IByteArrayInputStream *)a);
    id = IInputStream::Probe((IByteArrayInputStream *)a);

    assertEquals("", str, read(ia));
    assertEquals("", String("AbCdEfGhIj"), read(ib, 10));
    assertEquals("", String("bdfhjlnprtvxz"), skipRead(ic));
    assertEquals("", String("AbCdEfGdEfGhIjKlMnOpQrStUvWxYz"), markRead(id, 3, 4));
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

int mainOldAndroidByteArrayInputStreamTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/OldAndroidByteArrayInputStreamTest ====\n");
    testByteArrayInputStream();
    printf("\n==== end of libcore/math/OldAndroidByteArrayInputStreamTest ====\n");

    return 0;
}

}
}
