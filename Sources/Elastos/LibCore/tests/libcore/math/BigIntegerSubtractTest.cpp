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
 * Method: subtract
 */
public class BigIntegerSubtractTest extends TestCase {
#endif

    /**
     * Subtract two positive numbers of the same length.
     * The first is greater.
     */
#if 0
    public void testCase1() {
        byte aBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        byte bBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {9, 18, 27, 36, 45, 54, 63, 9, 18, 27};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(1, result.signum());
    }
#endif
void testCase1()
{
    signed char _aBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
    signed char _bBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3};
    signed char _rBytes[] = {9, 18, 27, 36, 45, 54, 63, 9, 18, 27};
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

    aNumber->Subtract(bNumber, (IBigInteger**)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", 1, sign);
}


    /**
     * Subtract two positive numbers of the same length.
     * The second is greater.
     */
#if 0
    public void testCase2() {
        byte aBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3};
        byte bBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {-10, -19, -28, -37, -46, -55, -64, -10, -19, -27};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(-1, result.signum());
    }
#endif
void testCase2()
{
    signed char _aBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3};
    signed char _bBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
    signed char _rBytes[] = {-10, -19, -28, -37, -46, -55, -64, -10, -19, -27};
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

    aNumber->Subtract(bNumber, (IBigInteger**)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", -1, sign);
}

    /**
     * Subtract two numbers of the same length and different signs.
     * The first is positive.
     * The first is greater in absolute value.
     */
#if 0
    public void testCase3() {
        byte aBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        byte bBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3};
        int aSign = 1;
        int bSign = -1;
        byte rBytes[] = {11, 22, 33, 44, 55, 66, 77, 11, 22, 33};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(1, result.signum());
    }
#endif
void testCase3()
{
    signed char _aBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
    signed char _bBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3};
    signed char _rBytes[] = {11, 22, 33, 44, 55, 66, 77, 11, 22, 33};
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

    aNumber->Subtract(bNumber, (IBigInteger**)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", 1, sign);
}

    /**
     * Subtract two numbers of the same length and different signs.
     * The first is positive.
     * The second is greater in absolute value.
     */
#if 0
    public void testCase4() {
        byte aBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3};
        byte bBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        int aSign = 1;
        int bSign = -1;
        byte rBytes[] = {11, 22, 33, 44, 55, 66, 77, 11, 22, 33};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(1, result.signum());
    }
#endif
void testCase4()
{
    signed char _aBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3};
    signed char _bBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
    signed char _rBytes[] = {11, 22, 33, 44, 55, 66, 77, 11, 22, 33};
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

    aNumber->Subtract(bNumber, (IBigInteger**)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", 1, sign);
}


    /**
     * Subtract two negative numbers of the same length.
     * The first is greater in absolute value.
     */
#if 0
    public void testCase5() {
        byte aBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        byte bBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3};
        int aSign = -1;
        int bSign = -1;
        byte rBytes[] = {-10, -19, -28, -37, -46, -55, -64, -10, -19, -27};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(-1, result.signum());
    }
#endif
void testCase5()
{
    signed char _aBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
    signed char _bBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3};
    signed char _rBytes[] = {-10, -19, -28, -37, -46, -55, -64, -10, -19, -27};
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

    aNumber->Subtract(bNumber, (IBigInteger**)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", -1, sign);
}


    /**
     * Subtract two negative numbers of the same length.
     * The second is greater in absolute value.
     */
#if 0
    public void testCase6() {
        byte aBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3};
        byte bBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        int aSign = -1;
        int bSign = -1;
        byte rBytes[] = {9, 18, 27, 36, 45, 54, 63, 9, 18, 27};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(1, result.signum());
    }
#endif

    /**
     * Subtract two numbers of the same length and different signs.
     * The first is negative.
     * The first is greater in absolute value.
     */
#if 0
    public void testCase7() {
        byte aBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        byte bBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3};
        int aSign = -1;
        int bSign = 1;
        byte rBytes[] = {-12, -23, -34, -45, -56, -67, -78, -12, -23, -33};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(-1, result.signum());
    }
#endif

    /**
     * Subtract two numbers of the same length and different signs.
     * The first is negative.
     * The second is greater in absolute value.
     */
#if 0
    public void testCase8() {
        byte aBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3};
        byte bBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        int aSign = -1;
        int bSign = 1;
        byte rBytes[] = {-12, -23, -34, -45, -56, -67, -78, -12, -23, -33};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(-1, result.signum());
    }
#endif

    /**
     * Subtract two positive numbers of different length.
     * The first is longer.
     */
#if 0
    public void testCase9() {
        byte aBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7};
        byte bBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {1, 2, 3, 3, -6, -15, -24, -40, -49, -58, -67, -6, -15, -23};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(1, result.signum());
    }
#endif

    /**
     * Subtract two positive numbers of different length.
     * The second is longer.
     */
#if 0
    public void testCase10() {
        byte aBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        byte bBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(-1, result.signum());
    }
#endif

    /**
     * Subtract two numbers of different length and different signs.
     * The first is positive.
     * The first is greater in absolute value.
     */
#if 0
    public void testCase11() {
        byte aBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7};
        byte bBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        int aSign = 1;
        int bSign = -1;
        byte rBytes[] = {1, 2, 3, 4, 15, 26, 37, 41, 52, 63, 74, 15, 26, 37};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(1, result.signum());
    }
#endif

    /**
     * Subtract two numbers of the same length and different signs.
     * The first is positive.
     * The second is greater in absolute value.
     */
