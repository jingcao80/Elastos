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

#ifndef __ELASTOS_MATH_MULTIPLICATION_H__
#define __ELASTOS_MATH_MULTIPLICATION_H__

#include <CBigInteger.h>
#include <elastos.h>

namespace Elastos {
namespace Math {

class Multiplication
{
private:
    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    /**
     * Multiplies a number by a positive integer.
     * @param val an arbitrary {@code BigInteger}
     * @param factor a positive {@code int} number
     * @return {@code val * factor}
     */
    static CARAPI MultiplyByPositiveInt(
        /* [in] */ IBigInteger* val,
        /* [in] */ Int32 factor,
        /* [out] */ IBigInteger** result);

    /**
     * Multiplies a number by a power of ten.
     * This method is used in {@code BigDecimal} class.
     * @param val the number to be multiplied
     * @param exp a positive {@code long} exponent
     * @return {@code val * 10<sup>exp</sup>}
     */
    static CARAPI MultiplyByTenPow(
        /* [in] */ IBigInteger* val,
        /* [in] */ Int64 exp,
        /* [out] */ IBigInteger** result);

    /**
     * It calculates a power of ten, which exponent could be out of 32-bit range.
     * Note that internally this method will be used in the worst case with
     * an exponent equals to: {@code Integer.MAX_VALUE - Integer.MIN_VALUE}.
     * @param exp the exponent of power of ten, it must be positive.
     * @return a {@code BigInteger} with value {@code 10<sup>exp</sup>}.
     */
    static CARAPI PowerOf10(
        /* [in] */ Int64 exp,
        /* [out] */ IBigInteger** result);

    /**
     * Multiplies a number by a power of five.
     * This method is used in {@code BigDecimal} class.
     * @param val the number to be multiplied
     * @param exp a positive {@code int} exponent
     * @return {@code val * 5<sup>exp</sup>}
     */
    static CARAPI MultiplyByFivePow(
        /* [in] */ IBigInteger* val,
        /* [in] */ Int32 exp,
        /* [out] */ IBigInteger** result);

    static CARAPI_(void) Init();

private:
    /** Just to denote that this class can't be instantiated. */
    Multiplication();

    Multiplication(
        /* [in] */ const Multiplication&);

    Multiplication& operator = (
        /* [in] */ const Multiplication&);

public:
    /**
     * An array with the first powers of ten in {@code BigInteger} version.
     * ({@code 10^0,10^1,...,10^31})
     */
    static AutoPtr<ArrayOf<IBigInteger*> > sBigTenPows;

    /**
     * An array with the first powers of five in {@code BigInteger} version.
     * ({@code 5^0,5^1,...,5^31})
     */
    static AutoPtr<ArrayOf<IBigInteger*> > sBigFivePows;

private:
    /**
     * An array with powers of ten that fit in the type {@code int}.
     * ({@code 10^0,10^1,...,10^9})
     */
    static const Int32 sTenPowsLength = 10;
    static const Int32 sTenPows[sTenPowsLength];

    /**
     * An array with powers of five that fit in the type {@code int}.
     * ({@code 5^0,5^1,...,5^13})
     */
    static const Int32 sFivePowsLength = 14;
    static const Int32 sFivePows[sFivePowsLength];
    static StaticInitializer sInitializer;
};

} // namespace Math
} // namespace Elastos

#endif // __ELASTOS_MATH_MULTIPLICATION_H__
