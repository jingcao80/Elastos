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
 * Methods: and, andNot
 */
public class BigIntegerNotTest extends TestCase {
#endif

    /**
     * andNot for two positive numbers; the first is longer
     */
#if 0
    public void testAndNotPosPosFirstLonger() {
        byte aBytes[] = {-128, 9, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117, 23, 87, -25, -75};
        byte bBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {0, -128, 9, 56, 100, 0, 0, 1, 1, 90, 1, -32, 0, 10, -126, 21, 82, -31, -96};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.andNot(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif
void testAndNotPosPosFirstLonger() {
    signed char _aBytes[] = {-128, 9, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117, 23, 87, -25, -75};
    signed char _bBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
    signed char _rBytes[] = {0, -128, 9, 56, 100, 0, 0, 1, 1, 90, 1, -32, 0, 10, -126, 21, 82, -31, -96};
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

    aNumber->AndNot(bNumber, (IBigInteger **)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", 1, sign);
}


    /**
     * andNot for two positive numbers; the first is shorter
     */
#if 0
    public void testAndNotPosPosFirstShorter() {
        byte aBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        byte bBytes[] = {-128, 9, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117, 23, 87, -25, -75};
        int aSign = 1;
        int bSign = 1;
        byte rBytes[] = {73, -92, -48, 4, 12, 6, 4, 32, 48, 64, 0, 8, 2};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.andNot(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif
void testAndNotPosPosFirstShorter() {
    signed char _aBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
    signed char _bBytes[] = {-128, 9, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117, 23, 87, -25, -75};
    signed char _rBytes[] = {73, -92, -48, 4, 12, 6, 4, 32, 48, 64, 0, 8, 2};
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

    aNumber->AndNot(bNumber, (IBigInteger **)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", 1, sign);
}


    /**
     * andNot for two negative numbers; the first is longer
     */
#if 0
    public void testAndNotNegNegFirstLonger() {
        byte aBytes[] = {-128, 9, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117, 23, 87, -25, -75};
        byte bBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        int aSign = -1;
        int bSign = -1;
        byte rBytes[] = {73, -92, -48, 4, 12, 6, 4, 32, 48, 64, 0, 8, 2};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.andNot(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif
void testAndNotNegNegFirstLonger() {
    signed char _aBytes[] = {-128, 9, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117, 23, 87, -25, -75};
    signed char _bBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
    signed char _rBytes[] = {73, -92, -48, 4, 12, 6, 4, 32, 48, 64, 0, 8, 2};
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

    aNumber->AndNot(bNumber, (IBigInteger **)&result);
    result->ToByteArray((ArrayOf<Byte> **)&resBytes);

    for(int i = 0; i < resBytes->GetLength(); i++) {
        assertEquals("data error", (*(ArrayOf<Byte> *)(&resBytes))[i], (*(ArrayOf<Byte> *)(&rBytes))[i]);
    }

    Int32 sign;
    result->GetSignum(&sign);
    assertEquals("incorrect sign", 1, sign);
}


    /**
     * andNot for a negative and a positive numbers; the first is longer
     */
#if 0
    public void testNegPosFirstLonger() {
        byte aBytes[] = {-128, 9, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117, 23, 87, -25, -75};
        byte bBytes[] = {-2, -3, -4, -4, 5, 14, 23, 39, 48, 57, 66, 5, 14, 23};
        int aSign = -1;
        int bSign = 1;
        byte rBytes[] = {-1, 127, -10, -57, -101, 1, 2, 2, 2, -96, -16, 8, -40, -59, 68, -88, -88, 16, 72};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger bNumber = new BigInteger(bSign, bBytes);
        BigInteger result = aNumber.andNot(bNumber);
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * Not for ZERO
     */
#if 0
    public void testNotZero() {
        byte rBytes[] = {-1};
        BigInteger aNumber = BigInteger.ZERO;
        BigInteger result = aNumber.not();
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * Not for ONE
     */
#if 0
    public void testNotOne() {
        byte rBytes[] = {-2};
        BigInteger aNumber = BigInteger.ONE;
        BigInteger result = aNumber.not();
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * Not for a positive number
     */
#if 0
    public void testNotPos() {
        byte aBytes[] = {-128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117};
        int aSign = 1;
        byte rBytes[] = {-1, 127, -57, -101, 1, 75, -90, -46, -92, -4, 14, -36, -27, 116};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.not();
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif

    /**
     * Not for a negative number
     */
#if 0
    public void testNotNeg() {
        byte aBytes[] = {-128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -117};
        int aSign = -1;
        byte rBytes[] = {0, -128, 56, 100, -2, -76, 89, 45, 91, 3, -15, 35, 26, -118};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.not();
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", 1, result.signum());
    }
#endif

    /**
     * Not for a negative number
     */

#if 0
    public void testNotSpecialCase() {
        byte aBytes[] = {-1, -1, -1, -1};
        int aSign = 1;
        byte rBytes[] = {-1, 0, 0, 0, 0};
        BigInteger aNumber = new BigInteger(aSign, aBytes);
        BigInteger result = aNumber.not();
        byte resBytes[] = new byte[rBytes.length];
        resBytes = result.toByteArray();
        for(int i = 0; i < resBytes.length; i++) {
            assertTrue(resBytes[i] == rBytes[i]);
        }
        assertEquals("incorrect sign", -1, result.signum());
    }
#endif
//==============================================================================

int mainBigIntegerNotTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/BigIntegerNotTest ====\n");
    testAndNotPosPosFirstLonger();
    testAndNotPosPosFirstShorter();
    testAndNotPosPosFirstShorter();
    printf("\n==== end of libcore/math/BigIntegerNotTest ====\n");

    return 0;
}

}
}