#if 0
    public void testCase12() {
        byte aBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        byte bBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7};
        int aSign = 1;
        int bSign = -1;
        byte rBytes[] = {1, 2, 3, 4, 15, 26, 37, 41, 52, 63, 74, 15, 26, 37};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(1, result.signum());
    }
#endif

    /**
     * Subtract two numbers of different length and different signs.
     * The first is negative.
     * The first is longer.
     */
#if 0
    public void testCase13() {
        byte aBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7};
        byte bBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        int aSign = -1;
        int bSign = 1;
        byte rBytes[] = {-2, -3, -4, -5, -16, -27, -38, -42, -53, -64, -75, -16, -27, -37};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(-1, result.signum());
    }
#endif

    /**
     * Subtract two numbers of the same length and different signs.
     * The first is negative.
     * The second is longer.
     */
#if 0
    public void testCase14() {
        byte aBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7};
        byte bBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        int aSign = -1;
        int bSign = 1;
        byte rBytes[] = {-2, -3, -4, -5, -16, -27, -38, -42, -53, -64, -75, -16, -27, -37};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(-1, result.signum());
    }
#endif

    /**
     * Subtract two negative numbers of different length.
     * The first is longer.
     */
#if 0
    public void testCase15() {
        byte aBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7};
        byte bBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        int aSign = -1;
        int bSign = -1;
        byte rBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(-1, result.signum());
}
#endif

    /**
     * Subtract two negative numbers of different length.
     * The second is longer.
     */
#if 0
    public void testCase16() {
        byte aBytes[] = {10, 20, 30, 40, 50, 60, 70, 10, 20, 30};
        byte bBytes[] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7};
        int aSign = -1;
        int bSign = -1;
        byte rBytes[] = {1, 2, 3, 3, -6, -15, -24, -40, -49, -58, -67, -6, -15, -23};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(1, result.signum());
    }
#endif

    /**
     * Subtract two positive equal in absolute value numbers.
     */
#if 0
    public void testCase17() {
        byte aBytes[] = {-120, 34, 78, -23, -111, 45, 127, 23, 45, -3};
        byte bBytes[] = {-120, 34, 78, -23, -111, 45, 127, 23, 45, -3};
        byte rBytes[] = {0};
        int aSign = 1;
        int bSign = 1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(0, result.signum());
    }
#endif

    /**
     * Subtract zero from a number.
     * The number is positive.
     */
#if 0
    public void testCase18() {
        byte aBytes[] = {120, 34, 78, -23, -111, 45, 127, 23, 45, -3};
        byte bBytes[] = {0};
        byte rBytes[] = {120, 34, 78, -23, -111, 45, 127, 23, 45, -3};
        int aSign = 1;
        int bSign = 0;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(1, result.signum());
    }
#endif

    /**
     * Subtract a number from zero.
     * The number is negative.
     */
#if 0
    public void testCase19() {
        byte aBytes[] = {0};
        byte bBytes[] = {120, 34, 78, -23, -111, 45, 127, 23, 45, -3};
        byte rBytes[] = {120, 34, 78, -23, -111, 45, 127, 23, 45, -3};
        int aSign = 0;
        int bSign = -1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(1, result.signum());
    }
#endif

    /**
     * Subtract zero from zero.
     */
#if 0
    public void testCase20() {
        byte aBytes[] = {0};
        byte bBytes[] = {0};
        byte rBytes[] = {0};
        int aSign = 0;
        int bSign = 0;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(0, result.signum());
    }
#endif

    /**
     * Subtract ZERO from a number.
     * The number is positive.
     */
#if 0
    public void testCase21() {
        byte aBytes[] = {120, 34, 78, -23, -111, 45, 127, 23, 45, -3};
        byte rBytes[] = {120, 34, 78, -23, -111, 45, 127, 23, 45, -3};
        int aSign = 1;
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = BigInteger.ZERO;
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(1, result.signum());
    }
#endif

    /**
     * Subtract a number from ZERO.
     * The number is negative.
     */
#if 0
    public void testCase22() {
        byte bBytes[] = {120, 34, 78, -23, -111, 45, 127, 23, 45, -3};
        byte rBytes[] = {120, 34, 78, -23, -111, 45, 127, 23, 45, -3};
        int bSign = -1;
        BigInteger aNumber = BigInteger.ZERO;
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(1, result.signum());
    }
#endif

    /**
     * Subtract ZERO from ZERO.
     */
#if 0
    public void testCase23() {
        byte rBytes[] = {0};
        BigInteger aNumber = BigInteger.ZERO;
        BigInteger bNumber = BigInteger.ZERO;
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(0, result.signum());
    }
#endif

    /**
     * Subtract ONE from ONE.
     */
#if 0
    public void testCase24() {
        byte rBytes[] = {0};
        BigInteger aNumber = BigInteger.ONE;
        BigInteger bNumber = BigInteger.ONE;
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(0, result.signum());
    }
#endif

    /**
     * Subtract two numbers so that borrow is 1.
     */
#if 0
    public void testCase25() {
        byte aBytes[] = {-1, -1, -1, -1, -1, -1, -1, -1};
        byte bBytes[] = {-128, -128, -128, -128, -128, -128, -128, -128, -128};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {-128, 127, 127, 127, 127, 127, 127, 127, 127};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.subtract(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals(-1, result.signum());
    }
#endif

//==============================================================================

int mainBigIntegerSubtractTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/BigIntegerSubtractTest ====\n");
    testCase1();
    testCase2();
    testCase3();
    testCase4();
    testCase5();
    printf("\n==== end of libcore/math/BigIntegerSubtractTest ====\n");

    return 0;
}

}
}

