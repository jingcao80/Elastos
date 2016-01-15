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

/**
 * Class:   java.math.BigInteger
 * Methods: abs, compareTo, equals, max, min, negate, signum
 */
public class BigIntegerCompareTest extends TestCase {
#endif

    /**
     * abs() for a positive number
     */
#if 0
    public void testAbsPositive() {
        byte aBytes[] = {1, 2, 3, 4, 5, 6, 7};
        int aSign = 1;
        byte rBytes[] = {1, 2, 3, 4, 5, 6, 7};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.abs();
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif
void testAbsPositive()
{
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(7);
    AutoPtr<ArrayOf<Byte> > rBytes = ArrayOf<Byte>::Alloc(7);

    signed char _aBytes[] = {1, 2, 3, 4, 5, 6, 7};
    signed char _rBytes[] = {1, 2, 3, 4, 5, 6, 7};
    memcpy(aBytes->GetPayload(), _aBytes, 7);
    memcpy(rBytes->GetPayload(), _rBytes, 7);

    int aSign = 1;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(aSign, *aBytes, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> result;
    AutoPtr<ArrayOf<Byte> > resBytes;

    aNumber->Abs((IBigInteger **)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", 1, sign);
}

    /**
     * abs() for a negative number
     */
#if 0
    public void testAbsNegative() {
        byte aBytes[] = {1, 2, 3, 4, 5, 6, 7};
        int aSign = -1;
        byte rBytes[] = {1, 2, 3, 4, 5, 6, 7};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.abs();
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif
void testAbsNegative()
{
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(7);
    AutoPtr<ArrayOf<Byte> > rBytes = ArrayOf<Byte>::Alloc(7);

    signed char _aBytes[] = {1, 2, 3, 4, 5, 6, 7};
    signed char _rBytes[] = {1, 2, 3, 4, 5, 6, 7};
    memcpy(aBytes->GetPayload(), _aBytes, 7);
    memcpy(rBytes->GetPayload(), _rBytes, 7);

    int aSign = -1;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(aSign, *aBytes, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> result;
    AutoPtr<ArrayOf<Byte> > resBytes;

    aNumber->Abs((IBigInteger **)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", 1, sign);
}

    /**
     * compareTo(BigInteger a).
     * Compare two positive numbers.
     * The first is greater.
     */
#if 0
    public void testCompareToPosPos1() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        byte bBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        int aSign = 1;
        int bSign = 1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        assertEquals(1, aNumber.compareTo(bNumber));
    }
#endif
void testCompareToPosPos1()
{
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(14);
    AutoPtr<ArrayOf<Byte> > bBytes = ArrayOf<Byte>::Alloc(10);

    signed char _aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
    signed char _bBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
    memcpy(aBytes->GetPayload(), _aBytes, 14);
    memcpy(bBytes->GetPayload(), _bBytes, 10);

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

    Int32 result;
    IComparable* comp = (IComparable *)aNumber->Probe(EIID_IComparable);
    comp->CompareTo(bNumber, &result);

    assertEquals("compareto", 1, result);
}


    /**
     * compareTo(BigInteger a).
     * Compare two positive numbers.
     * The first is less.
     */
#if 0
    public void testCompareToPosPos2() {
        byte aBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        byte bBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = 1;
        int bSign = 1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        assertEquals(-1, aNumber.compareTo(bNumber));
    }
#endif
void testCompareToPosPos2()
{
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(14);
    AutoPtr<ArrayOf<Byte> > bBytes = ArrayOf<Byte>::Alloc(10);

    signed char _aBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
    signed char _bBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
    memcpy(aBytes->GetPayload(), _aBytes, 14);
    memcpy(bBytes->GetPayload(), _bBytes, 10);

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

    Int32 result;

    IComparable* comp = (IComparable *)aNumber->Probe(EIID_IComparable);
    comp->CompareTo(bNumber, &result);
    assertEquals("compareto", -1, result);
}

    /**
     * compareTo(BigInteger a).
     * Compare two equal positive numbers.
     */
#if 0
    public void testCompareToEqualPos() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        byte bBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = 1;
        int bSign = 1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        assertEquals(0, aNumber.compareTo(bNumber));
    }
#endif
void testCompareToEqualPos()
{
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(14);
    AutoPtr<ArrayOf<Byte> > bBytes = ArrayOf<Byte>::Alloc(14);

    signed char _aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
    signed char _bBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
    memcpy(aBytes->GetPayload(), _aBytes, 14);
    memcpy(bBytes->GetPayload(), _bBytes, 14);

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

    Int32 result;

    IComparable* comp = (IComparable *)aNumber->Probe(EIID_IComparable);
    comp->CompareTo(bNumber, &result);
    assertEquals("testCompareToEqualPos", 0, result);
}

    /**
     * compareTo(BigInteger a).
     * Compare two negative numbers.
     * The first is greater in absolute value.
     */
#if 0
    public void testCompareToNegNeg1() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        byte bBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        int aSign = -1;
        int bSign = -1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        assertEquals(-1, aNumber.compareTo(bNumber));
    }
#endif
void testCompareToNegNeg1()
{
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(14);
    AutoPtr<ArrayOf<Byte> > bBytes = ArrayOf<Byte>::Alloc(10);

    signed char _aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
    signed char _bBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
    memcpy(aBytes->GetPayload(), _aBytes, 14);
    memcpy(bBytes->GetPayload(), _bBytes, 10);

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

    Int32 result;

    IComparable* comp = (IComparable *)aNumber->Probe(EIID_IComparable);
    comp->CompareTo(bNumber, &result);
    assertEquals("testCompareToNegNeg1", -1, result);
}

    /**
     * compareTo(BigInteger a).
     * Compare two negative numbers.
     * The first is less  in absolute value.
     */
#if 0
    public void testCompareNegNeg2() {
        byte aBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        byte bBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = -1;
        int bSign = -1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        assertEquals(1, aNumber.compareTo(bNumber));
    }
#endif


    /**
     * compareTo(BigInteger a).
     * Compare two equal negative numbers.
     */
#if 0
    public void testCompareToEqualNeg() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        byte bBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = -1;
        int bSign = -1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        assertEquals(0, aNumber.compareTo(bNumber));
    }
#endif


    /**
     * compareTo(BigInteger a).
     * Compare two numbers of different signs.
     * The first is positive.
     */
#if 0
    public void testCompareToDiffSigns1() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        byte bBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        int aSign = 1;
        int bSign = -1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        assertEquals(1, aNumber.compareTo(bNumber));
    }
#endif


    /**
     * compareTo(BigInteger a).
     * Compare two numbers of different signs.
     * The first is negative.
     */
#if 0
    public void testCompareToDiffSigns2() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        byte bBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        int aSign = -1;
        int bSign = 1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        assertEquals(-1, aNumber.compareTo(bNumber));
    }
#endif


