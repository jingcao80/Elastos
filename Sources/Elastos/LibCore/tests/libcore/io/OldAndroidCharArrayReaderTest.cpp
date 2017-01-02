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
 * ~/libcore/luni/src/test/java/libcore/java/io/OldAndroidCharArrayReaderTest.java
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

namespace Elastos {
namespace OldAndroidCharArrayReaderTest {


static String read(IReader *a);
static String skipRead(IReader *a);
static String read(IReader *a, int x);
static String markRead(IReader *a, int x, int y);


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

import java.io.CharArrayReader;
import java.io.IOException;
import java.io.Reader;
import junit.framework.Assert;
import junit.framework.TestCase;

/**
 * Basic tests for CharArrayReader.
 */
public class OldAndroidCharArrayReaderTest extends TestCase {

    public void testCharArrayReader() throws Exception {
        String str = "AbCdEfGhIjKlMnOpQrStUvWxYz";
        CharArrayReader a = new CharArrayReader(str.toCharArray());
        CharArrayReader b = new CharArrayReader(str.toCharArray());
        CharArrayReader c = new CharArrayReader(str.toCharArray());
        CharArrayReader d = new CharArrayReader(str.toCharArray());

        Assert.assertEquals(str, read(a));
        Assert.assertEquals("AbCdEfGhIj", read(b, 10));
        Assert.assertEquals("bdfhjlnprtvxz", skipRead(c));
        Assert.assertEquals("AbCdEfGdEfGhIjKlMnOpQrStUvWxYz", markRead(d, 3, 4));
    }
#endif
void testCharArrayReader()
{
    ECode ec;
    String str("AbCdEfGhIjKlMnOpQrStUvWxYz");
    AutoPtr<ICharArrayReader> a;
    AutoPtr<ICharArrayReader> b;
    AutoPtr<ICharArrayReader> c;
    AutoPtr<ICharArrayReader> d;

    ec = CCharArrayReader::New(str.GetChars(), (ICharArrayReader**)&a);
    ec = CCharArrayReader::New(str.GetChars(), (ICharArrayReader**)&b);
    ec = CCharArrayReader::New(str.GetChars(), (ICharArrayReader**)&c);
    ec = CCharArrayReader::New(str.GetChars(), (ICharArrayReader**)&d);



    assertEquals("testCharArrayReader", str, read(IReader::Probe(a)));
    assertEquals("testCharArrayReader", String("AbCdEfGhIj"), read(IReader::Probe(b), 10));
    assertEquals("testCharArrayReader", String("bdfhjlnprtvxz"), skipRead(IReader::Probe(c)));
    assertEquals("testCharArrayReader", String("AbCdEfGdEfGhIjKlMnOpQrStUvWxYz"), markRead(IReader::Probe(d), 3, 4));
}

#if 0
    public static String read(Reader a) throws IOException {
        int r;
        StringBuilder builder = new StringBuilder();
        do {
            r = a.read();
            if (r != -1)
                builder.append((char) r);
        } while (r != -1);
        return builder.toString();
    }

    public static String read(Reader a, int x) throws IOException {
        char[] b = new char[x];
        int len = a.read(b, 0, x);
        if (len < 0) {
            return "";
        }
        return new String(b, 0, len);
    }

    public static String skipRead(Reader a) throws IOException {
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

    public static String markRead(Reader a, int x, int y) throws IOException {
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

static String read(IReader *a)
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

static String skipRead(IReader *a)
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

static String read(IReader *a, int x)
{
    AutoPtr<ArrayOf<Char32> > b = ArrayOf<Char32>::Alloc(x);

    int len;

    a->Read((ArrayOf<Char32> *)b, 0, x, &len);
    if (len < 0) {
        return String("");
    }
    String str = String(*(ArrayOf<Char32> *)b, 0, len);
    return str;
}

static String markRead(IReader *a, int x, int y)
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

int mainOldAndroidCharArrayReaderTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/OldAndroidCharArrayReaderTest ====\n");
    testCharArrayReader();
    printf("\n==== end of libcore/math/OldAndroidCharArrayReaderTest ====\n");

    return 0;
}

}
}
