#ifndef __ELASTOS_IO_CHARBUFFER_H__
#define __ELASTOS_IO_CHARBUFFER_H__

#include "Buffer.h"

using Elastos::Core::ICharSequence;

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
 * <li>Use {@link java.nio.ByteBuffer#asCharBuffer() ByteBuffer.asCharBuffer}
 * to create a char buffer based on a byte buffer.</li>
 * </ul>
 */
class CharBuffer
    : public Buffer
    , public ICharBuffer
{
public:
    CAR_INTERFACE_DECL()

	/**
     * Creates a char buffer based on a newly allocated char array.
     *
     * @param capacity
     *            the capacity of the new buffer.
     * @return the created char buffer.
     * @throws IllegalArgumentException
     *             if {@code capacity} is less than zero.
     */
    static CARAPI Allocate(
        /* [in] */ Int32 capacity,
        /* [out] */ ICharBuffer** buffer);

    /**
     * Creates a new char buffer by wrapping the given char array.
     * <p>
     * Calling this method has the same effect as
     * {@code wrap(array, 0, array.length)}.
     *
     * @param array
     *            the char array which the new buffer will be based on.
     * @return the created char buffer.
     */
    static CARAPI Wrap(
        /* [in] */ ArrayOf<Char32>* array,
        /* [out] */ ICharBuffer** buffer);

    /**
     * Creates a new char buffer by wrapping the given char array.
     * <p>
     * The new buffer's position will be {@code start}, limit will be
     * {@code start + len}, capacity will be the length of the array.
     *
     * @param array
     *            the char array which the new buffer will be based on.
     * @param start
     *            the start index, must not be negative and not greater than
     *            {@code array.length}.
     * @param len
     *            the length, must not be negative and not greater than
     *            {@code array.length - start}.
     * @return the created char buffer.
     * @exception IndexOutOfBoundsException
     *                if either {@code start} or {@code len} is invalid.
     */
    static CARAPI Wrap(
        /* [in] */ ArrayOf<Char32>* array,
        /* [in] */ Int32 start,
        /* [in] */ Int32 len,
        /* [out] */ ICharBuffer** buffer);

    /**
     * Creates a new char buffer by wrapping the given char sequence.
     * <p>
     * Calling this method has the same effect as
     * {@code wrap(chseq, 0, chseq.length())}.
     *
     * @param chseq
     *            the char sequence which the new buffer will be based on.
     * @return the created char buffer.
     */
    static CARAPI Wrap(
        /* [in] */ ICharSequence* chseq,
        /* [out] */ ICharBuffer** buffer);

    /**
     * Creates a new char buffer by wrapping the given char sequence.
     * <p>
     * The new buffer's position will be {@code start}, limit will be
     * {@code end}, capacity will be the length of the char sequence. The new
     * buffer is read-only.
     *
     * @param chseq
     *            the char sequence which the new buffer will be based on.
     * @param start
     *            the start index, must not be negative and not greater than
     *            {@code chseq.length()}.
     * @param end
     *            the end index, must be no less than {@code start} and no
     *            greater than {@code chseq.length()}.
     * @return the created char buffer.
     * @exception IndexOutOfBoundsException
     *                if either {@code start} or {@code end} is invalid.
     */
    static CARAPI Wrap(
        /* [in] */ ICharSequence* chseq,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ ICharBuffer** charBuf);

    CARAPI GetArray(
        /* [out] */ IArrayOf** array);

    CARAPI GetArray(
        /*[out, callee] */ ArrayOf<Char32>** array);

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
        /* [out] */ ICharBuffer** buffer) = 0;

    /**
     * Get character at the specified index, with the first character
     * having index zero.
     *
     * @param index
     *            the index of the character to return.
     *
     * @param value
     *            the requested character.
     *
     * @Return NOERROR if successful or IndexOutOfBoundsException
     *             if {@code index < 0} or {@code index} is greater than the
     *             length of this sequence.
     */
    CARAPI GetCharAt(
        /* [in] */ Int32 index,
        /* [out] */ Char32* value);

    /**
     * Compacts this char buffer.
     * <p>
     * The remaining chars will be moved to the head of the buffer,
     * starting from position zero. Then the position is set to
     * {@code remaining()}; the limit is set to capacity; the mark is cleared.
     *
     * @return this buffer.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Compact() = 0;

    /**
     * Compare the remaining chars of this buffer to another char
     * buffer's remaining chars.
     *
     * @param otherBuffer
     *      another char buffer.
     *
     * @param result
     *      return a negative value if this is less than {@code otherBuffer}; 0 if
     *      this equals to {@code otherBuffer}; a positive value if this is
     *      greater than {@code otherBuffer}.
     *
     * @return NOERROR is successful, or  ClassCastException
     *                if {@code otherBuffer} is not a char buffer.
     */
    virtual CARAPI CompareTo(
        /* [in] */ ICharBuffer* otherBuffer,
        /* [out] */ Int32* result);

    /**
     * Returns a duplicated buffer that shares its content with this buffer.
     * <p>
     * The duplicated buffer's initial position, limit, capacity and mark are
     * the same as this buffer's. The duplicated buffer's read-only property and
     * byte order are the same as this buffer's, too.
     * <p>
     * The new buffer shares its content with this buffer, which means either
     * buffer's change of content will be visible to the other. The two buffer's
     * position, limit and mark are independent.
     *
     * @return a duplicated buffer that shares its content with this buffer.
     */
    virtual CARAPI Duplicate(
        /* [out] */ ICharBuffer** buffer) = 0;

    /**
     * Checks whether this buf is equal to another object.
     *
     * If other is not a char buffer then false is returned. Two
     * char buffers are equal if and only if their remaining chars are
     * exactly the same. Position, limit, capacity and mark are not considerd.
     *
     * @param other
     *      the object to compare with this char buffer.
     * return true if this char buffer is equal to other, false otherwise.
     */
     virtual CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* rst);

    /**
     * Returns the char at the current position and increases the position by 1.
     *
     * @return the char at the current position.
     * @exception BufferUnderflowException
     *                if the position is equal or greater than limit.
     */
    virtual CARAPI Get(
        /* [out] */ Char32* value) = 0;

    virtual CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Char32* value) = 0;

    /**
     * Reads chars from the current position into the specified char array and
     * increases the position by the number of chars read.
     * <p>
     * Calling this method has the same effect as
     * {@code get(dst, 0, dst.length)}.
     *
     * @param dst
     *            the destination char array.
     * @return this buffer.
     * @exception BufferUnderflowException
     *                if {@code dst.length} is greater than {@code remaining()}.
     */
    virtual CARAPI Get(
        /* [out] */ ArrayOf<Char32>* dst);

    /**
     * Reads chars from the current position into the specified char array,
     * starting from the specified offset, and increases the position by the
     * number of chars read.
     *
     * @param dst
     *            the target char array.
     * @param off
     *            the offset of the char array, must not be negative and not
     *            greater than {@code dst.length}.
     * @param len
     *            The number of chars to read, must be no less than zero and no
     *            greater than {@code dst.length - off}.
     * @return this buffer.
     * @exception IndexOutOfBoundsException
     *                if either {@code off} or {@code len} is invalid.
     * @exception BufferUnderflowException
     *                if {@code len} is greater than {@code remaining()}.
     */
    virtual CARAPI Get(
        /* [out] */ ArrayOf<Char32>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 charCount);

    /**
     * Returns the number of remaining chars.
     *
     * @return the number of remaining chars.
     */
    CARAPI GetLength(
        /* [out] */ Int32* number);

    /**
     * Returns the byte order used by this buffer when converting chars from/to
     * bytes.
     * <p>
     * If this buffer is not based on a byte buffer, then this always returns
     * the platform's native byte order.
     *
     * @return the byte order used by this buffer when converting chars from/to
     *         bytes.
     */
    virtual CARAPI GetOrder(
        /* [out] */ ByteOrder* byteOrder) = 0;

    /**
     * Writes the given char to the current position and increases the position
     * by 1.
     *
     * @param c
     *            the char to write.
     * @return this buffer.
     * @exception BufferOverflowException
     *                if position is equal or greater than limit.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ Char32 c) = 0;

    /**
     * Writes a char to the specified index of this buffer; the position is not
     * changed.
     *
     * @param index
     *            the index, must be no less than zero and less than the limit.
     * @param c
     *            the char to write.
     * @return this buffer.
     * @exception IndexOutOfBoundsException
     *                if index is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Char32 c) = 0;

    /**
     * Writes chars from the given char array to the current position and
     * increases the position by the number of chars written.
     * <p>
     * Calling this method has the same effect as
     * {@code put(src, 0, src.length)}.
     *
     * @param src
     *            the source char array.
     * @return this buffer.
     * @exception BufferOverflowException
     *                if {@code remaining()} is less than {@code src.length}.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    CARAPI Put(
        /* [in] */ ArrayOf<Char32>* src);

    /**
     * Writes chars from the given char array, starting from the specified offset,
     * to the current position and increases the position by the number of chars
     * written.
     *
     * @param src
     *            the source char array.
     * @param srcOffset
     *            the offset of char array, must not be negative and not greater
     *            than {@code src.length}.
     * @param charCount
     *            the number of chars to write, must be no less than zero and no
     *            greater than {@code src.length - srcOffset}.
     * @return this buffer.
     * @exception BufferOverflowException
     *                if {@code remaining()} is less than {@code charCount}.
     * @exception IndexOutOfBoundsException
     *                if either {@code srcOffset} or {@code charCount} is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ ArrayOf<Char32>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 charCount);

    /**
     * Writes all the remaining chars of the {@code src} char buffer to this
     * buffer's current position, and increases both buffers' position by the
     * number of chars copied.
     *
     * @param src
     *            the source char buffer.
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
        /* [in] */ ICharBuffer* src);

    /**
     * Writes all chars of the given string to the current position of this
     * buffer, and increases the position by the length of string.
     * <p>
     * Calling this method has the same effect as
     * {@code put(str, 0, str.length())}.
     *
     * @param str
     *            the string to write.
     * @return this buffer.
     * @exception BufferOverflowException
     *                if {@code remaining()} is less than the length of string.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    CARAPI Put(
        /* [in] */ const String& str);

    /**
     * Writes chars of the given string to the current position of this buffer,
     * and increases the position by the number of chars written.
     *
     * @param str
     *            the string to write.
     * @param start
     *            the first char to write, must not be negative and not greater
     *            than {@code str.length()}.
     * @param end
     *            the last char to write (excluding), must be less than
     *            {@code start} and not greater than {@code str.length()}.
     * @return this buffer.
     * @exception BufferOverflowException
     *                if {@code remaining()} is less than {@code end - start}.
     * @exception IndexOutOfBoundsException
     *                if either {@code start} or {@code end} is invalid.
     * @exception ReadOnlyBufferException
     *                if no changes may be made to the contents of this buffer.
     */
    virtual CARAPI Put(
        /* [in] */ const String& str,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Returns a sliced buffer that shares its content with this buffer.
     * <p>
     * The sliced buffer's capacity will be this buffer's {@code remaining()},
     * and its zero position will correspond to this buffer's current position.
     * The new buffer's position will be 0, limit will be its capacity, and its
     * mark is cleared. The new buffer's read-only property and byte order are
     * same as this buffer.
     * <p>
     * The new buffer shares its content with this buffer, which means either
     * buffer's change of content will be visible to the other. The two buffer's
     * position, limit and mark are independent.
     *
     * @return a sliced buffer that shares its content with this buffer.
     */
    virtual CARAPI Slice(
        /* [out] */ ICharBuffer** buffer) = 0;

    /**
     * Returns a new char buffer representing a sub-sequence of this buffer's
     * current remaining content.
     * <p>
     * The new buffer's position will be {@code position() + start}, limit will
     * be {@code position() + end}, capacity will be the same as this buffer.
     * The new buffer's read-only property and byte order are the same as this
     * buffer.
     * <p>
     * The new buffer shares its content with this buffer, which means either
     * buffer's change of content will be visible to the other. The two buffer's
     * position, limit and mark are independent.
     *
     * @param start
     *            the start index of the sub-sequence, referenced from the
     *            current buffer position. Must not be less than zero and not
     *            greater than the value obtained from a call to
     *            {@code remaining()}.
     * @param end
     *            the end index of the sub-sequence, referenced from the current
     *            buffer position. Must not be less than {@code start} and not
     *            be greater than the value obtained from a call to
     *            {@code remaining()}.
     * @return a new char buffer represents a sub-sequence of this buffer's
     *         current remaining content.
     * @exception IndexOutOfBoundsException
     *                if either {@code start} or {@code end} is invalid.
     */
    virtual CARAPI SubSequence(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ ICharSequence** csq) = 0;

    /**
     * Returns a string representing the current remaining chars of this buffer.
     *
     * @return a string representing the current remaining chars of this buffer.
     */
    virtual CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Write the given char to the current position and increases
     * the position by 1.
     *
     * @param c
     *      the char to write.
     * @return this buffer.
     */
    virtual CARAPI Append(
        /* [in] */ Char32 c);

    virtual CARAPI Append(
        /* [in] */ ICharSequence* csq);

    virtual CARAPI Append(
        /* [in] */ ICharSequence* csq,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Reads characters from this buffer and puts them into {@code target}. The
     * number of chars that are copied is either the number of remaining chars
     * in this buffer or the number of remaining chars in {@code target},
     * whichever is smaller.
     *
     * @param target
     *            the target char buffer.
     * @throws IllegalArgumentException
     *             if {@code target} is this buffer.
     * @throws IOException
     *             if an I/O error occurs.
     * @throws ReadOnlyBufferException
     *             if no changes may be made to the contents of {@code target}.
     * @return the number of chars copied or -1 if there are no chars left to be
     *         read from this buffer.
     */
    virtual CARAPI Read(
        /* [in] */ ICharBuffer* target,
        /* [out] */ Int32* number);

    virtual CARAPI HasArray(
        /* [out] */ Boolean* rst);

    /**
     * Indicates whether this buffer id direct. A direct buffer will try
     * its best to take advantage of native memory APIs and it may not stay
     * in the java heap, so it is not affected by garbage collection.
     *
     * A char buffer is direct if it is base on a byte buffer and the byte
     * buffer is direct.
     *
     * @return true is this buffer is direct, false otherwise.
     */
    virtual CARAPI IsDirect(
        /* [out] */ Boolean* rst) = 0;

protected:
    CharBuffer();

    /**
     * Constructs a {@code CharBuffer} with given capacity.
     *
     * @param capacity
     *            the capacity of the buffer.
     */
    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ Int64 effectiveDirectAddress);

    /**
     * Child class implements this method to realize {@code array()}.
     *
     * @see #array()
     */
    virtual CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Char32>** array) = 0;

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
        /* [out] */ Boolean* result) = 0;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CHARBUFFER_H__
