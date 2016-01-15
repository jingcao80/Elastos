
#ifndef __ELASTOS_IO_BYTEBUFFER_H__
#define __ELASTOS_IO_BYTEBUFFER_H__

#include "Buffer.h"

using Elastos::Core::IComparable;

namespace Elastos {
namespace IO {

/**
 * A buffer for bytes.
 * <p>
 * A byte buffer can be created in either one of the following ways:
 * <ul>
 * <li>{@link #allocate(int) Allocate} a new byte array and create a buffer
 * based on it;</li>
 * <li>{@link #allocateDirect(int) Allocate} a memory block and create a direct
 * buffer based on it;</li>
 * <li>{@link #wrap(byte[]) Wrap} an existing byte array to create a new
 * buffer.</li>
 * </ul>
 *
 */
class ByteBuffer
    : public Buffer
    , public IByteBuffer
    , public IComparable
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a byte buffer based on a newly allocated byte array.
     *
     * @param capacity
     *            the capacity of the new buffer
     * @return the created byte buffer.
     * @throws IllegalArgumentException
     *             if {@code capacity < 0}.
     */
    static CARAPI Allocate(
        /* [in] */ Int32 capacity,
        /* [out] */ IByteBuffer** buf);

    /**
     * Creates a direct byte buffer based on a newly allocated memory block.
     *
     * @param capacity
     *            the capacity of the new buffer
     * @return the created byte buffer.
     * @throws IllegalArgumentException
     *             if {@code capacity < 0}.
     */
    static CARAPI AllocateDirect(
        /* [in] */ Int32 capacity,
        /* [out] */ IByteBuffer** buf);

    /**
     * Creates a new byte buffer by wrapping the given byte array.
     * <p>
     * Calling this method has the same effect as
     * {@code wrap(array, 0, array.length)}.
     *
     * @param array
     *            the byte array which the new buffer will be based on
     * @return the created byte buffer.
     */
    static CARAPI Wrap(
        /* [in] */ ArrayOf<Byte>* array,
        /* [out] */ IByteBuffer** buf);

    /**
     * Creates a new byte buffer by wrapping the given byte array.
     * <p>
     * The new buffer's position will be {@code start}, limit will be
     * {@code start + byteCount}, capacity will be the length of the array.
     *
     * @param array
     *            the byte array which the new buffer will be based on.
     * @param start
     *            the start index, must not be negative and not greater than
     *            {@code array.length}.
     * @param byteCount
     *            the length, must not be negative and not greater than
     *            {@code array.length - start}.
     * @return the created byte buffer.
     * @exception IndexOutOfBoundsException
     *                if either {@code start} or {@code byteCount} is invalid.
     */
    static CARAPI Wrap(
        /* [in] */ ArrayOf<Byte>* array,
        /* [in] */ Int32 start,
        /* [in] */ Int32 byteCount,
        /* [out] */ IByteBuffer** buf);

    CARAPI GetArray(
        /* [out] */ IArrayOf** array);

    /**
     * Returns the byte array which this buffer is based on, if there is one.
     *
     * @return the byte array which this buffer is based on.
     * @exception ReadOnlyBufferException
     *                if this buffer is based on a read-only array.
     * @exception UnsupportedOperationException
     *                if this buffer is not based on an array.
     */
    CARAPI GetArray(
        /* [out, callee] */ ArrayOf<Byte>** array);

    /**
     * Returns the offset of the byte array which this buffer is based on, if
     * there is one.
     * <p>
     * The offset is the index of the array which corresponds to the zero
     * position of the buffer.
     *
     * @return the offset of the byte array which this buffer is based on.
     * @exception ReadOnlyBufferException
     *                if this buffer is based on a read-only array.
     * @exception UnsupportedOperationException
     *                if this buffer is not based on an array.
     */
    CARAPI GetArrayOffset(
        /* [out] */ Int32* offset);

