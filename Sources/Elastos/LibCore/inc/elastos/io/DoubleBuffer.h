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

#ifndef __ELASTOS_IO_DOUBLEBUFFER_H__
#define __ELASTOS_IO_DOUBLEBUFFER_H__

#include "Buffer.h"

namespace Elastos {
namespace IO {


/**
 * A buffer of chars.
 * <p>
 * A char buffer can be created in either one of the following ways:
 * <ul>
 * <li>{@link #allocate(int) Allocate} a new char array and create a buffer
 * based on it;</li>
 * <li>{@link #wrap(char[]) Wrap} an existing char array to create a new
 * buffer;</li>
 * <li>{@link #wrap(CharSequence) Wrap} an existing char sequence to create a
 * new buffer;</li>
 * <li>Use {@link java.nio.ByteBuffer#asDoubleBuffer() ByteBuffer.asDoubleBuffer}
 * to create a char buffer based on a byte buffer.</li>
 * </ul>
 */
class DoubleBuffer
    : public Buffer
    , public IDoubleBuffer
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a double buffer based on a newly allocated double array.
     *
     * @param capacity
     *            the capacity of the new buffer.
     * @return the created double buffer.
     * @throws IllegalArgumentException
     *             if {@code capacity} is less than zero.
     */
    static CARAPI Allocate(
        /* [in] */ Int32 capacity,
        /* [out] */ IDoubleBuffer** buf);

    /**
     * Creates a new double buffer by wrapping the given double array.
     * <p>
     * Calling this method has the same effect as
     * {@code wrap(array, 0, array.length)}.
     *
     * @param array
     *            the double array which the new buffer will be based on.
     * @return the created double buffer.
     */
    static CARAPI Wrap(
        /* [in] */ ArrayOf<Double>* array,
        /* [out] */ IDoubleBuffer** buf);

    /**
     * Creates a new double buffer by wrapping the given double array.
     * <p>
     * The new buffer's position will be {@code start}, limit will be
     * {@code start + doubleCount}, capacity will be the length of the array.
     *
     * @param array
     *            the double array which the new buffer will be based on.
     * @param start
     *            the start index, must not be negative and not greater than
     *            {@code array.length}.
     * @param doubleCount
     *            the length, must not be negative and not greater than
     *            {@code array.length - start}.
     * @return the created double buffer.
     * @exception IndexOutOfBoundsException
     *                if either {@code start} or {@code doubleCount} is invalid.
     */
    static CARAPI Wrap(
        /* [in] */ ArrayOf<Double>* array,
        /* [in] */ Int32 start,
        /* [in] */ Int32 doubleCount,
        /* [out] */ IDoubleBuffer** buf);

    CARAPI GetArray(
        /* [out] */ IArrayOf** array);

    CARAPI GetArray(
        /* [out, callee] */ ArrayOf<Double>** array);

    CARAPI GetArrayOffset(
        /* [out] */ Int32* offset);

    /**
     * Returns a read-only buffer that shares its content with this buffer.
     * <p>
     * The returned buffer is guaranteed to be a new instance, even if this
     * buffer is read-only itself. The new buffer's position, limit, capacity
     * and mark are the same as this buffer's.
     * <p>
     * The new buffer shares its content with this buffer, which means that this
     * buffer's change of content will be visible to the new buffer. The two
     * buffer's position, limit and mark are independent.
     *
     * @return a read-only version of this buffer.
     */
    virtual CARAPI AsReadOnlyBuffer(
        /* [out] */ IDoubleBuffer** buffer) = 0;

    /**
     * Compacts this double buffer.
     * <p>
     * The remaining doubles will be moved to the head of the buffer, staring
     * from position zero. Then the position is set to {@code remaining()}; the
     * limit is set to capacity; the mark is cleared.
     *
     * @return this buffer.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Compact() = 0;

    /**
     * Compare the remaining doubles of this buffer to another double buffer's
     * remaining doubles.
     *
     * @param otherBuffer
     *            another double buffer.
     * @return a negative value if this is less than {@code other}; 0 if this
     *         equals to {@code other}; a positive value if this is greater
     *         than {@code other}.
     * @exception ClassCastException
     *                if {@code other} is not a double buffer.
     */
    virtual CARAPI CompareTo(
        /* [in] */ IDoubleBuffer* otherBuffer,
        /* [out] */ Int32* result);

