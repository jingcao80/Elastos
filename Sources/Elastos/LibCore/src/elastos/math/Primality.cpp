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

#include "Primality.h"
#include "BigInt.h"

namespace Elastos {
namespace Math {

const Int32 Primality::Primes[] = {
    2, 3, 5, 7, 11, 13, 17, 19, 23, 29,
    31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101,
    103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167,
    173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239,
    241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313,
    317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397,
    401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467,
    479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569,
    571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643,
    647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733,
    739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823,
    827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911,
    919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997, 1009,
    1013, 1019, 1021
};

AutoPtr<ArrayOf<IBigInteger*> > Primality::BIprimes = NULL;

ECode Primality::NextProbablePrime(
    /* [in] */ CBigInteger* n,
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);

    // // To initialize the dual table of BigInteger primes
    if (BIprimes.Get() == NULL) {
        BIprimes = ArrayOf<IBigInteger*>::Alloc(PrimesLength);
        for (Int32 i = 0; i < PrimesLength; i++) {
            CBigInteger::ValueOf(Primes[i], &((*BIprimes)[i]));
        }
    }

    // PRE: n >= 0
    Int32 i, j;
//        Int32 certainty;

    AutoPtr<BigInt> ni = n->GetBigInt();
    // If n < "last prime of table" searches next prime in the table
    if (ni->BitLength() <= 10) {
        Int32 l = (Int32)ni->LongInt();
        if (l < Primes[PrimesLength - 1]) {
            for (i = 0; l >= Primes[i]; i++) {}

            *result = (*BIprimes)[i];
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }

    Int32 gapSize = 1024; // for searching of the next probable prime number
    AutoPtr< ArrayOf<Int32> > modules = ArrayOf<Int32>::Alloc(PrimesLength);
    AutoPtr< ArrayOf<Boolean> > isDivisible = ArrayOf<Boolean>::Alloc(gapSize);

    AutoPtr<BigInt> startPoint = new BigInt();
    FAIL_RETURN(ni->Copy(*startPoint));
    AutoPtr<BigInt> probPrime = new BigInt();

    // Fix startPoint to "next odd number":
    Int32 r;
    FAIL_RETURN(BigInt::RemainderByPositiveInt(*ni, 2, r));
    startPoint->AddPositiveInt(r + 1);

//        // To set the improved certainty of Miller-Rabin
//        j = startPoint.bitLength();
//        for (certainty = 2; j < BITS[certainty]; certainty++) {
//            ;
//        }

    // To calculate modules: N mod p1, N mod p2, ... for first primes.
    for (i = 0; i < PrimesLength; i++) {
        FAIL_RETURN(BigInt::RemainderByPositiveInt(*startPoint, Primes[i], r));
        (*modules)[i] = r - gapSize;
    }
    while (TRUE) {
        // At this point, all numbers in the gap are initialized as
        // probably primes
        memset(isDivisible->GetPayload(), 0, sizeof(Boolean) * isDivisible->GetLength());

        // To discard multiples of first primes
        for (i = 0; i < PrimesLength; i++) {
            (*modules)[i] = ((*modules)[i] + gapSize) % Primes[i];
            j = ((*modules)[i] == 0) ? 0 : (Primes[i] - (*modules)[i]);
            for (; j < gapSize; j += Primes[i]) {
                (*isDivisible)[j] = TRUE;
            }
        }
        // To execute Miller-Rabin for non-divisible numbers by all first
        // primes
        for (j = 0; j < gapSize; j++) {
            if (!(*isDivisible)[j]) {
                FAIL_RETURN(probPrime->PutCopy(*startPoint));
                probPrime->AddPositiveInt(j);
                if (probPrime->IsPrime(100)) {
                    return CBigInteger::NewFromBigInt(probPrime, result);
                }
            }
        }
        startPoint->AddPositiveInt(gapSize);
    }
    return NOERROR;
}

} // namespace Math
} // namespace Elastos
