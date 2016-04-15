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

import java.math.BigDecimal;
import java.math.BigInteger;
import java.math.MathContext;
import java.math.RoundingMode;

import junit.framework.TestCase;

/**
 * Class:  java.math.BigDecimal
 * Methods: constructors and fields
 */
public class BigDecimalConstructorsTest extends TestCase {
#endif

    /**
     * check ONE
     */
#if 0
    public void testFieldONE() {
        String oneS = "1";
        double oneD = 1.0;
        assertEquals("incorrect string value", oneS, BigDecimal.ONE.toString());
        assertEquals("incorrect double value", oneD, BigDecimal.ONE.doubleValue(), 0);
    }
#endif

    /**
     * check TEN
     */
#if 0
    public void testFieldTEN() {
        String oneS = "10";
        double oneD = 10.0;
        assertEquals("incorrect string value", oneS, BigDecimal.TEN.toString());
        assertEquals("incorrect double value", oneD, BigDecimal.TEN.doubleValue(), 0);
    }
#endif

    /**
     * check ZERO
     */
#if 0
    public void testFieldZERO() {
        String oneS = "0";
        double oneD = 0.0;
        assertEquals("incorrect string value", oneS, BigDecimal.ZERO.toString());
        assertEquals("incorrect double value", oneD, BigDecimal.ZERO.doubleValue(), 0);
    }
#endif

