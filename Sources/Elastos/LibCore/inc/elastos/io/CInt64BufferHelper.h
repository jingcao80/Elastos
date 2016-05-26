
#ifndef __ELASTOS_IO_CINT64BUFFERHELPER_H__
#define __ELASTOS_IO_CINT64BUFFERHELPER_H__

#include "_Elastos_IO_CInt64BufferHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace IO {

CarClass(CInt64BufferHelper)
    , public Singleton
    , public IInt64BufferHelper
{
public:
    CAR_SINGLETON_DECL()

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
    CARAPI Allocate(
        /* [in] */ Int32 capacity,
        /* [out] */ IInt64Buffer** int64Buffer);

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
    CARAPI Wrap(
        /* [in] */ ArrayOf<Int64>* array,
        /* [out] */ IInt64Buffer** int64Buffer);

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
    CARAPI Wrap(
        /* [in] */ ArrayOf<Int64>* array,
        /* [in] */ Int32 start,
        /* [in] */ Int32 int64Count,
        /* [out] */ IInt64Buffer** int64Buffer);

private:
    // TODO: Add your private member variables here.
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CINT64BUFFERHELPER_H__
