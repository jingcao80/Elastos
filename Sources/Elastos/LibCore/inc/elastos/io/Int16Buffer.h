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

#ifndef __ELASTOS_IO_INT16BUFFER_H__
#define __ELASTOS_IO_INT16BUFFER_H__

#include "Buffer.h"

namespace Elastos {
namespace IO {

/**
 * A buffer of shorts.
 * <p>
 * A short buffer can be created in either of the following ways:
 * <ul>
 * <li>{@link #allocate(int) Allocate} a new short array and create a buffer
 * based on it;</li>
 * <li>{@link #wrap(short[]) Wrap} an existing short array to create a new
 * buffer;</li>
 * <li>Use {@link java.nio.ByteBuffer#asShortBuffer() ByteBuffer.asShortBuffer}
 * to create a short buffer based on a byte buffer.</li>
 * </ul>
 */
class Int16Buffer
    : public Buffer
    , public IInt16Buffer
{
public:
    Int16Buffer();

    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ Int64 effectiveDirectAddress);

    CAR_INTERFACE_DECL()

    /**
     * Creates a short buffer based on a newly allocated short array.
     *
     * @param capacity
     *            the capacity of the new buffer.
     * @return the created short buffer.
     * @throws IllegalArgumentException
     *             if {@code capacity} is less than zero.
     */
    static CARAPI Allocate(
        /* [in] */ Int32 capacity,
        /* [out] */ IInt16Buffer** buf);

    /**
     * Creates a new short buffer by wrapping the given short array.
     * <p>
     * Calling this method has the same effect as
     * {@code wrap(array, 0, array.length)}.
     *
     * @param array
     *            the short array which the new buffer will be based on.
     * @return the created short buffer.
     */
    static CARAPI Wrap(
        /* [in] */ ArrayOf<Int16>* array,
        /* [out] */ IInt16Buffer** buf);

    /**
     * Creates a new short buffer by wrapping the given short array.
     * <p>
     * The new buffer's position will be {@code start}, limit will be
     * {@code start + shortCount}, capacity will be the length of the array.
     *
     * @param array
     *            the short array which the new buffer will be based on.
     * @param start
     *            the start index, must not be negative and not greater than
     *            {@code array.length}.
     * @param shortCount
     *            the length, must not be negative and not greater than
     *            {@code array.length - start}.
     * @return the created short buffer.
     * @exception IndexOutOfBoundsException
     *                if either {@code start} or {@code shortCount} is invalid.
     */
    static CARAPI Wrap(
        /* [in] */ ArrayOf<Int16>* array,
        /* [in] */ Int32 start,
        /* [in] */ Int32 int16Count,
        /* [out] */ IInt16Buffer** buf);

    CARAPI GetArray(
        /* [out] */ IArrayOf** array);

    CARAPI GetArray(
        /* [out, callee] */ ArrayOf<Int16>** array);

    CARAPI GetArrayOffset(
        /* [out] */ Int32* offset);

    /**
     * Returns a read-only buffer that shares its content with this buffer.
     * <p>
     * The returned buffer is guaranteed to be a new instance, even if this
     * buffer is read-only itself. The new buffer's position, limit, capacity
     * and mark are the same as this buffer's.
     * <p>
     * The new buffer shares its content with this buffer, which means this
     * buffer's change of content will be visible to the new buffer. The two
     * buffer's position, limit and mark are independent.
     *
     * @return a read-only version of this buffer.
     */
    virtual CARAPI AsReadOnlyBuffer(
        /* [out] */ IInt16Buffer** buffer) = 0;

    /**
     * Compacts this short buffer.
     * <p>
     * The remaining shorts will be moved to the head of the buffer, starting
     * from position zero. Then the position is set to {@code remaining()}; the
     * limit is set to capacity; the mark is cleared.
     *
     * @return this buffer.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Compact() = 0;

    /**
     * Compare the remaining shorts of this buffer to another short buffer's
     * remaining shorts.
     *
     * @param otherBuffer
     *            another short buffer.
     * @return a negative value if this is less than {@code otherBuffer}; 0 if
     *         this equals to {@code otherBuffer}; a positive value if this is
     *         greater than {@code otherBuffer}.
     * @exception ClassCastException
     *                if {@code otherBuffer} is not a short buffer.
     */
    virtual CARAPI CompareTo(
        /* [in] */ IInt16Buffer* otherBuffer,
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
        /* [out] */ IInt16Buffer** buffer) = 0;

