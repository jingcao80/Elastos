
#include "Random.h"
#include "CSystem.h"
#include "Math.h"
#include "AutoLock.h"

using Elastos::Core::CSystem;
using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL_2(Random, Object, IRandom, ISerializable)

const Int64 Random::sMultiplier = 0x5deece66dLL;
volatile Int64 Random::mSeedBase = 0;

Random::Random()
    : mHaveNextNextGaussian(FALSE)
    , mSeed(0)
    , mNextNextGaussian(0)
{}

ECode Random::constructor()
{
    // Note: Don't use identityHashCode(this) since that causes the monitor to
    // get inflated when we synchronize.
    AutoPtr<Elastos::Core::CSystem> system;
    Elastos::Core::CSystem::AcquireSingletonByFriend((Elastos::Core::CSystem**)&system);

    Int64 tm;
    system->GetCurrentTimeMillis(&tm);
    return SetSeed(tm + (mSeedBase++));
}

ECode Random::constructor(
    /* [in] */ Int64 seed)
{
    return SetSeed(seed);
}

Int32 Random::Next(
    /* [in] */ Int32 bits)
{
    AutoLock lock(this);

    mSeed = (mSeed * sMultiplier + 0xbLL) & ((1LL << 48) - 1);
    return (Int32)(mSeed >> (48 - bits));
}

/**
 * Returns a pseudo-random uniformly distributed {@code boolean}.
 */
ECode Random::NextBoolean(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = Next(1) != 0;
    return NOERROR;
}

/**
 * Fills {@code buf} with random bytes.
 */
ECode Random::NextBytes(
    /* [out] */ ArrayOf<Byte>* buf)
{
    VALIDATE_NOT_NULL(buf);
    Int32 rando = 0, count = 0, loop = 0;
    while (count < buf->GetLength()) {
        if (loop == 0) {
            NextInt32(&rando);
            loop = 3;
        }
        else {
            loop--;
        }
        (*buf)[count++] = (Byte)rando;
        rando >>= 8;
    }
    return NOERROR;
}

/**
 * Returns a pseudo-random uniformly distributed {@code double}
 * in the half-open range [0.0, 1.0).
 */
ECode Random::NextDouble(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);
    *value = ((((Int64)Next(26) << 27) + Next(27)) / (Double)(1LL << 53));
    return NOERROR;
}

/**
 * Returns a pseudo-random uniformly distributed {@code float}
 * in the half-open range [0.0, 1.0).
 */
ECode Random::NextFloat(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = (Next(24) / (Float)16777216);
    return NOERROR;
}

/**
 * Returns a pseudo-random (approximately) normally distributed
 * {@code double} with mean 0.0 and standard deviation 1.0.
 * This method uses the <i>polar method</i> of G. E. P. Box, M.
 * E. Muller, and G. Marsaglia, as described by Donald E. Knuth in <i>The
 * Art of Computer Programming, Volume 2: Seminumerical Algorithms</i>,
 * section 3.4.1, subsection C, algorithm P.
 */
ECode Random::NextGaussian(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    AutoLock lock(this);

    if (mHaveNextNextGaussian) {
        mHaveNextNextGaussian = FALSE;
        *value = mNextNextGaussian;
        return NOERROR;
    }

    Double v1, v2, s;
    do {
        NextDouble(&v1);
        NextDouble(&v2);
        v1 = 2 * v1 - 1;
        v2 = 2 * v2 - 1;
        s = v1 * v1 + v2 * v2;
    } while (s >= 1 || s == 0);

    // The specification says this uses StrictMath.
    Double multiplier = Elastos::Core::Math::Sqrt(-2 * Elastos::Core::Math::Log(s) / s);
    mNextNextGaussian = v2 * multiplier;
    mHaveNextNextGaussian = TRUE;
    *value = v1 * multiplier;
    return NOERROR;
}

/**
 * Returns a pseudo-random uniformly distributed {@code int}.
 */
ECode Random::NextInt32(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = Next(32);
    return NOERROR;
}

/**
 * Returns a pseudo-random uniformly distributed {@code int}
 * in the half-open range [0, n).
 */
ECode Random::NextInt32(
    /* [in] */ Int32 n,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = -1;
    if (n <= 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if ((n & -n) == n) {
        *value = (Int32)((n * (Int64)Next(31)) >> 31);
        return NOERROR;
    }

    Int32 bits, val;
    do {
        bits = Next(31);
        val = bits % n;
    } while (bits - val + (n - 1) < 0);
    *value = val;
    return NOERROR;
}

/**
 * Returns a pseudo-random uniformly distributed {@code long}.
 */
ECode Random::NextInt64(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);
    *value = ((Int64)Next(32) << 32) + Next(32);
    return NOERROR;
}

/**
 * Modifies the seed using a linear congruential formula presented in <i>The
 * Art of Computer Programming, Volume 2</i>, Section 3.2.1.
 */
ECode Random::SetSeed(
    /* [in] */ Int64 seed)
{
    AutoLock lock(this);

    mSeed = (seed ^ sMultiplier) & ((1LL << 48) - 1);
    mHaveNextNextGaussian = FALSE;
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos
