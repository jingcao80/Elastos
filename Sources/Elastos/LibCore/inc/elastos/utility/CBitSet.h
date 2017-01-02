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

#ifndef __ELASTOS_UTILITY_CBITSET_H__
#define __ELASTOS_UTILITY_CBITSET_H__

#include "_Elastos_Utility_CBitSet.h"
#include "Object.h"

using Elastos::Core::ICloneable;
using Elastos::Core::Object;
using Elastos::IO::IInt64Buffer;
using Elastos::IO::IByteBuffer;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Utility {

CarClass(CBitSet)
    , public Object
    , public IBitSet
    , public ISerializable
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBitSet();

    /**
     * Creates a new {@code BitSet} with size equal to 64 bits.
     */
    CARAPI constructor();

    /**
     * Creates a new {@code BitSet} with size equal to {@code bitCount}, rounded up to
     * a multiple of 64.
     *
     * @throws NegativeArraySizeException if {@code bitCount < 0}.
     */
    CARAPI constructor(
        /* [in] */ Int32 bitCount);

    /* private */
    CARAPI constructor(
        /* [in] */ ArrayOf<Int64>* bits);

    /**
     * Returns the bit at index {@code index}. Indexes greater than the current length return false.
     *
     * @throws IndexOutOfBoundsException if {@code index < 0}.
     */
    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Boolean* value);

    /**
     * Returns a new {@code BitSet} containing the
     * range of bits {@code [fromIndex, toIndex)}, shifted down so that the bit
     * at {@code fromIndex} is at bit 0 in the new {@code BitSet}.
     *
     * @throws IndexOutOfBoundsException
     *             if {@code fromIndex} or {@code toIndex} is negative, or if
     *             {@code toIndex} is smaller than {@code fromIndex}.
     */
    CARAPI Get(
        /* [in] */ Int32 fromIndex,
        /* [in] */ Int32 toIndex,
        /* [out] */ IBitSet** bs);

    /**
     * Sets the bit at index {@code index} to true.
     *
     * @throws IndexOutOfBoundsException if {@code index < 0}.
     */
    CARAPI Set(
        /* [in] */ Int32 index);

    /**
     * Sets the bit at index {@code index} to {@code state}.
     *
     * @throws IndexOutOfBoundsException if {@code index < 0}.
     */
    CARAPI Set(
        /* [in] */ Int32 index,
        /* [in] */ Boolean state);

    /**
     * Sets the range of bits {@code [fromIndex, toIndex)}.
     *
     * @throws IndexOutOfBoundsException
     *             if {@code fromIndex} or {@code toIndex} is negative, or if
     *             {@code toIndex} is smaller than {@code fromIndex}.
     */
    CARAPI Set(
        /* [in] */ Int32 fromIndex,
        /* [in] */ Int32 toIndex);

    /**
     * Sets the range of bits {@code [fromIndex, toIndex)} to {@code state}.
     *
     * @throws IndexOutOfBoundsException
     *             if {@code fromIndex} or {@code toIndex} is negative, or if
     *             {@code toIndex} is smaller than {@code fromIndex}.
     */
    CARAPI Set(
        /* [in] */ Int32 fromIndex,
        /* [in] */ Int32 toIndex,
        /* [in] */ Boolean state);

    /**
     * Clears the bit at index {@code index}.
     *
     * @throws IndexOutOfBoundsException if {@code index < 0}.
     */
    CARAPI Clear(
        /* [in] */ Int32 index);

    /**
     * Clears the range of bits {@code [fromIndex, toIndex)}.
     *
     * @throws IndexOutOfBoundsException
     *             if {@code fromIndex} or {@code toIndex} is negative, or if
     *             {@code toIndex} is smaller than {@code fromIndex}.
     */
    CARAPI Clear(
        /* [in] */ Int32 fromIndex,
        /* [in] */ Int32 toIndex);

    /**
     * Clears all the bits in this {@code BitSet}. This method does not change the capacity.
     * Use {@code clear} if you want to reuse this {@code BitSet} with the same capacity, but
     * create a new {@code BitSet} if you're trying to potentially reclaim memory.
     */
    CARAPI Clear();

    /**
     * Flips the bit at index {@code index}.
     *
     * @throws IndexOutOfBoundsException if {@code index < 0}.
     */
    CARAPI Flip(
        /* [in] */ Int32 index);

    /**
     * Flips the range of bits {@code [fromIndex, toIndex)}.
     *
     * @throws IndexOutOfBoundsException
     *             if {@code fromIndex} or {@code toIndex} is negative, or if
     *             {@code toIndex} is smaller than {@code fromIndex}.
     */
    CARAPI Flip(
        /* [in] */ Int32 fromIndex,
        /* [in] */ Int32 toIndex);

    /**
     * Returns true if {@code this.and(bs)} is non-empty, but may be faster than computing that.
     */
    CARAPI Intersects(
        /* [in] */ IBitSet* bs,
        /* [out] */ Boolean* result);

    /**
     * Logically ands the bits of this {@code BitSet} with {@code bs}.
     */
    CARAPI And(
        /* [in] */ IBitSet* bs);

    /**
     * Clears all bits in this {@code BitSet} which are also set in {@code bs}.
     */
    CARAPI AndNot(
        /* [in] */ IBitSet* bs);

    /**
     * Logically ors the bits of this {@code BitSet} with {@code bs}.
     */
    CARAPI Or(
        /* [in] */ IBitSet* bs);

    /**
     * Logically xors the bits of this {@code BitSet} with {@code bs}.
     */
    CARAPI Xor(
        /* [in] */ IBitSet* bs);

    /**
     * Returns the capacity in bits of the array implementing this {@code BitSet}. This is
     * unrelated to the length of the {@code BitSet}, and not generally useful.
     * Use {@link #nextSetBit} to iterate, or {@link #length} to find the highest set bit.
     */
    CARAPI GetSize(
        /* [out] */ Int32* size);

    /**
     * Returns the number of bits up to and including the highest bit set. This is unrelated to
     * the {@link #size} of the {@code BitSet}.
     */
    CARAPI GetLength(
        /* [out] */ Int32* len);

    /**
     * Returns the index of the first bit that is set on or after {@code index}, or -1
     * if no higher bits are set.
     * @throws IndexOutOfBoundsException if {@code index < 0}.
     */
    CARAPI NextSetBit(
        /* [in] */ Int32 index,
        /* [out] */ Int32* next);

    /**
     * Returns the index of the first bit that is clear on or after {@code index}.
     * Since all bits past the end are implicitly clear, this never returns -1.
     * @throws IndexOutOfBoundsException if {@code index < 0}.
     */
    CARAPI NextClearBit(
        /* [in] */ Int32 index,
        /* [out] */ Int32* next);

    /**
     * Returns the index of the first bit that is set on or before {@code index}, or -1 if
     * no lower bits are set or {@code index == -1}.
     * @throws IndexOutOfBoundsException if {@code index < -1}.
     * @hide 1.7
     */
    CARAPI PreviousSetBit(
        /* [in] */ Int32 index,
        /* [out] */ Int32* previous);

    /**
     * Returns the index of the first bit that is clear on or before {@code index}, or -1 if
     * no lower bits are clear or {@code index == -1}.
     * @throws IndexOutOfBoundsException if {@code index < -1}.
     * @hide 1.7
     */
    CARAPI PreviousClearBit(
        /* [in] */ Int32 index,
        /* [out] */ Int32* previous);

    /**
     * Returns true if all the bits in this {@code BitSet} are set to false, false otherwise.
     */
    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    /**
     * Returns the number of bits that are {@code true} in this {@code BitSet}.
     */
    CARAPI Cardinality(
        /* [out] */ Int32* number);

    /**
     * Equivalent to {@code BitSet.valueOf(LongBuffer.wrap(longs))}, but likely to be faster.
     * This is likely to be the fastest way to create a {@code BitSet} because it's closest
     * to the internal representation.
     * @hide 1.7
     */
    static CARAPI ValueOf(
        /* [in] */ ArrayOf<Int64>* int64Arr,
        /* [out] */ IBitSet** bs);

    /**
     * Returns a {@code BitSet} corresponding to {@code longBuffer}, interpreted as a little-endian
     * sequence of bits. This method does not alter the {@code LongBuffer}.
     * @hide 1.7
     */
    static CARAPI ValueOf(
        /* [in] */ IInt64Buffer* int64Arr,
        /* [out] */ IBitSet** bs);

    /**
     * Equivalent to {@code BitSet.valueOf(ByteBuffer.wrap(bytes))}.
     * @hide 1.7
     */
    static CARAPI ValueOf(
        /* [in] */ ArrayOf<Byte>* byteArr,
        /* [out] */ IBitSet** bs);

    /**
     * Returns a {@code BitSet} corresponding to {@code byteBuffer}, interpreted as a little-endian
     * sequence of bits. This method does not alter the {@code ByteBuffer}.
     * @hide 1.7
     */
    static CARAPI ValueOf(
        /* [in] */ IByteBuffer* byteArr,
        /* [out] */ IBitSet** bs);

    /**
     * Returns a new {@code long[]} containing a little-endian representation of the bits of
     * this {@code BitSet}, suitable for passing to {@code valueOf} to reconstruct
     * this {@code BitSet}.
     * @hide 1.7
     */
    CARAPI ToInt64Array(
        /* [out, callee] */ ArrayOf<Int64>** int64Arr);

    /**
     * Returns a new {@code byte[]} containing a little-endian representation the bits of
     * this {@code BitSet}, suitable for passing to {@code valueOf} to reconstruct
     * this {@code BitSet}.
     * @hide 1.7
     */
    CARAPI ToByteArray(
        /* [out, callee] */ ArrayOf<Byte>** byteArr);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ToString(
        /* [out] */ String* value);

    CARAPI Clone(
        /* [out] */ IInterface** object);

    CARAPI ToArray(
            /* [out] */ ArrayOf<Int64>** arr);

    CARAPI ToArray(
            /* [out] */ ArrayOf<Byte>** int64Arr);

