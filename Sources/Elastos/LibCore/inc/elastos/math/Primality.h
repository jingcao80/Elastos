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

#ifndef __ELASTOS_MATH_PRIMALITY_H__
#define __ELASTOS_MATH_PRIMALITY_H__

#include <CBigInteger.h>
#include <elastos.h>
#include <elquintet.h>

namespace Elastos {
namespace Math {

class Primality
{
public:
    /**
     * It uses the sieve of Eratosthenes to discard several composite numbers in
     * some appropriate range (at the moment {@code [this, this + 1024]}). After
     * this process it applies the Miller-Rabin test to the numbers that were
     * not discarded in the sieve.
     *
     * @see BigInteger#nextProbablePrime()
     */
    static CARAPI NextProbablePrime(
        /* [in] */ CBigInteger* n,
        /* [out] */ IBigInteger** result);

private:
    /** Just to denote that this class can't be instantiated. */
    Primality();

    Primality(
        /* [in] */ const Primality&);

    Primality& operator = (
        /* [in] */ const Primality&);

private:
    static const Int32 PrimesLength = 172;

    /** All prime numbers with bit length lesser than 10 bits. */
    static const Int32 Primes[PrimesLength];

    /** All {@code BigInteger} prime numbers with bit length lesser than 10 bits. */
    static AutoPtr<ArrayOf<IBigInteger*> > BIprimes;
};

} // namespace Math
} // namespace Elastos

#endif // __ELASTOS_MATH_PRIMALITY_H__
