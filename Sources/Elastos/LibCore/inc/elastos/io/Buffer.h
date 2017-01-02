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

#ifndef __ELASTOS_IO_BUFFER_H__
#define __ELASTOS_IO_BUFFER_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Object.h"

using Elastos::Core::IArrayOf;
using Elastos::Core::Object;

namespace Elastos {
namespace IO {

/**
 * A buffer is a list of elements of a specific primitive type.
 * <p>
 * A buffer can be described by the following properties:
 * <ul>
 * <li>Capacity: the number of elements a buffer can hold. Capacity may not be
 * negative and never changes.</li>
 * <li>Position: a cursor of this buffer. Elements are read or written at the
 * position if you do not specify an index explicitly. Position may not be
 * negative and not greater than the limit.</li>
 * <li>Limit: controls the scope of accessible elements. You can only read or
 * write elements from index zero to <code>limit - 1</code>. Accessing
 * elements out of the scope will cause an exception. Limit may not be negative
 * and not greater than capacity.</li>
 * <li>Mark: used to remember the current position, so that you can reset the
 * position later. Mark may not be negative and no greater than position.</li>
 * <li>A buffer can be read-only or read-write. Trying to modify the elements
 * of a read-only buffer will cause a <code>ReadOnlyBufferException</code>,
 * while changing the position, limit and mark of a read-only buffer is OK.</li>
 * <li>A buffer can be direct or indirect. A direct buffer will try its best to
 * take advantage of native memory APIs and it may not stay in the Java heap,
 * thus it is not affected by garbage collection.</li>
 * </ul>
 * <p>
 * Buffers are not thread-safe. If concurrent access to a buffer instance is
 * required, then the callers are responsible to take care of the
 * synchronization issues.
 */
class Buffer
    : public Object
    , public IBuffer
{
public:
    CAR_INTERFACE_DECL()

    virtual CARAPI GetArray(
        /* [out] */ IArrayOf** array) = 0;
    /**
     * Returns the offset into the array returned by {@code array} of the first
     * element of the buffer (optional operation). The backing array (if there is one)
     * is not necessarily the same size as the buffer, and position 0 in the buffer is
     * not necessarily the 0th element in the array. Use
     * {@code buffer.array()[offset + buffer.arrayOffset()} to access element {@code offset}
     * in {@code buffer}.
     *
     * <p>Use {@code hasArray} to ensure this method won't throw.
     * (A separate call to {@code isReadOnly} is not necessary.)
     *
     * @return the offset
     * @throws ReadOnlyBufferException if the buffer is read-only
     *         UnsupportedOperationException if the buffer does not expose an array
     * @since 1.6
     */
    virtual CARAPI GetArrayOffset(
       /* [out] */ Int32* offset) = 0;

    /**
     * Returns the capacity of this buffer.
     *
     * @return the number of elements that are contained in this buffer.
     */
    CARAPI GetCapacity(
        /* [out] */ Int32* cap);

    /**
     * Clears this buffer.
     * <p>
     * While the content of this buffer is not changed, the following internal
     * changes take place: the current position is reset back to the start of
     * the buffer, the value of the buffer limit is made equal to the capacity
     * and mark is cleared.
     *
     * @return this buffer.
     */
    CARAPI Clear();

    /**
     * Flips this buffer.
     * <p>
     * The limit is set to the current position, then the position is set to
     * zero, and the mark is cleared.
     * <p>
     * The content of this buffer is not changed.
     *
     * @return this buffer.
     */
    CARAPI Flip();

    /**
     * Returns true if {@code array} and {@code arrayOffset} won't throw. This method does not
     * return true for buffers not backed by arrays because the other methods would throw
     * {@code UnsupportedOperationException}, nor does it return true for buffers backed by
     * read-only arrays, because the other methods would throw {@code ReadOnlyBufferException}.
     * @since 1.6
     */
    virtual CARAPI HasArray(
        /* [out] */ Boolean* hasArray) = 0;

    /**
     * Indicates if there are elements remaining in this buffer, that is if
     * {@code position < limit}.
     *
     * @return {@code true} if there are elements remaining in this buffer,
     *         {@code false} otherwise.
     */
    CARAPI HasRemaining(
        /* [out] */ Boolean* hasRemaining);

    /**
     * Returns true if this is a direct buffer.
     * @since 1.6
     */
    virtual CARAPI IsDirect(
        /* [out] */ Boolean* isDirect) = 0;

    /**
     * Indicates whether this buffer is read-only.
     *
     * @return {@code true} if this buffer is read-only, {@code false}
     *         otherwise.
     */
    virtual CARAPI IsReadOnly(
        /* [out] */ Boolean* isReadOnly) = 0;

    /**
     * Returns the limit of this buffer.
     *
     * @return the limit of this buffer.
     */
    CARAPI GetLimit(
        /* [out] */ Int32* limit);

    /**
     * Sets the limit of this buffer.
     * <p>
     * If the current position in the buffer is in excess of
     * <code>newLimit</code> then, on returning from this call, it will have
     * been adjusted to be equivalent to <code>newLimit</code>. If the mark
     * is set and is greater than the new limit, then it is cleared.
     *
     * @param newLimit
     *            the new limit, must not be negative and not greater than
     *            capacity.
     * @return this buffer.
     * @exception IllegalArgumentException
     *                if <code>newLimit</code> is invalid.
     */
    CARAPI SetLimit(
        /* [in] */ Int32 newLimit);

    /**
     * Marks the current position, so that the position may return to this point
     * later by calling <code>reset()</code>.
     *
     * @return this buffer.
     */
    CARAPI Mark();

    /**
     * Returns the position of this buffer.
     *
     * @return the value of this buffer's current position.
     */
    CARAPI GetPosition(
        /* [out] */ Int32* pos);

    /**
     * Sets the position of this buffer.
     * <p>
     * If the mark is set and it is greater than the new position, then it is
     * cleared.
     *
     * @param newPosition
     *            the new position, must be not negative and not greater than
     *            limit.
     * @return this buffer.
     * @exception IllegalArgumentException
     *                if <code>newPosition</code> is invalid.
     */
    CARAPI SetPosition(
        /* [in] */ Int32 newPosition);

    virtual CARAPI SetPositionImpl(
        /* [in] */ Int32 newPosition);

    /**
     * Returns the number of remaining elements in this buffer, that is
     * {@code limit - position}.
     *
     * @return the number of remaining elements in this buffer.
     */
    CARAPI GetRemaining(
        /* [out] */ Int32* remaining);

    /**
     * Resets the position of this buffer to the <code>mark</code>.
     *
     * @return this buffer.
     * @exception InvalidMarkException
     *                if the mark is not set.
     */
    CARAPI Reset();

    /**
     * Rewinds this buffer.
     * <p>
     * The position is set to zero, and the mark is cleared. The content of this
     * buffer is not changed.
     *
     * @return this buffer.
     */
    CARAPI Rewind();

    CARAPI CheckWritable();

protected:

    Buffer();

    /**
     * Construct a buffer with the specified capacity.
     */
    CARAPI constructor(
        /* [in] */ Int32 elementSizeShift,
        /* [in] */ Int32 capacity,
        /* [in] */ Int64 effectiveDirectAddress);

    virtual ~Buffer();

    /**
     * Used for the scalar get/put operations.
     * Return TRUE is valid, otherwise is invalid index value.
     */
    virtual CARAPI CheckIndex(
        /* [in] */ Int32 index);

    /**
     * Used for the ByteBuffer operations that get types larger than a byte.
     */
    virtual CARAPI CheckIndex(
        /* [in] */ Int32 index,
        /* [in] */ Int32 sizeOfType);

    virtual CARAPI CheckGetBounds(
        /* [in] */  Int32 bytesPerElement,
        /* [in] */  Int32 length,
        /* [in] */  Int32 offset,
        /* [in] */  Int32 count,
        /* [out] */ Int32* bounds);

    virtual CARAPI CheckPutBounds(
        /* [in] */  Int32 bytesPerElement,
        /* [in] */  Int32 length,
        /* [in] */  Int32 offset,
        /* [in] */  Int32 count,
        /* [out] */ Int32* bounds);

    virtual CARAPI CheckStartEndRemaining(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Subverts the fact that limit(int) is final, for the benefit of MappedByteBufferAdapter.
     */
    virtual CARAPI SetLimitImpl(
        /* [in] */ Int32 newLimit);

    virtual CARAPI GetElementSizeShift(
        /* [out] */ Int32* elementSizeShift);

    virtual CARAPI GetEffectiveDirectAddress(
        /* [out] */ Handle64* effectiveDirectAddress);

public:
    /**
     * The capacity of this buffer, which never changes.
     */
    Int32 mCapacity;

    /**
     * <code>limit - 1</code> is the last element that can be read or written.
     * Limit must be no less than zero and no greater than <code>capacity</code>.
     */
    Int32 mLimit;

    /**
     * Mark is where position will be set when <code>reset()</code> is called.
     * Mark is not set by default. Mark is always no less than zero and no
     * greater than <code>position</code>.
     */
    Int32 mMark;

    /**
     * The current position of this buffer. Position is always no less than zero
     * and no greater than <code>limit</code>.
     */
    Int32 mPosition;

    /**
     * The log base 2 of the element size of this buffer.  Each typed subclass
     * (ByteBuffer, CharBuffer, etc.) is responsible for initializing this
     * value.  The value is used by JNI code in frameworks/base/ to avoid the
     * need for costly 'instanceof' tests.
     */
    Int32 mElementSizeShift;

    /**
     * For direct buffers, the underlying MemoryBlock; null otherwise.
     */
    Int64 mEffectiveDirectAddress;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_BUFFER_H__
