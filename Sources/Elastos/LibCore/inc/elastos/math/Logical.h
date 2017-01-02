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

#ifndef __ELASTOS_MATH_LOGICAL_H__
#define __ELASTOS_MATH_LOGICAL_H__

#include <CBigInteger.h>
#include <elastos.h>

namespace Elastos {
namespace Math {

/**
 * The library implements some logical operations over {@code BigInteger}. The
 * operations provided are listed below.
 * <ul type="circle">
 * <li>not</li>
 * <li>and</li>
 * <li>andNot</li>
 * <li>or</li>
 * <li>xor</li>
 * </ul>
 */
class Logical
{
public:
    /** @see BigInteger#not() */
    static AutoPtr<IBigInteger> Not(
        /* [in] */ CBigInteger* val);

    /** @see BigInteger#and(BigInteger) */
    static AutoPtr<IBigInteger> And(
        /* [in] */ CBigInteger* val,
        /* [in] */ CBigInteger* that);

    /** @return sign = 1, magnitude = val.magnitude & that.magnitude*/
    static AutoPtr<IBigInteger> AndPositive(
        /* [in] */ CBigInteger* val,
        /* [in] */ CBigInteger* that);

    /** @return sign = positive.magnitude & magnitude = -negative.magnitude */
    static AutoPtr<IBigInteger> AndDiffSigns(
        /* [in] */ CBigInteger* positive,
        /* [in] */ CBigInteger* negative);

    /** @return sign = -1, magnitude = -(-longer.magnitude & -shorter.magnitude)*/
    static AutoPtr<IBigInteger> AndNegative(
        /* [in] */ CBigInteger* longer,
        /* [in] */ CBigInteger* shorter);

    /** @see BigInteger#andNot(BigInteger) */
    static AutoPtr<IBigInteger> AndNot(
        /* [in] */ CBigInteger* val,
        /* [in] */ CBigInteger* that);

    /** @return sign = 1, magnitude = val.magnitude & ~that.magnitude*/
    static AutoPtr<IBigInteger> AndNotPositive(
        /* [in] */ CBigInteger* val,
        /* [in] */ CBigInteger* that);

    /** @return sign = 1, magnitude = positive.magnitude & ~(-negative.magnitude)*/
    static AutoPtr<IBigInteger> AndNotPositiveNegative(
        /* [in] */ CBigInteger* positive,
        /* [in] */ CBigInteger* negative);

    /** @return sign = -1, magnitude = -(-negative.magnitude & ~positive.magnitude)*/
    static AutoPtr<IBigInteger> AndNotNegativePositive(
        /* [in] */ CBigInteger* negative,
        /* [in] */ CBigInteger* positive);

    /** @return sign = 1, magnitude = -val.magnitude & ~(-that.magnitude)*/
    static AutoPtr<IBigInteger> AndNotNegative(
        /* [in] */ CBigInteger* val,
        /* [in] */ CBigInteger* that);

    /** @see BigInteger#or(BigInteger) */
    static AutoPtr<IBigInteger> Or(
        /* [in] */ CBigInteger* val,
        /* [in] */ CBigInteger* that);

    /** @return sign = 1, magnitude = longer.magnitude | shorter.magnitude*/
    static AutoPtr<IBigInteger> OrPositive(
        /* [in] */ CBigInteger* longer,
        /* [in] */ CBigInteger* shorter);

    /** @return sign = -1, magnitude = -(-val.magnitude | -that.magnitude) */
    static AutoPtr<IBigInteger> OrNegative(
        /* [in] */ CBigInteger* val,
        /* [in] */ CBigInteger* that);

    /** @return sign = -1, magnitude = -(positive.magnitude | -negative.magnitude) */
    static AutoPtr<IBigInteger> OrDiffSigns(
        /* [in] */ CBigInteger* positive,
        /* [in] */ CBigInteger* negative);

    /** @see BigInteger#xor(BigInteger) */
    static AutoPtr<IBigInteger> Xor(
        /* [in] */ CBigInteger* val,
        /* [in] */ CBigInteger* that);

    /** @return sign = 0, magnitude = longer.magnitude | shorter.magnitude */
    static AutoPtr<IBigInteger> XorPositive(
        /* [in] */ CBigInteger* longer,
        /* [in] */ CBigInteger* shorter);

    /** @return sign = 0, magnitude = -val.magnitude ^ -that.magnitude */
    static AutoPtr<IBigInteger> XorNegative(
        /* [in] */ CBigInteger* val,
        /* [in] */ CBigInteger* that);

    /** @return sign = 1, magnitude = -(positive.magnitude ^ -negative.magnitude)*/
    static AutoPtr<IBigInteger> XorDiffSigns(
        /* [in] */ CBigInteger* positive,
        /* [in] */ CBigInteger* negative);

private:
    /** Just to denote that this class can't be instantiated. */
    Logical();

    Logical(
        /* [in] */ const Logical&);

    Logical& operator = (
        /* [in] */ const Logical&);
};

} // namespace Math
} // namespace Elastos

#endif // __ELASTOS_MATH_LOGICAL_H__