    /**
     * Returns a char buffer which is based on the remaining content of this
     * byte buffer.
     * <p>
     * The new buffer's position is zero, its limit and capacity is the number
     * of remaining bytes divided by two, and its mark is not set. The new
     * buffer's read-only property and byte order are the same as this buffer's.
     * The new buffer is direct if this byte buffer is direct.
     * <p>
     * The new buffer shares its content with this buffer, which means either
     * buffer's change of content will be visible to the other. The two buffer's
     * position, limit and mark are independent.
     *
     * @return a char buffer which is based on the content of this byte buffer.
     */
    virtual CARAPI AsCharBuffer(
        /* [out] */ ICharBuffer** buffer) = 0;

    /**
     * Returns a double buffer which is based on the remaining content of this
     * byte buffer.
     * <p>
     * The new buffer's position is zero, its limit and capacity is the number
     * of remaining bytes divided by eight, and its mark is not set. The new
     * buffer's read-only property and byte order are the same as this buffer's.
     * The new buffer is direct if this byte buffer is direct.
     * <p>
     * The new buffer shares its content with this buffer, which means either
     * buffer's change of content will be visible to the other. The two buffer's
     * position, limit and mark are independent.
     *
     * @return a double buffer which is based on the content of this byte
     *         buffer.
     */
    virtual CARAPI AsDoubleBuffer(
        /* [out] */ IDoubleBuffer** buffer) = 0;

    /**
     * Returns a float buffer which is based on the remaining content of this
     * byte buffer.
     * <p>
     * The new buffer's position is zero, its limit and capacity is the number
     * of remaining bytes divided by four, and its mark is not set. The new
     * buffer's read-only property and byte order are the same as this buffer's.
     * The new buffer is direct if this byte buffer is direct.
     * <p>
     * The new buffer shares its content with this buffer, which means either
     * buffer's change of content will be visible to the other. The two buffer's
     * position, limit and mark are independent.
     *
     * @return a float buffer which is based on the content of this byte buffer.
     */
    virtual CARAPI AsFloatBuffer(
        /* [out] */ IFloatBuffer** buffer) = 0;

    /**
     * Returns a short buffer which is based on the remaining content of this
     * byte buffer.
     * <p>
     * The new buffer's position is zero, its limit and capacity is the number
     * of remaining bytes divided by two, and its mark is not set. The new
     * buffer's read-only property and byte order are the same as this buffer's.
     * The new buffer is direct if this byte buffer is direct.
     * <p>
     * The new buffer shares its content with this buffer, which means either
     * buffer's change of content will be visible to the other. The two buffer's
     * position, limit and mark are independent.
     *
     * @return a short buffer which is based on the content of this byte buffer.
     */
    virtual CARAPI AsInt16Buffer(
        /* [out] */ IInt16Buffer** buffer) = 0;

    /**
     * Returns a int buffer which is based on the remaining content of this byte
     * buffer.
     * <p>
     * The new buffer's position is zero, its limit and capacity is the number
     * of remaining bytes divided by four, and its mark is not set. The new
     * buffer's read-only property and byte order are the same as this buffer's.
     * The new buffer is direct if this byte buffer is direct.
     * <p>
     * The new buffer shares its content with this buffer, which means either
     * buffer's change of content will be visible to the other. The two buffer's
     * position, limit and mark are independent.
     *
     * @return a int buffer which is based on the content of this byte buffer.
     */
    virtual CARAPI AsInt32Buffer(
        /* [out] */ IInt32Buffer** buffer) = 0;

    /**
     * Returns a long buffer which is based on the remaining content of this
     * byte buffer.
     * <p>
     * The new buffer's position is zero, its limit and capacity is the number
     * of remaining bytes divided by eight, and its mark is not set. The new
     * buffer's read-only property and byte order are the same as this buffer's.
     * The new buffer is direct if this byte buffer is direct.
     * <p>
     * The new buffer shares its content with this buffer, which means either
     * buffer's change of content will be visible to the other. The two buffer's
     * position, limit and mark are independent.
     *
     * @return a long buffer which is based on the content of this byte buffer.
     */
    virtual CARAPI AsInt64Buffer(
        /* [out] */ IInt64Buffer** buffer) = 0;

