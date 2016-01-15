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
 * Methods: bitLength, shiftLeft, shiftRight,
 * clearBit, flipBit, setBit, testBit
 */
public class BigIntegerOperateBitsTest extends TestCase {
#endif

    /**
     * bitCount() of zero.
     */
#if 0
    public void testBitCountZero() {
        BigInteger aNumber = new BigInteger("0");
        assertEquals(0, aNumber.bitCount());
    }
#endif
void testBitCountZero()
{
    String a = String("0");
    Int32 bNumber;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(a, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    aNumber->BitCount(&bNumber);
    assertEquals("testBitCountZero", bNumber, 0);
}


    /**
     * bitCount() of a negative number.
     */
#if 0
    public void testBitCountNeg() {
        BigInteger aNumber = new BigInteger("-12378634756382937873487638746283767238657872368748726875");
        assertEquals(87, aNumber.bitCount());
    }
#endif
void testBitCountNeg()
{
    String a = String("-12378634756382937873487638746283767238657872368748726875");
    Int32 bNumber;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(a, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    aNumber->BitCount(&bNumber);
    assertEquals("testBitCountNeg", bNumber, 87);
}


    /**
     * bitCount() of a negative number.
     */
#if 0
    public void testBitCountPos() {
        BigInteger aNumber = new BigInteger("12378634756343564757582937873487638746283767238657872368748726875");
        assertEquals(107, aNumber.bitCount());
    }
#endif
void testBitCountPos()
{
    String a = String("12378634756343564757582937873487638746283767238657872368748726875");
    Int32 bNumber;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(a, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    aNumber->BitCount(&bNumber);
    assertEquals("testBitCountNeg", bNumber, 107);
}
    /**
     * bitLength() of zero.
     */
#if 0
    public void testBitLengthZero() {
        BigInteger aNumber = new BigInteger("0");
        assertEquals(0, aNumber.bitLength());
    }
#endif

    /**
     * bitLength() of a positive number.
     */
#if 0
    public void testBitLengthPositive1() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = 1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        assertEquals(108, aNumber.bitLength());
    }
#endif
void testBitLengthPositive1()
{
    Int32 bNumber;

    signed char _aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(sizeof(_aBytes));
    memcpy(aBytes->GetPayload(), _aBytes, sizeof(_aBytes));

    int aSign = 1;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(aSign, *aBytes, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    aNumber->BitLength(&bNumber);
    assertEquals("testBitLengthPositive2", bNumber, 108);
}
    /**
     * bitLength() of a positive number with the leftmost bit set
     */
#if 0
    public void testBitLengthPositive2() {
        byte aBytes[] = {-128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        assertEquals(96, aNumber.bitLength());
    }
#endif
void testBitLengthPositive2()
{
    Int32 bNumber;

    signed char _aBytes[] = {-128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(sizeof(_aBytes));
    memcpy(aBytes->GetPayload(), _aBytes, sizeof(_aBytes));

    int aSign = 1;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(aSign, *aBytes, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    aNumber->BitLength(&bNumber);
    assertEquals("testBitLengthPositive2", bNumber, 96);
}

    /**
     * bitLength() of a positive number which is a power of 2
     */
#if 0
    public void testBitLengthPositive3() {
        byte aBytes[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        int aSign = 1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        assertEquals(81, aNumber.bitLength());
    }
#endif

    /**
     * bitLength() of a negative number.
     */
#if 0
    public void testBitLengthNegative1() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, 3, 91};
        int aSign = -1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        assertEquals(108, aNumber.bitLength());
    }
#endif

    /**
     * bitLength() of a negative number with the leftmost bit set
     */
#if 0
    public void testBitLengthNegative2() {
        byte aBytes[] = {-128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = -1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        assertEquals(96, aNumber.bitLength());
    }
#endif

    /**
     * bitLength() of a negative number which is a power of 2
     */
#if 0
    public void testBitLengthNegative3() {
        byte aBytes[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        int aSign = -1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        assertEquals(80, aNumber.bitLength());
    }
#endif

    /**
     * clearBit(int n) of a negative n
     */
#if 0
    public void testClearBitException() {
        byte aBytes[] = {-1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = -7;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        try {
            aNumber.clearBit(number);
            fail("ArithmeticException has not been caught");
        } catch (ArithmeticException e) {
        }
    }
#endif

    /**
     * clearBit(int n) outside zero
     */
#if 0
    public void testClearBitZero() {
        byte aBytes[] = {0};
        int aSign = 0;
        int number = 0;
        byte rBytes[] = {0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.clearBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 0, result.signum());
    }
#endif

    /**
     * clearBit(int n) outside zero
     */
#if 0
    public void testClearBitZeroOutside1() {
        byte aBytes[] = {0};
        int aSign = 0;
        int number = 95;
        byte rBytes[] = {0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.clearBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 0, result.signum());
    }
#endif

    /**
     * clearBit(int n) inside a negative number
     */
#if 0
    public void testClearBitNegativeInside1() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = -1;
        int number = 15;
        byte rBytes[] = {-2, 127, -57, -101, 1, 75, -90, -46, -92, -4, 14, 92, -26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.clearBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * clearBit(int n) inside a negative number
     */
#if 0
    public void testClearBitNegativeInside2() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = -1;
        int number = 44;
        byte rBytes[] = {-2, 127, -57, -101, 1, 75, -90, -62, -92, -4, 14, -36, -26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.clearBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * clearBit(2) in the negative number with all ones in bit representation
     */
#if 0
    public void testClearBitNegativeInside3() {
        String as = "-18446744073709551615";
        int number = 2;
        BigInteger aNumber = new BigInteger(as);
        BigInteger result = aNumber.clearBit(number);
        assertEquals(as, result.toString());
    }
#endif

    /**
     * clearBit(0) in the negative number of length 1
     * with all ones in bit representation.
     * the resulting number's length is 2.
     */
#if 0
    public void testClearBitNegativeInside4() {
        String as = "-4294967295";
        String res = "-4294967296";
        int number = 0;
        BigInteger aNumber = new BigInteger(as);
        BigInteger result = aNumber.clearBit(number);
        assertEquals(res, result.toString());
    }
#endif

    /**
     * clearBit(0) in the negative number of length 2
     * with all ones in bit representation.
     * the resulting number's length is 3.
     */
#if 0
    public void testClearBitNegativeInside5() {
        String as = "-18446744073709551615";
        String res = "-18446744073709551616";
        int number = 0;
        BigInteger aNumber = new BigInteger(as);
        BigInteger result = aNumber.clearBit(number);
        assertEquals(res, result.toString());
    }
#endif

    /**
     * clearBit(int n) outside a negative number
     */
#if 0
    public void testClearBitNegativeOutside1() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = -1;
        int number = 150;
        byte rBytes[] = {-65, -1, -1, -1, -1, -1, -2, 127, -57, -101, 1, 75, -90, -46, -92, -4, 14, -36, -26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.clearBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * clearBit(int n) outside a negative number
     */
#if 0
    public void testClearBitNegativeOutside2() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = -1;
        int number = 165;
        byte rBytes[] = {-33, -1, -1, -1, -1, -1, -1, -1, -2, 127, -57, -101, 1, 75, -90, -46, -92, -4, 14, -36, -26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.clearBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * clearBit(int n) inside a positive number
     */
#if 0
    public void testClearBitPositiveInside1() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 20;
        byte rBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -31, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.clearBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * clearBit(int n) inside a positive number
     */
#if 0
    public void testClearBitPositiveInside2() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 17;
        byte rBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.clearBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * clearBit(int n) inside a positive number
     */
#if 0
    public void testClearBitPositiveInside3() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 45;
        byte rBytes[] = {1, -128, 56, 100, -2, -76, 89, 13, 91, 3, -15, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.clearBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * clearBit(int n) inside a positive number
     */
#if 0
    public void testClearBitPositiveInside4 () {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 50;
        byte rBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.clearBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * clearBit(int n) inside a positive number
     */
#if 0
    public void testClearBitPositiveInside5 () {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 63;
        byte rBytes[] = {1, -128, 56, 100, -2, 52, 89, 45, 91, 3, -15, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.clearBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * clearBit(int n) outside a positive number
     */
#if 0
    public void testClearBitPositiveOutside1() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 150;
        byte rBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.clearBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * clearBit(int n) outside a positive number
     */
#if 0
    public void testClearBitPositiveOutside2() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 191;
        byte rBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.clearBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * clearBit(int n) the leftmost bit in a negative number
     */
#if 0
    public void testClearBitTopNegative() {
        byte aBytes[] = {1, -128, 56, 100, -15, 35, 26};
        int aSign = -1;
        int number = 63;
        byte rBytes[] = {-1, 127, -2, 127, -57, -101, 14, -36, -26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.clearBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * flipBit(int n) of a negative n
     */
#if 0
    public void testFlipBitException() {
        byte aBytes[] = {-1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = -7;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        try {
            aNumber.flipBit(number);
            fail("ArithmeticException has not been caught");
        } catch (ArithmeticException e) {
        }
    }
#endif

    /**
     * flipBit(int n) zero
     */
#if 0
    public void testFlipBitZero() {
        byte aBytes[] = {0};
        int aSign = 0;
        int number = 0;
        byte rBytes[] = {1};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.flipBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * flipBit(int n) outside zero
     */
#if 0
    public void testFlipBitZeroOutside1() {
        byte aBytes[] = {0};
        int aSign = 0;
        int number = 62;
        byte rBytes[] = {64, 0, 0, 0, 0, 0, 0, 0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.flipBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue("incorrect value", resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * flipBit(int n) outside zero
     */
#if 0
    public void testFlipBitZeroOutside2() {
        byte aBytes[] = {0};
        int aSign = 0;
        int number = 63;
        byte rBytes[] = {0, -128, 0, 0, 0, 0, 0, 0, 0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.flipBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue("incorrect value", resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * flipBit(int n) the leftmost bit in a negative number
     */
#if 0
    public void testFlipBitLeftmostNegative() {
        byte aBytes[] = {1, -128, 56, 100, -15, 35, 26};
        int aSign = -1;
        int number = 48;
        byte rBytes[] = {-1, 127, -57, -101, 14, -36, -26, 49};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.flipBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * flipBit(int n) the leftmost bit in a positive number
     */
#if 0
    public void testFlipBitLeftmostPositive() {
        byte aBytes[] = {1, -128, 56, 100, -15, 35, 26};
        int aSign = 1;
        int number = 48;
        byte rBytes[] = {0, -128, 56, 100, -15, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.flipBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * flipBit(int n) inside a negative number
     */
#if 0
    public void testFlipBitNegativeInside1() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = -1;
        int number = 15;
        byte rBytes[] = {-2, 127, -57, -101, 1, 75, -90, -46, -92, -4, 14, 92, -26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.flipBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * flipBit(int n) inside a negative number
     */
#if 0
    public void testFlipBitNegativeInside2() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = -1;
        int number = 45;
        byte rBytes[] = {-2, 127, -57, -101, 1, 75, -90, -14, -92, -4, 14, -36, -26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.flipBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * flipBit(int n) inside a negative number with all ones in bit representation
     */
#if 0
    public void testFlipBitNegativeInside3() {
        String as = "-18446744073709551615";
        String res = "-18446744073709551611";
        int number = 2;
        BigInteger aNumber = new BigInteger(as);
        BigInteger result = aNumber.flipBit(number);
        assertEquals(res, result.toString());
    }
#endif

    /**
     * flipBit(0) in the negative number of length 1
     * with all ones in bit representation.
     * the resulting number's length is 2.
     */
#if 0
    public void testFlipBitNegativeInside4() {
        String as = "-4294967295";
        String res = "-4294967296";
        int number = 0;
        BigInteger aNumber = new BigInteger(as);
        BigInteger result = aNumber.flipBit(number);
        assertEquals(res, result.toString());
    }
#endif

    /**
     * flipBit(0) in the negative number of length 2
     * with all ones in bit representation.
     * the resulting number's length is 3.
     */
#if 0
    public void testFlipBitNegativeInside5() {
        String as = "-18446744073709551615";
        String res = "-18446744073709551616";
        int number = 0;
        BigInteger aNumber = new BigInteger(as);
        BigInteger result = aNumber.flipBit(number);
        assertEquals(res, result.toString());
    }
#endif

    /**
     * flipBit(int n) outside a negative number
     */
#if 0
    public void testFlipBitNegativeOutside1() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = -1;
        int number = 150;
        byte rBytes[] = {-65, -1, -1, -1, -1, -1, -2, 127, -57, -101, 1, 75, -90, -46, -92, -4, 14, -36, -26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.flipBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * flipBit(int n) outside a negative number
     */
#if 0
    public void testFlipBitNegativeOutside2() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = -1;
        int number = 191;
        byte rBytes[] = {-1, 127, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -2, 127, -57, -101, 1, 75, -90, -46, -92, -4, 14, -36, -26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.flipBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * flipBit(int n) inside a positive number
     */
#if 0
    public void testFlipBitPositiveInside1() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 15;
        byte rBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, -93, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.flipBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * flipBit(int n) inside a positive number
     */
#if 0
    public void testFlipBitPositiveInside2() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 45;
        byte rBytes[] = {1, -128, 56, 100, -2, -76, 89, 13, 91, 3, -15, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.flipBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * flipBit(int n) outside a positive number
     */
#if 0
    public void testFlipBitPositiveOutside1() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 150;
        byte rBytes[] = {64, 0, 0, 0, 0, 0, 1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.flipBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * flipBit(int n) outside a positive number
     */
#if 0
    public void testFlipBitPositiveOutside2() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 191;
        byte rBytes[] = {0, -128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.flipBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * setBit(int n) of a negative n
     */
#if 0
    public void testSetBitException() {
        byte aBytes[] = {-1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = -7;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        try {
            aNumber.setBit(number);
            fail("ArithmeticException has not been caught");
        } catch (ArithmeticException e) {
        }
    }
#endif

    /**
     * setBit(int n) outside zero
     */
#if 0
    public void testSetBitZero() {
        byte aBytes[] = {0};
        int aSign = 0;
        int number = 0;
        byte rBytes[] = {1};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.setBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * setBit(int n) outside zero
     */
#if 0
    public void testSetBitZeroOutside1() {
        byte aBytes[] = {0};
        int aSign = 0;
        int number = 95;
        byte rBytes[] = {0, -128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.setBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * setBit(int n) inside a positive number
     */
#if 0
    public void testSetBitPositiveInside1() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 20;
        byte rBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.setBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * setBit(int n) inside a positive number
     */
#if 0
    public void testSetBitPositiveInside2() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 17;
        byte rBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -13, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.setBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * setBit(int n) inside a positive number
     */
#if 0
    public void testSetBitPositiveInside3() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 45;
        byte rBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.setBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * setBit(int n) inside a positive number
     */
#if 0
    public void testSetBitPositiveInside4 () {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 50;
        byte rBytes[] = {1, -128, 56, 100, -2, -76, 93, 45, 91, 3, -15, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.setBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * setBit(int n) outside a positive number
     */
#if 0
    public void testSetBitPositiveOutside1() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 150;
        byte rBytes[] = {64, 0, 0, 0, 0, 0, 1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.setBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif
    /**
     * setBit(int n) outside a positive number
     */
#if 0
    public void testSetBitPositiveOutside2() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 223;
        byte rBytes[] = {0, -128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.setBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif
    /**
     * setBit(int n) the leftmost bit in a positive number
     */
#if 0
    public void testSetBitTopPositive() {
        byte aBytes[] = {1, -128, 56, 100, -15, 35, 26};
        int aSign = 1;
        int number = 63;
        byte rBytes[] = {0, -128, 1, -128, 56, 100, -15, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.setBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif
    /**
     * setBit(int n) the leftmost bit in a negative number
     */
#if 0
    public void testSetBitLeftmostNegative() {
        byte aBytes[] = {1, -128, 56, 100, -15, 35, 26};
        int aSign = -1;
        int number = 48;
        byte rBytes[] = {-1, 127, -57, -101, 14, -36, -26, 49};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.setBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif
    /**
     * setBit(int n) inside a negative number
     */
#if 0
    public void testSetBitNegativeInside1() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = -1;
        int number = 15;
        byte rBytes[] = {-2, 127, -57, -101, 1, 75, -90, -46, -92, -4, 14, -36, -26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.setBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif
    /**
     * setBit(int n) inside a negative number
     */
#if 0
    public void testSetBitNegativeInside2() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = -1;
        int number = 44;
        byte rBytes[] = {-2, 127, -57, -101, 1, 75, -90, -46, -92, -4, 14, -36, -26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.setBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif
    /**
     * setBit(int n) inside a negative number with all ones in bit representation
     */
#if 0
    public void testSetBitNegativeInside3() {
        String as = "-18446744073709551615";
        String res = "-18446744073709551611";
        int number = 2;
        BigInteger aNumber = new BigInteger(as);
        BigInteger result = aNumber.setBit(number);
        assertEquals(res, result.toString());
    }
#endif

    /**
     * setBit(0) in the negative number of length 1
     * with all ones in bit representation.
     * the resulting number's length is 2.
     */
#if 0
    public void testSetBitNegativeInside4() {
        String as = "-4294967295";
        int number = 0;
        BigInteger aNumber = new BigInteger(as);
        BigInteger result = aNumber.setBit(number);
        assertEquals(as, result.toString());
    }
#endif

    /**
     * setBit(0) in the negative number of length 2
     * with all ones in bit representation.
     * the resulting number's length is 3.
     */
#if 0
    public void testSetBitNegativeInside5() {
        String as = "-18446744073709551615";
        int number = 0;
        BigInteger aNumber = new BigInteger(as);
        BigInteger result = aNumber.setBit(number);
        assertEquals(as, result.toString());
    }
#endif

    /**
     * setBit(int n) outside a negative number
     */
#if 0
    public void testSetBitNegativeOutside1() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = -1;
        int number = 150;
        byte rBytes[] = {-2, 127, -57, -101, 1, 75, -90, -46, -92, -4, 14, -36, -26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.setBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * setBit(int n) outside a negative number
     */
#if 0
    public void testSetBitNegativeOutside2() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = -1;
        int number = 191;
        byte rBytes[] = {-2, 127, -57, -101, 1, 75, -90, -46, -92, -4, 14, -36, -26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.setBit(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * setBit: check the case when the number of bit to be set can be
     * represented as n * 32 + 31, where n is an arbitrary integer.
     * Here 191 = 5 * 32 + 31
     */
#if 0
    public void testSetBitBug1331() {
        BigInteger result = BigInteger.valueOf(0L).setBit(191);
        assertEquals("incorrect value", "3138550867693340381917894711603833208051177722232017256448", result.toString());
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * shiftLeft(int n), n = 0
     */
#if 0
    public void testShiftLeft1() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 0;
        byte rBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.shiftLeft(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * shiftLeft(int n), n < 0
     */
#if 0
    public void testShiftLeft2() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = -27;
        byte rBytes[] = {48, 7, 12, -97, -42, -117, 37, -85, 96};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.shiftLeft(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * shiftLeft(int n) a positive number, n > 0
     */
#if 0
    public void testShiftLeft3() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 27;
        byte rBytes[] = {12, 1, -61, 39, -11, -94, -55, 106, -40, 31, -119, 24, -48, 0, 0, 0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.shiftLeft(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * shiftLeft(int n) a positive number, n > 0
     */
#if 0
    public void testShiftLeft4() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 45;
        byte rBytes[] = {48, 7, 12, -97, -42, -117, 37, -85, 96, 126, 36, 99, 64, 0, 0, 0, 0, 0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.shiftLeft(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * shiftLeft(int n) a negative number, n > 0
     */
#if 0
    public void testShiftLeft5() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = -1;
        int number = 45;
        byte rBytes[] = {-49, -8, -13, 96, 41, 116, -38, 84, -97, -127, -37, -100, -64, 0, 0, 0, 0, 0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.shiftLeft(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * shiftRight(int n), n = 0
     */
#if 0
    public void testShiftRight1() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 0;
        byte rBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.shiftRight(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * shiftRight(int n), n < 0
     */
#if 0
    public void testShiftRight2() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = -27;
        byte rBytes[] = {12, 1, -61, 39, -11, -94, -55, 106, -40, 31, -119, 24, -48, 0, 0, 0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.shiftRight(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * shiftRight(int n), 0 < n < 32
     */
#if 0
    public void testShiftRight3() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 27;
        byte rBytes[] = {48, 7, 12, -97, -42, -117, 37, -85, 96};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.shiftRight(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * shiftRight(int n), n > 32
     */
#if 0
    public void testShiftRight4() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 45;
        byte rBytes[] = {12, 1, -61, 39, -11, -94, -55};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.shiftRight(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * shiftRight(int n), n is greater than bitLength()
     */
#if 0
    public void testShiftRight5() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 300;
        byte rBytes[] = {0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.shiftRight(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 0, result.signum());
    }
#endif

    /**
     * shiftRight a negative number;
     * shift distance is multiple of 32;
     * shifted bits are NOT zeroes.
     */
#if 0
    public void testShiftRightNegNonZeroesMul32() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 1, 0, 0, 0, 0, 0, 0, 0};
        int aSign = -1;
        int number = 64;
        byte rBytes[] = {-2, 127, -57, -101, 1, 75, -90, -46, -92};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.shiftRight(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * shiftRight a negative number;
     * shift distance is NOT multiple of 32;
     * shifted bits are NOT zeroes.
     */
#if 0
    public void testShiftRightNegNonZeroes() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 0, 0, 0, 0, 0, 0, 0, 0};
        int aSign = -1;
        int number = 68;
        byte rBytes[] = {-25, -4, 121, -80, 20, -70, 109, 42};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.shiftRight(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * shiftRight a negative number;
     * shift distance is NOT multiple of 32;
     * shifted bits are zeroes.
     */
#if 0
    public void testShiftRightNegZeroes() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        int aSign = -1;
        int number = 68;
        byte rBytes[] = {-25, -4, 121, -80, 20, -70, 109, 48};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.shiftRight(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * shiftRight a negative number;
     * shift distance is multiple of 32;
     * shifted bits are zeroes.
     */
#if 0
    public void testShiftRightNegZeroesMul32() {
        byte aBytes[] = {1, -128, 56, 100, -2, -76, 89, 45, 91, 0, 0, 0, 0, 0, 0, 0, 0};
        int aSign = -1;
        int number = 64;
        byte rBytes[] = {-2, 127, -57, -101, 1, 75, -90, -46, -91};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.shiftRight(number);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * testBit(int n) of a negative n
     */
#if 0
    public void testTestBitException() {
        byte aBytes[] = {-1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = -7;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        try {
            aNumber.testBit(number);
            fail("ArithmeticException has not been caught");
        } catch (ArithmeticException e) {
        }
    }
#endif

    /**
     * testBit(int n) of a positive number
     */
#if 0
    public void testTestBitPositive1() {
        byte aBytes[] = {-1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 7;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        assertTrue(!aNumber.testBit(number));
    }
#endif

    /**
     * testBit(int n) of a positive number
     */
#if 0
    public void testTestBitPositive2() {
        byte aBytes[] = {-1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 45;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        assertTrue(aNumber.testBit(number));
    }
#endif

    /**
     * testBit(int n) of a positive number, n > bitLength()
     */
#if 0
    public void testTestBitPositive3() {
        byte aBytes[] = {-1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = 1;
        int number = 300;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        assertTrue(!aNumber.testBit(number));
    }
#endif

    /**
     * testBit(int n) of a negative number
     */
#if 0
    public void testTestBitNegative1() {
        byte aBytes[] = {-1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = -1;
        int number = 7;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        assertTrue(aNumber.testBit(number));
    }
#endif

    /**
     * testBit(int n) of a positive n
     */
#if 0
    public void testTestBitNegative2() {
        byte aBytes[] = {-1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = -1;
        int number = 45;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        assertTrue(!aNumber.testBit(number));
    }
#endif

    /**
     * testBit(int n) of a positive n, n > bitLength()
     */
#if 0
    public void testTestBitNegative3() {
        byte aBytes[] = {-1, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26};
        int aSign = -1;
        int number = 300;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        assertTrue(aNumber.testBit(number));
    }
#endif
//==============================================================================

int mainBigIntegerOperateBitsTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/BigIntegerOperateBitsTest ====\n");
    testBitCountZero();
    testBitCountNeg();
    testBitCountPos();
    testBitLengthPositive1();
    testBitLengthPositive2();
    printf("\n==== end of libcore/math/BigIntegerOperateBitsTest ====\n");

    return 0;
}

}
}