    /**
     * compareTo(BigInteger a).
     * Compare a positive number to ZERO.
     */
#if 0
    public void testCompareToPosZero() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = 1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = BigInteger.ZERO;
        assertEquals(1, aNumber.compareTo(bNumber));
    }
#endif


    /**
     * compareTo(BigInteger a).
     * Compare ZERO to a positive number.
     */
#if 0
    public void testCompareToZeroPos() {
        byte bBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int bSign = 1;
        BigInteger aNumber = BigInteger.ZERO;
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        assertEquals(-1, aNumber.compareTo(bNumber));
    }
#endif


    /**
     * compareTo(BigInteger a).
     * Compare a negative number to ZERO.
     */
#if 0
    public void testCompareToNegZero() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = -1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = BigInteger.ZERO;
        assertEquals(-1, aNumber.compareTo(bNumber));
    }
#endif


    /**
     * compareTo(BigInteger a).
     * Compare ZERO to a negative number.
     */
#if 0
    public void testCompareToZeroNeg() {
        byte bBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int bSign = -1;
        BigInteger aNumber = BigInteger.ZERO;
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        assertEquals(1, aNumber.compareTo(bNumber));
    }
#endif


    /**
     * compareTo(BigInteger a).
     * Compare ZERO to ZERO.
     */
#if 0
    public void testCompareToZeroZero() {
        BigInteger aNumber = BigInteger.ZERO;
        BigInteger bNumber = BigInteger.ZERO;
        assertEquals(0, aNumber.compareTo(bNumber));
    }
#endif


    /**
     * equals(Object obj).
     * obj is not a BigInteger
     */
#if 0
    public void testEqualsObject() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = 1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        Object obj = new Object();
        assertFalse(aNumber.equals(obj));
    }
#endif


    /**
     * equals(null).
     */
#if 0
    public void testEqualsNull() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = 1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        assertFalse(aNumber.equals(null));
    }
#endif


    /**
     * equals(Object obj).
     * obj is a BigInteger.
     * numbers are equal.
     */
#if 0
    public void testEqualsBigIntegerTrue() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        byte bBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = 1;
        int bSign = 1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        Object bNumber = new BigInteger(bSign, bBytes);
        assertTrue(aNumber.equals(bNumber));
    }
