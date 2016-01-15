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


static void assertEquals(const String& aspect, const String& test)
{
    printf("aspect: [%s], test: [%s]\n", aspect.string(), test.string());
    assert(aspect.Equals(test) && "result not equals aspect!");
}

static void assertEquals(Double aspect, Double test)
{
    printf("aspect: %f, test: %f\n", aspect, test);
    assert(aspect == test);
}

static void assertEquals(Float aspect, Float test)
{
    printf("aspect: %f, test: %f\n", aspect, test);
    assert(aspect == test);
}

static void assertEquals(Int64 aspect, Int64 test)
{
    printf("aspect: %lld, test: %lld\n", aspect, test);
    assert(aspect == test);
}

static void assertEquals(const char *info, Int32 aspect, Int32 test)
{
    printf("aspect: %d, test: %d. %s\n", aspect, test, info);
    assert(aspect == test);
}

static void assertEquals(const char* info, Int64 test, Int64 aspect)
{
    printf("aspect: %lld, test: %lld. %s\n", aspect, test, info);
    assert(aspect == test);
}

static void assertEquals(const char* info, Byte test, Byte aspect)
{
    printf("aspect: %x, test: %x. %s\n", aspect, test, info);
    assert(aspect == test);
}


static void printArray(ArrayOf<IBigInteger *> *v, const char* info)
{
    printf("  >------Start print %s ------<\n", info);
    Int32 len = v->GetLength();
    for (Int32 i = 0; i < len; ++i) {
        if ((*v)[i]) {
            IBigInteger* bi = (*v)[i];
            Int32 sign = 0;
            bi->GetSignum(&sign);
            printf("    > %d: %p, sign %d\n", i, bi, sign);
        }
        else {
            printf("    > %d: NULL\n", i);
        }
    }
    printf("  >------End print %s ------<\n", info);
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
#endif

/**
 * Class:  java.math.BigInteger
 * Method: and
 */
#if 0
public class BigIntegerAndTest extends TestCase {
#endif

    /**
     * And for zero and a positive number
     */
#if 0
    public void testZeroPos() {
        byte aBytes[] = {0};
        byte bBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        int aSign = 0;
        int bSign = 1;
        byte rBytes[] = {0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 0, result.signum());
    }
#endif
void testZeroPos()
{
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(1);
    AutoPtr<ArrayOf<Byte> > bBytes = ArrayOf<Byte>::Alloc(14);
    AutoPtr<ArrayOf<Byte> > rBytes = ArrayOf<Byte>::Alloc(1);

    signed char _aBytes[] = {0};
    signed char _bBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
    signed char _rBytes[] = {0};
    memcpy(aBytes->GetPayload(), _aBytes, 1);
    memcpy(bBytes->GetPayload(), _bBytes, 14);
    memcpy(rBytes->GetPayload(), _rBytes, 1);

    int aSign = 0;
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

    aNumber->And(bNumber, (IBigInteger **)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", 0, sign);
}


    /**
     * And for zero and a negative number
     */
#if 0
    public void testZeroNeg() {
        byte aBytes[] = {0};
        byte bBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        int aSign = 0;
        int bSign = -1;
        byte rBytes[] = {0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 0, result.signum());
    }
#endif
void testZeroNeg()
{
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(1);
    AutoPtr<ArrayOf<Byte> > bBytes = ArrayOf<Byte>::Alloc(14);
    AutoPtr<ArrayOf<Byte> > rBytes = ArrayOf<Byte>::Alloc(1);

    signed char _aBytes[] = {0};
    signed char _bBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
    signed char _rBytes[] = {0};
    memcpy(aBytes->GetPayload(), _aBytes, 1);
    memcpy(bBytes->GetPayload(), _bBytes, 14);
    memcpy(rBytes->GetPayload(), _rBytes, 1);

    int aSign = 0;
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

    aNumber->And(bNumber, (IBigInteger **)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", 0, sign);
}

    /**
     * And for a positive number and zero
     */
#if 0
    public void testPosZero() {
        byte aBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        byte bBytes[] = {0};
        int aSign = 1;
        int bSign = 0;
        byte rBytes[] = {0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 0, result.signum());
    }
#endif
void testPosZero()
{
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(14);
    AutoPtr<ArrayOf<Byte> > bBytes = ArrayOf<Byte>::Alloc(1);
    AutoPtr<ArrayOf<Byte> > rBytes = ArrayOf<Byte>::Alloc(1);

    signed char _aBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
    signed char _bBytes[] = {0};
    signed char _rBytes[] = {0};
    memcpy(aBytes->GetPayload(), _aBytes, 14);
    memcpy(bBytes->GetPayload(), _bBytes, 1);
    memcpy(rBytes->GetPayload(), _rBytes, 1);

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

    aNumber->And(bNumber, (IBigInteger **)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", 0, sign);
}
    /**
     * And for a negative number and zero
     */
#if 0
    public void testNegPos() {
        byte aBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        byte bBytes[] = {0};
        int aSign = -1;
        int bSign = 0;
        byte rBytes[] = {0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 0, result.signum());
    }
#endif

    /**
     * And for zero and zero
     */
#if 0
    public void testZeroZero() {
        byte aBytes[] = {0};
        byte bBytes[] = {0};
        int aSign = 0;
        int bSign = 0;
        byte rBytes[] = {0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 0, result.signum());
    }
#endif

    /**
     * And for zero and one
     */
#if 0
    public void testZeroOne() {
        BigInteger aNumber = BigInteger.ZERO;
        BigInteger bNumber = BigInteger.ONE;
        BigInteger result = aNumber.and(bNumber);
        assertTrue(result.equals(BigInteger.ZERO));
        assertEquals("incorrect sign", 0, result.signum());
    }
#endif

    /**
     * And for one and one
     */
#if 0
    public void testOneOne() {
        BigInteger aNumber = BigInteger.ONE;
        BigInteger bNumber = BigInteger.ONE;
        BigInteger result = aNumber.and(bNumber);
        assertTrue(result.equals(BigInteger.ONE));
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * And for two positive numbers of the same length
     */
#if 0
    public void testPosPosSameLength() {
        byte aBytes[] = {-128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117};
        byte bBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {0, -128, 56, 100, 4, 4, 17, 37, 16, 1, 64, 1, 10, 3};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * And for two positive numbers; the first is longer
     */
#if 0
    public void testPosPosFirstLonger() {
        byte aBytes[] = {-128, 9, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117, 23, 87, -25, -75};
        byte bBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {0, -2, -76, 88, 44, 1, 2, 17, 35, 16, 9, 2, 5, 6, 21};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * And for two positive numbers; the first is shorter
     */
#if 0
    public void testPosPosFirstShorter() {
        byte aBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        byte bBytes[] = {-128, 9, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117, 23, 87, -25, -75};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {0, -2, -76, 88, 44, 1, 2, 17, 35, 16, 9, 2, 5, 6, 21};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * And for two negative numbers of the same length
     */
#if 0
    public void testNegNegSameLength() {
        byte aBytes[] = {-128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117};
        byte bBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        int aSign = -1;
        int bSign = -1;
        byte rBytes[] = {-1, 1, 2, 3, 3, 0, 65, -96, -48, -124, -60, 12, -40, -31, 97};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * And for two negative numbers; the first is longer
     */
#if 0
    public void testNegNegFirstLonger() {
        byte aBytes[] = {-128, 9, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117, 23, 87, -25, -75};
        byte bBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        int aSign = -1;
        int bSign = -1;
        byte rBytes[] = {-1, 127, -10, -57, -101, 1, 2, 2, 2, -96, -16, 8, -40, -59, 68, -88, -88, 16, 73};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * And for two negative numbers; the first is shorter
     */
#if 0
    public void testNegNegFirstShorter() {
        byte aBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        byte bBytes[] = {-128, 9, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117, 23, 87, -25, -75};
        int aSign = -1;
        int bSign = -1;
        byte rBytes[] = {-1, 127, -10, -57, -101, 1, 2, 2, 2, -96, -16, 8, -40, -59, 68, -88, -88, 16, 73};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * And for two numbers of different signs and the same length
     */
#if 0
    public void testPosNegSameLength() {
        byte aBytes[] = {-128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117};
        byte bBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        int aSign = 1;
        int bSign = -1;
        byte rBytes[] = {0, -6, -80, 72, 8, 75, 2, -79, 34, 16, -119};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * And for two numbers of different signs and the same length
     */
#if 0
    public void testNegPosSameLength() {
        byte aBytes[] = {-128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117};
        byte bBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        int aSign = -1;
        int bSign = 1;
        byte rBytes[] = {0, -2, 125, -60, -104, 1, 10, 6, 2, 32, 56, 2, 4, 4, 21};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * And for a negative and a positive numbers; the first is longer
     */
#if 0
    public void testNegPosFirstLonger() {
        byte aBytes[] = {-128, 9, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117, 23, 87, -25, -75};
        byte bBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        int aSign = -1;
        int bSign = 1;
        byte rBytes[] = {73, -92, -48, 4, 12, 6, 4, 32, 48, 64, 0, 8, 3};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * And for a negative and a positive numbers; the first is shorter
     */
#if 0
    public void testNegPosFirstShorter() {
        byte aBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        byte bBytes[] = {-128, 9, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117, 23, 87, -25, -75};
        int aSign = -1;
        int bSign = 1;
        byte rBytes[] = {0, -128, 9, 56, 100, 0, 0, 1, 1, 90, 1, -32, 0, 10, -126, 21, 82, -31, -95};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * And for a positive and a negative numbers; the first is longer
     */
#if 0
    public void testPosNegFirstLonger() {
        byte aBytes[] = {-128, 9, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117, 23, 87, -25, -75};
        byte bBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        int aSign = 1;
        int bSign = -1;
        byte rBytes[] = {0, -128, 9, 56, 100, 0, 0, 1, 1, 90, 1, -32, 0, 10, -126, 21, 82, -31, -95};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * And for a positive and a negative numbers; the first is shorter
     */
#if 0
    public void testPosNegFirstShorter() {
        byte aBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        byte bBytes[] = {-128, 9, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117, 23, 87, -25, -75};
        int aSign = 1;
        int bSign = -1;
        byte rBytes[] = {73, -92, -48, 4, 12, 6, 4, 32, 48, 64, 0, 8, 3};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * Test for a special case
     */
#if 0
    public void testSpecialCase1() {
        byte aBytes[] = {-1, -1, -1, -1};
        byte bBytes[] = {5, -4, -3, -2};
        int aSign = -1;
        int bSign = -1;
        byte rBytes[] = {-1, 0, 0, 0, 0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * Test for a special case
     */
#if 0
    public void testSpecialCase2() {
        byte aBytes[] = {-51};
        byte bBytes[] = {-52, -51, -50, -49, -48};
        int aSign = -1;
        int bSign = 1;
        byte rBytes[] = {0, -52, -51, -50, -49, 16};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.and(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

//==============================================================================

int mainBigIntegerAndTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/BigIntegerAndTest ====\n");
    testZeroPos();
    testZeroNeg();
    testPosZero();
    printf("\n==== end of libcore/math/BigIntegerAndTest ====\n");

    return 0;
}


