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
using Elastos::Core::INumber;
using Elastos::Math::IBigInteger;
using Elastos::Math::CBigInteger;
using Elastos::Math::IBigDecimal;
using Elastos::Math::CBigDecimal;

using Elastos::Math::IBigDecimalHelper;
using Elastos::Math::CBigDecimalHelper;
using Elastos::Core::EIID_IComparable;
using Elastos::Core::EIID_INumber;


namespace Elastos {
namespace Math {

static void assertEquals(const char *info, Int32 aspect, Int32 test)
{
    printf("aspect: %d, test: %d. %s\n", aspect, test, info);
    assert(aspect == test);
}

static void assertEquals(const char *info, double aspect, double test)
{
    printf("aspect: %f, test: %f. %s\n", aspect, test, info);
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
 * Methods: intValue, longValue, toByteArray(), valueOf(long val),
 * floatValue(), doubleValue()
 */
public class BigIntegerConvertTest extends TestCase {
#endif
    /**
     * Return the double value of ZERO.
     */
#if 0
    public void testDoubleValueZero() {
        String a = "0";
        double result = 0.0;
        double aNumber = new BigInteger(a).doubleValue();
        assertTrue(aNumber == result);
    }
#endif
void testDoubleValueZero()
{
    String a = String("0");
    double result = 0.0;
    double bNumber;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(a, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<INumber> cNumber;
    cNumber = (INumber *)aNumber->Probe(EIID_INumber);
    cNumber->DoubleValue(&bNumber);
    assertEquals("testDoubleValueZero", bNumber, result);
}

    /**
     * Convert a positive number to a double value.
     * The number's length is less than 64 bits.
     */
#if 0
    public void testDoubleValuePositive1() {
        String a = "27467238945";
        double result = 2.7467238945E10;
        double aNumber = new BigInteger(a).doubleValue();
        assertTrue(aNumber == result);
    }
#endif
void testDoubleValuePositive1()
{
    String a = String("27467238945");
    double result = 2.7467238945E10;
    double bNumber;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(a, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<INumber> cNumber;
    cNumber = (INumber *)aNumber->Probe(EIID_INumber);
    cNumber->DoubleValue(&bNumber);
    assertEquals("testDoubleValuePositive1", bNumber, result);
}


    /**
     * Convert a positive number to a double value.
     * The number's bit length is inside [63, 1024].
     */
#if 0
    public void testDoubleValuePositive2() {
        String a = "2746723894572364578265426346273456972";
        double result = 2.7467238945723645E36;
        double aNumber = new BigInteger(a).doubleValue();
        assertTrue(aNumber == result);
    }
#endif
void testDoubleValuePositive2()
{
    String a = String("2746723894572364578265426346273456972");
    double result = 2.7467238945723645E36;
    double bNumber;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(a, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<INumber> cNumber;
    cNumber = (INumber *)aNumber->Probe(EIID_INumber);
    cNumber->DoubleValue(&bNumber);
    assertEquals("testDoubleValuePositive2", bNumber, result);
}


    /**
     * Convert a negative number to a double value.
     * The number's bit length is less than 64 bits.
     */
#if 0
    public void testDoubleValueNegative1() {
        String a = "-27467238945";
        double result = -2.7467238945E10;
        double aNumber = new BigInteger(a).doubleValue();
        assertTrue(aNumber == result);
    }
#endif
void testDoubleValueNegative1()
{
    String a = String("-27467238945");
    double result = -2.7467238945E10;
    double bNumber;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(a, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<INumber> cNumber;
    cNumber = (INumber *)aNumber->Probe(EIID_INumber);
    cNumber->DoubleValue(&bNumber);
    assertEquals("testDoubleValueNegative1", bNumber, result);
}


    /**
     * Convert a negative number to a double value.
     * The number's bit length is inside [63, 1024].
     */
#if 0
    public void testDoubleValueNegative2() {
        String a = "-2746723894572364578265426346273456972";
        double result = -2.7467238945723645E36;
        double aNumber = new BigInteger(a).doubleValue();
        assertTrue(aNumber == result);
    }
#endif
void testDoubleValueNegative2()
{
    String a = String("-2746723894572364578265426346273456972");
    double result = -2.7467238945723645E36;
    double bNumber;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(a, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<INumber> cNumber;
    cNumber = (INumber *)aNumber->Probe(EIID_INumber);
    cNumber->DoubleValue(&bNumber);
    assertEquals("testDoubleValueNegative2", bNumber, result);
}


    /**
     * Convert a positive number to a double value.
     * Rounding is needed.
     * The rounding bit is 1 and the next bit to the left is 1.
     */
#if 0
    public void testDoubleValuePosRounded1() {
        byte[] a = {-128, 1, 2, 3, 4, 5, 60, 23, 1, -3, -5};
        int aSign = 1;
        double result = 1.54747264387948E26;
        double aNumber = new BigInteger(aSign, a).doubleValue();
        assertTrue(aNumber == result);
    }
#endif
void testDoubleValuePosRounded1()
{
    double result = 1.54747264387948E26;
    double bNumber;

    signed char _aBytes[] = {-128, 1, 2, 3, 4, 5, 60, 23, 1, -3, -5};
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(sizeof(_aBytes));
    memcpy(aBytes->GetPayload(), _aBytes, sizeof(_aBytes));

    int aSign = 1;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(aSign, *aBytes, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<INumber> cNumber;
    cNumber = (INumber *)aNumber->Probe(EIID_INumber);
    cNumber->DoubleValue(&bNumber);
    assertEquals("testDoubleValuePosRounded1", bNumber, result);
}


    /**
     * Convert a positive number to a double value.
     * Rounding is needed.
     * The rounding bit is 1 and the next bit to the left is 0
     * but some of dropped bits are 1s.
     */
#if 0
    public void testDoubleValuePosRounded2() {
        byte[] a = {-128, 1, 2, 3, 4, 5, 36, 23, 1, -3, -5};
        int aSign = 1;
        double result = 1.547472643879479E26;
        double aNumber = new BigInteger(aSign, a).doubleValue();
        assertTrue(aNumber == result);
    }
#endif
void testDoubleValuePosRounded2()
{
    double result = 1.547472643879479E26;
    double bNumber;

    signed char _aBytes[] = {-128, 1, 2, 3, 4, 5, 36, 23, 1, -3, -5};
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(sizeof(_aBytes));
    memcpy(aBytes->GetPayload(), _aBytes, sizeof(_aBytes));

    int aSign = 1;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(aSign, *aBytes, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<INumber> cNumber;
    cNumber = (INumber *)aNumber->Probe(EIID_INumber);
    cNumber->DoubleValue(&bNumber);
    assertEquals("testDoubleValuePosRounded2", bNumber, result);
}

        /**
     * Convert a positive number to a double value.
     * Rounding is NOT needed.
     */
#if 0
    public void testDoubleValuePosNotRounded() {
        byte[] a = {-128, 1, 2, 3, 4, 5, -128, 23, 1, -3, -5};
        int aSign = 1;
        double result = 1.5474726438794828E26;
        double aNumber = new BigInteger(aSign, a).doubleValue();
        assertTrue(aNumber == result);
    }
#endif
void testDoubleValuePosNotRounded()
{
    double result = 1.5474726438794828E26;
    double bNumber;

    signed char _aBytes[] = {-128, 1, 2, 3, 4, 5, -128, 23, 1, -3, -5};
    AutoPtr<ArrayOf<Byte> > aBytes = ArrayOf<Byte>::Alloc(sizeof(_aBytes));
    memcpy(aBytes->GetPayload(), _aBytes, sizeof(_aBytes));

    int aSign = 1;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(aSign, *aBytes, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<INumber> cNumber;
    cNumber = (INumber *)aNumber->Probe(EIID_INumber);
    cNumber->DoubleValue(&bNumber);
    assertEquals("testDoubleValuePosNotRounded", bNumber, result);
}


    /**
     * Convert a positive number to a double value.
     * Rounding is needed.
     */
#if 0
    public void testDoubleValueNegRounded1() {
        byte[] a = {-128, 1, 2, 3, 4, 5, 60, 23, 1, -3, -5};
        int aSign = -1;
        double result = -1.54747264387948E26;
        double aNumber = new BigInteger(aSign, a).doubleValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a positive number to a double value.
     * Rounding is needed.
     * The rounding bit is 1 and the next bit to the left is 0
     * but some of dropped bits are 1s.
     */
#if 0
    public void testDoubleValueNegRounded2() {
        byte[] a = {-128, 1, 2, 3, 4, 5, 36, 23, 1, -3, -5};
        int aSign = -1;
        double result = -1.547472643879479E26;
        double aNumber = new BigInteger(aSign, a).doubleValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a positive number to a double value.
     * Rounding is NOT needed.
     */
#if 0
    public void testDoubleValueNegNotRounded() {
        byte[] a = {-128, 1, 2, 3, 4, 5, -128, 23, 1, -3, -5};
        int aSign = -1;
        double result = -1.5474726438794828E26;
        double aNumber = new BigInteger(aSign, a).doubleValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a positive number to a double value.
     * The exponent is 1023 and the mantissa is all 1s.
     * The rounding bit is 0.
     * The result is Double.MAX_VALUE.
     */
#if 0
    public void testDoubleValuePosMaxValue() {
        byte[] a = {0, -1, -1, -1, -1, -1, -1, -8, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
           };
        int aSign = 1;
        double aNumber = new BigInteger(aSign, a).doubleValue();
        assertTrue(aNumber == Double.MAX_VALUE);
    }
#endif

    /**
     * Convert a negative number to a double value.
     * The exponent is 1023 and the mantissa is all 1s.
     * The result is -Double.MAX_VALUE.
     */
#if 0
    public void testDoubleValueNegMaxValue() {
        byte[] a = {0, -1, -1, -1, -1, -1, -1, -8, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
           };
        int aSign = -1;
        double aNumber = new BigInteger(aSign, a).doubleValue();
        assertTrue(aNumber == -Double.MAX_VALUE);
    }
#endif

    /**
     * Convert a positive number to a double value.
     * The exponent is 1023 and the mantissa is all 1s.
     * The rounding bit is 1.
     * The result is Double.POSITIVE_INFINITY.
     */
#if 0
    public void testDoubleValuePositiveInfinity1() {
        byte[] a = {-1, -1, -1, -1, -1, -1, -1, -8, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
           };
        int aSign = 1;
        double aNumber = new BigInteger(aSign, a).doubleValue();
        assertTrue(aNumber == Double.POSITIVE_INFINITY);
    }
#endif

    /**
     * Convert a positive number to a double value.
     * The number's bit length is greater than 1024.
     */
#if 0
    public void testDoubleValuePositiveInfinity2() {
        String a = "2746723894572364578265426346273456972283746872364768676747462342342342342342342342323423423423423423426767456345745293762384756238475634563456845634568934568347586346578648576478568456457634875673845678456786587345873645767456834756745763457863485768475678465783456702897830296720476846578634576384567845678346573465786457863";
        double aNumber = new BigInteger(a).doubleValue();
        assertTrue(aNumber == Double.POSITIVE_INFINITY);
    }
#endif

    /**
     * Convert a negative number to a double value.
     * The number's bit length is greater than 1024.
     */
#if 0
    public void testDoubleValueNegativeInfinity1() {
        String a = "-2746723894572364578265426346273456972283746872364768676747462342342342342342342342323423423423423423426767456345745293762384756238475634563456845634568934568347586346578648576478568456457634875673845678456786587345873645767456834756745763457863485768475678465783456702897830296720476846578634576384567845678346573465786457863";
        double aNumber = new BigInteger(a).doubleValue();
        assertTrue(aNumber == Double.NEGATIVE_INFINITY);
    }
#endif

    /**
     * Convert a negative number to a double value.
     * The exponent is 1023 and the mantissa is all 0s.
     * The rounding bit is 0.
     * The result is Double.NEGATIVE_INFINITY.
     */
#if 0
    public void testDoubleValueNegativeInfinity2() {
        byte[] a = {-1, -1, -1, -1, -1, -1, -1, -8, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
           };
        int aSign = -1;
        double aNumber = new BigInteger(aSign, a).doubleValue();
        assertTrue(aNumber == Double.NEGATIVE_INFINITY);
    }
#endif

    /**
     * Convert a positive number to a double value.
     * The exponent is 1023 and the mantissa is all 0s
     * but the 54th bit (implicit) is 1.
     */
#if 0
    public void testDoubleValuePosMantissaIsZero() {
        byte[] a = {-128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
           };
        int aSign = 1;
        double result = 8.98846567431158E307;
        double aNumber = new BigInteger(aSign, a).doubleValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a positive number to a double value.
     * The exponent is 1023 and the mantissa is all 0s
     * but the 54th bit (implicit) is 1.
     */
#if 0
    public void testDoubleValueNegMantissaIsZero() {
        byte[] a = {-128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
           };
        int aSign = -1;
        double aNumber = new BigInteger(aSign, a).doubleValue();
        assertTrue(aNumber == -8.98846567431158E307);
    }
#endif

    /**
     * Return the float value of ZERO.
     */
#if 0
    public void testFloatValueZero() {
        String a = "0";
        float result = 0.0f;
        float aNumber = new BigInteger(a).floatValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a positive number to a float value.
     * The number's length is less than 32 bits.
     */
#if 0
    public void testFloatValuePositive1() {
        String a = "27467238";
        float result = 2.7467238E7f;
        float aNumber = new BigInteger(a).floatValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a positive number to a float value.
     * The number's bit length is inside [32, 127].
     */
#if 0
    public void testFloatValuePositive2() {
        String a = "27467238945723645782";
        float result = 2.7467239E19f;
        float aNumber = new BigInteger(a).floatValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a negative number to a float value.
     * The number's bit length is less than 32 bits.
     */
#if 0
    public void testFloatValueNegative1() {
        String a = "-27467238";
        float result = -2.7467238E7f;
        float aNumber = new BigInteger(a).floatValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a negative number to a doufloatble value.
     * The number's bit length is inside [63, 1024].
     */
#if 0
    public void testFloatValueNegative2() {
        String a = "-27467238945723645782";
        float result = -2.7467239E19f;
        float aNumber = new BigInteger(a).floatValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a positive number to a float value.
     * Rounding is needed.
     * The rounding bit is 1 and the next bit to the left is 1.
     */
#if 0
    public void testFloatValuePosRounded1() {
        byte[] a = {-128, 1, -1, -4, 4, 5, 60, 23, 1, -3, -5};
        int aSign = 1;
        float result = 1.5475195E26f;
        float aNumber = new BigInteger(aSign, a).floatValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a positive number to a float value.
     * Rounding is needed.
     * The rounding bit is 1 and the next bit to the left is 0
     * but some of dropped bits are 1s.
     */
#if 0
    public void testFloatValuePosRounded2() {
        byte[] a = {-128, 1, 2, -128, 4, 5, 60, 23, 1, -3, -5};
        int aSign = 1;
        float result = 1.5474728E26f;
        float aNumber = new BigInteger(aSign, a).floatValue();
        assertTrue(aNumber == result);
    }
#endif

        /**
     * Convert a positive number to a float value.
     * Rounding is NOT needed.
     */
#if 0
    public void testFloatValuePosNotRounded() {
        byte[] a = {-128, 1, 2, 3, 4, 5, 60, 23, 1, -3, -5};
        int aSign = 1;
        float result = 1.5474726E26f;
        float aNumber = new BigInteger(aSign, a).floatValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a positive number to a float value.
     * Rounding is needed.
     */
#if 0
    public void testFloatValueNegRounded1() {
        byte[] a = {-128, 1, -1, -4, 4, 5, 60, 23, 1, -3, -5};
        int aSign = -1;
        float result = -1.5475195E26f;
        float aNumber = new BigInteger(aSign, a).floatValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a positive number to a float value.
     * Rounding is needed.
     * The rounding bit is 1 and the next bit to the left is 0
     * but some of dropped bits are 1s.
     */
#if 0
    public void testFloatValueNegRounded2() {
        byte[] a = {-128, 1, 2, -128, 4, 5, 60, 23, 1, -3, -5};
        int aSign = -1;
        float result = -1.5474728E26f;
        float aNumber = new BigInteger(aSign, a).floatValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a positive number to a float value.
     * Rounding is NOT needed.
     */
#if 0
    public void testFloatValueNegNotRounded() {
        byte[] a = {-128, 1, 2, 3, 4, 5, 60, 23, 1, -3, -5};
        int aSign = -1;
        float result = -1.5474726E26f;
        float aNumber = new BigInteger(aSign, a).floatValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a positive number to a float value.
     * The exponent is 1023 and the mantissa is all 1s.
     * The rounding bit is 0.
     * The result is Float.MAX_VALUE.
     */
#if 0
    public void testFloatValuePosMaxValue() {
        byte[] a = {0, -1, -1, -1, 0, -1, -1, -8, -1, -1, -1, -1, -1, -1, -1, -1, -1};
        int aSign = 1;
        float aNumber = new BigInteger(aSign, a).floatValue();
        assertTrue(aNumber == Float.MAX_VALUE);
    }
#endif

    /**
     * Convert a negative number to a float value.
     * The exponent is 1023 and the mantissa is all 1s.
     * The rounding bit is 0.
     * The result is -Float.MAX_VALUE.
     */
#if 0
    public void testFloatValueNegMaxValue() {
        byte[] a = {0, -1, -1, -1, 0, -1, -1, -8, -1, -1, -1, -1, -1, -1, -1, -1, -1};
        int aSign = -1;
        float aNumber = new BigInteger(aSign, a).floatValue();
        assertTrue(aNumber == -Float.MAX_VALUE);
    }
#endif

    /**
     * Convert a positive number to a float value.
     * The exponent is 1023 and the mantissa is all 1s.
     * The rounding bit is 1.
     * The result is Float.POSITIVE_INFINITY.
     */
#if 0
    public void testFloatValuePositiveInfinity1() {
        byte[] a = {0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
        int aSign = 1;
        float aNumber = new BigInteger(aSign, a).floatValue();
        assertTrue(aNumber == Float.POSITIVE_INFINITY);
    }
#endif

    /**
     * Convert a positive number to a float value.
     * The number's bit length is greater than 127.
     */
#if 0
    public void testFloatValuePositiveInfinity2() {
        String a = "2746723894572364578265426346273456972283746872364768676747462342342342342342342342323423423423423423426767456345745293762384756238475634563456845634568934568347586346578648576478568456457634875673845678456786587345873645767456834756745763457863485768475678465783456702897830296720476846578634576384567845678346573465786457863";
        float aNumber = new BigInteger(a).floatValue();
        assertTrue(aNumber == Float.POSITIVE_INFINITY);
    }
#endif

    /**
     * Convert a negative number to a float value.
     * The number's bit length is greater than 127.
     */
#if 0
    public void testFloatValueNegativeInfinity1() {
        String a = "-2746723894572364578265426346273456972283746872364768676747462342342342342342342342323423423423423423426767456345745293762384756238475634563456845634568934568347586346578648576478568456457634875673845678456786587345873645767456834756745763457863485768475678465783456702897830296720476846578634576384567845678346573465786457863";
        float aNumber = new BigInteger(a).floatValue();
        assertTrue(aNumber == Float.NEGATIVE_INFINITY);
    }
#endif

    /**
     * Convert a negative number to a float value.
     * The exponent is 1023 and the mantissa is all 0s.
     * The rounding bit is 0.
     * The result is Float.NEGATIVE_INFINITY.
     */
#if 0
    public void testFloatValueNegativeInfinity2() {
        byte[] a = {0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
        int aSign = -1;
        float aNumber = new BigInteger(aSign, a).floatValue();
        assertTrue(aNumber == Float.NEGATIVE_INFINITY);
    }
#endif

    /**
     * Convert a positive number to a float value.
     * The exponent is 1023 and the mantissa is all 0s
     * but the 54th bit (implicit) is 1.
     */
#if 0
    public void testFloatValuePosMantissaIsZero() {
        byte[] a = {-128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        int aSign = 1;
        float result = 1.7014118E38f;
        float aNumber = new BigInteger(aSign, a).floatValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a positive number to a double value.
     * The exponent is 1023 and the mantissa is all 0s
     * but the 54th bit (implicit) is 1.
     */
#if 0
    public void testFloatValueNegMantissaIsZero() {
        byte[] a = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        int aSign = -1;
        float aNumber = new BigInteger(aSign, a).floatValue();
        assertTrue(aNumber == Float.NEGATIVE_INFINITY);
    }
#endif

    /**
     * Convert a negative number to a float value.
     * The number's bit length is less than 32 bits.
     */
#if 0
    public void testFloatValueBug2482() {
        String a = "2147483649";
        float result = 2.14748365E9f;
        float aNumber = new BigInteger(a).floatValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a positive BigInteger to an integer value.
     * The low digit is positive
     */
#if 0
    public void testIntValuePositive1() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3};
        int resInt = 1496144643;
        int aNumber = new BigInteger(aBytes).intValue();
        assertTrue(aNumber == resInt);
    }
#endif

    /**
     * Convert a positive BigInteger to an integer value.
     * The low digit is positive
     */
#if 0
    public void testIntValuePositive2() {
        byte aBytes[] = {12, 56, 100};
        int resInt = 800868;
        int aNumber = new BigInteger(aBytes).intValue();
        assertTrue(aNumber == resInt);
    }
#endif

    /**
     * Convert a positive BigInteger to an integer value.
     * The low digit is negative.
     */
#if 0
    public void testIntValuePositive3() {
        byte aBytes[] = {56, 13, 78, -12, -5, 56, 100};
        int sign = 1;
        int resInt = -184862620;
        int aNumber = new BigInteger(sign, aBytes).intValue();
        assertTrue(aNumber == resInt);
    }
#endif

    /**
     * Convert a negative BigInteger to an integer value.
     * The low digit is negative.
     */
#if 0
    public void testIntValueNegative1() {
        byte aBytes[] = {12, 56, 100, -2, -76, -128, 45, 91, 3};
        int sign = -1;
        int resInt = 2144511229;
        int aNumber = new BigInteger(sign, aBytes).intValue();
        assertTrue(aNumber == resInt);
    }
#endif

    /**
     * Convert a negative BigInteger to an integer value.
     * The low digit is negative.
     */
#if 0
    public void testIntValueNegative2() {
        byte aBytes[] = {-12, 56, 100};
        int result = -771996;
        int aNumber = new BigInteger(aBytes).intValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a negative BigInteger to an integer value.
     * The low digit is positive.
     */
#if 0
    public void testIntValueNegative3() {
        byte aBytes[] = {12, 56, 100, -2, -76, 127, 45, 91, 3};
        int sign = -1;
        int resInt = -2133678851;
        int aNumber = new BigInteger(sign, aBytes).intValue();
        assertTrue(aNumber == resInt);
    }
#endif

    /**
     * Convert a BigInteger to a positive long value
     * The BigInteger is longer than int.
     */
#if 0
    public void testLongValuePositive1() {
        byte aBytes[] = {12, 56, 100, -2, -76, 89, 45, 91, 3, 120, -34, -12, 45, 98};
        long result = 3268209772258930018L;
        long aNumber = new BigInteger(aBytes).longValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a number to a positive long value
     * The number fits in a long.
     */
#if 0
    public void testLongValuePositive2() {
        byte aBytes[] = {12, 56, 100, 18, -105, 34, -18, 45};
        long result = 880563758158769709L;
        long aNumber = new BigInteger(aBytes).longValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a number to a negative long value
     * The BigInteger is longer than int.
     */
#if 0
    public void testLongValueNegative1() {
        byte aBytes[] = {12, -1, 100, -2, -76, -128, 45, 91, 3};
        long result = -43630045168837885L;
        long aNumber = new BigInteger(aBytes).longValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * Convert a number to a negative long value
     * The number fits in a long.
     */
#if 0
    public void testLongValueNegative2() {
        byte aBytes[] = {-12, 56, 100, 45, -101, 45, 98};
        long result = -3315696807498398L;
        long aNumber = new BigInteger(aBytes).longValue();
        assertTrue(aNumber == result);
    }
#endif

    /**
     * valueOf (long val): convert Integer.MAX_VALUE to a BigInteger.
     */
#if 0
    public void testValueOfIntegerMax() {
        long longVal = Integer.MAX_VALUE;
        BigInteger aNumber = BigInteger.valueOf(longVal);
        byte rBytes[] = {127, -1, -1, -1};
        byte resBytes[] = new byte[rBytes.length];
        resBytes = aNumber.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, aNumber.signum());
    }
#endif

    /**
     * valueOf (long val): convert Integer.MIN_VALUE to a BigInteger.
     */
#if 0
    public void testValueOfIntegerMin() {
        long longVal = Integer.MIN_VALUE;
        BigInteger aNumber = BigInteger.valueOf(longVal);
        byte rBytes[] = {-128, 0, 0, 0};
        byte resBytes[] = new byte[rBytes.length];
        resBytes = aNumber.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, aNumber.signum());
    }
#endif

    /**
     * valueOf (long val): convert Long.MAX_VALUE to a BigInteger.
     */
#if 0
    public void testValueOfLongMax() {
        long longVal = Long.MAX_VALUE;
        BigInteger aNumber = BigInteger.valueOf(longVal);
        byte rBytes[] = {127, -1, -1, -1, -1, -1, -1, -1};
        byte resBytes[] = new byte[rBytes.length];
        resBytes = aNumber.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, aNumber.signum());
    }
#endif

    /**
     * valueOf (long val): convert Long.MIN_VALUE to a BigInteger.
     */
#if 0
    public void testValueOfLongMin() {
        long longVal = Long.MIN_VALUE;
        BigInteger aNumber = BigInteger.valueOf(longVal);
        byte rBytes[] = {-128, 0, 0, 0, 0, 0, 0, 0};
        byte resBytes[] = new byte[rBytes.length];
        resBytes = aNumber.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, aNumber.signum());
    }
#endif

    /**
     * valueOf (long val): convert a positive long value to a BigInteger.
     */
#if 0
    public void testValueOfLongPositive1() {
        long longVal = 268209772258930018L;
        BigInteger aNumber = BigInteger.valueOf(longVal);
        byte rBytes[] = {3, -72, -33, 93, -24, -56, 45, 98};
        byte resBytes[] = new byte[rBytes.length];
        resBytes = aNumber.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, aNumber.signum());
    }
#endif

    /**
     * valueOf (long val): convert a positive long value to a BigInteger.
     * The long value fits in integer.
     */
#if 0
    public void testValueOfLongPositive2() {
        long longVal = 58930018L;
        BigInteger aNumber = BigInteger.valueOf(longVal);
        byte rBytes[] = {3, -125, 51, 98};
        byte resBytes[] = new byte[rBytes.length];
        resBytes = aNumber.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, aNumber.signum());
    }
#endif

    /**
     * valueOf (long val): convert a negative long value to a BigInteger.
     */
#if 0
    public void testValueOfLongNegative1() {
        long longVal = -268209772258930018L;
        BigInteger aNumber = BigInteger.valueOf(longVal);
        byte rBytes[] = {-4, 71, 32, -94, 23, 55, -46, -98};
        byte resBytes[] = new byte[rBytes.length];
        resBytes = aNumber.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, aNumber.signum());
    }
#endif

    /**
     * valueOf (long val): convert a negative long value to a BigInteger.
     * The long value fits in integer.
     */
#if 0
    public void testValueOfLongNegative2() {
        long longVal = -58930018L;
        BigInteger aNumber = BigInteger.valueOf(longVal);
        byte rBytes[] = {-4, 124, -52, -98};
        byte resBytes[] = new byte[rBytes.length];
        resBytes = aNumber.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, aNumber.signum());
    }
#endif

    /**
     * valueOf (long val): convert a zero long value to a BigInteger.
     */
#if 0
    public void testValueOfLongZero() {
        long longVal = 0L;
        BigInteger aNumber = BigInteger.valueOf(longVal);
        byte rBytes[] = {0};
        byte resBytes[] = new byte[rBytes.length];
        resBytes = aNumber.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 0, aNumber.signum());
    }
#endif

//==============================================================================

int mainBigIntegerConvertTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/BigIntegerConvertTest ====\n");
    testDoubleValueZero();
    testDoubleValuePositive1();
    testDoubleValuePositive2();
    testDoubleValueNegative1();
    testDoubleValueNegative2();
    testDoubleValuePosRounded1();
    testDoubleValuePosRounded2();
    testDoubleValuePosNotRounded();
    printf("\n==== end of libcore/math/BigIntegerConvertTest ====\n");

    return 0;
}

}
}
