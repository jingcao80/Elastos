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

static void assertEquals(const char *info, Int64 aspect, Int64 test)
{
    printf("aspect: %lld, test: %lld. %s\n", aspect, test, info);
    assert(aspect == test);
}

static void assertEquals(const char *info, Double aspect, Double test)
{
    printf("aspect: %f, test: %f. %s\n", aspect, test, info);
    assert(aspect == test);
}

static void assertEquals(const char *info, Float aspect, Float test)
{
    printf("aspect: %f, test: %f. %s\n", aspect, test, info);
    assert(aspect == test);
}

static void assertEquals(const char *info, String aspect, String test)
{
    printf("aspect: %s, test: %s. %s\n", aspect.string(), test.string(), info);
    assert(aspect.Equals(test) == 0);
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
import java.math.BigDecimal;
import java.math.BigInteger;

/**
 * Class:  java.math.BigDecimal
 * Methods: doubleValue, floatValue, intValue, longValue,
 * valueOf, toString, toBigInteger
 */
public class BigDecimalConvertTest extends TestCase {
#endif

    /**
     * Double value of a negative BigDecimal
     */

#if 0
    public void testDoubleValueNeg() {
        String a = "-123809648392384754573567356745735.63567890295784902768787678287E+21";
        BigDecimal aNumber = new BigDecimal(a);
        double result = -1.2380964839238476E53;
        assertEquals("incorrect value", result, aNumber.doubleValue(), 0);
    }
#endif
void testDoubleValueNeg()
{
    String a = String("-123809648392384754573567356745735.63567890295784902768787678287E+21");
    double result = -1.2380964839238476E53;
    Double bNumber;

    AutoPtr<IBigDecimal> aNumber;
    ECode ec = CBigDecimal::New(a, (IBigDecimal**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    AutoPtr<INumber> cNumber;
    cNumber = (INumber *)aNumber->Probe(EIID_INumber);
    cNumber->DoubleValue(&bNumber);
    assertEquals("incorrect value", bNumber, result);
}

    /**
     * Double value of a positive BigDecimal
     */
#if 0
    public void testDoubleValuePos() {
        String a = "123809648392384754573567356745735.63567890295784902768787678287E+21";
        BigDecimal aNumber = new BigDecimal(a);
        double result = 1.2380964839238476E53;
        assertEquals("incorrect value", result, aNumber.doubleValue(), 0);
    }
#endif
void testDoubleValuePos()
{
    String a = String("123809648392384754573567356745735.63567890295784902768787678287E+21");
    double result = 1.2380964839238476E53;
    Double bNumber;

    AutoPtr<IBigDecimal> aNumber;
    ECode ec = CBigDecimal::New(a, (IBigDecimal**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    AutoPtr<INumber> cNumber;
    cNumber = (INumber *)aNumber->Probe(EIID_INumber);
    cNumber->DoubleValue(&bNumber);
    assertEquals("incorrect value", bNumber, result);
}

    /**
     * Double value of a large positive BigDecimal
     */
#if 0
    public void testDoubleValuePosInfinity() {
        String a = "123809648392384754573567356745735.63567890295784902768787678287E+400";
        BigDecimal aNumber = new BigDecimal(a);
        double result = Double.POSITIVE_INFINITY;
        assertEquals("incorrect value", result, aNumber.doubleValue(), 0);
    }
#endif
void testDoubleValuePosInfinity()
{
    const Double DOUBLE_POSITIVE_INFINITY = 1.0/0.0;

    String a = String("123809648392384754573567356745735.63567890295784902768787678287E+400");
    double result = DOUBLE_POSITIVE_INFINITY;
    Double bNumber;

    AutoPtr<IBigDecimal> aNumber;
    ECode ec = CBigDecimal::New(a, (IBigDecimal**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    AutoPtr<INumber> cNumber;
    cNumber = (INumber *)aNumber->Probe(EIID_INumber);
    cNumber->DoubleValue(&bNumber);
    assertEquals("incorrect value", bNumber, result);
}

    /**
     * Double value of a large negative BigDecimal
     */
#if 0
    public void testDoubleValueNegInfinity() {
        String a = "-123809648392384754573567356745735.63567890295784902768787678287E+400";
        BigDecimal aNumber = new BigDecimal(a);
        double result = Double.NEGATIVE_INFINITY;
        assertEquals("incorrect value", result, aNumber.doubleValue(), 0);
    }
#endif
void testDoubleValueNegInfinity()
{
    const Double DOUBLE_NEGATIVE_INFINITY = -1.0/0.0;

    String a = String("123809648392384754573567356745735.63567890295784902768787678287E+400");
    double result = DOUBLE_NEGATIVE_INFINITY;
    Double bNumber;

    AutoPtr<IBigDecimal> aNumber;
    ECode ec = CBigDecimal::New(a, (IBigDecimal**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    AutoPtr<INumber> cNumber;
    cNumber = (INumber *)aNumber->Probe(EIID_INumber);
    cNumber->DoubleValue(&bNumber);
    assertEquals("incorrect value", bNumber, result);
}

    /**
     * Double value of a small negative BigDecimal
     */
#if 0
    public void testDoubleValueMinusZero() {
        String a = "-123809648392384754573567356745735.63567890295784902768787678287E-400";
        BigDecimal aNumber = new BigDecimal(a);
        long minusZero = -9223372036854775808L;
        double result = aNumber.doubleValue();
        assertTrue("incorrect value", Double.doubleToLongBits(result) == minusZero);
    }
#endif
void testDoubleValueMinusZero()
{
    String a = String("-123809648392384754573567356745735.63567890295784902768787678287E-400");
    double result;
    Int64 minusZero = -9223372036854775808L;
    Double bNumber;

    AutoPtr<IBigDecimal> aNumber;
    ECode ec = CBigDecimal::New(a, (IBigDecimal**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    AutoPtr<INumber> cNumber;
    cNumber = (INumber *)aNumber->Probe(EIID_INumber);
    cNumber->DoubleValue(&bNumber);

    assertEquals("incorrect value", *((Int64 *)&bNumber), minusZero);
}
    /**
     * Double value of a small positive BigDecimal
     */
#if 0
    public void testDoubleValuePlusZero() {
        String a = "123809648392384754573567356745735.63567890295784902768787678287E-400";
        BigDecimal aNumber = new BigDecimal(a);
        long zero = 0;
        double result = aNumber.doubleValue();
        assertTrue("incorrect value", Double.doubleToLongBits(result) == zero);
    }
#endif
void testDoubleValuePlusZero()
{
    String a = String("123809648392384754573567356745735.63567890295784902768787678287E-400");
    double result;
    Int64 zero = 0;
    Double bNumber;

    AutoPtr<IBigDecimal> aNumber;
    ECode ec = CBigDecimal::New(a, (IBigDecimal**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    AutoPtr<INumber> cNumber;
    cNumber = (INumber *)aNumber->Probe(EIID_INumber);
    cNumber->DoubleValue(&bNumber);

    assertEquals("incorrect value", *((Int64 *)&bNumber), zero);
}

    /**
     * Float value of a negative BigDecimal
     */
#if 0
    public void testFloatValueNeg() {
        String a = "-1238096483923847.6356789029578E+21";
        BigDecimal aNumber = new BigDecimal(a);
        float result = -1.2380965E36F;
        assertTrue("incorrect value", aNumber.floatValue() == result);
    }
#endif
void testFloatValueNeg()
{
    String a = String("-1238096483923847.6356789029578E+21");
    float result = -1.2380965E36F;
    Float bNumber;

    AutoPtr<IBigDecimal> aNumber;
    ECode ec = CBigDecimal::New(a, (IBigDecimal**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    AutoPtr<INumber> cNumber;
    cNumber = (INumber *)aNumber->Probe(EIID_INumber);
    cNumber->FloatValue(&bNumber);

    assertEquals("incorrect value", bNumber, result);
}

    /**
     * Float value of a positive BigDecimal
     */
#if 0
    public void testFloatValuePos() {
        String a = "1238096483923847.6356789029578E+21";
        BigDecimal aNumber = new BigDecimal(a);
        float result = 1.2380965E36F;
        assertTrue("incorrect value", aNumber.floatValue() == result);
    }
#endif
void testFloatValuePos()
{
    String a = String("1238096483923847.6356789029578E+21");
    float result = 1.2380965E36F;
    Float bNumber;

    AutoPtr<IBigDecimal> aNumber;
    ECode ec = CBigDecimal::New(a, (IBigDecimal**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    AutoPtr<INumber> cNumber;
    cNumber = (INumber *)aNumber->Probe(EIID_INumber);
    cNumber->FloatValue(&bNumber);

    assertEquals("incorrect value", bNumber, result);
}
    /**
     * Float value of a large positive BigDecimal
     */
#if 0
    public void testFloatValuePosInfinity() {
        String a = "123809648373567356745735.6356789787678287E+200";
        BigDecimal aNumber = new BigDecimal(a);
        float result = Float.POSITIVE_INFINITY;
        assertTrue("incorrect value", aNumber.floatValue() == result);
    }
#endif

    /**
     * Float value of a large negative BigDecimal
     */
#if 0
    public void testFloatValueNegInfinity() {
        String a = "-123809648392384755735.63567887678287E+200";
        BigDecimal aNumber = new BigDecimal(a);
        float result = Float.NEGATIVE_INFINITY;
        assertTrue("incorrect value", aNumber.floatValue() == result);
    }
#endif

    /**
     * Float value of a small negative BigDecimal
     */
#if 0
    public void testFloatValueMinusZero() {
        String a = "-123809648392384754573567356745735.63567890295784902768787678287E-400";
        BigDecimal aNumber = new BigDecimal(a);
        int minusZero = -2147483648;
        float result = aNumber.floatValue();
        assertTrue("incorrect value", Float.floatToIntBits(result) == minusZero);
    }
#endif

    /**
     * Float value of a small positive BigDecimal
     */
#if 0
    public void testFloatValuePlusZero() {
        String a = "123809648392384754573567356745735.63567890295784902768787678287E-400";
        BigDecimal aNumber = new BigDecimal(a);
        int zero = 0;
        float result = aNumber.floatValue();
        assertTrue("incorrect value", Float.floatToIntBits(result) == zero);
    }
#endif

    /**
     * Integer value of a negative BigDecimal
     */
#if 0
    public void testIntValueNeg() {
        String a = "-123809648392384754573567356745735.63567890295784902768787678287E+21";
        BigDecimal aNumber = new BigDecimal(a);
        int result = 218520473;
        assertTrue("incorrect value", aNumber.intValue() == result);
    }
#endif

    /**
     * Integer value of a positive BigDecimal
     */
#if 0
    public void testIntValuePos() {
        String a = "123809648392384754573567356745735.63567890295784902768787678287E+21";
        BigDecimal aNumber = new BigDecimal(a);
        int result = -218520473;
        assertTrue("incorrect value", aNumber.intValue() == result);
    }
#endif

    /**
     * Long value of a negative BigDecimal
     */
#if 0
    public void testLongValueNeg() {
        String a = "-123809648392384754573567356745735.63567890295784902768787678287E+21";
        BigDecimal aNumber = new BigDecimal(a);
        long result = -1246043477766677607L;
        assertTrue("incorrect value", aNumber.longValue() == result);
    }
#endif

    /**
     * Long value of a positive BigDecimal
     */
#if 0
    public void testLongValuePos() {
        String a = "123809648392384754573567356745735.63567890295784902768787678287E+21";
        BigDecimal aNumber = new BigDecimal(a);
        long result = 1246043477766677607L;
        assertTrue("incorrect value", aNumber.longValue() == result);
    }
#endif

    /**
     * scaleByPowerOfTen(int n)
     */
#if 0
    public void testScaleByPowerOfTen1() {
        String a = "1231212478987482988429808779810457634781384756794987";
        int aScale = 13;
        BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
        BigDecimal result = aNumber.scaleByPowerOfTen(10);
        String res = "1231212478987482988429808779810457634781384756794.987";
        int resScale = 3;
        assertEquals("incorrect value", res, result.toString());
        assertEquals("incorrect scale", resScale, result.scale());
    }
#endif

    /**
     * scaleByPowerOfTen(int n)
     */
#if 0
    public void testScaleByPowerOfTen2() {
        String a = "1231212478987482988429808779810457634781384756794987";
        int aScale = -13;
        BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
        BigDecimal result = aNumber.scaleByPowerOfTen(10);
        String res = "1.231212478987482988429808779810457634781384756794987E+74";
        int resScale = -23;
        assertEquals("incorrect value", res, result.toString());
        assertEquals("incorrect scale", resScale, result.scale());
    }
#endif

    /**
     * Convert a positive BigDecimal to BigInteger
     */
#if 0
    public void testToBigIntegerPos1() {
        String a = "123809648392384754573567356745735.63567890295784902768787678287E+21";
        BigInteger bNumber = new BigInteger("123809648392384754573567356745735635678902957849027687");
        BigDecimal aNumber = new BigDecimal(a);
        BigInteger result = aNumber.toBigInteger();
        assertTrue("incorrect value", result.equals(bNumber));
    }
#endif

    /**
     * Convert a positive BigDecimal to BigInteger
     */
#if 0
    public void testToBigIntegerPos2() {
        String a = "123809648392384754573567356745735.63567890295784902768787678287E+15";
        BigInteger bNumber = new BigInteger("123809648392384754573567356745735635678902957849");
        BigDecimal aNumber = new BigDecimal(a);
        BigInteger result = aNumber.toBigInteger();
        assertTrue("incorrect value", result.equals(bNumber));
    }
#endif

    /**
     * Convert a positive BigDecimal to BigInteger
     */
#if 0
    public void testToBigIntegerPos3() {
        String a = "123809648392384754573567356745735.63567890295784902768787678287E+45";
        BigInteger bNumber = new BigInteger("123809648392384754573567356745735635678902957849027687876782870000000000000000");
        BigDecimal aNumber = new BigDecimal(a);
        BigInteger result = aNumber.toBigInteger();
        assertTrue("incorrect value", result.equals(bNumber));
    }
#endif

    /**
     * Convert a negative BigDecimal to BigInteger
     */
#if 0
    public void testToBigIntegerNeg1() {
        String a = "-123809648392384754573567356745735.63567890295784902768787678287E+21";
        BigInteger bNumber = new BigInteger("-123809648392384754573567356745735635678902957849027687");
        BigDecimal aNumber = new BigDecimal(a);
        BigInteger result = aNumber.toBigInteger();
        assertTrue("incorrect value", result.equals(bNumber));
    }
#endif

    /**
     * Convert a negative BigDecimal to BigInteger
     */
#if 0
    public void testToBigIntegerNeg2() {
        String a = "-123809648392384754573567356745735.63567890295784902768787678287E+15";
        BigInteger bNumber = new BigInteger("-123809648392384754573567356745735635678902957849");
        BigDecimal aNumber = new BigDecimal(a);
        BigInteger result = aNumber.toBigInteger();
        assertTrue("incorrect value", result.equals(bNumber));
    }
#endif

    /**
     * Convert a negative BigDecimal to BigInteger
     */
#if 0
    public void testToBigIntegerNeg3() {
        String a = "-123809648392384754573567356745735.63567890295784902768787678287E+45";
        BigInteger bNumber = new BigInteger("-123809648392384754573567356745735635678902957849027687876782870000000000000000");
        BigDecimal aNumber = new BigDecimal(a);
        BigInteger result = aNumber.toBigInteger();
         assertTrue("incorrect value", result.equals(bNumber));
    }
#endif

    /**
     * Convert a small BigDecimal to BigInteger
     */
#if 0
    public void testToBigIntegerZero() {
        String a = "-123809648392384754573567356745735.63567890295784902768787678287E-500";
        BigInteger bNumber = new BigInteger("0");
        BigDecimal aNumber = new BigDecimal(a);
        BigInteger result = aNumber.toBigInteger();
        assertTrue("incorrect value", result.equals(bNumber));
    }
#endif

    /**
     * toBigIntegerExact()
     */
#if 0
    public void testToBigIntegerExact1() {
        String a = "-123809648392384754573567356745735.63567890295784902768787678287E+45";
        BigDecimal aNumber = new BigDecimal(a);
        String res = "-123809648392384754573567356745735635678902957849027687876782870000000000000000";
        BigInteger result = aNumber.toBigIntegerExact();
        assertEquals("incorrect value", res, result.toString());
    }
#endif

    /**
     * toBigIntegerExact()
     */
#if 0
    public void testToBigIntegerExactException() {
        String a = "-123809648392384754573567356745735.63567890295784902768787678287E-10";
        BigDecimal aNumber = new BigDecimal(a);
        try {
            aNumber.toBigIntegerExact();
            fail("java.lang.ArithmeticException has not been thrown");
        } catch (java.lang.ArithmeticException e) {
            return;
        }
    }
#endif

    /**
     * Convert a positive BigDecimal to an engineering string representation
     */
#if 0
    public void testToEngineeringStringPos() {
        String a = "123809648392384754573567356745735.63567890295784902768787678287E-501";
        BigDecimal aNumber = new BigDecimal(a);
        String result = "123.80964839238475457356735674573563567890295784902768787678287E-471";
        assertEquals("incorrect value", result, aNumber.toEngineeringString());
    }
#endif

    /**
     * Convert a negative BigDecimal to an engineering string representation
     */
#if 0
    public void testToEngineeringStringNeg() {
        String a = "-123809648392384754573567356745735.63567890295784902768787678287E-501";
        BigDecimal aNumber = new BigDecimal(a);
        String result = "-123.80964839238475457356735674573563567890295784902768787678287E-471";
        assertEquals("incorrect value", result, aNumber.toEngineeringString());
    }
#endif

    /**
     * Convert a negative BigDecimal to an engineering string representation
     */
#if 0
    public void testToEngineeringStringZeroPosExponent() {
        String a = "0.0E+16";
        BigDecimal aNumber = new BigDecimal(a);
        String result = "0E+15";
        assertEquals("incorrect value", result, aNumber.toEngineeringString());
    }
#endif

    /**
     * Convert a negative BigDecimal to an engineering string representation
     */
#if 0
    public void testToEngineeringStringZeroNegExponent() {
        String a = "0.0E-16";
        BigDecimal aNumber = new BigDecimal(a);
        String result = "0.00E-15";
        assertEquals("incorrect value", result, aNumber.toEngineeringString());
    }
#endif

    /**
     * Convert a negative BigDecimal with a negative exponent to a plain string
     * representation; scale == 0.
     */
 #if 0
    public void testToPlainStringNegNegExp() {
        String a = "-123809648392384754573567356745735.63567890295784902768787678287E-100";
        BigDecimal aNumber = new BigDecimal(a);
        String result = "-0.000000000000000000000000000000000000000000000000000000000000000000012380964839238475457356735674573563567890295784902768787678287";
        assertTrue("incorrect value", aNumber.toPlainString().equals(result));
    }
#endif

    /**
     * Convert a negative BigDecimal with a positive exponent
     * to a plain string representation;
     * scale == 0.
     */
 #if 0
    public void testToPlainStringNegPosExp() {
        String a = "-123809648392384754573567356745735.63567890295784902768787678287E100";
        BigDecimal aNumber = new BigDecimal(a);
        String result = "-1238096483923847545735673567457356356789029578490276878767828700000000000000000000000000000000000000000000000000000000000000000000000";
        assertTrue("incorrect value", aNumber.toPlainString().equals(result));
    }
#endif

    /**
     * Convert a positive BigDecimal with a negative exponent
     * to a plain string representation;
     * scale == 0.
     */
 #if 0
    public void testToPlainStringPosNegExp() {
        String a = "123809648392384754573567356745735.63567890295784902768787678287E-100";
        BigDecimal aNumber = new BigDecimal(a);
        String result = "0.000000000000000000000000000000000000000000000000000000000000000000012380964839238475457356735674573563567890295784902768787678287";
        assertTrue("incorrect value", aNumber.toPlainString().equals(result));
    }
#endif

    /**
     * Convert a negative BigDecimal with a negative exponent
     * to a plain string representation;
     * scale == 0.
     */
 #if 0
    public void testToPlainStringPosPosExp() {
        String a = "123809648392384754573567356745735.63567890295784902768787678287E+100";
        BigDecimal aNumber = new BigDecimal(a);
        String result = "1238096483923847545735673567457356356789029578490276878767828700000000000000000000000000000000000000000000000000000000000000000000000";
        assertTrue("incorrect value", aNumber.toPlainString().equals(result));
    }
#endif

    /**
     * Convert a BigDecimal to a string representation;
     * scale == 0.
     */
 #if 0
    public void testToStringZeroScale() {
        String a = "-123809648392384754573567356745735635678902957849027687876782870";
        BigDecimal aNumber = new BigDecimal(new BigInteger(a));
        String result = "-123809648392384754573567356745735635678902957849027687876782870";
        assertTrue("incorrect value", aNumber.toString().equals(result));
    }
#endif

    /**
     * Convert a positive BigDecimal to a string representation
     */
#if 0
    public void testToStringPos() {
        String a = "123809648392384754573567356745735.63567890295784902768787678287E-500";
        BigDecimal aNumber = new BigDecimal(a);
        String result = "1.2380964839238475457356735674573563567890295784902768787678287E-468";
        assertTrue("incorrect value", aNumber.toString().equals(result));
    }
#endif

    /**
     * Convert a negative BigDecimal to a string representation
     */
#if 0
    public void testToStringNeg() {
        String a = "-123.4564563673567380964839238475457356735674573563567890295784902768787678287E-5";
        BigDecimal aNumber = new BigDecimal(a);
        String result = "-0.001234564563673567380964839238475457356735674573563567890295784902768787678287";
        assertTrue("incorrect value", aNumber.toString().equals(result));
    }
#endif

    /**
     * Create a BigDecimal from a positive long value; scale == 0
     */
#if 0
    public void testValueOfPosZeroScale() {
        long a = 98374823947823578L;
        BigDecimal aNumber = BigDecimal.valueOf(a);
        String result = "98374823947823578";
        assertTrue("incorrect value", aNumber.toString().equals(result));
    }
#endif

    /**
     * Create a BigDecimal from a negative long value; scale is 0
     */
#if 0
    public void testValueOfNegZeroScale() {
        long a = -98374823947823578L;
        BigDecimal aNumber = BigDecimal.valueOf(a);
        String result = "-98374823947823578";
        assertTrue("incorrect value", aNumber.toString().equals(result));
    }
#endif

    /**
     * Create a BigDecimal from a negative long value; scale is positive
     */
#if 0
    public void testValueOfNegScalePos() {
        long a = -98374823947823578L;
        int scale = 12;
        BigDecimal aNumber = BigDecimal.valueOf(a, scale);
        String result = "-98374.823947823578";
        assertTrue("incorrect value", aNumber.toString().equals(result));
    }
#endif

    /**
     * Create a BigDecimal from a negative long value; scale is negative
     */
#if 0
    public void testValueOfNegScaleNeg() {
        long a = -98374823947823578L;
        int scale = -12;
        BigDecimal aNumber = BigDecimal.valueOf(a, scale);
        String result = "-9.8374823947823578E+28";
        assertTrue("incorrect value", aNumber.toString().equals(result));
    }
#endif

    /**
     * Create a BigDecimal from a negative long value; scale is positive
     */
#if 0
    public void testValueOfPosScalePos() {
        long a = 98374823947823578L;
        int scale = 12;
        BigDecimal aNumber = BigDecimal.valueOf(a, scale);
        String result = "98374.823947823578";
        assertTrue("incorrect value", aNumber.toString().equals(result));
    }
#endif

    /**
     * Create a BigDecimal from a negative long value; scale is negative
     */
#if 0
    public void testValueOfPosScaleNeg() {
        long a = 98374823947823578L;
        int scale = -12;
        BigDecimal aNumber = BigDecimal.valueOf(a, scale);
        String result = "9.8374823947823578E+28";
        assertTrue("incorrect value", aNumber.toString().equals(result));
    }
#endif

    /**
     * Create a BigDecimal from a negative double value
     */
#if 0
    public void testValueOfDoubleNeg() {
        double a = -65678765876567576.98788767;
        BigDecimal result = BigDecimal.valueOf(a);
        String res = "-65678765876567576";
        int resScale = 0;
        assertEquals("incorrect value", res, result.toString());
        assertEquals("incorrect scale", resScale, result.scale());
    }
#endif

    /**
     * Create a BigDecimal from a positive double value
     */
#if 0
    public void testValueOfDoublePos1() {
        double a = 65678765876567576.98788767;
        BigDecimal result = BigDecimal.valueOf(a);
        String res = "65678765876567576";
        int resScale = 0;
        assertEquals("incorrect value", res, result.toString());
        assertEquals("incorrect scale", resScale, result.scale());
    }
#endif

    /**
     * Create a BigDecimal from a positive double value
     */
#if 0
    public void testValueOfDoublePos2() {
        double a = 12321237576.98788767;
        BigDecimal result = BigDecimal.valueOf(a);
        String res = "12321237576.987888";
        int resScale = 6;
        assertEquals("incorrect value", res, result.toString());
        assertEquals("incorrect scale", resScale, result.scale());
    }
#endif

    /**
     * Create a BigDecimal from a positive double value
     */
#if 0
    public void testValueOfDoublePos3() {
        double a = 12321237576.9878838;
        BigDecimal result = BigDecimal.valueOf(a);
        String res = "12321237576.987885";
        int resScale = 6;
        assertEquals("incorrect value", res, result.toString());
        assertEquals("incorrect scale", resScale, result.scale());
    }
#endif

    /**
     * valueOf(Double.NaN)
     */
#if 0
    public void testValueOfDoubleNaN() {
        double a = Double.NaN;
        try {
            BigDecimal.valueOf(a);
            fail("NumberFormatException has not been thrown for Double.NaN");
        } catch (NumberFormatException e) {
            return;
        }
    }
#endif


//==============================================================================

int mainBigDecimalConvertTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/BigDecimalConvertTest ====\n");
    testDoubleValueNeg();
    testDoubleValuePos();
    testDoubleValuePosInfinity();
    testDoubleValueMinusZero();
    testDoubleValuePlusZero();
    testFloatValueNeg();
    testFloatValuePos();
    printf("\n==== end of libcore/math/BigDecimalConvertTest ====\n");

    return 0;
}

}
}

