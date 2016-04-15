/*
 * ~/libcore/luni/src/test/java/libcore/java/io/FilterInputStreamNullSourceTest.java
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


static void assertReadsFailWithIoException(IInputStream *in);
static void assertReadsFailWithNullPointerException(IInputStream *in);

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

import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.FilterInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.LineNumberInputStream;
import java.io.PushbackInputStream;
import java.security.DigestInputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.zip.CRC32;
import java.util.zip.CheckedInputStream;
import java.util.zip.Inflater;
import java.util.zip.InflaterInputStream;
import javax.crypto.CipherInputStream;
import javax.crypto.NullCipher;
import junit.framework.TestCase;

public final class FilterInputStreamNullSourceTest extends TestCase {
#endif

#if 0
    public void testBufferedInputStream() throws IOException {
        assertReadsFailWithIoException(new BufferedInputStream(null));
        assertReadsFailWithIoException(new BufferedInputStream(null, 1024));
    }
#endif
static void testBufferedInputStream()
{
    ECode ec;
    AutoPtr<IBufferedInputStream> bin;
    AutoPtr<IInputStream> in;

    ec = CBufferedInputStream::New((IInputStream *)NULL, (IBufferedInputStream**)&bin);
    in = IInputStream::Probe(bin);
    assertReadsFailWithIoException(in);

    bin = NULL;
    in = NULL;
    ec = CBufferedInputStream::New((IInputStream *)NULL, 1024, (IBufferedInputStream**)&bin);
    in = IInputStream::Probe(bin);
    assertReadsFailWithIoException(in);
}

#if 0
    public void testCheckedInputStream() throws IOException {
        assertReadsFailWithNullPointerException(new CheckedInputStream(null, new CRC32()));
    }
#endif
static void testCheckedInputStream()
{
    ECode ec;
    AutoPtr<ICheckedInputStream> cin;
    AutoPtr<IInputStream> in;

    AutoPtr<IChecksum> crc;
    ec = CCRC32::New((IChecksum**)&crc);

    ec = CCheckedInputStream::New((IInputStream *)NULL, crc, (ICheckedInputStream**)&cin);
    in = IInputStream::Probe(cin);
    assertReadsFailWithNullPointerException(in);
}

#if 0
    public void testCipherInputStream() throws IOException {
        InputStream in = new CipherInputStream(null, new NullCipher());
        try {
            in.read();
            fail();
        } catch (NullPointerException expected) {
        }

        assertEquals(0, in.available());

        try {
            in.close();
            fail();
        } catch (NullPointerException expected) {
        }
    }
#endif
static void testCipherInputStream()
{
    ECode ec;
    AutoPtr<INullCipher> nc;
    AutoPtr<ICipherInputStream> cis;
    AutoPtr<IInputStream> in;
    AutoPtr<ICipher> c;

    ec = CNullCipher::New((INullCipher**)&nc);
    c = ICipher::Probe(nc);
    ec = CCipherInputStream::New((IInputStream *)NULL, (ICipher *)c, (ICipherInputStream**)&cis);

    in = IInputStream::Probe(cis);

    Int32 value;
    Int32 number;

    ec = in->Read(&value);
    if (ec == (ECode)E_NULL_POINTER_EXCEPTION) {
        printf("testCipherInputStream, catched\n");
    } else {
        printf("testCipherInputStream, UNCATCHED\n");
    }

    ec = in->Available(&number);
    if (ec == (ECode)E_NULL_POINTER_EXCEPTION) {
        printf("testCipherInputStream, catched\n");
    } else {
        printf("testCipherInputStream, UNCATCHED\n");
    }
    assertEquals("testCipherInputStream, catched", 0, number);

    ec = in->Close();
    if (ec == (ECode)E_NULL_POINTER_EXCEPTION) {
        printf("testCipherInputStream, catched\n");
    } else {
        printf("testCipherInputStream, UNCATCHED\n");
    }
}

#if 0
    public void testDataInputStream() throws IOException {
        assertReadsFailWithNullPointerException(new DataInputStream(null));
    }
#endif
static void testDataInputStream()
{
    ECode ec;
    AutoPtr<IDataInputStream> dis;
    AutoPtr<IInputStream> in;

    ec = CDataInputStream::New(NULL, (IDataInputStream**)&dis);
    in = IInputStream::Probe(dis);

    assertReadsFailWithNullPointerException(in);
}


#if 0
    public void testDigestInputStream() throws IOException, NoSuchAlgorithmException {
        MessageDigest md5 = MessageDigest.getInstance("MD5");
        assertReadsFailWithNullPointerException(new DigestInputStream(null, md5));
    }
#endif
static void testDigestInputStream()
{
/*
    AutoPtr<IMessageDigestHelper> helper;
    // CMessageDigestHelper::AcquireSingleton((IMessageDigestHelper**)&helper);
    AutoPtr<IMessageDigest> md;
    AutoPtr<IDigestInputStream> dis;
    helper->GetInstance(String("MD5"), (IMessageDigest**)&md);

    MessageDigest md5 = MessageDigest.getInstance("MD5");
    assertReadsFailWithNullPointerException((IDigestInputStream *)dis);
*/
}

#if 0
    public void testFilterInputStream() throws IOException {
        assertReadsFailWithNullPointerException(new FilterInputStream(null) {});
    }
#endif
static void testFilterInputStream()
{
    ECode ec;
    AutoPtr<IFilterInputStream> fis;
    AutoPtr<IInputStream> in;

    ec = CFilterInputStream::New((IInputStream *)NULL, (IFilterInputStream**)&fis);
    in = IInputStream::Probe(fis);

    assertReadsFailWithNullPointerException(in);
}

