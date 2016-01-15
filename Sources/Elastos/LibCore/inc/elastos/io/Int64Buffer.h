#ifndef __ELASTOS_IO_INT64BUFFER_H__
#define __ELASTOS_IO_INT64BUFFER_H__

#include "Buffer.h"

namespace Elastos {
namespace IO {

/**
 * A buffer of longs.
 * <p>
 * A long buffer can be created in either of the following ways:
 * <ul>
 * <li>{@link #allocate(int) Allocate} a new long array and create a buffer
 * based on it;</li>
 * <li>{@link #wrap(long[]) Wrap} an existing long array to create a new
 * buffer;</li>
 * <li>Use {@link java.nio.ByteBuffer#asLongBuffer() ByteBuffer.asLongBuffer}
 * to create a long buffer based on a byte buffer.</li>
 * </ul>
 */
class Int64Buffer
    : public Buffer
    , public IInt64Buffer
{
public:
    Int64Buffer();

    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ Int64 effectiveDirectAddress);

    CAR_INTERFACE_DECL()

    /**
     * Creates a long buffer based on a newly allocated long array.
     *
     * @param capacity
     *            the capacity of the new buffer.
     * @return the created long buffer.
     * @throws IllegalArgumentException
     *             if {@code capacity} is less than zero.
     */
    static CARAPI Allocate(
        /* [in] */ Int32 capacity,
        /* [out] */ IInt64Buffer** buf);

    /**
     * Creates a new long buffer by wrapping the given long array.
     * <p>
     * Calling this method has the same effect as
     * {@code wrap(array, 0, array.length)}.
     *
     * @param array
     *            the long array which the new buffer will be based on.
     * @return the created long buffer.
     */
    static CARAPI Wrap(
        /* [in] */ ArrayOf<Int64>* array,
        /* [out] */ IInt64Buffer** buf);

    /**
     * Creates a new long buffer by wrapping the given long array.
     * <p>
     * The new buffer's position will be {@code start}, limit will be
     * {@code start + longCount}, capacity will be the length of the array.
     *
     * @param array
     *            the long array which the new buffer will be based on.
     * @param start
     *            the start index, must not be negative and not greater than
     *            {@code array.length}.
     * @param longCount
     *            the length, must not be negative and not greater than
     *            {@code array.length - start}.
     * @return the created long buffer.
     * @exception IndexOutOfBoundsException
     *                if either {@code start} or {@code longCount} is invalid.
     */
    static CARAPI Wrap(
        /* [in] */ ArrayOf<Int64>* array,
        /* [in] */ Int32 start,
        /* [in] */ Int32 int64Count,
        /* [out] */ IInt64Buffer** buf);

    CARAPI GetArray(
        /* [out] */ IArrayOf** array);

    CARAPI GetArray(
        /* [out, callee] */ ArrayOf<Int64>** array);

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
        /* [out] */ IInt64Buffer** buffer) = 0;

    /**
     * Compacts this long buffer.
     * <p>
     * The remaining longs will be moved to the head of the buffer, staring from
     * position zero. Then the position is set to {@code remaining()}; the
     * limit is set to capacity; the mark is cleared.
     *
     * @return this buffer.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Compact() = 0;

    /**
     * Compare the remaining longs of this buffer to another long buffer's
     * remaining longs.
     *
     * @param otherBuffer
     *            another long buffer.
     * @return a negative value if this is less than {@code otherBuffer}; 0 if
     *         this equals to {@code otherBuffer}; a positive value if this is
     *         greater than {@code otherBuffer}
     * @exception ClassCastException
     *                if {@code otherBuffer} is not a long buffer.
     */
    virtual CARAPI CompareTo(
        /* [in] */ IInt64Buffer* otherBuffer,
        /* [out] */ Int32* result);

    /**
     * Returns a duplicated buffer that shares its content with this buffer.
     * <p>
     * The duplicated buffer's position, limit, capacity and mark are the same
     * as this buffer. The duplicated buffer's read-only property and byte order
     * are same as this buffer's, too.
     * <p>
     * The new buffer shares its content with this buffer, which means either
     * buffer's change of content will be visible to the other. The two buffer's
     * position, limit and mark are independent.
     *
     * @return a duplicated buffer that shares its content with this buffer.
     */
    virtual CARAPI Duplicate(
        /* [out] */ IInt64Buffer** buffer) = 0;

