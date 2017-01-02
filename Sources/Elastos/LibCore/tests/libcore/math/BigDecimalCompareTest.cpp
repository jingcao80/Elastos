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
 * Methods: abs, compareTo, equals, hashCode,
 * max, min, negate, signum
 */
public class BigDecimalCompareTest extends TestCase {
#endif

    /**
     * Abs() of a negative BigDecimal
     */
#if 0
    public void testAbsNeg() {
        String a = "-123809648392384754573567356745735.63567890295784902768787678287E+21";
        BigDecimal aNumber = new BigDecimal(a);
        String result = "123809648392384754573567356745735635678902957849027687.87678287";
        assertEquals("incorrect value", result, aNumber.abs().toString());
    }
#endif
void testAbsNeg()
{

    String a = String("-123809648392384754573567356745735.63567890295784902768787678287E+21");
    String result = String("123809648392384754573567356745735635678902957849027687.87678287");

    AutoPtr<IBigDecimal> aDecimal;
    ECode ec = CBigDecimal::New(a, (IBigDecimal**)&aDecimal);
    if (FAILED(ec) || aDecimal == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    AutoPtr<IBigDecimal> rDecimal;
    aDecimal->Abs((IBigDecimal**)&rDecimal);

    String str;
    str = Object::ToString(rDecimal);

    assertEquals("incorrect value", result, str);
}


    /**
     * Abs() of a positive BigDecimal
     */
#if 0
    public void testAbsPos() {
        String a = "123809648392384754573567356745735.63567890295784902768787678287E+21";
        BigDecimal aNumber = new BigDecimal(a);
        String result = "123809648392384754573567356745735635678902957849027687.87678287";
        assertEquals("incorrect value", result, aNumber.abs().toString());
    }
#endif
void testAbsPos()
{

    String a = String("123809648392384754573567356745735.63567890295784902768787678287E+21");
    String result = String("123809648392384754573567356745735635678902957849027687.87678287");

    AutoPtr<IBigDecimal> aDecimal;
    ECode ec = CBigDecimal::New(a, (IBigDecimal**)&aDecimal);
    if (FAILED(ec) || aDecimal == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    AutoPtr<IBigDecimal> rDecimal;
    aDecimal->Abs((IBigDecimal**)&rDecimal);

    String str;
    str = Object::ToString(rDecimal);

    assertEquals("incorrect value", result, str);
}

    /**
     * Abs(MathContext) of a negative BigDecimal
     */
#if 0
    public void testAbsMathContextNeg() {
        String a = "-123809648392384754573567356745735.63567890295784902768787678287E+21";
        BigDecimal aNumber = new BigDecimal(a);
        int precision = 15;
        RoundingMode rm = RoundingMode.HALF_DOWN;
        MathContext mc = new MathContext(precision, rm);
        String result = "1.23809648392385E+53";
        int resScale = -39;
        BigDecimal res = aNumber.abs(mc);
        assertEquals("incorrect value", result, res.toString());
        assertEquals("incorrect scale", resScale, res.scale());
    }
#endif
void testAbsMathContextNeg()
{

    String a = String("-123809648392384754573567356745735.63567890295784902768787678287E+21");
    String result = String("1.23809648392385E+53");
    int resScale = -39;

    AutoPtr<IBigDecimal> aDecimal;
    ECode ec = CBigDecimal::New(a, (IBigDecimal**)&aDecimal);
    if (FAILED(ec) || aDecimal == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    int precision = 15;
    RoundingMode rm = RoundingMode_HALF_DOWN;
    AutoPtr<IMathContext> mc;
    ec = CMathContext::New(precision, rm, (IMathContext**)&mc);
    if (FAILED(ec) || mc == NULL) {
        printf(" Failed to create CMathContext. Error %08X\n", ec);
    }

    AutoPtr<IBigDecimal> rDecimal;
    aDecimal->Abs(mc, (IBigDecimal**)&rDecimal);

    String str;
    str = Object::ToString(rDecimal);
    Int32 scale;
    rDecimal->GetScale(&scale);

    assertEquals("incorrect value", result, str);
    assertEquals("incorrect scale", resScale, scale);
}

    /**
     * Abs(MathContext) of a positive BigDecimal
     */
#if 0
    public void testAbsMathContextPos() {
        String a = "123809648392384754573567356745735.63567890295784902768787678287E+21";
        BigDecimal aNumber = new BigDecimal(a);
        int precision = 41;
        RoundingMode rm = RoundingMode.HALF_EVEN;
        MathContext mc = new MathContext(precision, rm);
        String result = "1.2380964839238475457356735674573563567890E+53";
        int resScale = -13;
        BigDecimal res = aNumber.abs(mc);
        assertEquals("incorrect value", result, res.toString());
        assertEquals("incorrect scale", resScale, res.scale());
    }
#endif
void testAbsMathContextPos()
{

    String a = String("123809648392384754573567356745735.63567890295784902768787678287E+21");
    String result = String("1.23809648392385E+53");
    int resScale = -13;

    AutoPtr<IBigDecimal> aDecimal;
    ECode ec = CBigDecimal::New(a, (IBigDecimal**)&aDecimal);
    if (FAILED(ec) || aDecimal == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    int precision = 41;
    RoundingMode rm = RoundingMode_HALF_EVEN;
    AutoPtr<IMathContext> mc;
    ec = CMathContext::New(precision, rm, (IMathContext**)&mc);
    if (FAILED(ec) || mc == NULL) {
        printf(" Failed to create CMathContext. Error %08X\n", ec);
    }

    AutoPtr<IBigDecimal> rDecimal;
    aDecimal->Abs(mc, (IBigDecimal**)&rDecimal);

    String str;
    str = Object::ToString(rDecimal);
    Int32 scale;
    rDecimal->GetScale(&scale);

    assertEquals("incorrect value", result, str);
    assertEquals("incorrect scale", resScale, scale);
}

    /**
     * Compare to a number of an equal scale
     */
#if 0
    public void testCompareEqualScale1() {
        String a = "12380964839238475457356735674573563567890295784902768787678287";
        int aScale = 18;
        String b = "4573563567890295784902768787678287";
        int bScale = 18;
        BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
        BigDecimal bNumber = new BigDecimal(new BigInteger(b), bScale);
        int result = 1;
        assertEquals("incorrect result", result, aNumber.compareTo(bNumber));
    }
#endif
void testCompareEqualScale1()
{

    String a = String("12380964839238475457356735674573563567890295784902768787678287");
    String b = String("4573563567890295784902768787678287");

    int aScale = 18;
    int bScale = 18;
    int result = 1;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(a, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> bNumber;
    ec = CBigInteger::New(b, (IBigInteger**)&bNumber);
    if (FAILED(ec) || bNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigDecimal> aDecimal;
    ec = CBigDecimal::New((IBigInteger*)aNumber, aScale, (IBigDecimal**)&aDecimal);
    if (FAILED(ec) || aDecimal == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    AutoPtr<IBigDecimal> bDecimal;
    ec = CBigDecimal::New((IBigInteger*)bNumber, bScale, (IBigDecimal**)&bDecimal);
    if (FAILED(ec) || bDecimal == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    IComparable* comp = (IComparable *)aNumber->Probe(EIID_IComparable);

    Int32 r;
    comp->CompareTo(bDecimal, &r);

    assertEquals("incorrect result", result, r);
}

    /**
     * Compare to a number of an equal scale
     */
#if 0
    public void testCompareEqualScale2() {
        String a = "12380964839238475457356735674573563567890295784902768787678287";
        int aScale = 18;
        String b = "4573563923487289357829759278282992758247567890295784902768787678287";
        int bScale = 18;
        BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
        BigDecimal bNumber = new BigDecimal(new BigInteger(b), bScale);
        int result = -1;
        assertEquals("incorrect result", result, aNumber.compareTo(bNumber));
    }
#endif
void testCompareEqualScale2()
{

    String a = String("12380964839238475457356735674573563567890295784902768787678287");
    String b = String("4573563923487289357829759278282992758247567890295784902768787678287");

    int aScale = 18;
    int bScale = 18;
    int result = -1;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(a, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> bNumber;
    ec = CBigInteger::New(b, (IBigInteger**)&bNumber);
    if (FAILED(ec) || bNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigDecimal> aDecimal;
    ec = CBigDecimal::New((IBigInteger*)aNumber, aScale, (IBigDecimal**)&aDecimal);
    if (FAILED(ec) || aDecimal == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    AutoPtr<IBigDecimal> bDecimal;
    ec = CBigDecimal::New((IBigInteger*)bNumber, bScale, (IBigDecimal**)&bDecimal);
    if (FAILED(ec) || bDecimal == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    IComparable* comp = (IComparable *)aNumber->Probe(EIID_IComparable);

    Int32 r;
    comp->CompareTo(bDecimal, &r);

    assertEquals("incorrect result", result, r);
}


    /**
     * Compare to a number of an greater scale
     */
#if 0
    public void testCompareGreaterScale1() {
        String a = "12380964839238475457356735674573563567890295784902768787678287";
        int aScale = 28;
        String b = "4573563567890295784902768787678287";
        int bScale = 18;
        BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
        BigDecimal bNumber = new BigDecimal(new BigInteger(b), bScale);
        int result = 1;
        assertEquals("incorrect result", result, aNumber.compareTo(bNumber));
    }
#endif
void testCompareGreaterScale1()
{

    String a = String("12380964839238475457356735674573563567890295784902768787678287");
    String b = String("4573563923487289357829759278282992758247567890295784902768787678287");

    int aScale = 28;
    int bScale = 18;
    int result = 1;

    AutoPtr<IBigInteger> aNumber;
    ECode ec = CBigInteger::New(a, (IBigInteger**)&aNumber);
    if (FAILED(ec) || aNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigInteger> bNumber;
    ec = CBigInteger::New(b, (IBigInteger**)&bNumber);
    if (FAILED(ec) || bNumber == NULL) {
        printf(" Failed to create CBigInteger. Error %08X\n", ec);
    }

    AutoPtr<IBigDecimal> aDecimal;
    ec = CBigDecimal::New((IBigInteger*)aNumber, aScale, (IBigDecimal**)&aDecimal);
    if (FAILED(ec) || aDecimal == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    AutoPtr<IBigDecimal> bDecimal;
    ec = CBigDecimal::New((IBigInteger*)bNumber, bScale, (IBigDecimal**)&bDecimal);
    if (FAILED(ec) || bDecimal == NULL) {
        printf(" Failed to create CBigDecimal. Error %08X\n", ec);
    }

    IComparable* comp = (IComparable *)aNumber->Probe(EIID_IComparable);

    Int32 r;
    comp->CompareTo(bDecimal, &r);

    assertEquals("incorrect result", result, r);
}

    /**
     * Compare to a number of an greater scale
     */
#if 0
    public void testCompareGreaterScale2() {
        String a = "12380964839238475457356735674573563567890295784902768787678287";
        int aScale = 48;
        String b = "4573563567890295784902768787678287";
        int bScale = 2;
        BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
        BigDecimal bNumber = new BigDecimal(new BigInteger(b), bScale);
        int result = -1;
        assertEquals("incorrect result", result, aNumber.compareTo(bNumber));
    }
#endif

    /**
     * Compare to a number of an less scale
     */
#if 0
    public void testCompareLessScale1() {
        String a = "12380964839238475457356735674573563567890295784902768787678287";
        int aScale = 18;
        String b = "4573563567890295784902768787678287";
        int bScale = 28;
        BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
        BigDecimal bNumber = new BigDecimal(new BigInteger(b), bScale);
        int result = 1;
        assertEquals("incorrect result", result, aNumber.compareTo(bNumber));
    }
#endif

    /**
     * Compare to a number of an less scale
     */
#if 0
    public void testCompareLessScale2() {
        String a = "12380964839238475457356735674573";
        int aScale = 36;
        String b = "45735635948573894578349572001798379183767890295784902768787678287";
        int bScale = 48;
        BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
        BigDecimal bNumber = new BigDecimal(new BigInteger(b), bScale);
        int result = -1;
        assertEquals("incorrect result", result, aNumber.compareTo(bNumber));
    }
#endif

    /**
     * Equals() for unequal BigDecimals
     */
#if 0
    public void testEqualsUnequal1() {
       String a = "92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = -24;
       String b = "7472334223847623782375469293018787918347987234564568";
       int bScale = 13;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal bNumber = new BigDecimal(new BigInteger(b), bScale);
       assertFalse(aNumber.equals(bNumber));
    }
#endif

    /**
     * Equals() for unequal BigDecimals
     */
#if 0
    public void testEqualsUnequal2() {
       String a = "92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = -24;
       String b = "92948782094488478231212478987482988429808779810457634781384756794987";
       int bScale = 13;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal bNumber = new BigDecimal(new BigInteger(b), bScale);
       assertFalse(aNumber.equals(bNumber));
    }
#endif

    /**
     * Equals() for unequal BigDecimals
     */
#if 0
    public void testEqualsUnequal3() {
       String a = "92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = -24;
       String b = "92948782094488478231212478987482988429808779810457634781384756794987";
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       assertFalse(aNumber.equals(b));
    }
#endif

    /**
     * equals() for equal BigDecimals
     */
#if 0
    public void testEqualsEqual() {
       String a = "92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = -24;
       String b = "92948782094488478231212478987482988429808779810457634781384756794987";
       int bScale = -24;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal bNumber = new BigDecimal(new BigInteger(b), bScale);
       assertEquals(aNumber, bNumber);
    }
#endif

    /**
     * equals() for equal BigDecimals
     */
#if 0
    public void testEqualsNull() {
       String a = "92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = -24;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       assertFalse(aNumber.equals(null));
    }
#endif

    /**
     * hashCode() for equal BigDecimals
     */
#if 0
    public void testHashCodeEqual() {
       String a = "92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = -24;
       String b = "92948782094488478231212478987482988429808779810457634781384756794987";
       int bScale = -24;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal bNumber = new BigDecimal(new BigInteger(b), bScale);
       assertEquals("incorrect value", aNumber.hashCode(), bNumber.hashCode());
    }
#endif

    /**
     * hashCode() for unequal BigDecimals
     */
#if 0
    public void testHashCodeUnequal() {
       String a = "8478231212478987482988429808779810457634781384756794987";
       int aScale = 41;
       String b = "92948782094488478231212478987482988429808779810457634781384756794987";
       int bScale = -24;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal bNumber = new BigDecimal(new BigInteger(b), bScale);
       assertTrue("incorrect value", aNumber.hashCode() != bNumber.hashCode());
    }
#endif

    /**
     * max() for equal BigDecimals
     */
#if 0
    public void testMaxEqual() {
       String a = "8478231212478987482988429808779810457634781384756794987";
       int aScale = 41;
       String b = "8478231212478987482988429808779810457634781384756794987";
       int bScale = 41;
       String c = "8478231212478987482988429808779810457634781384756794987";
       int cScale = 41;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal bNumber = new BigDecimal(new BigInteger(b), bScale);
       BigDecimal cNumber = new BigDecimal(new BigInteger(c), cScale);
       assertEquals("incorrect value", cNumber, aNumber.max(bNumber));
    }
#endif

    /**
     * max() for unequal BigDecimals
     */
#if 0
    public void testMaxUnequal1() {
       String a = "92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = 24;
       String b = "92948782094488478231212478987482988429808779810457634781384756794987";
       int bScale = 41;
       String c = "92948782094488478231212478987482988429808779810457634781384756794987";
       int cScale = 24;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal bNumber = new BigDecimal(new BigInteger(b), bScale);
       BigDecimal cNumber = new BigDecimal(new BigInteger(c), cScale);
       assertEquals("incorrect value", cNumber, aNumber.max(bNumber));
    }
#endif

    /**
     * max() for unequal BigDecimals
     */
#if 0
    public void testMaxUnequal2() {
       String a = "92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = 41;
       String b = "94488478231212478987482988429808779810457634781384756794987";
       int bScale = 41;
       String c = "92948782094488478231212478987482988429808779810457634781384756794987";
       int cScale = 41;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal bNumber = new BigDecimal(new BigInteger(b), bScale);
       BigDecimal cNumber = new BigDecimal(new BigInteger(c), cScale);
       assertEquals("incorrect value", cNumber, aNumber.max(bNumber));
    }
#endif

    /**
     * min() for equal BigDecimals
     */
#if 0
    public void testMinEqual() {
       String a = "8478231212478987482988429808779810457634781384756794987";
       int aScale = 41;
       String b = "8478231212478987482988429808779810457634781384756794987";
       int bScale = 41;
       String c = "8478231212478987482988429808779810457634781384756794987";
       int cScale = 41;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal bNumber = new BigDecimal(new BigInteger(b), bScale);
       BigDecimal cNumber = new BigDecimal(new BigInteger(c), cScale);
       assertEquals("incorrect value", cNumber, aNumber.min(bNumber));
    }
#endif

    /**
     * min() for unequal BigDecimals
     */
#if 0
    public void testMinUnequal1() {
       String a = "92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = 24;
       String b = "92948782094488478231212478987482988429808779810457634781384756794987";
       int bScale = 41;
       String c = "92948782094488478231212478987482988429808779810457634781384756794987";
       int cScale = 41;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal bNumber = new BigDecimal(new BigInteger(b), bScale);
       BigDecimal cNumber = new BigDecimal(new BigInteger(c), cScale);
       assertEquals("incorrect value", cNumber, aNumber.min(bNumber));
    }
#endif

    /**
     * min() for unequal BigDecimals
     */
#if 0
    public void testMinUnequal2() {
       String a = "92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = 41;
       String b = "94488478231212478987482988429808779810457634781384756794987";
       int bScale = 41;
       String c = "94488478231212478987482988429808779810457634781384756794987";
       int cScale = 41;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal bNumber = new BigDecimal(new BigInteger(b), bScale);
       BigDecimal cNumber = new BigDecimal(new BigInteger(c), cScale);
       assertEquals("incorrect value", cNumber, aNumber.min(bNumber));
    }
#endif

    /**
     * plus() for a positive BigDecimal
     */
#if 0
    public void testPlusPositive() {
       String a = "92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = 41;
       String c = "92948782094488478231212478987482988429808779810457634781384756794987";
       int cScale = 41;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal cNumber = new BigDecimal(new BigInteger(c), cScale);
       assertEquals("incorrect value", cNumber, aNumber.plus());
    }
#endif

    /**
     * plus(MathContext) for a positive BigDecimal
     */
#if 0
    public void testPlusMathContextPositive() {
       String a = "92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = 41;
       int precision = 37;
       RoundingMode rm = RoundingMode.FLOOR;
       MathContext mc = new MathContext(precision, rm);
       String c = "929487820944884782312124789.8748298842";
       int cScale = 10;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal res = aNumber.plus(mc);
       assertEquals("incorrect value", c, res.toString());
       assertEquals("incorrect scale", cScale, res.scale());
    }
#endif

    /**
     * plus() for a negative BigDecimal
     */
#if 0
    public void testPlusNegative() {
       String a = "-92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = 41;
       String c = "-92948782094488478231212478987482988429808779810457634781384756794987";
       int cScale = 41;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal cNumber = new BigDecimal(new BigInteger(c), cScale);
       assertEquals("incorrect value", cNumber, aNumber.plus());
    }
#endif

    /**
     * plus(MathContext) for a negative BigDecimal
     */
#if 0
    public void testPlusMathContextNegative() {
       String a = "-92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = 49;
       int precision = 46;
       RoundingMode rm = RoundingMode.CEILING;
       MathContext mc = new MathContext(precision, rm);
       String c = "-9294878209448847823.121247898748298842980877981";
       int cScale = 27;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal res = aNumber.plus(mc);
       assertEquals("incorrect value", c, res.toString());
       assertEquals("incorrect scale", cScale, res.scale());
    }
#endif

    /**
     * negate() for a positive BigDecimal
     */
#if 0
    public void testNegatePositive() {
       String a = "92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = 41;
       String c = "-92948782094488478231212478987482988429808779810457634781384756794987";
       int cScale = 41;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal cNumber = new BigDecimal(new BigInteger(c), cScale);
       assertEquals("incorrect value", cNumber, aNumber.negate());
    }
#endif

    /**
     * negate(MathContext) for a positive BigDecimal
     */
#if 0
    public void testNegateMathContextPositive() {
       String a = "92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = 41;
       int precision = 37;
       RoundingMode rm = RoundingMode.FLOOR;
       MathContext mc = new MathContext(precision, rm);
       String c = "-929487820944884782312124789.8748298843";
       int cScale = 10;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal res = aNumber.negate(mc);
       assertEquals("incorrect value", c, res.toString());
       assertEquals("incorrect scale", cScale, res.scale());
    }
#endif

    /**
     * negate() for a negative BigDecimal
     */
#if 0
    public void testNegateNegative() {
       String a = "-92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = 41;
       String c = "92948782094488478231212478987482988429808779810457634781384756794987";
       int cScale = 41;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal cNumber = new BigDecimal(new BigInteger(c), cScale);
       assertEquals("incorrect value", cNumber, aNumber.negate());
    }
#endif

    /**
     * negate(MathContext) for a negative BigDecimal
     */
#if 0
    public void testNegateMathContextNegative() {
       String a = "-92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = 49;
       int precision = 46;
       RoundingMode rm = RoundingMode.CEILING;
       MathContext mc = new MathContext(precision, rm);
       String c = "9294878209448847823.121247898748298842980877982";
       int cScale = 27;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       BigDecimal res = aNumber.negate(mc);
       assertEquals("incorrect value", c, res.toString());
       assertEquals("incorrect scale", cScale, res.scale());
    }
#endif

    /**
     * signum() for a positive BigDecimal
     */
#if 0
    public void testSignumPositive() {
       String a = "92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = 41;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       assertEquals("incorrect value", 1, aNumber.signum());
    }
#endif

    /**
     * signum() for a negative BigDecimal
     */
#if 0
    public void testSignumNegative() {
       String a = "-92948782094488478231212478987482988429808779810457634781384756794987";
       int aScale = 41;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       assertEquals("incorrect value", -1, aNumber.signum());
    }
#endif

    /**
     * signum() for zero
     */
#if 0
    public void testSignumZero() {
       String a = "0";
       int aScale = 41;
       BigDecimal aNumber = new BigDecimal(new BigInteger(a), aScale);
       assertEquals("incorrect value", 0, aNumber.signum());
    }
#endif

    /*
     * Regression test for HARMONY-6406
     */
#if 0
    public void testApproxPrecision() {
        BigDecimal testInstance = BigDecimal.TEN.multiply(new BigDecimal("0.1"));
        int result = testInstance.compareTo(new BigDecimal("1.00"));
        assertEquals(0, result);
    }
#endif

//==============================================================================

int mainBigDecimalCompareTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/BigDecimalCompareTest ====\n");
    testAbsNeg();
    testAbsPos();
    testAbsMathContextNeg();
    testAbsMathContextPos();
    testCompareEqualScale1();
    testCompareEqualScale2();
    testCompareGreaterScale1();
    printf("\n==== end of libcore/math/BigDecimalCompareTest ====\n");

    return 0;
}

}
}

