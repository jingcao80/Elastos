#ifndef __ELASTOS_IO_FLOATBUFFER_H__
#define __ELASTOS_IO_FLOATBUFFER_H__

#include "Buffer.h"

namespace Elastos {
namespace IO {

/**
 * A buffer of floats.
 * <p>
 * A float buffer can be created in either of the following ways:
 * <ul>
 * <li>{@link #allocate(int) Allocate} a new float array and create a buffer
 * based on it;</li>
 * <li>{@link #wrap(float[]) Wrap} an existing float array to create a new
 * buffer;</li>
 * <li>Use {@link java.nio.ByteBuffer#asFloatBuffer() ByteBuffer.asFloatBuffer}
 * to create a float buffer based on a byte buffer.</li>
 * </ul>
 */
class FloatBuffer
    : public Buffer
    , public IFloatBuffer
{
public:
    CAR_INTERFACE_DECL()

    FloatBuffer();

    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ Int64 effectiveDirectAddress);

    /**
     * Creates a float buffer based on a newly allocated float array.
     *
     * @param capacity
     *            the capacity of the new buffer.
     * @return the created float buffer.
     * @throws IllegalArgumentException
     *             if {@code capacity} is less than zero.
     */
    static CARAPI Allocate(
        /* [in] */ Int32 capacity,
        /* [out] */ IFloatBuffer** buf);

    /**
     * Creates a new float buffer by wrapping the given float array.
     * <p>
     * Calling this method has the same effect as
     * {@code wrap(array, 0, array.length)}.
     *
     * @param array
     *            the float array which the new buffer will be based on.
     * @return the created float buffer.
     */
    static CARAPI Wrap(
        /* [in] */ ArrayOf<Float>* array,
        /* [out] */ IFloatBuffer** buf);

    /**
     * Creates a new float buffer by wrapping the given float array.
     * <p>
     * The new buffer's position will be {@code start}, limit will be
     * {@code start + floatCount}, capacity will be the length of the array.
     *
     * @param array
     *            the float array which the new buffer will be based on.
     * @param start
     *            the start index, must not be negative and not greater than
     *            {@code array.length}.
     * @param floatCount
     *            the length, must not be negative and not greater than
     *            {@code array.length - start}.
     * @return the created float buffer.
     * @exception IndexOutOfBoundsException
     *                if either {@code start} or {@code floatCount} is invalid.
     * @exception NullPointerException
     *                if {@code array} is null.
     */
    static CARAPI Wrap(
        /* [in] */ ArrayOf<Float>* array,
        /* [in] */ Int32 start,
        /* [in] */ Int32 floatCount,
        /* [out] */ IFloatBuffer** buf);

    CARAPI GetArray(
        /* [out] */ IArrayOf** array);

    CARAPI GetArray(
        /* [out, callee] */ ArrayOf<Float>** array);

    CARAPI GetArrayOffset(
        /* [out] */ Int32* offset);

    virtual CARAPI AsReadOnlyBuffer(
        /* [out] */ IFloatBuffer** buffer) = 0;

    virtual CARAPI Compact() = 0;

    virtual CARAPI CompareTo(
        /* [in] */ IFloatBuffer* otherBuffer,
        /* [out] */ Int32* result);

    virtual CARAPI Duplicate(
        /* [out] */ IFloatBuffer** buffer) = 0;

    /**
     * Checks whether this float buffer is equal to another object. If {@code
     * other} is not a {@code FloatBuffer} then {@code false} is returned.
     *
     * <p>Two float buffers are equal if their remaining floats are equal.
     * Position, limit, capacity and mark are not considered.
     *
     * <p>This method considers two floats {@code a} and {@code b} to be equal
     * if {@code a == b} or if {@code a} and {@code b} are both {@code NaN}.
     * Unlike {@link Float#equals}, this method considers {@code -0.0} and
     * {@code +0.0} to be equal.
     *
     * @param other
     *            the object to compare with this float buffer.
     * @return {@code true} if this float buffer is equal to {@code other},
     *         {@code false} otherwise.
     */
    virtual CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* rst);

    virtual CARAPI Get(
        /* [out] */ Float* value) = 0;

    virtual CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Float* value) = 0;

    virtual CARAPI Get(
        /* [out] */ ArrayOf<Float>* dst);

    virtual CARAPI Get(
        /* [out] */ ArrayOf<Float>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 floatCount);

    CARAPI HasArray(
        /* [out] */ Boolean* hasArray);

    virtual CARAPI GetOrder(
        /* [out] */ ByteOrder* byteOrder) = 0;

    virtual CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Float>** array) = 0;

    virtual CARAPI ProtectedArrayOffset(
        /* [out] */ Int32* offset) = 0;

    virtual CARAPI ProtectedHasArray(
        /* [out] */ Boolean* hasArray) = 0;

    virtual CARAPI Put(
        /* [in] */ Float d) = 0;

    virtual CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Float d) = 0;

    CARAPI Put(
        /* [in] */ const ArrayOf<Float>& src);

    virtual CARAPI Put(
        /* [in] */ const ArrayOf<Float>& src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 floatCount);

    virtual CARAPI Put(
        /* [in] */ IFloatBuffer* src);

    virtual CARAPI Slice(
        /* [out] */ IFloatBuffer** buffer) = 0;

};
} // namespace IO
} // namespace Elastos
#endif // __ELASTOS_IO_FLOATBUFFER_H__
