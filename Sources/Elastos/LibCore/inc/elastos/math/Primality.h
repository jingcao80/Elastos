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