private:
    /**
     * Updates 'longCount' by inspecting 'bits'. Assumes that the new longCount is <= the current
     * longCount, to avoid scanning large tracts of empty array. This means it's safe to call
     * directly after a clear operation that may have cleared the highest set bit, but
     * not safe after an xor operation that may have cleared the highest set bit or
     * made a new highest set bit. In that case, you'd need to set 'longCount' to a conservative
     * estimate before calling this method.
     */
    CARAPI_(void) ShrinkSize();

    static CARAPI_(AutoPtr< ArrayOf<Int64> >) ArrayForBits(
        /* [in] */ Int32 bitCount);

    /**
     * Ensures that our long[] can hold at least 64 * desiredLongCount bits.
     */
    CARAPI_(void) EnsureCapacity(
        /* [in] */ Int32 desiredInt64Count);

    CARAPI CheckIndex(
        /* [in] */ Int32 index);

    CARAPI CheckRange(
        /* [in] */ Int32 fromIndex,
        /* [in] */ Int32 toIndex);

private:
    static const Int64 ALL_ONES = ~0ll;

    /**
     * The bits. Access bit n thus:
     *
     *   boolean bit = (bits[n / 64] | (1 << n)) != 0;
     *
     * Note that Java's shift operators truncate their rhs to the log2 size of the lhs.
     * That is, there's no "% 64" needed because it's implicit in the shift.
     *
     * TODO: would int[] be significantly more efficient for Android at the moment?
     */
    AutoPtr< ArrayOf<Int64> > mBits;

    /**
     * The number of elements of 'bits' that are actually in use (non-zero). Amongst other
     * things, this guarantees that isEmpty is cheap, because we never have to examine the array.
     */
    Int32 mInt64Count;
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CBITSET_H__