    /**
     * Returns a duplicated buffer that shares its content with this buffer.
     * <p>
     * The duplicated buffer's position, limit, capacity and mark are the same
     * as this buffer's. The duplicated buffer's read-only property and byte
     * order are the same as this buffer's, too.
     * <p>
     * The new buffer shares its content with this buffer, which means either
     * buffer's change of content will be visible to the other. The two buffer's
     * position, limit and mark are independent.
     *
     * @return a duplicated buffer that shares its content with this buffer.
     */
    virtual CARAPI Duplicate(
        /* [out] */ IDoubleBuffer** buffer) = 0;

    /**
     * Checks whether this double buffer is equal to another object. If {@code
     * other} is not a {@code DoubleBuffer} then {@code false} is returned.
     *
     * <p>Two double buffers are equal if their remaining doubles are equal.
     * Position, limit, capacity and mark are not considered.
     *
     * <p>This method considers two doubles {@code a} and {@code b} to be equal
     * if {@code a == b} or if {@code a} and {@code b} are both {@code NaN}.
     * Unlike {@link Double#equals}, this method considers {@code -0.0} and
     * {@code +0.0} to be equal.
     *
     * @param other
     *            the object to compare with this double buffer.
     * @return {@code true} if this double buffer is equal to {@code other},
     *         {@code false} otherwise.
     */
    virtual CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* rst);

    /**
     * Returns the double at the current position and increases the position by
     * 1.
     *
     * @return the double at the current position.
     * @exception BufferUnderflowException
     *                if the position is equal or greater than limit.
     */
    virtual CARAPI Get(
        /* [out] */ Double* value) = 0;

