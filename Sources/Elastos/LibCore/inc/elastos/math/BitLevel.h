#ifndef __ELASTOS_MATH_BITLEVEL_H__
#define __ELASTOS_MATH_BITLEVEL_H__

#include <CBigInteger.h>
#include <elastos.h>

namespace Elastos {
namespace Math {

/**
 * Static library that provides all the <b>bit level</b> operations for
 * {@link BigInteger}. The operations are:
 * <ul type="circle">
 * <li>Left Shifting</li>
 * <li>Right Shifting</li>
 * <li>Bit clearing</li>
 * <li>Bit setting</li>
 * <li>Bit counting</li>
 * <li>Bit testing</li>
 * <li>Getting of the lowest bit set</li>
 * </ul>
 * All operations are provided in immutable way, and some in both mutable and
 * immutable.
 */
class BitLevel
{

public:
    /** @see BigInteger#bitLength() */
    static Int32 BitLength(
        /* [in] */ CBigInteger& val);

    /** @see BigInteger#bitCount() */
    static Int32 BitCount(
        /* [in] */ CBigInteger& val);

    /**
     * Performs a fast bit testing for positive numbers. The bit to to be tested
     * must be in the range {@code [0, val.bitLength()-1]}
     */
    static Boolean TestBit(
        /* [in] */ CBigInteger& val,
        /* [in] */ Int32 n);

    /**
     * Check if there are 1s in the lowest bits of this BigInteger
     *
     * @param numberOfBits the number of the lowest bits to check
     * @return false if all bits are 0s, true otherwise
     */
    static Boolean NonZeroDroppedBits(
        /* [in] */ Int32 numberOfBits,
        /* [in] */ const ArrayOf<Int32>& digits);

    static Void ShiftLeftOneBit(
        /* [in,out] */ ArrayOf<Int32>& result,
        /* [in] */ const ArrayOf<Int32>& source,
        /* [in] */ Int32 srcLen);

    static CARAPI ShiftLeftOneBit(
        /* [in] */ CBigInteger* source,
        /* [out] */ IBigInteger** result);

    /** @see BigInteger#shiftRight(Int32) */
    static CARAPI ShiftRight(
        /* [in] */ CBigInteger* source,
        /* [in] */ Int32 count,
        /* [out] */ IBigInteger** result);

    /**
     * Shifts right an array of integers. Total shift distance in bits is
     * intCount * 32 + count.
     *
     * @param result
     *            the destination array
     * @param resultLen
     *            the destination array's length
     * @param source
     *            the source array
     * @param intCount
     *            the number of elements to be shifted
     * @param count
     *            the number of bits to be shifted
     * @return dropped bit's are all zero (i.e. remaider is zero)
     */
    static Boolean ShiftRight(
        /* [in] */ ArrayOf<Int32>& result,
        /* [in] */ const ArrayOf<Int32>& source,
        /* [in] */ Int32 intCount,
        /* [in] */ Int32 count);

    /**
     * Performs a flipBit on the BigInteger, returning a BigInteger with the the
     * specified bit flipped.
     */
    static CARAPI FlipBit(
        /* [in] */ CBigInteger* val,
        /* [in] */ Int32 n,
        /* [out] */ IBigInteger** result);

private:
    /** Just to denote that this class can't be instantiated. */
    BitLevel();

    BitLevel(
        /* [in] */ const BitLevel&);

    BitLevel& operator = (
        /* [in] */ const BitLevel&);
};

} // namespace Math
} // namespace Elastos

#endif // __ELASTOS_MATH_BITLEVEL_H__
