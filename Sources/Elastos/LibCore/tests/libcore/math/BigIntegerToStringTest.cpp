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
/**
 * @author Elena Semukhina
 */

package org.apache.harmony.tests.java.math;

import java.math.BigInteger;

import junit.framework.TestCase;

/**
 * Class:   java.math.BigInteger
 * Method: toString(int radix)
 */
public class BigIntegerToStringTest extends TestCase {
#endif

    /**
     * If 36 < radix < 2 it should be set to 10
     */
#if 0
    public void testRadixOutOfRange() {
        String value = "442429234853876401";
        int radix = 10;
        BigInteger aNumber = new BigInteger(value, radix);
        String result = aNumber.toString(45);
        assertTrue(result.equals(value));
    }
#endif
void testRadixOutOfRange()
{
    String value = String("442429234853876401");
    Int32  radix = 10;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(value, radix, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    String result;
    aNumber->ToString(10, &result);

    if (result.Equals(value)) {
        printf("corrent testRadixOutOfRange\n");
    } else {
        printf("fail testRadixOutOfRange\n");
    }

}


    /**
     * test negative number of radix 2
     */
#if 0
    public void testRadix2Neg() {
        String value = "-101001100010010001001010101110000101010110001010010101010101010101010101010101010101010101010010101";
        int radix = 2;
        BigInteger aNumber = new BigInteger(value, radix);
        String result = aNumber.toString(radix);
        assertTrue(result.equals(value));
    }
#endif


    /**
     * test positive number of radix 2
     */
#if 0
    public void testRadix2Pos() {
        String value = "101000011111000000110101010101010101010001001010101010101010010101010101010000100010010";
        int radix = 2;
        BigInteger aNumber = new BigInteger(value, radix);
        String result = aNumber.toString(radix);
        assertTrue(result.equals(value));
    }
#endif


    /**
     * test negative number of radix 10
     */
#if 0
    public void testRadix10Neg() {
        String value = "-2489756308572364789878394872984";
        int radix = 16;
        BigInteger aNumber = new BigInteger(value, radix);
        String result = aNumber.toString(radix);
        assertTrue(result.equals(value));
    }
#endif


    /**
     * test positive number of radix 10
     */
#if 0
    public void testRadix10Pos() {
        String value = "2387627892347567398736473476";
        int radix = 16;
        BigInteger aNumber = new BigInteger(value, radix);
        String result = aNumber.toString(radix);
        assertTrue(result.equals(value));
    }
#endif


    /**
     * test negative number of radix 16
     */
#if 0
    public void testRadix16Neg() {
        String value = "-287628a883451b800865c67e8d7ff20";
        int radix = 16;
        BigInteger aNumber = new BigInteger(value, radix);
        String result = aNumber.toString(radix);
        assertTrue(result.equals(value));
    }
#endif


    /**
     * test positive number of radix 16
     */
#if 0
    public void testRadix16Pos() {
        String value = "287628a883451b800865c67e8d7ff20";
        int radix = 16;
        BigInteger aNumber = new BigInteger(value, radix);
        String result = aNumber.toString(radix);
        assertTrue(result.equals(value));
    }
#endif


    /**
     * test negative number of radix 24
     */
#if 0
    public void testRadix24Neg() {
        String value = "-287628a88gmn3451b8ijk00865c67e8d7ff20";
        int radix = 24;
        BigInteger aNumber = new BigInteger(value, radix);
        String result = aNumber.toString(radix);
        assertTrue(result.equals(value));
    }
#endif


    /**
     * test positive number of radix 24
     */
#if 0
    public void testRadix24Pos() {
        String value = "287628a883451bg80ijhk0865c67e8d7ff20";
        int radix = 24;
        BigInteger aNumber = new BigInteger(value, radix);
        String result = aNumber.toString(radix);
        assertTrue(result.equals(value));
    }
#endif


    /**
     * test negative number of radix 24
     */
#if 0
    public void testRadix36Neg() {
        String value = "-uhguweut98iu4h3478tq3985pq98yeiuth33485yq4aiuhalai485yiaehasdkr8tywi5uhslei8";
        int radix = 36;
        BigInteger aNumber = new BigInteger(value, radix);
        String result = aNumber.toString(radix);
        assertTrue(result.equals(value));
    }
#endif


    /**
     * test positive number of radix 24
     */
#if 0
    public void testRadix36Pos() {
        String value = "23895lt45y6vhgliuwhgi45y845htsuerhsi4586ysuerhtsio5y68peruhgsil4568ypeorihtse48y6";
        int radix = 36;
        BigInteger aNumber = new BigInteger(value, radix);
        String result = aNumber.toString(radix);
        assertTrue(result.equals(value));
    }
}
#endif

//==============================================================================

int mainBigIntegerToStringTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/BigIntegerToStringTest ====\n");
    testRadixOutOfRange();
    printf("\n==== end of libcore/math/BigIntegerToStringTest ====\n");

    return 0;
}

}
}