    /**
     * Returns a double at the specified index; the position is not changed.
     *
     * @param index
     *            the index, must not be negative and less than limit.
     * @return a double at the specified index.
     * @exception IndexOutOfBoundsException
     *                if index is invalid.
     */
    virtual CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Double* value) = 0;

    /**
     * Reads doubles from the current position into the specified double array
     * and increases the position by the number of doubles read.
     * <p>
     * Calling this method has the same effect as
     * {@code get(dst, 0, dst.length)}.
     *
     * @param dst
     *            the destination double array.
     * @return this buffer.
     * @exception BufferUnderflowException
     *                if {@code dst.length} is greater than {@code remaining()}.
     */
    virtual CARAPI Get(
        /* [out] */ ArrayOf<Double>* dst);

    /**
     * Reads doubles from the current position into the specified double array,
     * starting from the specified offset, and increases the position by the
     * number of doubles read.
     *
     * @param dst
     *            the target double array.
     * @param dstOffset
     *            the offset of the double array, must not be negative and not
     *            greater than {@code dst.length}.
     * @param doubleCount
     *            the number of doubles to read, must be no less than zero and
     *            not greater than {@code dst.length - dstOffset}.
     * @return this buffer.
     * @exception IndexOutOfBoundsException
     *                if either {@code dstOffset} or {@code doubleCount} is invalid.
     * @exception BufferUnderflowException
     *                if {@code doubleCount} is greater than {@code remaining()}.
     */
    virtual CARAPI Get(
        /* [out] */ ArrayOf<Double>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 doubleCount);

    /**
     * Indicates whether this buffer is direct. A direct buffer will try its
     * best to take advantage of native memory APIs and it may not stay in the
     * Java heap, so it is not affected by garbage collection.
     * <p>
     * A double buffer is direct if it is based on a byte buffer and the byte
     * buffer is direct.
     *
     * @return {@code true} if this buffer is direct, {@code false} otherwise.
     */
    virtual CARAPI IsDirect(
        /* [out] */ Boolean* rst) = 0;

    /**
     * Returns the byte order used by this buffer when converting doubles
     * from/to bytes.
     * <p>
     * If this buffer is not based on a byte buffer, then this always returns
     * the platform's native byte order.
     *
     * @return the byte order used by this buffer when converting doubles
     *         from/to bytes.
     */
    virtual CARAPI GetOrder(
        /* [out] */ ByteOrder* byteOrder) = 0;

    /**
     * Child class implements this method to realize {@code array()}.
     *
     * @see #array()
     */
    virtual CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Double>** array) = 0;

    /**
     * Child class implements this method to realize {@code arrayOffset()}.
     *
     * @see #arrayOffset()
     */
    virtual CARAPI ProtectedArrayOffset(
        /* [out] */ Int32* offset) = 0;

    /**
     * Child class implements this method to realize {@code hasArray()}.
     *
     * @see #hasArray()
     */
    virtual CARAPI ProtectedHasArray(
        /* [out] */ Boolean* hasArray) = 0;

    /**
     * Writes the given double to the current position and increases the
     * position by 1.
     *
     * @param d
     *            the double to write.
     * @return this buffer.
     * @exception BufferOverflowException
     *                if position is equal or greater than limit.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ Double d) = 0;

    /**
     * Write a double to the specified index of this buffer and the position is
     * not changed.
     *
     * @param index
     *            the index, must not be negative and less than the limit.
     * @param d
     *            the double to write.
     * @return this buffer.
     * @exception IndexOutOfBoundsException
     *                if index is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Double d) = 0;

    /**
     * Writes doubles from the given double array to the current position and
     * increases the position by the number of doubles written.
     * <p>
     * Calling this method has the same effect as
     * {@code put(src, 0, src.length)}.
     *
     * @param src
     *            the source double array.
     * @return this buffer.
     * @exception BufferOverflowException
     *                if {@code remaining()} is less than {@code src.length}.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    CARAPI Put(
        /* [in] */ const ArrayOf<Double>& src);

    /**
     * Writes doubles from the given double array, starting from the specified
     * offset, to the current position and increases the position by the number
     * of doubles written.
     *
     * @param src
     *            the source double array.
     * @param srcOffset
     *            the offset of double array, must not be negative and not
     *            greater than {@code src.length}.
     * @param doubleCount
     *            the number of doubles to write, must be no less than zero and
     *            not greater than {@code src.length - srcOffset}.
     * @return this buffer.
     * @exception BufferOverflowException
     *                if {@code remaining()} is less than {@code doubleCount}.
     * @exception IndexOutOfBoundsException
     *                if either {@code srcOffset} or {@code doubleCount} is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ const ArrayOf<Double>& src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 doubleCount);

    /**
     * Writes all the remaining doubles of the {@code src} double buffer to this
     * buffer's current position, and increases both buffers' position by the
     * number of doubles copied.
     *
     * @param src
     *            the source double buffer.
     * @return this buffer.
     * @exception BufferOverflowException
     *                if {@code src.remaining()} is greater than this buffer's
     *                {@code remaining()}.
     * @exception IllegalArgumentException
     *                if {@code src} is this buffer.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ IDoubleBuffer* src);

    /**
     * Returns a sliced buffer that shares its content with this buffer.
     * <p>
     * The sliced buffer's capacity will be this buffer's {@code remaining()},
     * and its zero position will correspond to this buffer's current position.
     * The new buffer's position will be 0, limit will be its capacity, and its
     * mark is cleared. The new buffer's read-only property and byte order are
     * the same as this buffer's.
     * <p>
     * The new buffer shares its content with this buffer, which means either
     * buffer's change of content will be visible to the other. The two buffer's
     * position, limit and mark are independent.
     *
     * @return a sliced buffer that shares its content with this buffer.
     */
    virtual CARAPI Slice(
        /* [out] */ IDoubleBuffer** buffer) = 0;

    CARAPI HasArray(
        /* [out] */ Boolean* hasArray);

protected:
    DoubleBuffer();

    /**
     * Constructs a {@code DoubleBuffer} with given capacity.
     *
     * @param capacity
     *            the capacity of the buffer.
     */
    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ Int64 effectiveDirectAddress);
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_DOUBLEBUFFER_H__
