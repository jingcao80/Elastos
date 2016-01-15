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

import junit.framework.TestCase;
import java.math.BigInteger;

/**
 * Class:  java.math.BigInteger
 * Method: xor
 */
public class BigIntegerXorTest extends TestCase {
#endif

	/**
     * Xor for zero and a positive number
     */
#if 0
    public void testZeroPos() {
        String numA = "0";
        String numB = "27384627835298756289327365";
        String res = "27384627835298756289327365";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
#endif
static void testZeroPos()
{
    String numA = String("0");
    String numB = String("27384627835298756289327365");
    String res = String("27384627835298756289327365");

    AutoPtr<IBigInteger> aNumber, bNumber;

    ECode ec = CBigInteger::New(numA, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    ec = CBigInteger::New(numB, (IBigInteger**)&bNumber);
    if (FAILED(ec) || bNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> result;
    aNumber->Xor(bNumber, (IBigInteger**)&result);

    String str;
    result->ToString(10, &str);

    if (str.Equals(res)) {
        printf("corrent testPosNegFirstShorter\n");
    } else {
        printf("fail testPosNegFirstShorter\n");
    }
}

    /**
     * Xor for zero and a negative number
     */
#if 0
    public void testZeroNeg() {
        String numA = "0";
        String numB = "-27384627835298756289327365";
        String res = "-27384627835298756289327365";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
#endif


    /**
     * Xor for a positive number and zero
     */
#if 0
    public void testPosZero() {
        String numA = "27384627835298756289327365";
        String numB = "0";
        String res = "27384627835298756289327365";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
#endif


    /**
     * Xor for a negative number and zero
     */
#if 0
    public void testNegPos() {
        String numA = "-27384627835298756289327365";
        String numB = "0";
        String res = "-27384627835298756289327365";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
#endif


    /**
     * Xor for zero and zero
     */
#if 0
    public void testZeroZero() {
        String numA = "0";
        String numB = "0";
        String res = "0";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
#endif


    /**
     * Xor for zero and one
     */
#if 0
    public void testZeroOne() {
        String numA = "0";
        String numB = "1";
        String res = "1";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
#endif


    /**
     * Xor for one and one
     */
#if 0
    public void testOneOne() {
        String numA = "1";
        String numB = "1";
        String res = "0";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
#endif


    /**
     * Xor for two positive numbers of the same length
     */
#if 0
    public void testPosPosSameLength() {
        String numA = "283746278342837476784564875684767";
        String numB = "293478573489347658763745839457637";
        String res = "71412358434940908477702819237626";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
#endif


    /**
     * Xor for two positive numbers; the first is longer
     */
#if 0
    public void testPosPosFirstLonger() {
        String numA = "2837462783428374767845648748973847593874837948575684767";
        String numB = "293478573489347658763745839457637";
        String res = "2837462783428374767845615168483972194300564226167553530";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
#endif


    /**
     * Xor for two positive numbers; the first is shorter
     */
#if 0
    public void testPosPosFirstShorter() {
        String numA = "293478573489347658763745839457637";
        String numB = "2837462783428374767845648748973847593874837948575684767";
        String res = "2837462783428374767845615168483972194300564226167553530";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
#endif


    /**
     * Xor for two negative numbers of the same length
     */
#if 0
    public void testNegNegSameLength() {
        String numA = "-283746278342837476784564875684767";
        String numB = "-293478573489347658763745839457637";
        String res = "71412358434940908477702819237626";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
#endif


    /**
     * Xor for two negative numbers; the first is longer
     */
#if 0
    public void testNegNegFirstLonger() {
        String numA = "-2837462783428374767845648748973847593874837948575684767";
        String numB = "-293478573489347658763745839457637";
        String res = "2837462783428374767845615168483972194300564226167553530";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
#endif


    /**
     * Xor for two negative numbers; the first is shorter
     */
#if 0
    public void testNegNegFirstShorter() {
        String numA = "293478573489347658763745839457637";
        String numB = "2837462783428374767845648748973847593874837948575684767";
        String res = "2837462783428374767845615168483972194300564226167553530";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
#endif


    /**
     * Xor for two numbers of different signs and the same length
     */
#if 0
    public void testPosNegSameLength() {
        String numA = "283746278342837476784564875684767";
        String numB = "-293478573489347658763745839457637";
        String res = "-71412358434940908477702819237628";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
#endif


    /**
     * Xor for two numbers of different signs and the same length
     */
#if 0
    public void testNegPosSameLength() {
        String numA = "-283746278342837476784564875684767";
        String numB = "293478573489347658763745839457637";
        String res = "-71412358434940908477702819237628";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
#endif


    /**
     * Xor for a negative and a positive numbers; the first is longer
     */
#if 0
    public void testNegPosFirstLonger() {
        String numA = "-2837462783428374767845648748973847593874837948575684767";
        String numB = "293478573489347658763745839457637";
        String res = "-2837462783428374767845615168483972194300564226167553532";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
#endif


    /**
     * Xor for two negative numbers; the first is shorter
     */
#if 0
    public void testNegPosFirstShorter() {
        String numA = "-293478573489347658763745839457637";
        String numB = "2837462783428374767845648748973847593874837948575684767";
        String res = "-2837462783428374767845615168483972194300564226167553532";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
#endif


    /**
     * Xor for a positive and a negative numbers; the first is longer
     */
#if 0
    public void testPosNegFirstLonger() {
        String numA = "2837462783428374767845648748973847593874837948575684767";
        String numB = "-293478573489347658763745839457637";
        String res = "-2837462783428374767845615168483972194300564226167553532";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
#endif


    /**
     * Xor for a positive and a negative number; the first is shorter
     */
#if 0
    public void testPosNegFirstShorter() {
        String numA = "293478573489347658763745839457637";
        String numB = "-2837462783428374767845648748973847593874837948575684767";
        String res = "-2837462783428374767845615168483972194300564226167553532";
        BigInteger aNumber = new BigInteger(numA);
        BigInteger bNumber = new BigInteger(numB);
        BigInteger result = aNumber.xor(bNumber);
        assertTrue(res.equals(result.toString()));
    }
}
#endif
static void testPosNegFirstShorter()
{
    String numA = String("293478573489347658763745839457637");
    String numB = String("-2837462783428374767845648748973847593874837948575684767");
    String res = String("-2837462783428374767845615168483972194300564226167553532");

    AutoPtr<IBigInteger> aNumber, bNumber;

    ECode ec = CBigInteger::New(numA, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    ec = CBigInteger::New(numB, (IBigInteger**)&bNumber);
    if (FAILED(ec) || bNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> result;
    aNumber->Xor(bNumber, (IBigInteger**)&result);

    String str;
    result->ToString(10, &str);

    if (str.Equals(res)) {
        printf("corrent testPosNegFirstShorter\n");
    } else {
        printf("fail testPosNegFirstShorter\n");
    }
}



//==============================================================================

int mainBigIntegerXorTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/BigIntegerXorTest ====\n");
    testPosNegFirstShorter();
    testZeroPos();
    printf("\n==== end of libcore/math/BigIntegerXorTest ====\n");

    return 0;
}

}
}
