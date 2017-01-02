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
 * ~/libcore/luni/src/test/java/libcore/java/io/OldAndroidBufferedReaderTest.java
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
using Elastos::IO::IObjectOutputStream;

namespace Elastos {
namespace OldAndroidBufferedReaderTest {


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

import java.io.BufferedReader;
import java.io.IOException;
import java.io.Reader;
import java.io.StringReader;
import junit.framework.TestCase;

/**
 * Tests to verify that simple functionality works for BufferedReaders.
 */
public class OldAndroidBufferedReaderTest extends TestCase {

    public void testBufferedReader() throws Exception {
        String str = "AbCdEfGhIjKlMnOpQrStUvWxYz";
        StringReader aa = new StringReader(str);
        StringReader ba = new StringReader(str);
        StringReader ca = new StringReader(str);
        StringReader da = new StringReader(str);

        BufferedReader a = new BufferedReader(aa, 5);
        try {
            assertEquals(str, read(a));
        } finally {
            a.close();
        }

        BufferedReader b = new BufferedReader(ba, 15);
        try {
            assertEquals("AbCdEfGhIj", read(b, 10));
        } finally {
            b.close();
        }

        BufferedReader c = new BufferedReader(ca);
        try {
            assertEquals("bdfhjlnprtvxz", skipRead(c));
        } finally {
            c.close();
        }

        BufferedReader d = new BufferedReader(da);
        try {
            assertEquals("AbCdEfGdEfGhIjKlMnOpQrStUvWxYz", markRead(d, 3, 4));
        } finally {
            d.close();
        }
    }
#endif
static void testBufferedReader()
{
    ECode ec;
    String str = String("AbCdEfGhIjKlMnOpQrStUvWxYz");
    AutoPtr<IStringReader> aa;
    AutoPtr<IStringReader> ba;
    AutoPtr<IStringReader> ca;
    AutoPtr<IStringReader> da;


    ec = CStringReader::New(str, (IStringReader**)&aa);
    ec = CStringReader::New(str, (IStringReader**)&ba);
    ec = CStringReader::New(str, (IStringReader**)&ca);
    ec = CStringReader::New(str, (IStringReader**)&da);

    {
        AutoPtr<IReader> ir;
        AutoPtr<IBufferedReader> iba;
        AutoPtr<ICloseable> ac;

        ir = IReader::Probe((IStringReader *)aa);
        ac = ICloseable::Probe((IStringReader *)aa);
//        ec = CBufferedReader::New((IReader *)ir, 5, (IBufferedReader**)&iba);

        ir = IReader::Probe((IBufferedReader *)iba);
        assertEquals("", str, read((IReader *)ir));

        ac->Close();
    }

    {
        AutoPtr<IReader> ir;
        AutoPtr<IBufferedReader> iba;
        AutoPtr<ICloseable> ac;

        ir = IReader::Probe((IStringReader *)aa);
        ac = ICloseable::Probe((IStringReader *)aa);
//        ec = CBufferedReader::New((IReader *)ir, 15, (IBufferedReader**)&iba);

        ir = IReader::Probe((IBufferedReader *)iba);
        assertEquals("", String("AbCdEfGhIj"), read((IReader *)ir));

        ac->Close();

    }

    {
        AutoPtr<IReader> ir;
        AutoPtr<IBufferedReader> iba;
        AutoPtr<ICloseable> ac;

        ir = IReader::Probe((IStringReader *)aa);
        ac = ICloseable::Probe((IStringReader *)aa);
//        ec = CBufferedReader::New((IReader *)ir, 15, (IBufferedReader**)&iba);

        ir = IReader::Probe((IBufferedReader *)iba);
        assertEquals("", String("AbCdEfGhIj"), read((IReader *)ir, 10));

        ac->Close();

    }

    {
        AutoPtr<IReader> ir;
        AutoPtr<IBufferedReader> iba;
        AutoPtr<ICloseable> ac;

        ir = IReader::Probe((IStringReader *)aa);
        ac = ICloseable::Probe((IStringReader *)aa);
//        ec = CBufferedReader::New((IReader *)ir, 15, (IBufferedReader**)&iba);

        ir = IReader::Probe((IBufferedReader *)iba);
        assertEquals("", String("bdfhjlnprtvxz"), skipRead((IReader *)ir));

        ac->Close();

    }

    {
        AutoPtr<IReader> ir;
        AutoPtr<IBufferedReader> iba;
        AutoPtr<ICloseable> ac;

        ir = IReader::Probe((IStringReader *)aa);
        ac = ICloseable::Probe((IStringReader *)aa);
//        ec = CBufferedReader::New((IReader *)ir, 15, (IBufferedReader**)&iba);

        ir = IReader::Probe((IBufferedReader *)iba);
        assertEquals("", String("AbCdEfGdEfGhIjKlMnOpQrStUvWxYz"), markRead((IReader *)ir, 3, 4));

        ac->Close();

    }
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

int mainOldAndroidBufferedReaderTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/OldAndroidBufferedReaderTest ====\n");
    testBufferedReader();
    printf("\n==== end of libcore/math/OldAndroidBufferedReaderTest ====\n");

    return 0;
}

}
}
