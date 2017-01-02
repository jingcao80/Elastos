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

#ifndef __ELASTOS_IO_INT32BUFFER_H__
#define __ELASTOS_IO_INT32BUFFER_H__

#include "Buffer.h"

namespace Elastos {
namespace IO {

/**
 * A buffer of ints.
 * <p>
 * A int buffer can be created in either of the following ways:
 * <ul>
 * <li>{@link #allocate(int) Allocate} a new int array and create a buffer
 * based on it;</li>
 * <li>{@link #wrap(int[]) Wrap} an existing int array to create a new buffer;</li>
 * <li>Use {@link java.nio.ByteBuffer#asIntBuffer() ByteBuffer.asIntBuffer} to
 * create a int buffer based on a byte buffer.</li>
 * </ul>
 */
class Int32Buffer
    : public Buffer
    , public IInt32Buffer
{
public:
    Int32Buffer();

    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ Int64 effectiveDirectAddress);

    CAR_INTERFACE_DECL()

    /**
     * Creates an int buffer based on a newly allocated int array.
     *
     * @param capacity
     *            the capacity of the new buffer.
     * @return the created int buffer.
     * @throws IllegalArgumentException
     *             if {@code capacity} is less than zero.
     */
    static CARAPI Allocate(
        /* [in] */ Int32 capacity,
        /* [out] */ IInt32Buffer** buf);

    /**
     * Creates a new int buffer by wrapping the given int array.
     * <p>
     * Calling this method has the same effect as
     * {@code wrap(array, 0, array.length)}.
     *
     * @param array
     *            the int array which the new buffer will be based on.
     * @return the created int buffer.
     */
    static CARAPI Wrap(
        /* [in] */ ArrayOf<Int32>* array,
        /* [out] */ IInt32Buffer** buf);

    /**
     * Creates a new int buffer by wrapping the given int array.
     * <p>
     * The new buffer's position will be {@code start}, limit will be
     * {@code start + intCount}, capacity will be the length of the array.
     *
     * @param array
     *            the int array which the new buffer will be based on.
     * @param start
     *            the start index, must not be negative and not greater than
     *            {@code array.length}
     * @param intCount
     *            the length, must not be negative and not greater than
     *            {@code array.length - start}.
     * @return the created int buffer.
     * @exception IndexOutOfBoundsException
     *                if either {@code start} or {@code intCount} is invalid.
     */
    static CARAPI Wrap(
        /* [in] */ ArrayOf<Int32>* array,
        /* [in] */ Int32 start,
        /* [in] */ Int32 int32Count,
        /* [out] */ IInt32Buffer** buf);

    CARAPI GetArray(
        /* [out] */ IArrayOf** array);

    CARAPI GetArray(
        /* [out, callee] */ ArrayOf<Int32>** array);

    CARAPI GetArrayOffset(
        /* [out] */ Int32* offset);

    /**
     * Returns a read-only buffer that shares its content with this buffer.
     * <p>
     * The returned buffer is guaranteed to be a new instance, even this buffer
     * is read-only itself. The new buffer's position, limit, capacity and mark
     * are the same as this buffer's.
     * <p>
     * The new buffer shares its content with this buffer, which means this
     * buffer's change of content will be visible to the new buffer. The two
     * buffer's position, limit and mark are independent.
     *
     * @return a read-only version of this buffer.
     */
    virtual CARAPI AsReadOnlyBuffer(
        /* [out] */ IInt32Buffer** buffer) = 0;

    /**
     * Compacts this int buffer.
     * <p>
     * The remaining ints will be moved to the head of the buffer, starting from
     * position zero. Then the position is set to {@code remaining()}; the
     * limit is set to capacity; the mark is cleared.
     *
     * @return this buffer.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Compact() = 0;

    /**
     * Compares the remaining ints of this buffer to another int buffer's
     * remaining ints.
     *
     * @param otherBuffer
     *            another int buffer.
     * @return a negative value if this is less than {@code other}; 0 if this
     *         equals to {@code other}; a positive value if this is greater
     *         than {@code other}.
     * @exception ClassCastException
     *                if {@code other} is not an int buffer.
     */
    virtual CARAPI CompareTo(
        /* [in] */ IInt32Buffer* otherBuffer,
        /* [out] */ Int32* result);

    /**
     * Returns a duplicated buffer that shares its content with this buffer.
     * <p>
     * The duplicated buffer's position, limit, capacity and mark are the same
     * as this buffer. The duplicated buffer's read-only property and byte order
     * are the same as this buffer's.
     * <p>
     * The new buffer shares its content with this buffer, which means either
     * buffer's change of content will be visible to the other. The two buffer's
     * position, limit and mark are independent.
     *
     * @return a duplicated buffer that shares its content with this buffer.
     */
    virtual CARAPI Duplicate(
        /* [out] */ IInt32Buffer** buffer) = 0;

    /**
     * Checks whether this int buffer is equal to another object.
     * <p>
     * If {@code other} is not a int buffer then {@code false} is returned. Two
     * int buffers are equal if and only if their remaining ints are exactly the
     * same. Position, limit, capacity and mark are not considered.
     *
     * @param other
     *            the object to compare with this int buffer.
     * @return {@code true} if this int buffer is equal to {@code other},
     *         {@code false} otherwise.
     */
    virtual CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* rst);