#if 0
    public void testInflaterInputStream() throws IOException {
        try {
            new InflaterInputStream(null);
            fail();
        } catch (NullPointerException expected) {
        }
        try {
            new InflaterInputStream(null, new Inflater());
            fail();
        } catch (NullPointerException expected) {
        }
        try {
            new InflaterInputStream(null, new Inflater(), 1024);
            fail();
        } catch (NullPointerException expected) {
        }
    }
#endif
static void testInflaterInputStream()
{
    ECode ec;
    AutoPtr<IInflaterInputStream> iis;
    AutoPtr<IInflater> inflater;

    ec = CInflaterInputStream::New((IInputStream *)NULL, (IInflaterInputStream**)&iis);
    if (ec == (ECode)E_NULL_POINTER_EXCEPTION) {
        printf("testInflaterInputStream, catched\n");
    } else {
        printf("testInflaterInputStream, UNCATCHED\n");
    }

    ec = CInflater::New((IInflater**)&inflater);
    ec = CInflaterInputStream::New((IInputStream *)NULL, inflater, (IInflaterInputStream**)&iis);
    if (ec == (ECode)E_NULL_POINTER_EXCEPTION) {
        printf("testInflaterInputStream, catched\n");
    } else {
        printf("testInflaterInputStream, UNCATCHED\n");
    }

    ec = CInflaterInputStream::New((IInputStream *)NULL, inflater, 1024, (IInflaterInputStream**)&iis);
    if (ec == (ECode)E_NULL_POINTER_EXCEPTION) {
        printf("testInflaterInputStream, catched\n");
    } else {
        printf("testInflaterInputStream, UNCATCHED\n");
    }
}

#if 0
    public void testLineNumberInputStream() throws IOException {
        assertReadsFailWithNullPointerException(new LineNumberInputStream(null));
    }
#endif
static void testLineNumberInputStream()
{
    ECode ec;
    AutoPtr<ILineNumberInputStream> lis;
    AutoPtr<IInputStream> in;

    ec = CLineNumberInputStream::New((IInputStream *)NULL, (ILineNumberInputStream**)&lis);

    in = IInputStream::Probe(lis);

    assertReadsFailWithNullPointerException(in);
}

#if 0
    public void testPushbackInputStream() throws IOException {
        assertReadsFailWithIoException(new PushbackInputStream(null));
        assertReadsFailWithIoException(new PushbackInputStream(null, 1024));
    }
#endif
static void testPushbackInputStream()
{
    ECode ec;
    AutoPtr<IPushbackInputStream> pis;
    AutoPtr<IInputStream> in;

    ec = CPushbackInputStream::New((IInputStream *)NULL, (IPushbackInputStream**)&pis);
    in = IInputStream::Probe(pis);
    assertReadsFailWithIoException(in);

    pis = NULL;
    in = NULL;
    ec = CPushbackInputStream::New((IInputStream *)NULL, 1024, (IPushbackInputStream**)&pis);
    in = IInputStream::Probe(pis);
    assertReadsFailWithIoException(in);
}

#if 0
    private void assertReadsFailWithIoException(InputStream in) throws IOException {
        try {
            in.read();
            fail();
        } catch (IOException expected) {
        }

        try {
            in.available();
            fail();
        } catch (IOException expected) {
        }

        in.close();
    }

    private void assertReadsFailWithNullPointerException(InputStream in) throws IOException {
        try {
            in.read();
            fail();
        } catch (NullPointerException expected) {
        }

        try {
            in.available();
            fail();
        } catch (NullPointerException expected) {
        }

        try {
            in.close();
            fail();
        } catch (NullPointerException expected) {
        }
    }
}
#endif
static void assertReadsFailWithIoException(IInputStream *in)
{
    ECode ec;
    Int32 value;
    Int32 number;

    ec = in->Read(&value);
    if (ec == (ECode)E_IO_EXCEPTION) {
        printf("assertReadsFailWithIoException, catched\n");
    } else {
        printf("assertReadsFailWithIoException, UNCATCHED\n");
    }

    ec = in->Available(&number);
    if (ec == (ECode)E_IO_EXCEPTION) {
        printf("assertReadsFailWithIoException, catched\n");
    } else {
        printf("assertReadsFailWithIoException, UNCATCHED\n");
    }

    in->Close();
}

static void assertReadsFailWithNullPointerException(IInputStream *in)
{
    ECode ec;
    Int32 value;
    Int32 number;

    ec = in->Read(&value);
    if (ec == (ECode)E_NULL_POINTER_EXCEPTION) {
        printf("assertReadsFailWithNullPointerException, catched\n");
    } else {
        printf("assertReadsFailWithNullPointerException, UNCATCHED\n");
    }

    ec = in->Available(&number);
    if (ec == (ECode)E_NULL_POINTER_EXCEPTION) {
        printf("assertReadsFailWithNullPointerException, catched\n");
    } else {
        printf("assertReadsFailWithNullPointerException, UNCATCHED\n");
    }

    in->Close();
}




//==============================================================================

int mainFilterInputStreamNullSourceTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/FilterInputStreamNullSourceTest ====\n");
    testBufferedInputStream();
    testCheckedInputStream();
    testCipherInputStream();
    testDataInputStream();
    testDigestInputStream();
    testInflaterInputStream();
    testLineNumberInputStream();
    testPushbackInputStream();
    printf("\n==== end of libcore/math/FilterInputStreamNullSourceTest ====\n");

    return 0;
}

}
}