    /**
     * Returns a read-only buffer that shares its content with this buffer.
     * <p>
     * The returned buffer is guaranteed to be a new instance, even if this
     * buffer is read-only itself. The new buffer's position, limit, capacity
     * and mark are the same as this buffer.
     * <p>
     * The new buffer shares its content with this buffer, which means this
     * buffer's change of content will be visible to the new buffer. The two
     * buffer's position, limit and mark are independent.
     *
     * @return a read-only version of this buffer.
     */
    virtual CARAPI AsReadOnlyBuffer(
        /* [out] */ IByteBuffer** buffer) = 0;

    /**
     * Compacts this byte buffer.
     * <p>
     * The remaining bytes will be moved to the head of the
     * buffer, starting from position zero. Then the position is set to
     * {@code remaining()}; the limit is set to capacity; the mark is
     * cleared.
     *
     * @return {@code this}
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Compact() = 0;

    /**
     * Compares the remaining bytes of this buffer to another byte buffer's
     * remaining bytes.
     *
     * @param otherBuffer
     *            another byte buffer.
     * @return a negative value if this is less than {@code other}; 0 if this
     *         equals to {@code other}; a positive value if this is greater
     *         than {@code other}.
     * @exception ClassCastException
     *                if {@code other} is not a byte buffer.
     */
    virtual CARAPI CompareTo(
        /* [in] */ IByteBuffer* otherBuffer,
        /* [out] */ Int32* result);

    CARAPI CompareTo(
        /* [in] */ IInterface* otherBuffer,
        /* [out] */ Int32* result);

    /**
     * Returns a duplicated buffer that shares its content with this buffer.
     * <p>
     * The duplicated buffer's position, limit, capacity and mark are the same
     * as this buffer's. The duplicated buffer's read-only property and byte
     * order are the same as this buffer's too.
     * <p>
     * The new buffer shares its content with this buffer, which means either
     * buffer's change of content will be visible to the other. The two buffer's
     * position, limit and mark are independent.
     *
     * @return a duplicated buffer that shares its content with this buffer.
     */
    virtual CARAPI Duplicate(
        /* [out] */ IByteBuffer** buffer) = 0;

    /**
     * Returns the byte at the current position and increases the position by 1.
     *
     * @return the byte at the current position.
     * @exception BufferUnderflowException
     *                if the position is equal or greater than limit.
     */
    virtual CARAPI Get(
        /* [out] */ Byte* value) = 0;

    /**
     * Returns the byte at the specified index and does not change the position.
     *
     * @param index
     *            the index, must not be negative and less than limit.
     * @return the byte at the specified index.
     * @exception IndexOutOfBoundsException
     *                if index is invalid.
     */
    virtual CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Byte* value) = 0;

    /**
     * Reads bytes from the current position into the specified byte array and
     * increases the position by the number of bytes read.
     * <p>
     * Calling this method has the same effect as
     * {@code get(dst, 0, dst.length)}.
     *
     * @param dst
     *            the destination byte array.
     * @return {@code this}
     * @exception BufferUnderflowException
     *                if {@code dst.length} is greater than {@code remaining()}.
     */
    virtual CARAPI Get(
        /* [out] */ ArrayOf<Byte>* dst);

    /**
     * Reads bytes from the current position into the specified byte array,
     * starting at the specified offset, and increases the position by the
     * number of bytes read.
     *
     * @param dst
     *            the target byte array.
     * @param dstOffset
     *            the offset of the byte array, must not be negative and
     *            not greater than {@code dst.length}.
     * @param byteCount
     *            the number of bytes to read, must not be negative and not
     *            greater than {@code dst.length - dstOffset}
     * @return {@code this}
     * @exception IndexOutOfBoundsException if {@code dstOffset < 0 ||  byteCount < 0}
     * @exception BufferUnderflowException if {@code byteCount > remaining()}
     */
    virtual CARAPI Get(
        /* [out] */ ArrayOf<Byte>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 byteCount);

