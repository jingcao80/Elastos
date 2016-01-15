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

package org.apache.harmony.tests.java.math;

import java.math.BigDecimal;
import java.math.RoundingMode;

public class RoundingModeTest extends junit.framework.TestCase {

    /**
     * java.math.RoundingMode#valueOf(int)
     */
    public void test_valueOfI() {
        assertEquals("valueOf failed for ROUND_CEILING", RoundingMode.valueOf(BigDecimal.ROUND_CEILING), RoundingMode.CEILING);
        assertEquals("valueOf failed for ROUND_DOWN", RoundingMode.valueOf(BigDecimal.ROUND_DOWN), RoundingMode.DOWN);
        assertEquals("valueOf failed for ROUND_FLOOR", RoundingMode.valueOf(BigDecimal.ROUND_FLOOR), RoundingMode.FLOOR);
        assertEquals("valueOf failed for ROUND_HALF_DOWN", RoundingMode.valueOf(BigDecimal.ROUND_HALF_DOWN), RoundingMode.HALF_DOWN);
        assertEquals("valueOf failed for ROUND_HALF_EVEN", RoundingMode.valueOf(BigDecimal.ROUND_HALF_EVEN), RoundingMode.HALF_EVEN);
        assertEquals("valueOf failed for ROUND_HALF_UP", RoundingMode.valueOf(BigDecimal.ROUND_HALF_UP), RoundingMode.HALF_UP);
        assertEquals("valueOf failed for ROUND_UNNECESSARY", RoundingMode.valueOf(BigDecimal.ROUND_UNNECESSARY), RoundingMode.UNNECESSARY);
        assertEquals("valueOf failed for ROUND_UP", RoundingMode.valueOf(BigDecimal.ROUND_UP), RoundingMode.UP);
        try {
            RoundingMode.valueOf(13);
            fail("IllegalArgumentException expected for RoundingMode(13)");
        } catch (IllegalArgumentException e) {
        }
        try {
            RoundingMode.valueOf(-1);
            fail("IllegalArgumentException expected for RoundingMode(-1)");
        } catch (IllegalArgumentException e) {
        }
    }

#endif

//==============================================================================

int mainRoundingModeTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/RoundingModeTest ====\n");
    printf("\n==== end of libcore/math/RoundingModeTest ====\n");

    return 0;
}


}
}