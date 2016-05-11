#ifndef __ELASTOS_IO_CBYTEBUFFERHELPER_H__
#define __ELASTOS_IO_CBYTEBUFFERHELPER_H__

#include "_Elastos_IO_CByteBufferHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace IO {

CarClass(CByteBufferHelper)
    , public Singleton
    , public IByteBufferHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Creates a byte buffer based on a newly allocated byte array.
     *
     * @param capacity
     *            the capacity of the new buffer
     * @return the created byte buffer.
     * @throws IllegalArgumentException
     *             if {@code capacity < 0}.
     */
    CARAPI Allocate(
        /* [in] */ Int32 capacity,
        /* [out] */ IByteBuffer** byteBuf);

    /**
     * Creates a direct byte buffer based on a newly allocated memory block.
     *
     * @param capacity
     *            the capacity of the new buffer
     * @return the created byte buffer.
     * @throws IllegalArgumentException
     *             if {@code capacity < 0}.
     */
    CARAPI AllocateDirect(
        /* [in] */ Int32 capacity,
        /* [out] */ IByteBuffer** byteBuf);

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
    CARAPI Wrap(
        /* [in] */ ArrayOf<Byte>* array,
        /* [out] */ IByteBuffer** byteBuf);

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
    CARAPI Wrap(
        /* [in] */ ArrayOf<Byte>* array,
        /* [in] */ Int32 start,
        /* [in] */ Int32 byteCount,
        /* [out] */ IByteBuffer** byteBuf);
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CBYTEBUFFERHELPER_H__