    /**
     * Returns the char at the current position and increases the position by 2.
     * <p>
     * The 2 bytes starting at the current position are composed into a char
     * according to the current byte order and returned.
     *
     * @return the char at the current position.
     * @exception BufferUnderflowException
     *                if the position is greater than {@code limit - 2}.
     */
    virtual CARAPI GetChar(
        /* [out] */ Char32* value) = 0;

    /**
     * Returns the char at the specified index.
     * <p>
     * The 2 bytes starting from the specified index are composed into a char
     * according to the current byte order and returned. The position is not
     * changed.
     *
     * @param index
     *            the index, must not be negative and equal or less than
     *            {@code limit - 2}.
     * @return the char at the specified index.
     * @exception IndexOutOfBoundsException
     *                if {@code index} is invalid.
     */
    virtual CARAPI GetChar(
        /* [in] */ Int32 index,
        /* [out] */ Char32* value) = 0;

    /**
     * Returns the double at the current position and increases the position by
     * 8.
     * <p>
     * The 8 bytes starting from the current position are composed into a double
     * according to the current byte order and returned.
     *
     * @return the double at the current position.
     * @exception BufferUnderflowException
     *                if the position is greater than {@code limit - 8}.
     */
    virtual CARAPI GetDouble(
        /* [out] */ Double* value) = 0;

    /**
     * Returns the double at the specified index.
     * <p>
     * The 8 bytes starting at the specified index are composed into a double
     * according to the current byte order and returned. The position is not
     * changed.
     *
     * @param index
     *            the index, must not be negative and equal or less than
     *            {@code limit - 8}.
     * @return the double at the specified index.
     * @exception IndexOutOfBoundsException
     *                if {@code index} is invalid.
     */
    virtual CARAPI GetDouble(
        /* [in] */ Int32 index,
        /* [out] */ Double* value) = 0;

    /**
     * Returns the float at the current position and increases the position by
     * 4.
     * <p>
     * The 4 bytes starting at the current position are composed into a float
     * according to the current byte order and returned.
     *
     * @return the float at the current position.
     * @exception BufferUnderflowException
     *                if the position is greater than {@code limit - 4}.
     */
    virtual CARAPI GetFloat(
        /* [out] */ Float* value) = 0;

    /**
     * Returns the float at the specified index.
     * <p>
     * The 4 bytes starting at the specified index are composed into a float
     * according to the current byte order and returned. The position is not
     * changed.
     *
     * @param index
     *            the index, must not be negative and equal or less than
     *            {@code limit - 4}.
     * @return the float at the specified index.
     * @exception IndexOutOfBoundsException
     *                if {@code index} is invalid.
     */
    virtual CARAPI GetFloat(
        /* [in] */ Int32 index,
        /* [out] */ Float* value) = 0;

    /**
     * Returns the int at the current position and increases the position by 4.
     * <p>
     * The 4 bytes starting at the current position are composed into a int
     * according to the current byte order and returned.
     *
     * @return the int at the current position.
     * @exception BufferUnderflowException
     *                if the position is greater than {@code limit - 4}.
     */
    virtual CARAPI GetInt32(
        /* [out] */ Int32* value) = 0;

    /**
     * Returns the int at the specified index.
     * <p>
     * The 4 bytes starting at the specified index are composed into a int
     * according to the current byte order and returned. The position is not
     * changed.
     *
     * @param index
     *            the index, must not be negative and equal or less than
     *            {@code limit - 4}.
     * @return the int at the specified index.
     * @exception IndexOutOfBoundsException
     *                if {@code index} is invalid.
     */
    virtual CARAPI GetInt32(
        /* [in] */ Int32 index,
        /* [out] */ Int32* value) = 0;

    /**
     * Returns the long at the current position and increases the position by 8.
     * <p>
     * The 8 bytes starting at the current position are composed into a long
     * according to the current byte order and returned.
     *
     * @return the long at the current position.
     * @exception BufferUnderflowException
     *                if the position is greater than {@code limit - 8}.
     */
    virtual CARAPI GetInt64(
        /* [out] */ Int64* value) = 0;

