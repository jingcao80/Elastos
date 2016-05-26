
#ifndef __ELASTOS_IO_CFLOATBUFFERHELPER_H__
#define __ELASTOS_IO_CFLOATBUFFERHELPER_H__

#include "_Elastos_IO_CFloatBufferHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace IO {

CarClass(CFloatBufferHelper)
    , public Singleton
    , public IFloatBufferHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Creates a float buffer based on a newly allocated float array.
     *
     * @param capacity
     *            the capacity of the new buffer.
     * @return the created float buffer.
     * @throws IllegalArgumentException
     *             if {@code capacity} is less than zero.
     */
    CARAPI Allocate(
        /* [in] */ Int32 capacity,
        /* [out] */ IFloatBuffer** floatBuffer);

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
    CARAPI Wrap(
        /* [in] */ ArrayOf<Float>* array,
        /* [out] */ IFloatBuffer** floatBuffer);

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
    CARAPI Wrap(
        /* [in] */ ArrayOf<Float>* array,
        /* [in] */ Int32 start,
        /* [in] */ Int32 floatCount,
        /* [out] */ IFloatBuffer** floatBuffer);

private:
    // TODO: Add your private member variables here.
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CFLOATBUFFERHELPER_H__
