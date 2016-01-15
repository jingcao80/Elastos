#include <elautoptr.h>
#include <elastos/coredef.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/Arrays.h>

using namespace Elastos;
using Elastos::Core::StringUtils;
using Elastos::Core::Math;
using Elastos::Math::IBigInteger;
using Elastos::Math::CBigInteger;
using Elastos::Math::IBigDecimal;
using Elastos::Math::CBigDecimal;
using Elastos::Math::IBigDecimalHelper;
using Elastos::Math::CBigDecimalHelper;
using Elastos::Core::EIID_IComparable;


namespace Elastos {
namespace Math {

static void assertEquals(const char *info, Int32 aspect, Int32 test)
{
    printf("aspect: %d, test: %d. %s\n", aspect, test, info);
    assert(aspect == test);
}

static void assertEquals(const char *info, const String& aspect, const String& test)
{
    printf("aspect: [%s], test: [%s] %s\n", aspect.string(), test.string(), info);
    assert(aspect.Equals(test) && "result not equals aspect!");
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

package org.apache.harmony.tests.java.math;

import java.math.BigDecimal;
import java.math.MathContext;
import java.math.RoundingMode;

public class MathContextTest extends junit.framework.TestCase {
#endif

    /**
     * java.math.MathContext#MathContext(...)
     */
#if 0
    public void test_MathContextConstruction() {
        String a = "-12380945E+61";
        BigDecimal aNumber = new BigDecimal(a);
        MathContext mcIntRm6hd = new MathContext(6, RoundingMode.HALF_DOWN);
        MathContext mcStr6hd = new MathContext("precision=6 roundingMode=HALF_DOWN");
        MathContext mcInt6 = new MathContext(6);
        MathContext mcInt134 = new MathContext(134);

        // getPrecision()
        assertEquals("MathContext.getPrecision() returns incorrect value",
                6, mcIntRm6hd.getPrecision() );
        assertEquals("MathContext.getPrecision() returns incorrect value",
                134, mcInt134.getPrecision() );

        // getRoundingMode()
        assertEquals("MathContext.getRoundingMode() returns incorrect value",
                RoundingMode.HALF_UP,
                mcInt6.getRoundingMode());
        assertEquals("MathContext.getRoundingMode() returns incorrect value",
                RoundingMode.HALF_DOWN, mcIntRm6hd.getRoundingMode() );

        // toString()
        assertEquals("MathContext.toString() returning incorrect value",
                "precision=6 roundingMode=HALF_DOWN", mcIntRm6hd.toString() );
        assertEquals("MathContext.toString() returning incorrect value",
                "precision=6 roundingMode=HALF_UP", mcInt6.toString() );

        // equals(.)
        assertEquals("Equal MathContexts are not equal ",
                mcIntRm6hd, mcStr6hd );
        assertFalse("Different MathContexts are reported as equal ",
                mcInt6.equals(mcStr6hd) );
        assertFalse("Different MathContexts are reported as equal ",
                mcInt6.equals(mcInt134) );

        // hashCode(.)
        assertEquals("Equal MathContexts have different hashcodes ",
                mcIntRm6hd.hashCode(), mcStr6hd.hashCode() );
        assertFalse("Different MathContexts have equal hashcodes ",
                mcInt6.hashCode() == mcStr6hd.hashCode() );
        assertFalse("Different MathContexts have equal hashcodes ",
                mcInt6.hashCode() == mcInt134.hashCode() );

        // other:
        BigDecimal res = aNumber.abs(mcInt6);
        assertEquals("MathContext Constructor with int precision failed",
                new BigDecimal("1.23809E+68"),
                res);
    }
#endif
void test_MathContextConstruction()
{
        String a = String("-12380945E+61");
        AutoPtr<IBigDecimal> aNumber;

        ECode ec = CBigDecimal::New(a, (IBigDecimal**)&aNumber);
        if (FAILED(ec) || aNumber == NULL) {
            printf(" Failed to create CBigInteger. Error %08X\n", ec);
        }

        AutoPtr<IMathContext> mcIntRm6hd;
        ec = CMathContext::New(6, RoundingMode_HALF_DOWN, (IMathContext**)&mcIntRm6hd);
        if (FAILED(ec) || mcIntRm6hd == NULL) {
            printf(" Failed to create CMathContext. Error %08X\n", ec);
        }

/*        AutoPtr<IMathContext> mcStr6hd;
        ec = CMathContext::New(String("precision=6 roundingMode=HALF_DOWN"), (IMathContext**)&mcStr6hd);
        if (FAILED(ec) || mcStr6hd == NULL) {
            printf(" Failed to create CMathContext. Error %08X\n", ec);
        }
*/
        AutoPtr<IMathContext> mcInt6;
        ec = CMathContext::New(6, (IMathContext**)&mcInt6);
        if (FAILED(ec) || mcInt6 == NULL) {
            printf(" Failed to create CMathContext. Error %08X\n", ec);
        }

        AutoPtr<IMathContext> mcInt134;
        ec = CMathContext::New(134, (IMathContext**)&mcInt134);
        if (FAILED(ec) || mcInt134 == NULL) {
            printf(" Failed to create CMathContext. Error %08X\n", ec);
        }

        // getPrecision()
        Int32 r;
        mcIntRm6hd->GetPrecision(&r);
        assertEquals("MathContext.getPrecision() returns incorrect value", 6, r );
        mcInt134->GetPrecision(&r);
        assertEquals("MathContext.getPrecision() returns incorrect value", 134, r );

        // toString()
/*        String str;
        mcIntRm6hd->ToString(&str);
        assertEquals("MathContext.toString() returning incorrect value", String("precision=6 roundingMode=HALF_DOWN"), str);
        mcInt6->ToString(&str);
        assertEquals("MathContext.toString() returning incorrect value", String("precision=6 roundingMode=HALF_UP"), str);
*/
}

//==============================================================================

int mainMathContextTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/MathContextTest ====\n");
    printf("\n==== end of libcore/math/MathContextTest ====\n");

    return 0;
}

}
}
