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
 * Class:   java.math.BigInteger
 * Methods: divide, remainder, mod, and divideAndRemainder
 */
public class BigIntegerDivideTest extends TestCase {
#endif

    /**
     * Divide by zero
     */
#if 0
    public void testCase1() {
        byte aBytes[] = {1, 2, 3, 4, 5, 6, 7};
        byte bBytes[] = {0};
        int aSign = 1;
        int bSign = 0;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        try {
            aNumber.divide(bNumber);
            fail("ArithmeticException has not been caught");
        } catch (ArithmeticException e) {
        }
    }
#endif
static void testCase1() {
    signed char _aBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127};
    signed char _bBytes[] = {0};
    signed char _rBytes[] = {1};
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(sizeof(_aBytes));
    AutoPtr<ArrayOf<Byte> > bBytes = ArrayOf<Byte>::Alloc(sizeof(_bBytes));
    AutoPtr<ArrayOf<Byte> > rBytes = ArrayOf<Byte>::Alloc(sizeof(_rBytes));

    memcpy(aBytes->GetPayload(), _aBytes, sizeof(_aBytes));
    memcpy(bBytes->GetPayload(), _bBytes, sizeof(_bBytes));
    memcpy(rBytes->GetPayload(), _rBytes, sizeof(_rBytes));