    /**
     * Checks whether this short buffer is equal to another object.
     * <p>
     * If {@code other} is not a short buffer then {@code false} is returned.
     * Two short buffers are equal if and only if their remaining shorts are
     * exactly the same. Position, limit, capacity and mark are not considered.
     *
     * @param other
     *            the object to compare with this short buffer.
     * @return {@code true} if this short buffer is equal to {@code other},
     *         {@code false} otherwise.
     */
    virtual CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* rst);

    /**
     * Returns the short at the current position and increases the position by
     * 1.
     *
     * @return the short at the current position.
     * @exception BufferUnderflowException
     *                if the position is equal or greater than limit.
     */
    virtual CARAPI Get(
        /* [out] */ Int16* value) = 0;

    /**
     * Returns the short at the specified index; the position is not changed.
     *
     * @param index
     *            the index, must not be negative and less than limit.
     * @return a short at the specified index.
     * @exception IndexOutOfBoundsException
     *                if index is invalid.
     */
    virtual CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Int16* value) = 0;

    /**
     * Reads shorts from the current position into the specified short array and
     * increases the position by the number of shorts read.
     * <p>
     * Calling this method has the same effect as
     * {@code get(dst, 0, dst.length)}.
     *
     * @param dst
     *            the destination short array.
     * @return this buffer.
     * @exception BufferUnderflowException
     *                if {@code dst.length} is greater than {@code remaining()}.
     */
    virtual CARAPI Get(
        /* [out] */ ArrayOf<Int16>* dst);

    /**
     * Reads shorts from the current position into the specified short array,
     * starting from the specified offset, and increases the position by the
     * number of shorts read.
     *
     * @param dst
     *            the target short array.
     * @param dstOffset
     *            the offset of the short array, must not be negative and not
     *            greater than {@code dst.length}.
     * @param shortCount
     *            the number of shorts to read, must be no less than zero and
     *            not greater than {@code dst.length - dstOffset}.
     * @return this buffer.
     * @exception IndexOutOfBoundsException
     *                if either {@code dstOffset} or {@code shortCount} is invalid.
     * @exception BufferUnderflowException
     *                if {@code shortCount} is greater than {@code remaining()}.
     */
    virtual CARAPI Get(
        /* [out] */ ArrayOf<Int16>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 int16Count);

    CARAPI HasArray(
        /* [out] */ Boolean* hasArray);

    /**
     * Returns the byte order used by this buffer when converting shorts from/to
     * bytes.
     * <p>
     * If this buffer is not based on a byte buffer, then always return the
     * platform's native byte order.
     *
     * @return the byte order used by this buffer when converting shorts from/to
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
        /* [out, callee] */ ArrayOf<Int16>** array) = 0;

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
     * Writes the given short to the current position and increases the position
     * by 1.
     *
     * @param s
     *            the short to write.
     * @return this buffer.
     * @exception BufferOverflowException
     *                if position is equal or greater than limit.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ Int16 d) = 0;

    /**
     * Writes a short to the specified index of this buffer; the position is not
     * changed.
     *
     * @param index
     *            the index, must not be negative and less than the limit.
     * @param s
     *            the short to write.
     * @return this buffer.
     * @exception IndexOutOfBoundsException
     *                if index is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Int16 d) = 0;

    /**
     * Writes shorts from the given short array to the current position and
     * increases the position by the number of shorts written.
     * <p>
     * Calling this method has the same effect as
     * {@code put(src, 0, src.length)}.
     *
     * @param src
     *            the source short array.
     * @return this buffer.
     * @exception BufferOverflowException
     *                if {@code remaining()} is less than {@code src.length}.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    CARAPI Put(
        /* [in] */ ArrayOf<Int16>* src);

    /**
     * Writes shorts from the given short array, starting from the specified
     * offset, to the current position and increases the position by the number
     * of shorts written.
     *
     * @param src
     *            the source short array.
     * @param srcOffset
     *            the offset of short array, must not be negative and not
     *            greater than {@code src.length}.
     * @param shortCount
     *            the number of shorts to write, must be no less than zero and
     *            not greater than {@code src.length - srcOffset}.
     * @return this buffer.
     * @exception BufferOverflowException
     *                if {@code remaining()} is less than {@code shortCount}.
     * @exception IndexOutOfBoundsException
     *                if either {@code srcOffset} or {@code shortCount} is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ ArrayOf<Int16>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 int16Count);

    /**
     * Writes all the remaining shorts of the {@code src} short buffer to this
     * buffer's current position, and increases both buffers' position by the
     * number of shorts copied.
     *
     * @param src
     *            the source short buffer.
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
        /* [in] */ IInt16Buffer* src);

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
        /* [out] */ IInt16Buffer** buffer) = 0;

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_INT16BUFFER_H__