    /**
     * Checks whether this long buffer is equal to another object.
     * <p>
     * If {@code other} is not a long buffer then {@code false} is returned. Two
     * long buffers are equal if and only if their remaining longs are exactly
     * the same. Position, limit, capacity and mark are not considered.
     *
     * @param other
     *            the object to compare with this long buffer.
     * @return {@code true} if this long buffer is equal to {@code other},
     *         {@code false} otherwise.
     */
    virtual CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* rst);

    /**
     * Returns the long at the current position and increase the position by 1.
     *
     * @return the long at the current position.
     * @exception BufferUnderflowException
     *                if the position is equal or greater than limit.
     */
    virtual CARAPI Get(
        /* [out] */ Int64* value) = 0;

    /**
     * Returns the long at the specified index; the position is not changed.
     *
     * @param index
     *            the index, must not be negative and less than limit.
     * @return the long at the specified index.
     * @exception IndexOutOfBoundsException
     *                if index is invalid.
     */
    virtual CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Int64* value) = 0;

    /**
     * Reads longs from the current position into the specified long array and
     * increases the position by the number of longs read.
     * <p>
     * Calling this method has the same effect as
     * {@code get(dst, 0, dst.length)}.
     *
     * @param dst
     *            the destination long array.
     * @return this buffer.
     * @exception BufferUnderflowException
     *                if {@code dst.length} is greater than {@code remaining()}.
     */
    virtual CARAPI Get(
        /* [out] */ ArrayOf<Int64>* dst);

    /**
     * Reads longs from the current position into the specified long array,
     * starting from the specified offset, and increase the position by the
     * number of longs read.
     *
     * @param dst
     *            the target long array.
     * @param dstOffset
     *            the offset of the long array, must not be negative and not
     *            greater than {@code dst.length}.
     * @param longCount
     *            the number of longs to read, must be no less than zero and not
     *            greater than {@code dst.length - dstOffset}.
     * @return this buffer.
     * @exception IndexOutOfBoundsException
     *                if either {@code dstOffset} or {@code longCount} is invalid.
     * @exception BufferUnderflowException
     *                if {@code longCount} is greater than {@code remaining()}.
     */
    virtual CARAPI Get(
        /* [out] */ ArrayOf<Int64>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 int64Count);

    CARAPI HasArray(
        /* [out] */ Boolean* hasArray);

    /**
     * Returns the byte order used by this buffer when converting longs from/to
     * bytes.
     * <p>
     * If this buffer is not based on a byte buffer, then always return the
     * platform's native byte order.
     *
     * @return the byte order used by this buffer when converting longs from/to
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
        /* [out, callee] */ ArrayOf<Int64>** array) = 0;

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
     * Writes the given long to the current position and increases the position
     * by 1.
     *
     * @param l
     *            the long to write.
     * @return this buffer.
     * @exception BufferOverflowException
     *                if position is equal or greater than limit.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ Int64 d) = 0;

    /**
     * Writes a long to the specified index of this buffer; the position is not
     * changed.
     *
     * @param index
     *            the index, must not be negative and less than the limit.
     * @param l
     *            the long to write.
     * @return this buffer.
     * @exception IndexOutOfBoundsException
     *                if index is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Int64 d) = 0;

    /**
     * Writes longs from the given long array to the current position and
     * increases the position by the number of longs written.
     * <p>
     * Calling this method has the same effect as
     * {@code put(src, 0, src.length)}.
     *
     * @param src
     *            the source long array.
     * @return this buffer.
     * @exception BufferOverflowException
     *                if {@code remaining()} is less than {@code src.length}.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    CARAPI Put(
        /* [in] */ const ArrayOf<Int64>& src);

    /**
     * Writes longs from the given long array, starting from the specified
     * offset, to the current position and increases the position by the number
     * of longs written.
     *
     * @param src
     *            the source long array.
     * @param srcOffset
     *            the offset of long array, must not be negative and not greater
     *            than {@code src.length}.
     * @param longCount
     *            the number of longs to write, must be no less than zero and
     *            not greater than {@code src.length - srcOffset}.
     * @return this buffer.
     * @exception BufferOverflowException
     *                if {@code remaining()} is less than {@code longCount}.
     * @exception IndexOutOfBoundsException
     *                if either {@code srcOffset} or {@code longCount} is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ const ArrayOf<Int64>& src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 int64Count);

    /**
     * Writes all the remaining longs of the {@code src} long buffer to this
     * buffer's current position, and increases both buffers' position by the
     * number of longs copied.
     *
     * @param src
     *            the source long buffer.
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
        /* [in] */ IInt64Buffer* src);

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
        /* [out] */ IInt64Buffer** buffer) = 0;

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_INT64BUFFER_H__
