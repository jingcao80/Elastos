/*
 * ~/libcore/luni/src/test/java/libcore/java/io/ObjectOutputStreamTest.java
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
namespace ObjectOutputStreamTest {

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
import java.io.ObjectOutputStream;
import junit.framework.TestCase;

public final class ObjectOutputStreamTest extends TestCase {
    public void testLongString() throws Exception {
        // Most modified UTF-8 is limited to 64KiB, but serialized strings can have an 8-byte
        // length, so this should never throw java.io.UTFDataFormatException...
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < 64*1024 * 2; ++i) {
            sb.append('a');
        }
        String s = sb.toString();
        ObjectOutputStream os = new ObjectOutputStream(new ByteArrayOutputStream());
        os.writeObject(s);
    }
}
#endif
static void testLongString()
{
    ECode ec;

    // Most modified UTF-8 is limited to 64KiB, but serialized strings can have an 8-byte
    // length, so this should never throw java.io.UTFDataFormatException...
    StringBuilder sb;

    for (int i = 0; i < 64*1024 * 2; ++i) {
        sb.AppendChar((Char32)'[');
    }
    String s = sb.ToString();

    AutoPtr<IByteArrayOutputStream> baos;
    AutoPtr<IObjectOutputStream> oos;
    AutoPtr<IObjectOutput> os;

    ec = CByteArrayOutputStream::New((IByteArrayOutputStream**)&baos);
//    ec = CObjectOutputStream::New((IObjectOutputStream**)&oos);

    os = IObjectOutput::Probe(oos);

    AutoPtr<ICharSequence> seq;
    AutoPtr<IInterface> pif;
    CString::New(s, (ICharSequence**)&seq);
    pif = seq->Probe(EIID_IInterface);

    os->WriteObject((IInterface *)pif);
}


//==============================================================================

int mainObjectOutputStreamTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/ObjectOutputStreamTest ====\n");
    testLongString();
    printf("\n==== end of libcore/math/ObjectOutputStreamTest ====\n");

    return 0;
}

}
}