#endif


    /**
     * equals(Object obj).
     * obj is a BigInteger.
     * numbers are not equal.
     */
#if 0
    public void testEqualsBigIntegerFalse() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        byte bBytes[] = {45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = 1;
        int bSign = 1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        Object bNumber = new BigInteger(bSign, bBytes);
        assertFalse(aNumber.equals(bNumber));
    }
#endif


    /**
     * max(BigInteger val).
     * the first is greater.
     */
#if 0
    public void testMaxGreater() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        byte bBytes[] = {45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.max(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertTrue("incorrect sign", result.signum() == 1);
    }
#endif


    /**
     * max(BigInteger val).
     * the first is less.
     */
#if 0
    public void testMaxLess() {
        byte aBytes[] = {45, 91, 3, -15, 35, 26, 3, 91};
        byte bBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.max(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertTrue("incorrect sign", result.signum() == 1);
    }
#endif


    /**
     * max(BigInteger val).
     * numbers are equal.
     */
#if 0
    public void testMaxEqual() {
        byte aBytes[] = {45, 91, 3, -15, 35, 26, 3, 91};
        byte bBytes[] = {45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {45, 91, 3, -15, 35, 26, 3, 91};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.max(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif


    /**
     * max(BigInteger val).
     * max of negative and ZERO.
     */
#if 0
    public void testMaxNegZero() {
        byte aBytes[] = {45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = -1;
        byte rBytes[] = {0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = BigInteger.ZERO;
        BigInteger result = aNumber.max(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertTrue("incorrect sign", result.signum() == 0);
    }
#endif


    /**
     * min(BigInteger val).
     * the first is greater.
     */
#if 0
    public void testMinGreater() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        byte bBytes[] = {45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {45, 91, 3, -15, 35, 26, 3, 91};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.min(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif


    /**
     * min(BigInteger val).
     * the first is less.
     */
#if 0
    public void testMinLess() {
        byte aBytes[] = {45, 91, 3, -15, 35, 26, 3, 91};
        byte bBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {45, 91, 3, -15, 35, 26, 3, 91};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.min(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif


    /**
     * min(BigInteger val).
     * numbers are equal.
     */
#if 0
    public void testMinEqual() {
        byte aBytes[] = {45, 91, 3, -15, 35, 26, 3, 91};
        byte bBytes[] = {45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {45, 91, 3, -15, 35, 26, 3, 91};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.min(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertTrue("incorrect sign", result.signum() == 1);
    }
#endif


    /**
     * max(BigInteger val).
     * min of positive and ZERO.
     */
#if 0
    public void testMinPosZero() {
        byte aBytes[] = {45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = 1;
        byte rBytes[] = {0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = BigInteger.ZERO;
        BigInteger result = aNumber.min(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertTrue("incorrect sign", result.signum() == 0);
    }
#endif


    /**
     * negate() a positive number.
     */
#if 0
    public void testNegatePositive() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = 1;
        byte rBytes[] = {-13, -57, -101, 1, 75, -90, -46, -92, -4, 14, -36, -27, -4, -91};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.negate();
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertTrue("incorrect sign", result.signum() == -1);
    }
#endif


    /**
     * negate() a negative number.
     */
#if 0
    public void testNegateNegative() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = -1;
        byte rBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.negate();
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertTrue("incorrect sign", result.signum() == 1);
    }
#endif


    /**
     * negate() ZERO.
     */
#if 0
    public void testNegateZero() {
        byte rBytes[] = {0};
        BigInteger aNumber = BigInteger.ZERO;
        BigInteger result = aNumber.negate();
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 0, result.signum());
    }
#endif


    /**
     * signum() of a positive number.
     */
#if 0
    public void testSignumPositive() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = 1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        assertEquals("incorrect sign", 1, aNumber.signum());
    }
#endif


    /**
     * signum() of a negative number.
     */
#if 0
    public void testSignumNegative() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = -1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        assertEquals("incorrect sign", -1, aNumber.signum());
    }
#endif


    /**
     * signum() of ZERO.
     */
#if 0
    public void testSignumZero() {
        BigInteger aNumber = BigInteger.ZERO;
        assertEquals("incorrect sign", 0, aNumber.signum());
    }
}
#endif

//==============================================================================

int mainBigIntegerCompareTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/BigIntegerCompareTest ====\n");
    testAbsPositive();
    testAbsNegative();
    testCompareToPosPos1();
    printf("\n==== end of libcore/math/BigIntegerCompareTest ====\n");

    return 0;
}

}
}