    /**
     * Returns the int at the current position and increases the position by 1.
     *
     * @return the int at the current position.
     * @exception BufferUnderflowException
     *                if the position is equal or greater than limit.
     */
    virtual CARAPI Get(
        /* [out] */ Int32* value) = 0;

    /**
     * Returns an int at the specified index; the position is not changed.
     *
     * @param index
     *            the index, must not be negative and less than limit.
     * @return an int at the specified index.
     * @exception IndexOutOfBoundsException
     *                if index is invalid.
     */
    virtual CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value) = 0;

    /**
     * Reads ints from the current position into the specified int array and
     * increases the position by the number of ints read.
     * <p>
     * Calling this method has the same effect as
     * {@code get(dst, 0, dst.length)}.
     *
     * @param dst
     *            the destination int array.
     * @return this buffer.
     * @exception BufferUnderflowException
     *                if {@code dst.length} is greater than {@code remaining()}.
     */
    virtual CARAPI Get(
        /* [out] */ ArrayOf<Int32>* dst);

    /**
     * Reads ints from the current position into the specified int array,
     * starting from the specified offset, and increases the position by the
     * number of ints read.
     *
     * @param dst
     *            the target int array.
     * @param dstOffset
     *            the offset of the int array, must not be negative and not
     *            greater than {@code dst.length}.
     * @param intCount
     *            the number of ints to read, must be no less than zero and not
     *            greater than {@code dst.length - dstOffset}.
     * @return this buffer.
     * @exception IndexOutOfBoundsException
     *                if either {@code dstOffset} or {@code intCount} is invalid.
     * @exception BufferUnderflowException
     *                if {@code intCount} is greater than {@code remaining()}.
     */
    virtual CARAPI Get(
        /* [out] */ ArrayOf<Int32>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 int32Count);

    CARAPI HasArray(
        /* [out] */ Boolean* hasArray);

    /**
     * Returns the byte order used by this buffer when converting ints from/to
     * bytes.
     * <p>
     * If this buffer is not based on a byte buffer, then always return the
     * platform's native byte order.
     *
     * @return the byte order used by this buffer when converting ints from/to
     *         bytes.
     */
    virtual CARAPI GetOrder(
        /* [out] */ ByteOrder* byteOrder) = 0;

    /**
     * Child class implements this method to realize {@code array()}.
     *
     * @return see {@code array()}
     */
    virtual CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Int32>** array) = 0;

    /**
     * Child class implements this method to realize {@code arrayOffset()}.
     *
     * @return see {@code arrayOffset()}
     */
    virtual CARAPI ProtectedArrayOffset(
        /* [out] */ Int32* offset) = 0;

    /**
     * Child class implements this method to realize {@code hasArray()}.
     *
     * @return see {@code hasArray()}
     */
    virtual CARAPI ProtectedHasArray(
        /* [out] */ Boolean* hasArray) = 0;

    /**
     * Writes the given int to the current position and increases the position
     * by 1.
     *
     * @param i
     *            the int to write.
     * @return this buffer.
     * @exception BufferOverflowException
     *                if position is equal or greater than limit.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ Int32 d) = 0;

    /**
     * Write a int to the specified index of this buffer; the position is not
     * changed.
     *
     * @param index
     *            the index, must not be negative and less than the limit.
     * @param i
     *            the int to write.
     * @return this buffer.
     * @exception IndexOutOfBoundsException
     *                if index is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Int32 d) = 0;

    /**
     * Writes ints from the given int array to the current position and
     * increases the position by the number of ints written.
     * <p>
     * Calling this method has the same effect as
     * {@code put(src, 0, src.length)}.
     *
     * @param src
     *            the source int array.
     * @return this buffer.
     * @exception BufferOverflowException
     *                if {@code remaining()} is less than {@code src.length}.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    CARAPI Put(
        /* [in] */ ArrayOf<Int32>* src);

    /**
     * Writes ints from the given int array, starting from the specified offset,
     * to the current position and increases the position by the number of ints
     * written.
     *
     * @param src
     *            the source int array.
     * @param srcOffset
     *            the offset of int array, must not be negative and not greater
     *            than {@code src.length}.
     * @param intCount
     *            the number of ints to write, must be no less than zero and not
     *            greater than {@code src.length - srcOffset}.
     * @return this buffer.
     * @exception BufferOverflowException
     *                if {@code remaining()} is less than {@code intCount}.
     * @exception IndexOutOfBoundsException
     *                if either {@code srcOffset} or {@code intCount} is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ ArrayOf<Int32>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 int32Count);

    /**
     * Writes all the remaining ints of the {@code src} int buffer to this
     * buffer's current position, and increases both buffers' position by the
     * number of ints copied.
     *
     * @param src
     *            the source int buffer.
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
        /* [in] */ IInt32Buffer* src);

    /**
     * Returns a sliced buffer that shares its content with this buffer.
     * <p>
     * The sliced buffer's capacity will be this buffer's {@code remaining()},
     * and its zero position will correspond to this buffer's current position.
     * The new buffer's position will be 0, limit will be its capacity, and its
     * mark is cleared. The new buffer's read-only property and byte order are
     * same as this buffer's.
     * <p>
     * The new buffer shares its content with this buffer, which means either
     * buffer's change of content will be visible to the other. The two buffer's
     * position, limit and mark are independent.
     *
     * @return a sliced buffer that shares its content with this buffer.
     */
    virtual CARAPI Slice(
        /* [out] */ IInt32Buffer** buffer) = 0;

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_INT32BUFFER_H__