    /**
     * Returns the long at the specified index.
     * <p>
     * The 8 bytes starting at the specified index are composed into a long
     * according to the current byte order and returned. The position is not
     * changed.
     *
     * @param index
     *            the index, must not be negative and equal or less than
     *            {@code limit - 8}.
     * @return the long at the specified index.
     * @exception IndexOutOfBoundsException
     *                if {@code index} is invalid.
     */
    virtual CARAPI GetInt64(
        /* [in] */ Int32 index,
        /* [out] */ Int64* value) = 0;

    /**
     * Returns the short at the current position and increases the position by 2.
     * <p>
     * The 2 bytes starting at the current position are composed into a short
     * according to the current byte order and returned.
     *
     * @return the short at the current position.
     * @exception BufferUnderflowException
     *                if the position is greater than {@code limit - 2}.
     */
    virtual CARAPI GetInt16(
        /* [out] */ Int16* value) = 0;

    /**
     * Returns the short at the specified index.
     * <p>
     * The 2 bytes starting at the specified index are composed into a short
     * according to the current byte order and returned. The position is not
     * changed.
     *
     * @param index
     *            the index, must not be negative and equal or less than
     *            {@code limit - 2}.
     * @return the short at the specified index.
     * @exception IndexOutOfBoundsException
     *                if {@code index} is invalid.
     */
    virtual CARAPI GetInt16(
        /* [in] */ Int32 index,
        /* [out] */ Int16* value) = 0;

    /**
     * Returns the byte order used by this buffer when converting bytes from/to
     * other primitive types.
     * <p>
     * The default byte order of byte buffer is always
     * {@link ByteOrder#BIG_ENDIAN BIG_ENDIAN}
     *
     * @return the byte order used by this buffer when converting bytes from/to
     *         other primitive types.
     */
    CARAPI GetOrder(
        /* [out] */ ByteOrder* byteOrder);

    /**
     * Sets the byte order of this buffer.
     *
     * @param byteOrder
     *            the byte order to set. If {@code null} then the order
     *            will be {@link ByteOrder#LITTLE_ENDIAN LITTLE_ENDIAN}.
     * @return {@code this}
     * @see ByteOrder
     */
    CARAPI SetOrder(
        /* [in] */ ByteOrder byteOrder);

    /**
     * Subverts the fact that order(ByteOrder) is final, for the benefit of MappedByteBufferAdapter.
     */
    virtual CARAPI SetOrderImpl(
        /* [in] */ ByteOrder byteOrder);