    /**
     * new BigDecimal(BigInteger value)
     */
#if 0
    public void testConstrBI() {
        String a = "1231212478987482988429808779810457634781384756794987";
        BigInteger bA = new BigInteger(a);
        BigDecimal aNumber = new BigDecimal(bA);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", 0, aNumber.scale());

        try {
            new BigDecimal((BigInteger) null);
        	fail("No NullPointerException");
        } catch (NullPointerException e) {
        	//expected
        }
    }
#endif
void testConstrBI()
{
    String a = String("1231212478987482988429808779810457634781384756794987");

    AutoPtr<IBigInteger> bA;
    ECode ec = CBigInteger::New(a, (IBigInteger**)&bA);
    if (FAILED(ec) || bA == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigDecimal> aNumber;
    ec = CBigDecimal::New((IBigInteger*)bA, (IBigDecimal**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> rInteger;
    aNumber->GetUnscaledValue((IBigInteger**)&rInteger);

    IComparable* comp = (IComparable *)rInteger->Probe(EIID_IComparable);
    Int32 r;
    comp->CompareTo(rInteger, &r);

    if (r != 0) {
        printf("incorrect value\n");
    }

    aNumber->GetScale(&r);

    assertEquals("incorrect scale", 0, r);

}

    /**
     * new BigDecimal(BigInteger value, int scale)
     */
#if 0
    public void testConstrBIScale() {
        String a = "1231212478987482988429808779810457634781384756794987";
        BigInteger bA = new BigInteger(a);
        int aScale = 10;
        BigDecimal aNumber = new BigDecimal(bA, aScale);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif
void testConstrBIScale()
{
    String a = String("1231212478987482988429808779810457634781384756794987");
    int aScale = 10;

    AutoPtr<IBigInteger> bA;
    ECode ec = CBigInteger::New(a, (IBigInteger**)&bA);
    if (FAILED(ec) || bA == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigDecimal> aNumber;
    ec = CBigDecimal::New((IBigInteger*)bA, aScale, (IBigDecimal**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> rInteger;
    aNumber->GetUnscaledValue((IBigInteger**)&rInteger);

    IComparable* comp = (IComparable *)rInteger->Probe(EIID_IComparable);
    Int32 r;
    comp->CompareTo(rInteger, &r);

    if (r != 0) {
        printf("incorrect value\n");
    }

    aNumber->GetScale(&r);

    assertEquals("incorrect scale", 0, r);

}

    /**
     * new BigDecimal(BigInteger value, MathContext)
     */
#if 0
    public void testConstrBigIntegerMathContext() {
        String a = "1231212478987482988429808779810457634781384756794987";
        BigInteger bA = new BigInteger(a);
        int precision = 46;
        RoundingMode rm = RoundingMode.CEILING;
        MathContext mc = new MathContext(precision, rm);
        String res = "1231212478987482988429808779810457634781384757";
        int resScale = -6;
        BigDecimal result = new BigDecimal(bA, mc);
        assertEquals("incorrect value", res, result.unscaledValue().toString());
        assertEquals("incorrect scale", resScale, result.scale());
    }
#endif
void testConstrBigIntegerMathContext()
{
    String a = String("1231212478987482988429808779810457634781384756794987");
    int precision = 46;
    RoundingMode rm = RoundingMode_CEILING;
    String res = String("1231212478987482988429808779810457634781384757");
    int resScale = -6;

    AutoPtr<IBigInteger> bA;
    ECode ec = CBigInteger::New(a, (IBigInteger**)&bA);
    if (FAILED(ec) || bA == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IMathContext> mc;
    ec = CMathContext::New(precision, rm, (IMathContext**)&mc);
    if (FAILED(ec) || mc == NULL) {
        printf(" Failed to create CMathContext. Error %08X\n", ec);
    }

    AutoPtr<IBigDecimal> result;
    ec = CBigDecimal::New((IBigInteger*)bA, mc, (IBigDecimal**)&result);
    if (FAILED(ec) || result == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> rInteger;
    result->GetUnscaledValue((IBigInteger**)&rInteger);

    String str;
    str = Object::ToString(rInteger);
    Int32 scale;
    result->GetScale(&scale);

    assertEquals("incorrect value", res, str);
    assertEquals("incorrect scale", resScale, scale);

}


    /**
     * new BigDecimal(BigInteger value, int scale, MathContext)
     */
#if 0
    public void testConstrBigIntegerScaleMathContext() {
        String a = "1231212478987482988429808779810457634781384756794987";
        BigInteger bA = new BigInteger(a);
        int aScale = 10;
        int precision = 46;
        RoundingMode rm = RoundingMode.CEILING;
        MathContext mc = new MathContext(precision, rm);
        String res = "1231212478987482988429808779810457634781384757";
        int resScale = 4;
        BigDecimal result = new BigDecimal(bA, aScale, mc);
        assertEquals("incorrect value", res, result.unscaledValue().toString());
        assertEquals("incorrect scale", resScale, result.scale());
    }
#endif
void testConstrBigIntegerScaleMathContext()
{
    String a = String("1231212478987482988429808779810457634781384756794987");
    int precision = 46;
    RoundingMode rm = RoundingMode_CEILING;
    String res = String("1231212478987482988429808779810457634781384757");
    int aScale = 10;
    int resScale = 4;

    AutoPtr<IBigInteger> bA;
    ECode ec = CBigInteger::New(a, (IBigInteger**)&bA);
    if (FAILED(ec) || bA == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IMathContext> mc;
    ec = CMathContext::New(precision, rm, (IMathContext**)&mc);
    if (FAILED(ec) || mc == NULL) {
        printf(" Failed to create CMathContext. Error %08X\n", ec);
    }

    AutoPtr<IBigDecimal> result;
    ec = CBigDecimal::New((IBigInteger*)bA, aScale, mc, (IBigDecimal**)&result);
    if (FAILED(ec) || result == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> rInteger;
    result->GetUnscaledValue((IBigInteger**)&rInteger);

    String str;
    str = Object::ToString(rInteger);
    Int32 scale;
    result->GetScale(&scale);

    assertEquals("incorrect value", res, str);
    assertEquals("incorrect scale", resScale, scale);

}


    /**
     * new BigDecimal(char[] value);
     */
#if 0
    public void testConstrChar() {
        char value[] = {'-', '1', '2', '3', '8', '0', '.', '4', '7', '3', '8', 'E', '-', '4', '2', '3'};
        BigDecimal result = new BigDecimal(value);
        String res = "-1.23804738E-419";
        int resScale = 427;
        assertEquals("incorrect value", res, result.toString());
        assertEquals("incorrect scale", resScale, result.scale());

        try {
            // Regression for HARMONY-783
            new BigDecimal(new char[] {});
            fail("NumberFormatException has not been thrown");
        } catch (NumberFormatException e) {
        }
     }
#endif
void testConstrChar()
{
    Char32 _value[] = {'-', '1', '2', '3', '8', '0', '.', '4', '7', '3', '8', 'E', '-', '4', '2', '3'};
    String res = String("-1.23804738E-419");
    int resScale = 427;

    AutoPtr<ArrayOf<Char32> > value = ArrayOf<Char32>::Alloc(sizeof(_value));
    memcpy(value->GetPayload(), value, sizeof(_value));

    AutoPtr<IBigDecimal> result;
    ECode ec = CBigDecimal::New(*value, (IBigDecimal**)&result);
    if (FAILED(ec) || result == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    String str;
    str = Object::ToString(result);
    Int32 scale;
    result->GetScale(&scale);

    assertEquals("incorrect value", res, str);
    assertEquals("incorrect scale", resScale, scale);

    AutoPtr<ArrayOf<Char32> > value2 = ArrayOf<Char32>::Alloc(0);

    AutoPtr<IBigDecimal> result2;
    ec = CBigDecimal::New(*value2, (IBigDecimal**)&result2);
    if (FAILED(ec) || result2 == NULL) {
        printf("NumberFormatException has been thrown\n");
    } else {
        printf("NumberFormatException has not been thrown\n");
    }
}

    /**
     * new BigDecimal(char[] value, int offset, int len);
     */
#if 0
    public void testConstrCharIntInt() {
        char value[] = {'-', '1', '2', '3', '8', '0', '.', '4', '7', '3', '8', 'E', '-', '4', '2', '3'};
        int offset = 3;
        int len = 12;
        BigDecimal result = new BigDecimal(value, offset, len);
        String res = "3.804738E-40";
        int resScale = 46;
        assertEquals("incorrect value", res, result.toString());
        assertEquals("incorrect scale", resScale, result.scale());

        try {
            // Regression for HARMONY-783
            new BigDecimal(new char[] {}, 0, 0);
            fail("NumberFormatException has not been thrown");
        } catch (NumberFormatException e) {
        }
     }
#endif
void testConstrCharIntInt()
{
    Char32 _value[] = {'-', '1', '2', '3', '8', '0', '.', '4', '7', '3', '8', 'E', '-', '4', '2', '3'};
    int offset = 3;
    int len = 12;
    String res = String("3.804738E-40");
    int resScale = 427;

    AutoPtr<ArrayOf<Char32> > value = ArrayOf<Char32>::Alloc(sizeof(_value));
    memcpy(value->GetPayload(), value, sizeof(_value));

    AutoPtr<IBigDecimal> result;
    ECode ec = CBigDecimal::New(*value, offset, len, (IBigDecimal**)&result);
    if (FAILED(ec) || result == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    String str;
    str = Object::ToString(result);
    Int32 scale;
    result->GetScale(&scale);

    assertEquals("incorrect value", res, str);
    assertEquals("incorrect scale", resScale, scale);

    AutoPtr<ArrayOf<Char32> > value2 = ArrayOf<Char32>::Alloc(0);

    AutoPtr<IBigDecimal> result2;
    ec = CBigDecimal::New(*value2, 0, 0, (IBigDecimal**)&result2);
    if (FAILED(ec) || result2 == NULL) {
        printf("NumberFormatException has been thrown\n");
    } else {
        printf("NumberFormatException has not been thrown\n");
    }

}

    /**
     * new BigDecimal(char[] value, int offset, int len, MathContext mc);
     */
#if 0
    public void testConstrCharIntIntMathContext() {
        char value[] = {'-', '1', '2', '3', '8', '0', '.', '4', '7', '3', '8', 'E', '-', '4', '2', '3'};
        int offset = 3;
        int len = 12;
        int precision = 4;
        RoundingMode rm = RoundingMode.CEILING;
        MathContext mc = new MathContext(precision, rm);
        BigDecimal result = new BigDecimal(value, offset, len, mc);
        String res = "3.805E-40";
        int resScale = 43;
        assertEquals("incorrect value", res, result.toString());
        assertEquals("incorrect scale", resScale, result.scale());

        try {
            // Regression for HARMONY-783
            new BigDecimal(new char[] {}, 0, 0, MathContext.DECIMAL32);
            fail("NumberFormatException has not been thrown");
        } catch (NumberFormatException e) {
        }
     }
#endif

    /**
     * new BigDecimal(char[] value, int offset, int len, MathContext mc);
     */
#if 0
    public void testConstrCharIntIntMathContextException1() {
        char value[] = {'-', '1', '2', '3', '8', '0', '.', '4', '7', '3', '8', 'E', '-', '4', '2', '3'};
        int offset = 3;
        int len = 120;
        int precision = 4;
        RoundingMode rm = RoundingMode.CEILING;
        MathContext mc = new MathContext(precision, rm);
        try {
            new BigDecimal(value, offset, len, mc);
            fail("NumberFormatException has not been thrown");
        } catch (NumberFormatException e) {
        }
     }
#endif

    /**
     * new BigDecimal(char[] value, int offset, int len, MathContext mc);
     */
#if 0
    public void testConstrCharIntIntMathContextException2() {
        char value[] = {'-', '1', '2', '3', '8', '0', ',', '4', '7', '3', '8', 'E', '-', '4', '2', '3'};
        int offset = 3;
        int len = 120;
        int precision = 4;
        RoundingMode rm = RoundingMode.CEILING;
        MathContext mc = new MathContext(precision, rm);
        try {
            new BigDecimal(value, offset, len, mc);
            fail("NumberFormatException has not been thrown");
        } catch (NumberFormatException e) {
        }
     }
#endif

    /**
     * new BigDecimal(char[] value, MathContext mc);
     */
#if 0
    public void testConstrCharMathContext() {
        try {
            // Regression for HARMONY-783
            new BigDecimal(new char[] {}, MathContext.DECIMAL32);
            fail("NumberFormatException has not been thrown");
        } catch (NumberFormatException e) {
        }
    }
#endif

    /**
     * new BigDecimal(double value) when value is NaN
     */
#if 0
    public void testConstrDoubleNaN() {
        double a = Double.NaN;
        try {
            new BigDecimal(a);
            fail("NumberFormatException has not been caught");
        } catch (NumberFormatException e) {
        }
    }
#endif

    /**
     * new BigDecimal(double value) when value is positive infinity
     */
#if 0
    public void testConstrDoublePosInfinity() {
        double a = Double.POSITIVE_INFINITY;
        try {
            new BigDecimal(a);
            fail("NumberFormatException has not been caught");
        } catch (NumberFormatException e) {
        }
    }
#endif

    /**
     * new BigDecimal(double value) when value is positive infinity
     */
#if 0
    public void testConstrDoubleNegInfinity() {
        double a = Double.NEGATIVE_INFINITY;
        try {
            new BigDecimal(a);
            fail("NumberFormatException has not been caught");
        } catch (NumberFormatException e) {
        }
    }
#endif

    /**
     * new BigDecimal(double value)
     */
#if 0
    public void testConstrDouble() {
        double a = 732546982374982347892379283571094797.287346782359284756;
        int aScale = 0;
        BigInteger bA = new BigInteger("732546982374982285073458350476230656");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
     * new BigDecimal(double, MathContext)
     */
#if 0
    public void testConstrDoubleMathContext() {
        double a = 732546982374982347892379283571094797.287346782359284756;
        int precision = 21;
        RoundingMode rm = RoundingMode.CEILING;
        MathContext mc = new MathContext(precision, rm);
        String res = "732546982374982285074";
        int resScale = -15;
        BigDecimal result = new BigDecimal(a, mc);
        assertEquals("incorrect value", res, result.unscaledValue().toString());
        assertEquals("incorrect scale", resScale, result.scale());
    }
#endif

    /**
     * new BigDecimal(0.1)
     */
#if 0
    public void testConstrDouble01() {
        double a = 1.E-1;
        int aScale = 55;
        BigInteger bA = new BigInteger("1000000000000000055511151231257827021181583404541015625");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
     * new BigDecimal(0.555)
     */
#if 0
    public void testConstrDouble02() {
        double a = 0.555;
        int aScale = 53;
        BigInteger bA = new BigInteger("55500000000000004884981308350688777863979339599609375");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
     * new BigDecimal(-0.1)
     */
#if 0
    public void testConstrDoubleMinus01() {
        double a = -1.E-1;
        int aScale = 55;
        BigInteger bA = new BigInteger("-1000000000000000055511151231257827021181583404541015625");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
     * new BigDecimal(int value)
     */
#if 0
    public void testConstrInt() {
        int a = 732546982;
        String res = "732546982";
        int resScale = 0;
        BigDecimal result = new BigDecimal(a);
        assertEquals("incorrect value", res, result.unscaledValue().toString());
        assertEquals("incorrect scale", resScale, result.scale());
    }
#endif

    /**
     * new BigDecimal(int, MathContext)
     */
#if 0
    public void testConstrIntMathContext() {
        int a = 732546982;
        int precision = 21;
        RoundingMode rm = RoundingMode.CEILING;
        MathContext mc = new MathContext(precision, rm);
        String res = "732546982";
        int resScale = 0;
        BigDecimal result = new BigDecimal(a, mc);
        assertEquals("incorrect value", res, result.unscaledValue().toString());
        assertEquals("incorrect scale", resScale, result.scale());
    }
#endif

    /**
     * new BigDecimal(long value)
     */
#if 0
    public void testConstrLong() {
        long a = 4576578677732546982L;
        String res = "4576578677732546982";
        int resScale = 0;
        BigDecimal result = new BigDecimal(a);
        assertEquals("incorrect value", res, result.unscaledValue().toString());
        assertEquals("incorrect scale", resScale, result.scale());
    }
#endif

    /**
     * new BigDecimal(long, MathContext)
     */
#if 0
    public void testConstrLongMathContext() {
        long a = 4576578677732546982L;
        int precision = 5;
        RoundingMode rm = RoundingMode.CEILING;
        MathContext mc = new MathContext(precision, rm);
        String res = "45766";
        int resScale = -14;
        BigDecimal result = new BigDecimal(a, mc);
        assertEquals("incorrect value", res, result.unscaledValue().toString());
        assertEquals("incorrect scale", resScale, result.scale());
    }
#endif

    /**
     * new BigDecimal(double value) when value is denormalized
     */
#if 0
    public void testConstrDoubleDenormalized() {
        double a = 2.274341322658976E-309;
        int aScale = 1073;
        BigInteger bA = new BigInteger("227434132265897633950269241702666687639731047124115603942986140264569528085692462493371029187342478828091760934014851133733918639492582043963243759464684978401240614084312038547315281016804838374623558434472007664427140169018817050565150914041833284370702366055678057809362286455237716100382057360123091641959140448783514464639706721250400288267372238950016114583259228262046633530468551311769574111763316146065958042194569102063373243372766692713192728878701004405568459288708477607744497502929764155046100964958011009313090462293046650352146796805866786767887226278836423536035611825593567576424943331337401071583562754098901412372708947790843318760718495117047155597276492717187936854356663665005157041552436478744491526494952982062613955349661409854888916015625");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
     * new BigDecimal(String value)
     * when value is not a valid representation of BigDecimal.
     */
#if 0
    public void testConstrStringException() {
        String a = "-238768.787678287a+10";
        try {
            new BigDecimal(a);
            fail("NumberFormatException has not been caught");
        } catch (NumberFormatException e) {}
    }
#endif

    /**
     * new BigDecimal(String value) when exponent is empty.
     */
#if 0
    public void testConstrStringExceptionEmptyExponent1() {
        String a = "-238768.787678287e";
        try {
            new BigDecimal(a);
            fail("NumberFormatException has not been caught");
        } catch (NumberFormatException e) {
        }
    }
#endif

    /**
     * new BigDecimal(String value) when exponent is empty.
     */
#if 0
    public void testConstrStringExceptionEmptyExponent2() {
        String a = "-238768.787678287e-";
        try {
            new BigDecimal(a);
            fail("NumberFormatException has not been caught");
        } catch (NumberFormatException e) {
        }
    }
#endif

    /**
     * new BigDecimal(String value) when exponent is greater than
     * Integer.MAX_VALUE.
     */
#if 0
    public void testConstrStringExceptionExponentGreaterIntegerMax() {
        String a = "-238768.787678287e214748364767876";
        try {
            new BigDecimal(a);
            fail("NumberFormatException has not been caught");
        } catch (NumberFormatException e) {
        }
    }
#endif

    /**
     * new BigDecimal(String value) when exponent is less than
     * Integer.MIN_VALUE.
     */
#if 0
    public void testConstrStringExceptionExponentLessIntegerMin() {
        String a = "-238768.787678287e-214748364767876";
        try {
            new BigDecimal(a);
            fail("NumberFormatException has not been caught");
        } catch (NumberFormatException e) {
        }
    }
#endif

    /**
     * new BigDecimal(String value)
     * when exponent is Integer.MAX_VALUE.
     */
#if 0
    public void testConstrStringExponentIntegerMax() {
        String a = "-238768.787678287e2147483647";
        int aScale = -2147483638;
        BigInteger bA = new BigInteger("-238768787678287");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
     * new BigDecimal(String value)
     * when exponent is Integer.MIN_VALUE.
     */
#if 0
    public void testConstrStringExponentIntegerMin() {
        String a = ".238768e-2147483648";
        try {
           new BigDecimal(a);
           fail("NumberFormatException expected");
       } catch (NumberFormatException e) {
       }
    }
#endif

    /**
     * new BigDecimal(String value); value does not contain exponent
     */
  #if 0
    public void testConstrStringWithoutExpPos1() {
        String a = "732546982374982347892379283571094797.287346782359284756";
        int aScale = 18;
        BigInteger bA = new BigInteger("732546982374982347892379283571094797287346782359284756");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
     * new BigDecimal(String value); value does not contain exponent
     */
  #if 0
    public void testConstrStringWithoutExpPos2() {
        String a = "+732546982374982347892379283571094797.287346782359284756";
        int aScale = 18;
        BigInteger bA = new BigInteger("732546982374982347892379283571094797287346782359284756");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
     * new BigDecimal(String value); value does not contain exponent
     */
  #if 0
    public void testConstrStringWithoutExpNeg() {
        String a = "-732546982374982347892379283571094797.287346782359284756";
        int aScale = 18;
        BigInteger bA = new BigInteger("-732546982374982347892379283571094797287346782359284756");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
     * new BigDecimal(String value); value does not contain exponent
     * and decimal point
     */
  #if 0
    public void testConstrStringWithoutExpWithoutPoint() {
        String a = "-732546982374982347892379283571094797287346782359284756";
        int aScale = 0;
        BigInteger bA = new BigInteger("-732546982374982347892379283571094797287346782359284756");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
       * new BigDecimal(String value); value contains exponent
       * and does not contain decimal point
       */
  #if 0
    public void testConstrStringWithExponentWithoutPoint1() {
          String a = "-238768787678287e214";
          int aScale = -214;
          BigInteger bA = new BigInteger("-238768787678287");
          BigDecimal aNumber = new BigDecimal(a);
          assertEquals("incorrect value", bA, aNumber.unscaledValue());
          assertEquals("incorrect scale", aScale, aNumber.scale());
      }
#endif

    /**
     * new BigDecimal(String value); value contains exponent
     * and does not contain decimal point
     */
#if 0
    public void testConstrStringWithExponentWithoutPoint2() {
        String a = "-238768787678287e-214";
        int aScale = 214;
        BigInteger bA = new BigInteger("-238768787678287");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
     * new BigDecimal(String value); value contains exponent
     * and does not contain decimal point
     */
#if 0
    public void testConstrStringWithExponentWithoutPoint3() {
        String a = "238768787678287e-214";
        int aScale = 214;
        BigInteger bA = new BigInteger("238768787678287");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
     * new BigDecimal(String value); value contains exponent
     * and does not contain decimal point
     */
#if 0
    public void testConstrStringWithExponentWithoutPoint4() {
        String a = "238768787678287e+214";
        int aScale = -214;
        BigInteger bA = new BigInteger("238768787678287");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
     * new BigDecimal(String value); value contains exponent
     * and does not contain decimal point
     */
#if 0
    public void testConstrStringWithExponentWithoutPoint5() {
        String a = "238768787678287E214";
        int aScale = -214;
        BigInteger bA = new BigInteger("238768787678287");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
     * new BigDecimal(String value);
     * value contains both exponent and decimal point
     */
#if 0
    public void testConstrStringWithExponentWithPoint1() {
        String a = "23985439837984782435652424523876878.7678287e+214";
        int aScale = -207;
        BigInteger bA = new BigInteger("239854398379847824356524245238768787678287");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
     * new BigDecimal(String value);
     * value contains both exponent and decimal point
     */
#if 0
    public void testConstrStringWithExponentWithPoint2() {
        String a = "238096483923847545735673567457356356789029578490276878.7678287e-214";
        int aScale = 221;
        BigInteger bA = new BigInteger("2380964839238475457356735674573563567890295784902768787678287");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
     * new BigDecimal(String value);
     * value contains both exponent and decimal point
     */
#if 0
    public void testConstrStringWithExponentWithPoint3() {
        String a = "2380964839238475457356735674573563567890.295784902768787678287E+21";
        int aScale = 0;
        BigInteger bA = new BigInteger("2380964839238475457356735674573563567890295784902768787678287");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
     * new BigDecimal(String value);
     * value contains both exponent and decimal point
     */
#if 0
    public void testConstrStringWithExponentWithPoint4() {
        String a = "23809648392384754573567356745735635678.90295784902768787678287E+21";
        int aScale = 2;
        BigInteger bA = new BigInteger("2380964839238475457356735674573563567890295784902768787678287");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
     * new BigDecimal(String value);
     * value contains both exponent and decimal point
     */
#if 0
    public void testConstrStringWithExponentWithPoint5() {
        String a = "238096483923847545735673567457356356789029.5784902768787678287E+21";
        int aScale = -2;
        BigInteger bA = new BigInteger("2380964839238475457356735674573563567890295784902768787678287");
        BigDecimal aNumber = new BigDecimal(a);
        assertEquals("incorrect value", bA, aNumber.unscaledValue());
        assertEquals("incorrect scale", aScale, aNumber.scale());
    }
#endif

    /**
     * new BigDecimal(String value, MathContext)
     */
#if 0
    public void testConstrStringMathContext() {
        String a = "-238768787678287e214";
        int precision = 5;
        RoundingMode rm = RoundingMode.CEILING;
        MathContext mc = new MathContext(precision, rm);
        String res = "-23876";
        int resScale = -224;
        BigDecimal result = new BigDecimal(a, mc);
        assertEquals("incorrect value", res, result.unscaledValue().toString());
        assertEquals("incorrect scale", resScale, result.scale());
    }
#endif

//==============================================================================

int mainBigDecimalConstructorsTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/BigDecimalConstructorsTest ====\n");
    testConstrBI();
    testConstrBIScale();
    testConstrBigIntegerMathContext();
    testConstrBigIntegerScaleMathContext();
    testConstrChar();
    testConstrCharIntInt();
    printf("\n==== end of libcore/math/BigDecimalConstructorsTest ====\n");

    return 0;
}

}
}