    int aSign = 1;
    int bSign = 0;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(aSign, *aBytes, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> bNumber;
    ec = CBigInteger::New(bSign, *bBytes, (IBigInteger**)&bNumber);
    if (FAILED(ec) || bNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> result;
    AutoPtr<ArrayOf<Byte> > resBytes;

    ec = aNumber->Divide(bNumber, (IBigInteger **)&result);
    if (FAILED(ec) || result == NULL) {
        printf(" ArithmeticException has been caught. Error %08X\n", ec);
    } else {
        printf(" ArithmeticException has not been caught %08X\n", ec);
    }
}


    /**
     * Divide by ZERO
     */
#if 0
    public void testCase2() {
        byte aBytes[] = {1, 2, 3, 4, 5, 6, 7};
        int aSign = 1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = BigInteger.ZERO;
        try {
            aNumber.divide(bNumber);
            fail("ArithmeticException has not been caught");
        } catch (ArithmeticException e) {
        }
    }
#endif


    /**
     * Divide two equal positive numbers
     */
#if 0
    public void testCase3() {
        byte aBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127};
        byte bBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {1};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.divide(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif
static void testCase3() {
    signed char _aBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127};
    signed char _bBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127};
    signed char _rBytes[] = {1};
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(sizeof(_aBytes));
    AutoPtr<ArrayOf<Byte> > bBytes = ArrayOf<Byte>::Alloc(sizeof(_bBytes));
    AutoPtr<ArrayOf<Byte> > rBytes = ArrayOf<Byte>::Alloc(sizeof(_rBytes));

    memcpy(aBytes->GetPayload(), _aBytes, sizeof(_aBytes));
    memcpy(bBytes->GetPayload(), _bBytes, sizeof(_bBytes));
    memcpy(rBytes->GetPayload(), _rBytes, sizeof(_rBytes));

    int aSign = 1;
    int bSign = 1;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(aSign, *aBytes, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> bNumber;
    ec = CBigInteger::New(bSign, *bBytes, (IBigInteger**)&bNumber);
    if (FAILED(ec) || bNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> result;
    AutoPtr<ArrayOf<Byte> > resBytes;

    aNumber->Divide(bNumber, (IBigInteger **)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", 1, sign);
}


    /**
     * Divide two equal in absolute value numbers of different signs.
     */
#if 0
    public void testCase4() {
        byte aBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127};
        byte bBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127};
        int aSign = -1;
        int bSign = 1;
        byte rBytes[] = {-1};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.divide(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif
static void testCase4() {
    signed char _aBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127};
    signed char _bBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127};
    signed char _rBytes[] = {-1};
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(sizeof(_aBytes));
    AutoPtr<ArrayOf<Byte> > bBytes = ArrayOf<Byte>::Alloc(sizeof(_bBytes));
    AutoPtr<ArrayOf<Byte> > rBytes = ArrayOf<Byte>::Alloc(sizeof(_rBytes));

    memcpy(aBytes->GetPayload(), _aBytes, sizeof(_aBytes));
    memcpy(bBytes->GetPayload(), _bBytes, sizeof(_bBytes));
    memcpy(rBytes->GetPayload(), _rBytes, sizeof(_rBytes));

    int aSign = -1;
    int bSign = 1;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(aSign, *aBytes, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> bNumber;
    ec = CBigInteger::New(bSign, *bBytes, (IBigInteger**)&bNumber);
    if (FAILED(ec) || bNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> result;
    AutoPtr<ArrayOf<Byte> > resBytes;

    aNumber->Divide(bNumber, (IBigInteger **)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", -1, sign);
}


    /**
     * Divide two numbers of different length and different signs.
     * The second is longer.
     */
#if 0
    public void testCase5() {
        byte aBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127};
        byte bBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127, 1, 2, 3, 4, 5};
        int aSign = -1;
        int bSign = 1;
        byte rBytes[] = {0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.divide(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 0, result.signum());
    }
#endif
static void testCase5() {
    signed char _aBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127};
    signed char _bBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127, 1, 2, 3, 4, 5};
    signed char _rBytes[] = {0};
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(sizeof(_aBytes));
    AutoPtr<ArrayOf<Byte> > bBytes = ArrayOf<Byte>::Alloc(sizeof(_bBytes));
    AutoPtr<ArrayOf<Byte> > rBytes = ArrayOf<Byte>::Alloc(sizeof(_rBytes));

    memcpy(aBytes->GetPayload(), _aBytes, sizeof(_aBytes));
    memcpy(bBytes->GetPayload(), _bBytes, sizeof(_bBytes));
    memcpy(rBytes->GetPayload(), _rBytes, sizeof(_rBytes));

    int aSign = -1;
    int bSign = 1;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(aSign, *aBytes, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> bNumber;
    ec = CBigInteger::New(bSign, *bBytes, (IBigInteger**)&bNumber);
    if (FAILED(ec) || bNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> result;
    AutoPtr<ArrayOf<Byte> > resBytes;

    aNumber->Divide(bNumber, (IBigInteger **)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", 0, sign);
}


    /**
     * Divide two positive numbers of the same length.
     * The second is greater.
     */
#if 0
    public void testCase6() {
        byte aBytes[] = {1, 100, 56, 7, 98, -1, 39, -128, 127};
        byte bBytes[] = {15, 100, 56, 7, 98, -1, 39, -128, 127};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.divide(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 0, result.signum());
    }
#endif
static void testCase6() {
    signed char _aBytes[] = {1, 100, 56, 7, 98, -1, 39, -128, 127};
    signed char _bBytes[] = {15, 100, 56, 7, 98, -1, 39, -128, 127};
    signed char _rBytes[] = {0};
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(sizeof(_aBytes));
    AutoPtr<ArrayOf<Byte> > bBytes = ArrayOf<Byte>::Alloc(sizeof(_bBytes));
    AutoPtr<ArrayOf<Byte> > rBytes = ArrayOf<Byte>::Alloc(sizeof(_rBytes));

    memcpy(aBytes->GetPayload(), _aBytes, sizeof(_aBytes));
    memcpy(bBytes->GetPayload(), _bBytes, sizeof(_bBytes));
    memcpy(rBytes->GetPayload(), _rBytes, sizeof(_rBytes));

    int aSign = 1;
    int bSign = 1;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(aSign, *aBytes, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> bNumber;
    ec = CBigInteger::New(bSign, *bBytes, (IBigInteger**)&bNumber);
    if (FAILED(ec) || bNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> result;
    AutoPtr<ArrayOf<Byte> > resBytes;

    aNumber->Divide(bNumber, (IBigInteger **)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", 0, sign);
}


    /**
     * Divide two positive numbers.
     */
#if 0
    public void testCase7() {
        byte aBytes[] = {1, 100, 56, 7, 98, -1, 39, -128, 127, 5, 6, 7, 8, 9};
        byte bBytes[] = {15, 48, -29, 7, 98, -1, 39, -128};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {23, 115, 11, 78, 35, -11};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.divide(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif
static void testCase7() {
    signed char _aBytes[] = {1, 100, 56, 7, 98, -1, 39, -128, 127, 5, 6, 7, 8, 9};
    signed char _bBytes[] = {15, 48, -29, 7, 98, -1, 39, -128};
    signed char _rBytes[] = {23, 115, 11, 78, 35, -11};
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(sizeof(_aBytes));
    AutoPtr<ArrayOf<Byte> > bBytes = ArrayOf<Byte>::Alloc(sizeof(_bBytes));
    AutoPtr<ArrayOf<Byte> > rBytes = ArrayOf<Byte>::Alloc(sizeof(_rBytes));

    memcpy(aBytes->GetPayload(), _aBytes, sizeof(_aBytes));
    memcpy(bBytes->GetPayload(), _bBytes, sizeof(_bBytes));
    memcpy(rBytes->GetPayload(), _rBytes, sizeof(_rBytes));

    int aSign = 1;
    int bSign = 1;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(aSign, *aBytes, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> bNumber;
    ec = CBigInteger::New(bSign, *bBytes, (IBigInteger**)&bNumber);
    if (FAILED(ec) || bNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> result;
    AutoPtr<ArrayOf<Byte> > resBytes;

    aNumber->Divide(bNumber, (IBigInteger **)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", 1, sign);
}


    /**
     * Divide a positive number by a negative one.
     */
#if 0
    public void testCase8() {
        byte aBytes[] = {1, 100, 56, 7, 98, -1, 39, -128, 127, 5, 6, 7, 8, 9};
        byte bBytes[] = {15, 48, -29, 7, 98, -1, 39, -128};
        int aSign = 1;
        int bSign = -1;
        byte rBytes[] = {-24, -116, -12, -79, -36, 11};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.divide(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif
static void testCase8() {
    signed char _aBytes[] = {1, 100, 56, 7, 98, -1, 39, -128, 127, 5, 6, 7, 8, 9};
    signed char _bBytes[] = {15, 48, -29, 7, 98, -1, 39, -128};
    signed char _rBytes[] = {-24, -116, -12, -79, -36, 11};
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(sizeof(_aBytes));
    AutoPtr<ArrayOf<Byte> > bBytes = ArrayOf<Byte>::Alloc(sizeof(_bBytes));
    AutoPtr<ArrayOf<Byte> > rBytes = ArrayOf<Byte>::Alloc(sizeof(_rBytes));

    memcpy(aBytes->GetPayload(), _aBytes, sizeof(_aBytes));
    memcpy(bBytes->GetPayload(), _bBytes, sizeof(_bBytes));
    memcpy(rBytes->GetPayload(), _rBytes, sizeof(_rBytes));

    int aSign = 1;
    int bSign = -1;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(aSign, *aBytes, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> bNumber;
    ec = CBigInteger::New(bSign, *bBytes, (IBigInteger**)&bNumber);
    if (FAILED(ec) || bNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> result;
    AutoPtr<ArrayOf<Byte> > resBytes;

    aNumber->Divide(bNumber, (IBigInteger **)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", -1, sign);
}


    /**
     * Divide a negative number by a positive one.
     */
#if 0
    public void testCase9() {
        byte aBytes[] = {1, 100, 56, 7, 98, -1, 39, -128, 127, 5, 6, 7, 8, 9};
        byte bBytes[] = {15, 48, -29, 7, 98, -1, 39, -128};
        int aSign = -1;
        int bSign = 1;
        byte rBytes[] = {-24, -116, -12, -79, -36, 11};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.divide(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif
static void testCase9() {
    signed char _aBytes[] = {1, 100, 56, 7, 98, -1, 39, -128, 127, 5, 6, 7, 8, 9};
    signed char _bBytes[] = {15, 48, -29, 7, 98, -1, 39, -128};
    signed char _rBytes[] = {-24, -116, -12, -79, -36, 11};
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(sizeof(_aBytes));
    AutoPtr<ArrayOf<Byte> > bBytes = ArrayOf<Byte>::Alloc(sizeof(_bBytes));
    AutoPtr<ArrayOf<Byte> > rBytes = ArrayOf<Byte>::Alloc(sizeof(_rBytes));

    memcpy(aBytes->GetPayload(), _aBytes, sizeof(_aBytes));
    memcpy(bBytes->GetPayload(), _bBytes, sizeof(_bBytes));
    memcpy(rBytes->GetPayload(), _rBytes, sizeof(_rBytes));

    int aSign = -1;
    int bSign = 1;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(aSign, *aBytes, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> bNumber;
    ec = CBigInteger::New(bSign, *bBytes, (IBigInteger**)&bNumber);
    if (FAILED(ec) || bNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> result;
    AutoPtr<ArrayOf<Byte> > resBytes;

    aNumber->Divide(bNumber, (IBigInteger **)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", -1, sign);
}


    /**
     * Divide two negative numbers.
     */
#if 0
    public void testCase10() {
        byte aBytes[] = {1, 100, 56, 7, 98, -1, 39, -128, 127, 5, 6, 7, 8, 9};
        byte bBytes[] = {15, 48, -29, 7, 98, -1, 39, -128};
        int aSign = -1;
        int bSign = -1;
        byte rBytes[] = {23, 115, 11, 78, 35, -11};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.divide(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif
static void testCase10() {
    signed char _aBytes[] = {1, 100, 56, 7, 98, -1, 39, -128, 127, 5, 6, 7, 8, 9};
    signed char _bBytes[] = {15, 48, -29, 7, 98, -1, 39, -128};
    signed char _rBytes[] = {23, 115, 11, 78, 35, -11};
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(sizeof(_aBytes));
    AutoPtr<ArrayOf<Byte> > bBytes = ArrayOf<Byte>::Alloc(sizeof(_bBytes));
    AutoPtr<ArrayOf<Byte> > rBytes = ArrayOf<Byte>::Alloc(sizeof(_rBytes));

    memcpy(aBytes->GetPayload(), _aBytes, sizeof(_aBytes));
    memcpy(bBytes->GetPayload(), _bBytes, sizeof(_bBytes));
    memcpy(rBytes->GetPayload(), _rBytes, sizeof(_rBytes));

    int aSign = -1;
    int bSign = -1;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(aSign, *aBytes, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> bNumber;
    ec = CBigInteger::New(bSign, *bBytes, (IBigInteger**)&bNumber);
    if (FAILED(ec) || bNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> result;
    AutoPtr<ArrayOf<Byte> > resBytes;

    aNumber->Divide(bNumber, (IBigInteger **)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", 1, sign);
}


    /**
     * Divide zero by a negative number.
     */
#if 0
    public void testCase11() {
        byte aBytes[] = {0};
        byte bBytes[] = {15, 48, -29, 7, 98, -1, 39, -128};
        int aSign = 0;
        int bSign = -1;
        byte rBytes[] = {0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.divide(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 0, result.signum());
    }
#endif


    /**
     * Divide ZERO by a negative number.
     */
#if 0
    public void testCase12() {
        byte bBytes[] = {15, 48, -29, 7, 98, -1, 39, -128};
        int bSign = -1;
        byte rBytes[] = {0};
        BigInteger aNumber = BigInteger.ZERO;
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.divide(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 0, result.signum());
    }
#endif


    /**
     * Divide a positive number by ONE.
     */
#if 0
    public void testCase13() {
        byte aBytes[] = {15, 48, -29, 7, 98, -1, 39, -128};
        int aSign = 1;
        byte rBytes[] = {15, 48, -29, 7, 98, -1, 39, -128};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = BigInteger.ONE;
        BigInteger result = aNumber.divide(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif


    /**
     * Divide ONE by ONE.
     */
#if 0
    public void testCase14() {
        byte rBytes[] = {1};
        BigInteger aNumber = BigInteger.ONE;
        BigInteger bNumber = BigInteger.ONE;
        BigInteger result = aNumber.divide(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif


    /**
     * Verifies the case when borrow != 0 in the private divide method.
     */
#if 0
    public void testDivisionKnuth1() {
        byte aBytes[] = {-7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7};
        byte bBytes[] = {-3, -3, -3, -3};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {0, -5, -12, -33, -96, -36, -105, -56, 92, 15, 48, -109};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.divide(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif


    /**
     * Verifies the case when the divisor is already normalized.
     */
#if 0
    public void testDivisionKnuthIsNormalized() {
        byte aBytes[] = {-9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5};
        byte bBytes[] = {-1, -1, -1, -1, -1, -1, -1, -1};
        int aSign = -1;
        int bSign = -1;
        byte rBytes[] = {0, -9, -8, -7, -6, -5, -4, -3};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.divide(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif


    /**
     * Verifies the case when the first digits of the dividend
     * and divisor equal.
     */
#if 0
    public void testDivisionKnuthFirstDigitsEqual() {
        byte aBytes[] = {2, -3, -4, -5, -1, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5};
        byte bBytes[] = {2, -3, -4, -5, -1, -1, -1, -1};
        int aSign = -1;
        int bSign = -1;
        byte rBytes[] = {0, -1, -1, -1, -1, -2, -88, -60, 41};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.divide(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif


    /**
     * Divide the number of one digit by the number of one digit
     */
#if 0
    public void testDivisionKnuthOneDigitByOneDigit() {
        byte aBytes[] = {113, -83, 123, -5};
        byte bBytes[] = {2, -3, -4, -5};
        int aSign = 1;
        int bSign = -1;
        byte rBytes[] = {-37};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.divide(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif


    /**
     * Divide the number of multi digits by the number of one digit
     */
#if 0
    public void testDivisionKnuthMultiDigitsByOneDigit() {
        byte aBytes[] = {113, -83, 123, -5, 18, -34, 67, 39, -29};
        byte bBytes[] = {2, -3, -4, -5};
        int aSign = 1;
        int bSign = -1;
        byte rBytes[] = {-38, 2, 7, 30, 109, -43};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.divide(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif


    /**
     * Remainder of division by zero
     */
#if 0
    public void testCase15() {
        byte aBytes[] = {1, 2, 3, 4, 5, 6, 7};
        byte bBytes[] = {0};
        int aSign = 1;
        int bSign = 0;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        try {
            aNumber.remainder(bNumber);
            fail("ArithmeticException has not been caught");
        } catch (ArithmeticException e) {
        }
    }
#endif


    /**
     * Remainder of division of equal numbers
     */
#if 0
    public void testCase16() {
        byte aBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127};
        byte bBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.remainder(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 0, result.signum());
    }
#endif


    /**
     * Remainder of division of two positive numbers
     */
#if 0
    public void testCase17() {
        byte aBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127, 75};
        byte bBytes[] = {27, -15, 65, 39, 100};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {12, -21, 73, 56, 27};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.remainder(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif


    /**
     * Remainder of division of two negative numbers
     */
#if 0
    public void testCase18() {
        byte aBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127, 75};
        byte bBytes[] = {27, -15, 65, 39, 100};
        int aSign = -1;
        int bSign = -1;
        byte rBytes[] = {-13, 20, -74, -57, -27};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.remainder(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif


    /**
     * Remainder of division of two numbers of different signs.
     * The first is positive.
     */
#if 0
    public void testCase19() {
        byte aBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127, 75};
        byte bBytes[] = {27, -15, 65, 39, 100};
        int aSign = 1;
        int bSign = -1;
        byte rBytes[] = {12, -21, 73, 56, 27};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.remainder(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif


    /**
     * Remainder of division of two numbers of different signs.
     * The first is negative.
     */
#if 0
    public void testCase20() {
        byte aBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127, 75};
        byte bBytes[] = {27, -15, 65, 39, 100};
        int aSign = -1;
        int bSign = 1;
        byte rBytes[] = {-13, 20, -74, -57, -27};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.remainder(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif


    /**
     * Tests the step D6 from the Knuth algorithm
     */
#if 0
    public void testRemainderKnuth1() {
        byte aBytes[] = {-9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1};
        byte bBytes[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {1, 2, 3, 4, 5, 6, 7, 7, 18, -89};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.remainder(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif


    /**
     * Divide the number of one digit by the number of one digit
     */
#if 0
    public void testRemainderKnuthOneDigitByOneDigit() {
        byte aBytes[] = {113, -83, 123, -5};
        byte bBytes[] = {2, -3, -4, -50};
        int aSign = 1;
        int bSign = -1;
        byte rBytes[] = {2, -9, -14, 53};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.remainder(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif


    /**
     * Divide the number of multi digits by the number of one digit
     */
#if 0
    public void testRemainderKnuthMultiDigitsByOneDigit() {
        byte aBytes[] = {113, -83, 123, -5, 18, -34, 67, 39, -29};
        byte bBytes[] = {2, -3, -4, -50};
        int aSign = 1;
        int bSign = -1;
        byte rBytes[] = {2, -37, -60, 59};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.remainder(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif


    /**
     * divideAndRemainder of two numbers of different signs.
     * The first is negative.
     */
#if 0
    public void testCase21() {
        byte aBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127, 75};
        byte bBytes[] = {27, -15, 65, 39, 100};
        int aSign = -1;
        int bSign = 1;
        byte rBytes[][] = {
                {-5, 94, -115, -74, -85, 84},
                {-13, 20, -74, -57, -27}
        };
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result[] = aNumber.divideAndRemainder(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result[0].toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            if (resBytes[i] != rBytes[0][i]) {
                fail("Incorrect quotation");
            }
        }
        assertEquals(-1, result[0].signum());
        resBytes = result[1].toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            if (resBytes[i] != rBytes[1][i]) {
                fail("Incorrect remainder");
            }
            assertEquals(-1, result[1].signum());
        }
    }
#endif


    /**
     * mod when modulus is negative
     */
#if 0
    public void testCase22() {
        byte aBytes[] = {1, 2, 3, 4, 5, 6, 7};
        byte bBytes[] = {1, 30, 40, 56, -1, 45};
        int aSign = 1;
        int bSign = -1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        try {
            aNumber.mod(bNumber);
            fail("ArithmeticException has not been caught");
        } catch (ArithmeticException e) {
        }
    }
#endif


    /**
     * mod when a divisor is positive
     */
#if 0
    public void testCase23() {
        byte aBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127, 75};
        byte bBytes[] = {27, -15, 65, 39, 100};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {12, -21, 73, 56, 27};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.mod(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif


    /**
     * mod when a divisor is negative
     */
#if 0
    public void testCase24() {
        byte aBytes[] = {-127, 100, 56, 7, 98, -1, 39, -128, 127, 75};
        byte bBytes[] = {27, -15, 65, 39, 100};
        int aSign = -1;
        int bSign = 1;
        byte rBytes[] = {15, 5, -9, -17, 73};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.mod(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

//==============================================================================

int mainBigIntegerDivideTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/BigIntegerDivideTest ====\n");
    testCase1();
    testCase3();
    testCase4();
    testCase5();
    testCase6();
    testCase7();
    testCase8();
    testCase9();
    testCase10();
    printf("\n==== end of libcore/math/BigIntegerDivideTest ====\n");

    return 0;
}

}
}