    /**
     * Child class implements this method to realize {@code array()}.
     *
     * @see #array()
     */
    virtual CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Byte>** array) = 0;

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
     * Writes the given byte to the current position and increases the position
     * by 1.
     *
     * @param b
     *            the byte to write.
     * @return {@code this}
     * @exception BufferOverflowException
     *                if position is equal or greater than limit.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ Byte b) = 0;

    /**
     * Write a byte to the specified index of this buffer without changing the
     * position.
     *
     * @param index
     *            the index, must not be negative and less than the limit.
     * @param b
     *            the byte to write.
     * @return {@code this}
     * @exception IndexOutOfBoundsException
     *                if {@code index} is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Byte b) = 0;

    /**
     * Writes bytes in the given byte array to the current position and
     * increases the position by the number of bytes written.
     * <p>
     * Calling this method has the same effect as
     * {@code put(src, 0, src.length)}.
     *
     * @param src
     *            the source byte array.
     * @return {@code this}
     * @exception BufferOverflowException
     *                if {@code remaining()} is less than {@code src.length}.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    CARAPI Put(
        /* [in] */ ArrayOf<Byte>* src);

    /**
     * Writes bytes in the given byte array, starting from the specified offset,
     * to the current position and increases the position by the number of bytes
     * written.
     *
     * @param src
     *            the source byte array.
     * @param srcOffset
     *            the offset of byte array, must not be negative and not greater
     *            than {@code src.length}.
     * @param byteCount
     *            the number of bytes to write, must not be negative and not
     *            greater than {@code src.length - srcOffset}.
     * @return {@code this}
     * @exception BufferOverflowException
     *                if {@code remaining()} is less than {@code byteCount}.
     * @exception IndexOutOfBoundsException
     *                if either {@code srcOffset} or {@code byteCount} is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ ArrayOf<Byte>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 byteCount);

    /**
     * Writes all the remaining bytes of the {@code src} byte buffer to this
     * buffer's current position, and increases both buffers' position by the
     * number of bytes copied.
     *
     * @param src
     *            the source byte buffer.
     * @return {@code this}
     * @exception BufferOverflowException
     *                if {@code src.remaining()} is greater than this buffer's
     *                {@code remaining()}.
     * @exception IllegalArgumentException
     *                if {@code src} is this buffer.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ IByteBuffer* src);

    /**
     * Writes the given char to the current position and increases the position
     * by 2.
     * <p>
     * The char is converted to bytes using the current byte order.
     *
     * @param value
     *            the char to write.
     * @return {@code this}
     * @exception BufferOverflowException
     *                if position is greater than {@code limit - 2}.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI PutChar(
        /* [in] */ Char32 value) = 0;

    /**
     * Writes the given char to the specified index of this buffer.
     * <p>
     * The char is converted to bytes using the current byte order. The position
     * is not changed.
     *
     * @param index
     *            the index, must not be negative and equal or less than
     *            {@code limit - 2}.
     * @param value
     *            the char to write.
     * @return {@code this}
     * @exception IndexOutOfBoundsException
     *                if {@code index} is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI PutChar(
        /* [in] */ Int32 index,
        /* [in] */ Char32 value) = 0;

    /**
     * Writes the given double to the current position and increases the position
     * by 8.
     * <p>
     * The double is converted to bytes using the current byte order.
     *
     * @param value
     *            the double to write.
     * @return {@code this}
     * @exception BufferOverflowException
     *                if position is greater than {@code limit - 8}.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI PutDouble(
        /* [in] */ Double value) = 0;

    /**
     * Writes the given double to the specified index of this buffer.
     * <p>
     * The double is converted to bytes using the current byte order. The
     * position is not changed.
     *
     * @param index
     *            the index, must not be negative and equal or less than
     *            {@code limit - 8}.
     * @param value
     *            the double to write.
     * @return {@code this}
     * @exception IndexOutOfBoundsException
     *                if {@code index} is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI PutDouble(
        /* [in] */ Int32 index,
        /* [in] */ Double value) = 0;

    /**
     * Writes the given float to the current position and increases the position
     * by 4.
     * <p>
     * The float is converted to bytes using the current byte order.
     *
     * @param value
     *            the float to write.
     * @return {@code this}
     * @exception BufferOverflowException
     *                if position is greater than {@code limit - 4}.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI PutFloat(
        /* [in] */ Float value) = 0;

    /**
     * Writes the given float to the specified index of this buffer.
     * <p>
     * The float is converted to bytes using the current byte order. The
     * position is not changed.
     *
     * @param index
     *            the index, must not be negative and equal or less than
     *            {@code limit - 4}.
     * @param value
     *            the float to write.
     * @return {@code this}
     * @exception IndexOutOfBoundsException
     *                if {@code index} is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI PutFloat(
        /* [in] */ Int32 index,
        /* [in] */ Float value) = 0;

    /**
     * Writes the given short to the current position and increases the position
     * by 2.
     * <p>
     * The short is converted to bytes using the current byte order.
     *
     * @param value
     *            the short to write.
     * @return {@code this}
     * @exception BufferOverflowException
     *                if position is greater than {@code limit - 2}.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI PutInt16(
        /* [in] */ Int16 value) = 0;

    /**
     * Writes the given short to the specified index of this buffer.
     * <p>
     * The short is converted to bytes using the current byte order. The
     * position is not changed.
     *
     * @param index
     *            the index, must not be negative and equal or less than
     *            {@code limit - 2}.
     * @param value
     *            the short to write.
     * @return {@code this}
     * @exception IndexOutOfBoundsException
     *                if {@code index} is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI PutInt16(
        /* [in] */ Int32 index,
        /* [in] */ Int16 value) = 0;

    /**
     * Writes the given int to the current position and increases the position by
     * 4.
     * <p>
     * The int is converted to bytes using the current byte order.
     *
     * @param value
     *            the int to write.
     * @return {@code this}
     * @exception BufferOverflowException
     *                if position is greater than {@code limit - 4}.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI PutInt32(
        /* [in] */ Int32 value) = 0;

    /**
     * Writes the given int to the specified index of this buffer.
     * <p>
     * The int is converted to bytes using the current byte order. The position
     * is not changed.
     *
     * @param index
     *            the index, must not be negative and equal or less than
     *            {@code limit - 4}.
     * @param value
     *            the int to write.
     * @return {@code this}
     * @exception IndexOutOfBoundsException
     *                if {@code index} is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI PutInt32(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value) = 0;

    /**
     * Writes the given long to the current position and increases the position
     * by 8.
     * <p>
     * The long is converted to bytes using the current byte order.
     *
     * @param value
     *            the long to write.
     * @return {@code this}
     * @exception BufferOverflowException
     *                if position is greater than {@code limit - 8}.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI PutInt64(
        /* [in] */ Int64 value) = 0;

    /**
     * Writes the given long to the specified index of this buffer.
     * <p>
     * The long is converted to bytes using the current byte order. The position
     * is not changed.
     *
     * @param index
     *            the index, must not be negative and equal or less than
     *            {@code limit - 8}.
     * @param value
     *            the long to write.
     * @return {@code this}
     * @exception IndexOutOfBoundsException
     *                if {@code index} is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI PutInt64(
        /* [in] */ Int32 index,
        /* [in] */ Int64 value) = 0;

    /**
     * Returns a sliced buffer that shares its content with this buffer.
     * <p>
     * The sliced buffer's capacity will be this buffer's
     * {@code remaining()}, and it's zero position will correspond to
     * this buffer's current position. The new buffer's position will be 0,
     * limit will be its capacity, and its mark is cleared. The new buffer's
     * read-only property and byte order are the same as this buffer's.
     * <p>
     * The new buffer shares its content with this buffer, which means either
     * buffer's change of content will be visible to the other. The two buffer's
     * position, limit and mark are independent.
     *
     * @return a sliced buffer that shares its content with this buffer.
     */
    virtual CARAPI Slice(
        /* [out] */ IByteBuffer** buffer) = 0;

    /**
     * Indicates whether this buffer is direct.
     *
     * @return {@code true} if this buffer is direct, {@code false} otherwise.
     */
    virtual CARAPI IsDirect(
        /* [out] */ Boolean* isDirect) = 0;

    CARAPI HasArray(
        /* [out] */ Boolean* hasArray);

    CARAPI IsAccessible(
        /* [out] */ Boolean* hasArray);

    CARAPI SetAccessible(
        /* [in] */ Boolean accessible);

    /**
     * Checks whether this byte buffer is equal to another object.
     * <p>
     * If {@code other} is not a byte buffer then {@code false} is returned. Two
     * byte buffers are equal if and only if their remaining bytes are exactly
     * the same. Position, limit, capacity and mark are not considered.
     *
     * @param other
     *            the object to compare with this byte buffer.
     * @return {@code true} if this byte buffer is equal to {@code other},
     *         {@code false} otherwise.
     */
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* isEquals);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

protected:

    ByteBuffer();

    /**
     * Constructs a {@code ByteBuffer} with given capacity.
     *
     * @param capacity
     *            the capacity of the buffer.
     */
    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ Int64 effectiveDirectAddress);

public:
    /**
     * The byte order of this buffer, default is {@code BIG_ENDIAN}.
     */
    ByteOrder mOrder;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_BYTEBUFFER_H__
